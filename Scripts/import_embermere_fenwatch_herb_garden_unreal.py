"""Import Embermere's project-owned Fenwatch herb garden."""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_FBX = (
    PROJECT_ROOT
    / "ArtSource/Blender/Environment/FenwatchHerbGarden/"
    "SM_EmbermereFenwatchHerbGarden_01.fbx"
)
DESTINATION_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage"
ASSET_NAME = "SM_EmbermereFenwatchHerbGarden_01"
ASSET_PATH = "{}/{}".format(DESTINATION_PATH, ASSET_NAME)
SHARED_MATERIALS = {
    "M_Waystone": DESTINATION_PATH + "/M_Waystone",
    "M_WaystoneMoss": DESTINATION_PATH + "/M_WaystoneMoss",
    "M_WaystoneEmber": DESTINATION_PATH + "/M_WaystoneEmber",
    "M_EmberLampIron": DESTINATION_PATH + "/M_EmberLampIron",
    "M_EmbermereTimber": DESTINATION_PATH + "/M_EmbermereTimber",
}
COLLISION_MATERIAL = DESTINATION_PATH + "/M_CollisionPreview"


def fail(message):
    unreal.log_error("Embermere Fenwatch herb-garden import failed: {}".format(message))
    sys.exit(1)


def collision_box_count(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0
    aggregate = body_setup.get_editor_property("agg_geom")
    return len(aggregate.get_editor_property("box_elems"))


def main():
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

    if unreal.EditorAssetLibrary.does_asset_exist(COLLISION_MATERIAL):
        if not unreal.EditorAssetLibrary.delete_asset(COLLISION_MATERIAL):
            fail("could not remove orphan collision material")

    assignments = []
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        shared_path = SHARED_MATERIALS.get(slot_name)
        if not shared_path:
            fail("unexpected material slot {}".format(slot_name))
        shared = unreal.EditorAssetLibrary.load_asset(shared_path)
        if not shared:
            fail("missing shared material {}".format(shared_path))
        mesh.set_material(index, shared)
        assignments.append({"slot": slot_name, "material": shared_path})
    if len(assignments) != 5:
        fail("expected five shared material slots, found {}".format(len(assignments)))
    if not unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False):
        fail("could not save {}".format(ASSET_PATH))

    import_data = mesh.get_editor_property("asset_import_data")
    import_class = import_data.get_class().get_name() if import_data else "None"
    if import_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_class))
    if mesh.get_num_triangles(0) != 4312:
        fail("expected 4312 triangles, found {}".format(mesh.get_num_triangles(0)))
    if collision_box_count(mesh) != 4:
        fail("expected four authored collision boxes, found {}".format(collision_box_count(mesh)))
    bounds = mesh.get_bounds()
    dimensions = bounds.box_extent * 2.0
    expected = (356.0, 212.0, 195.0)
    actual = (float(dimensions.x), float(dimensions.y), float(dimensions.z))
    if any(abs(actual[index] - expected[index]) > 1.0 for index in range(3)):
        fail("unexpected imported dimensions {}".format(actual))
    if abs(float(bounds.origin.z) - 97.5) > 1.0:
        fail("unexpected imported Z origin {}".format(bounds.origin.z))

    unreal.log("Embermere Fenwatch herb-garden material assignments: {}".format(assignments))
    unreal.log(
        "EMBERMERE_FENWATCH_HERB_GARDEN_IMPORT_SUCCESS: "
        "4312 triangles, four collision boxes, five shared materials"
    )


if __name__ == "__main__":
    main()
