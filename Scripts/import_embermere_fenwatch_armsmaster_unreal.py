"""Import and place Embermere's presentation-only Fenwatch armsmaster."""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_FBX = (
    PROJECT_ROOT
    / "ArtSource/Blender/Characters/NPCs/FenwatchArmsmaster/"
    "SM_EmbermereFenwatchArmsmaster_01.fbx"
)
DESTINATION_PATH = "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster"
ASSET_NAME = "SM_EmbermereFenwatchArmsmaster_01"
ASSET_PATH = "{}/{}".format(DESTINATION_PATH, ASSET_NAME)
SKIN_MATERIAL_PATH = DESTINATION_PATH + "/M_FenwatchArmsmasterSkin"
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
PLACEMENT_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
PLACEMENT_LOCATION = (-1320.0, -920.0, 0.0)
PLACEMENT_YAW = 100.0
PLACEMENT_FOLDER = "04_Fab_Zone_Pass/01_Village/Embermere_Originals"
ORIGINAL_TAG = unreal.Name("EmbermereOriginalArt")

SHARED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber",
    "M_FenwatchArmsmasterSkin": SKIN_MATERIAL_PATH,
}


def fail(message):
    unreal.log_error("Embermere Fenwatch armsmaster integration failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def make_rotation(yaw):
    rotation = unreal.Rotator()
    rotation.pitch = 0.0
    rotation.yaw = float(yaw)
    rotation.roll = 0.0
    return rotation


def import_static_mesh():
    if not SOURCE_FBX.is_file():
        fail("missing source FBX {}".format(SOURCE_FBX))

    if unreal.EditorAssetLibrary.does_asset_exist(ASSET_PATH):
        if not unreal.EditorAssetLibrary.delete_asset(ASSET_PATH):
            fail("could not remove prior mesh {}".format(ASSET_PATH))

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", str(SOURCE_FBX))
    task.set_editor_property("destination_path", DESTINATION_PATH)
    task.set_editor_property("destination_name", ASSET_NAME)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", False)
    task.set_editor_property("replace_existing_settings", False)
    task.set_editor_property("save", True)

    options = unreal.FbxImportUI()
    options.set_editor_property("automated_import_should_detect_type", False)
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_as_skeletal", False)
    options.set_editor_property("import_materials", True)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_STATIC_MESH)
    options.set_editor_property("original_import_type", unreal.FBXImportType.FBXIT_STATIC_MESH)
    static_data = options.get_editor_property("static_mesh_import_data")
    static_data.set_editor_property("combine_meshes", True)
    static_data.set_editor_property("generate_lightmap_u_vs", True)
    static_data.set_editor_property("auto_generate_collision", False)

    task.set_editor_property("factory", unreal.FbxFactory())
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("FBX did not produce {}".format(ASSET_PATH))
    return mesh, list(task.get_editor_property("imported_object_paths"))


def assign_materials(mesh):
    assignments = []
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        material_path = SHARED_MATERIALS.get(slot_name)
        if not material_path:
            fail("unexpected material slot {}".format(slot_name))
        material = unreal.EditorAssetLibrary.load_asset(material_path)
        if not material:
            fail("missing material {} for slot {}".format(material_path, slot_name))
        mesh.set_material(index, material)
        assignments.append({"slot": slot_name, "material": material_path})

    if len(assignments) != len(SHARED_MATERIALS):
        fail("expected 6 material slots, found {}".format(len(assignments)))
    if not unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False):
        fail("could not save {}".format(ASSET_PATH))

    skin = unreal.EditorAssetLibrary.load_asset(SKIN_MATERIAL_PATH)
    if not skin or not isinstance(skin, unreal.Material):
        fail("import did not produce the project-owned armsmaster skin material")
    if not unreal.EditorAssetLibrary.save_loaded_asset(skin, only_if_is_dirty=False):
        fail("could not save {}".format(SKIN_MATERIAL_PATH))
    return assignments


def simple_collision_count(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0
    aggregate = body_setup.get_editor_property("agg_geom")
    return sum(
        len(aggregate.get_editor_property(property_name))
        for property_name in ("box_elems", "sphere_elems", "sphyl_elems", "convex_elems")
    )


def validate_mesh(mesh):
    import_data = mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_data_class))

    collision_count = simple_collision_count(mesh)
    if collision_count != 0:
        fail("armsmaster must remain non-colliding, found {} shapes".format(collision_count))

    bounds = mesh.get_bounds()
    size = bounds.box_extent * 2.0
    if not all((
        abs(float(size.x) - 154.5) <= 1.0,
        abs(float(size.y) - 87.0) <= 1.0,
        abs(float(size.z) - 228.0) <= 1.0,
        abs(float(bounds.origin.z) - float(bounds.box_extent.z)) <= 1.0,
    )):
        fail("armsmaster bounds drifted: origin={}, extent={}".format(
            bounds.origin,
            bounds.box_extent,
        ))

    triangle_count = mesh.get_num_triangles(0)
    if triangle_count != 2800:
        fail("expected 2800 imported triangles, found {}".format(triangle_count))
    return collision_count, size, triangle_count


def place_armsmaster(mesh):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        if actor_label(actor) == PLACEMENT_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(PLACEMENT_LABEL)

    actor = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermereNpcPresentationActor,
        unreal.Vector(*PLACEMENT_LOCATION),
        make_rotation(PLACEMENT_YAW),
        False,
    )
    if not actor:
        fail("could not spawn {}".format(PLACEMENT_LABEL))

    actor.set_actor_label(PLACEMENT_LABEL)
    actor.set_folder_path(unreal.Name(PLACEMENT_FOLDER))
    actor.set_editor_property("static_visual_mesh", mesh)
    actor.set_editor_property("skeletal_visual_mesh", None)
    actor.set_editor_property("animation_class", None)
    actor.set_editor_property("prefer_skeletal_visual", False)
    actor.set_editor_property("visual_relative_transform", unreal.Transform())
    actor.refresh_presentation()

    tags = list(actor.get_editor_property("tags"))
    if ORIGINAL_TAG not in tags:
        tags.append(ORIGINAL_TAG)
        actor.set_editor_property("tags", tags)

    if actor.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.STATIC_MESH:
        fail("armsmaster did not resolve to the static presentation lane")
    if not actor.is_presentation_collision_disabled():
        fail("armsmaster presentation collision is enabled")
    if actor.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation actor must not own interaction behavior")
    if actor.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("presentation actor must not own trainer behavior")

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return removed, actor_label(actor), [str(tag) for tag in actor.tags]


def main():
    mesh, imported_paths = import_static_mesh()
    assignments = assign_materials(mesh)
    collision_count, size, triangle_count = validate_mesh(mesh)
    removed, created, tags = place_armsmaster(mesh)
    unreal.log("Embermere Fenwatch armsmaster import paths: {}".format(imported_paths))
    unreal.log("Embermere Fenwatch armsmaster material assignments: {}".format(assignments))
    unreal.log(
        "Embermere Fenwatch armsmaster integration passed: triangles={}, dimensions={}, collision={}, removed={}, created={}, tags={}".format(
            triangle_count,
            size,
            collision_count,
            removed,
            created,
            tags,
        )
    )


if __name__ == "__main__":
    main()
