"""Import Mara's Fenwatch keeper art and apply it without changing quest logic."""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_FBX = (
    PROJECT_ROOT
    / "ArtSource/Blender/Characters/NPCs/FenwatchKeeper/SM_EmbermereFenwatchKeeper_Mara_01.fbx"
)
DESTINATION_PATH = "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper"
ASSET_NAME = "SM_EmbermereFenwatchKeeper_Mara_01"
ASSET_PATH = "{}/{}".format(DESTINATION_PATH, ASSET_NAME)
SKIN_MATERIAL_PATH = DESTINATION_PATH + "/M_FenwatchKeeperSkin"
BLUEPRINT_PATH = "/Game/Blueprints/BP_QuestGiver"
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
MARA_LABEL = "Quest_Giver_Mara_Fenwatch"
ORIGINAL_TAG = unreal.Name("EmbermereOriginalArt")

SHARED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber",
    "M_FenwatchKeeperSkin": SKIN_MATERIAL_PATH,
}


def fail(message):
    unreal.log_error("Embermere Fenwatch keeper integration failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def relative_rotation():
    rotation = unreal.Rotator()
    rotation.pitch = 0.0
    rotation.yaw = 100.0
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

    # Use the classic importer so asset behavior stays aligned with the reviewed
    # Embermere Blender lane. This mesh intentionally authors no collision.
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
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
    skin = unreal.EditorAssetLibrary.load_asset(SKIN_MATERIAL_PATH)
    if not skin or not isinstance(skin, unreal.Material):
        fail("import did not produce the project-owned skin material")
    unreal.EditorAssetLibrary.save_loaded_asset(skin, only_if_is_dirty=False)
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
        fail("keeper art must remain non-colliding, found {} shapes".format(collision_count))

    bounds = mesh.get_bounds()
    size = bounds.box_extent * 2.0
    if not (90.0 <= float(size.x) <= 125.0):
        fail("unexpected imported width {}".format(size.x))
    if not (45.0 <= float(size.y) <= 90.0):
        fail("unexpected imported depth {}".format(size.y))
    if not (190.0 <= float(size.z) <= 215.0):
        fail("unexpected imported height {}".format(size.z))
    if abs(float(bounds.origin.z) - float(bounds.box_extent.z)) > 1.0:
        fail("mesh pivot is not at ground level: origin={} extent={}".format(
            bounds.origin.z,
            bounds.box_extent.z,
        ))

    triangle_count = mesh.get_num_triangles(0)
    if not (1000 <= triangle_count <= 8000):
        fail("unexpected imported triangle count {}".format(triangle_count))
    return collision_count, size, triangle_count


def configure_component(component, mesh):
    component.modify()
    component.set_static_mesh(mesh)
    # The legacy quest-giver actor origin sits 140 cm above the terrain so the
    # old cube marker was centered correctly. Offset only the visual, keeping
    # interaction and quest logic at their established world transform.
    component.set_editor_property("relative_location", unreal.Vector(0.0, 0.0, -140.0))
    component.set_editor_property("relative_rotation", relative_rotation())
    component.set_relative_scale3d(unreal.Vector(1.0, 1.0, 1.0))
    component.set_collision_profile_name(unreal.Name("NoCollision"))
    component.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
    body_instance = component.get_editor_property("body_instance")
    body_instance.set_editor_property("collision_profile_name", unreal.Name("NoCollision"))
    body_instance.set_editor_property("collision_enabled", unreal.CollisionEnabled.NO_COLLISION)
    component.set_editor_property("body_instance", body_instance)
    component.set_editor_property("cast_shadow", True)
    component.set_editor_property("can_ever_affect_navigation", False)


def configure_blueprint(mesh):
    blueprint = unreal.EditorAssetLibrary.load_asset(BLUEPRINT_PATH)
    if not blueprint or not isinstance(blueprint, unreal.Blueprint):
        fail("missing quest-giver Blueprint {}".format(BLUEPRINT_PATH))
    subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    component = None
    for handle in subsystem.k2_gather_subobject_data_for_blueprint(blueprint):
        data = subsystem.k2_find_subobject_data_from_handle(handle)
        candidate = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
        if isinstance(candidate, unreal.StaticMeshComponent):
            component = candidate
            break
    if not component:
        fail("quest-giver Blueprint has no static mesh component template")
    configure_component(component, mesh)
    unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
    if not unreal.EditorAssetLibrary.save_loaded_asset(blueprint, only_if_is_dirty=False):
        fail("could not save {}".format(BLUEPRINT_PATH))
    return component.get_name()


def configure_saved_mara(mesh):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    mara = None
    for actor in actor_subsystem.get_all_level_actors():
        if actor_label(actor) == MARA_LABEL:
            mara = actor
            break
    if not mara:
        fail("could not find {}".format(MARA_LABEL))

    component = mara.get_component_by_class(unreal.StaticMeshComponent)
    if not component:
        fail("saved Mara actor has no static mesh component")
    mara.modify()
    configure_component(component, mesh)
    tags = list(mara.get_editor_property("tags"))
    if ORIGINAL_TAG not in tags:
        tags.append(ORIGINAL_TAG)
        mara.set_editor_property("tags", tags)

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return actor_label(mara), [str(tag) for tag in mara.get_editor_property("tags")]


def main():
    mesh, imported_paths = import_static_mesh()
    assignments = assign_materials(mesh)
    collision_count, size, triangle_count = validate_mesh(mesh)
    blueprint_component = configure_blueprint(mesh)
    saved_actor, tags = configure_saved_mara(mesh)
    unreal.log("Embermere Fenwatch keeper import paths: {}".format(imported_paths))
    unreal.log("Embermere Fenwatch keeper material assignments: {}".format(assignments))
    unreal.log(
        "Embermere Fenwatch keeper integration passed: triangles={}, dimensions={}, collision={}, blueprint_component={}, saved_actor={}, tags={}".format(
            triangle_count,
            size,
            collision_count,
            blueprint_component,
            saved_actor,
            tags,
        )
    )


if __name__ == "__main__":
    main()
