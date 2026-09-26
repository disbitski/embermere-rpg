"""Import the project-owned ancient ruin relief through classic FBX."""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
ASSET_NAME = "SM_EmbermereAncientRuinRelief_01"
SOURCE_FBX = PROJECT_ROOT / "ArtSource/Blender/Environment/AncientRuinRelief" / (ASSET_NAME + ".fbx")
DESTINATION_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage"
ASSET_PATH = DESTINATION_PATH + "/" + ASSET_NAME
SHARED_MATERIALS = {
    "M_Waystone": DESTINATION_PATH + "/M_Waystone",
    "M_WaystoneMoss": DESTINATION_PATH + "/M_WaystoneMoss",
    "M_EmberLampIron": DESTINATION_PATH + "/M_EmberLampIron",
    "M_WaystoneEmber": DESTINATION_PATH + "/M_WaystoneEmber",
}


def fail(message):
    unreal.log_error("Embermere ancient ruin relief import failed: {}".format(message))
    sys.exit(1)


def collision_count(mesh):
    body = mesh.get_editor_property("body_setup")
    if not body:
        return 0
    aggregate = body.get_editor_property("agg_geom")
    return sum(len(aggregate.get_editor_property(name)) for name in (
        "box_elems", "sphere_elems", "sphyl_elems", "convex_elems"
    ))


def main():
    if not SOURCE_FBX.is_file():
        fail("missing source FBX {}".format(SOURCE_FBX))
    if unreal.EditorAssetLibrary.does_asset_exist(ASSET_PATH):
        fail("refusing to overwrite saved mesh {}; review a separate reimport".format(ASSET_PATH))

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
    assignments = {}
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot = str(static_material.get_editor_property("material_slot_name"))
        path = SHARED_MATERIALS.get(slot)
        if not path:
            fail("unexpected material slot {}".format(slot))
        shared = unreal.EditorAssetLibrary.load_asset(path)
        if not shared:
            fail("missing shared material {}".format(path))
        mesh.set_material(index, shared)
        assignments[slot] = path
    if set(assignments) != set(SHARED_MATERIALS):
        fail("material slot contract drifted: {}".format(assignments))
    if not unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False):
        fail("could not save {}".format(ASSET_PATH))

    import_data = mesh.get_editor_property("asset_import_data")
    if not import_data or import_data.get_class().get_name() != "FbxStaticMeshImportData":
        fail("asset is not classic-FBX imported")
    if mesh.get_num_triangles(0) != 2900 or collision_count(mesh) != 0:
        fail("unexpected mesh triangles/collision: {}/{}".format(mesh.get_num_triangles(0), collision_count(mesh)))
    bounds = mesh.get_bounds()
    size = bounds.box_extent * 2.0
    actual = (float(size.x), float(size.y), float(size.z))
    if any(abs(actual[i] - expected) > 1.0 for i, expected in enumerate((206.0, 55.805, 216.0))):
        fail("mesh bounds drifted: {}".format(actual))
    unreal.log("EMBERMERE_ANCIENT_RUIN_RELIEF_IMPORT_SUCCESS: 2900 triangles, four shared materials, zero authored collision, classic FBX")


if __name__ == "__main__":
    main()
