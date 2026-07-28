"""Import and place Embermere's original road signpost in the starter map."""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_FBX = PROJECT_ROOT / "ArtSource/Blender/Environment/RoadSignpost/SM_EmbermereRoadSignpost_01.fbx"
DESTINATION_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage"
ASSET_NAME = "SM_EmbermereRoadSignpost_01"
ASSET_PATH = "{}/{}".format(DESTINATION_PATH, ASSET_NAME)
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ORIGINAL_TAG = unreal.Name("EmbermereOriginalArt")
PLACEMENT_LABEL = "Embermere_RoadSignpost_01"
PLACEMENT_LOCATION = (20.0, -170.0, 0.0)
PLACEMENT_YAW = 22.0
PLACEMENT_FOLDER = "04_Fab_Zone_Pass/02_Road/Embermere_Originals"

SHARED_MATERIALS = {
    "M_Waystone": DESTINATION_PATH + "/M_Waystone",
    "M_WaystoneMoss": DESTINATION_PATH + "/M_WaystoneMoss",
    "M_WaystoneEmber": DESTINATION_PATH + "/M_WaystoneEmber",
    "M_EmberLampIron": DESTINATION_PATH + "/M_EmberLampIron",
}
TIMBER_MATERIAL_PATH = DESTINATION_PATH + "/M_EmbermereTimber"
INTERCHANGE_COLLISION_MATERIAL = DESTINATION_PATH + "/M_CollisionPreview"


def fail(message):
    unreal.log_error("Embermere road signpost integration failed: {}".format(message))
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

    # Recreate the mesh package so a failed generic Interchange attempt cannot
    # leave stale import provenance or silently discard authored UBX collision.
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

    # UE 5.8's generic Interchange path does not preserve reviewed UBX boxes.
    task.set_editor_property("factory", unreal.FbxFactory())
    task.set_editor_property("options", options)

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("FBX did not produce {}".format(ASSET_PATH))

    if unreal.EditorAssetLibrary.does_asset_exist(INTERCHANGE_COLLISION_MATERIAL):
        if not unreal.EditorAssetLibrary.delete_asset(INTERCHANGE_COLLISION_MATERIAL):
            fail("could not remove orphan {}".format(INTERCHANGE_COLLISION_MATERIAL))
    return mesh, list(task.get_editor_property("imported_object_paths"))


def reuse_shared_materials(mesh):
    assignments = []
    static_materials = list(mesh.get_editor_property("static_materials"))
    for index, static_material in enumerate(static_materials):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        shared_path = SHARED_MATERIALS.get(slot_name)
        if not shared_path:
            assignments.append({"slot": slot_name, "material": "imported"})
            continue
        material = unreal.EditorAssetLibrary.load_asset(shared_path)
        if not material:
            fail("missing shared material {}".format(shared_path))
        mesh.set_material(index, material)
        assignments.append({"slot": slot_name, "material": shared_path})
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
    return assignments


def save_imported_timber_material():
    material = unreal.EditorAssetLibrary.load_asset(TIMBER_MATERIAL_PATH)
    if not material or not isinstance(material, unreal.MaterialInterface):
        fail("missing imported timber material {}".format(TIMBER_MATERIAL_PATH))
    if not unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False):
        fail("could not save imported timber material {}".format(TIMBER_MATERIAL_PATH))
    return material


def collision_box_count(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0
    aggregate = body_setup.get_editor_property("agg_geom")
    return len(aggregate.get_editor_property("box_elems"))


def place_signpost(mesh):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        if actor_label(actor) == PLACEMENT_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(PLACEMENT_LABEL)

    actor = actor_subsystem.spawn_actor_from_object(
        mesh,
        unreal.Vector(*PLACEMENT_LOCATION),
        make_rotation(PLACEMENT_YAW),
        False,
    )
    if not actor:
        fail("could not spawn {}".format(PLACEMENT_LABEL))
    actor.set_actor_label(PLACEMENT_LABEL)
    actor.set_folder_path(unreal.Name(PLACEMENT_FOLDER))
    tags = list(actor.get_editor_property("tags"))
    if ORIGINAL_TAG not in tags:
        tags.append(ORIGINAL_TAG)
        actor.set_editor_property("tags", tags)

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return removed, actor_label(actor)


def main():
    mesh, imported_paths = import_static_mesh()
    assignments = reuse_shared_materials(mesh)
    save_imported_timber_material()
    import_data = mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_data_class))

    box_count = collision_box_count(mesh)
    if box_count != 2:
        fail("expected 2 imported collision boxes, found {}".format(box_count))

    bounds = mesh.get_bounds()
    if abs(float(bounds.origin.z) - 132.0) > 2.0 or abs(float(bounds.box_extent.z) - 132.0) > 2.0:
        fail("unexpected imported Z bounds origin={} extent={}".format(bounds.origin.z, bounds.box_extent.z))
    if not unreal.EditorAssetLibrary.does_asset_exist(TIMBER_MATERIAL_PATH):
        fail("missing saved timber material {}".format(TIMBER_MATERIAL_PATH))

    removed, created = place_signpost(mesh)
    unreal.log("Embermere road signpost import paths: {}".format(imported_paths))
    unreal.log("Embermere road signpost material assignments: {}".format(assignments))
    unreal.log(
        "Embermere road signpost integration passed: triangles={}, collision boxes={}, removed={}, created={}".format(
            mesh.get_num_triangles(0),
            box_count,
            removed,
            created,
        )
    )


if __name__ == "__main__":
    main()
