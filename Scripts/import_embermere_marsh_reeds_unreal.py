"""Import and place Embermere's visual-only marsh reed cluster."""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_FBX = (
    PROJECT_ROOT
    / "ArtSource/Blender/Environment/MarshReeds"
    / "SM_EmbermereMarshReedCluster_01.fbx"
)
DESTINATION_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage"
ASSET_NAME = "SM_EmbermereMarshReedCluster_01"
ASSET_PATH = "{}/{}".format(DESTINATION_PATH, ASSET_NAME)
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ORIGINAL_TAG = unreal.Name("EmbermereOriginalArt")
GROUND_DRESSING_TAG = unreal.Name("EmbermereGroundDressing")
PLACEMENT_FOLDER = "04_Fab_Zone_Pass/03_Wilderness/Embermere_Originals"

PLACEMENTS = {
    "Embermere_MarshReeds_RoadSouth_01": ((1300.0, -180.0, 0.0), 15.0, 0.78),
    "Embermere_MarshReeds_RoadNorth_01": ((1020.0, 1650.0, 0.0), 75.0, 0.72),
    "Embermere_MarshReeds_WildSouth_01": ((2860.0, 80.0, 0.0), -30.0, 0.85),
    "Embermere_MarshReeds_WildNorth_01": ((3400.0, 1700.0, 0.0), 45.0, 0.80),
}

SHARED_MATERIALS = {
    "M_EmbermereGround": "/Game/Art/Embermere/Environment/M_EmbermereGround",
    "M_Waystone": DESTINATION_PATH + "/M_Waystone",
    "M_WaystoneMoss": DESTINATION_PATH + "/M_WaystoneMoss",
    "M_EmbermereTimber": DESTINATION_PATH + "/M_EmbermereTimber",
}


def fail(message):
    unreal.log_error("Embermere marsh reed integration failed: {}".format(message))
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


def reuse_shared_materials(mesh):
    assignments = []
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        shared_path = SHARED_MATERIALS.get(slot_name)
        if not shared_path:
            fail("unexpected material slot {}".format(slot_name))
        material = unreal.EditorAssetLibrary.load_asset(shared_path)
        if not material:
            fail("missing shared material {}".format(shared_path))
        mesh.set_material(index, material)
        assignments.append({"slot": slot_name, "material": shared_path})
    if len(assignments) != len(SHARED_MATERIALS):
        fail("expected 4 shared material slots, found {}".format(len(assignments)))
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
    return assignments


def collision_shape_count(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0
    aggregate = body_setup.get_editor_property("agg_geom")
    return sum(
        len(aggregate.get_editor_property(property_name))
        for property_name in (
            "box_elems",
            "sphere_elems",
            "sphyl_elems",
            "convex_elems",
            "tapered_capsule_elems",
        )
    )


def place_reeds(mesh):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for actor in list(actor_subsystem.get_all_level_actors()):
        if actor_label(actor) in PLACEMENTS:
            actor_subsystem.destroy_actor(actor)

    created = []
    for label, (location, yaw, scale) in PLACEMENTS.items():
        actor = actor_subsystem.spawn_actor_from_object(
            mesh,
            unreal.Vector(*location),
            make_rotation(yaw),
            False,
        )
        if not actor:
            fail("could not spawn {}".format(label))
        actor.set_actor_label(label)
        actor.set_folder_path(unreal.Name(PLACEMENT_FOLDER))
        actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
        tags = list(actor.get_editor_property("tags"))
        for tag in (ORIGINAL_TAG, GROUND_DRESSING_TAG):
            if tag not in tags:
                tags.append(tag)
        actor.set_editor_property("tags", tags)

        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        if not component:
            fail("{} has no static mesh component".format(label))
        component.modify()
        component.set_collision_profile_name(unreal.Name("NoCollision"))
        component.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
        created.append(label)

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return sorted(created)


def main():
    mesh, imported_paths = import_static_mesh()
    assignments = reuse_shared_materials(mesh)

    import_data = mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_data_class))

    shape_count = collision_shape_count(mesh)
    if shape_count != 0:
        fail("visual-only reeds must import with zero collision shapes, found {}".format(
            shape_count,
        ))

    bounds = mesh.get_bounds()
    if abs(float(bounds.box_extent.z) - 94.9) > 2.0:
        fail("unexpected imported Z bounds origin={} extent={}".format(
            bounds.origin.z,
            bounds.box_extent.z,
        ))

    created = place_reeds(mesh)
    unreal.log("Embermere marsh reed import paths: {}".format(imported_paths))
    unreal.log("Embermere marsh reed material assignments: {}".format(assignments))
    unreal.log(
        "Embermere marsh reed integration passed: triangles={}, collision shapes={}, created={}".format(
            mesh.get_num_triangles(0),
            shape_count,
            created,
        )
    )


if __name__ == "__main__":
    main()
