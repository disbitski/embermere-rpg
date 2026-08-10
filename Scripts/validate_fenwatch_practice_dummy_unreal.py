"""Validate the saved Fenwatch practice-dummy asset and map integration."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ASSET_PATH = (
    "/Game/Art/Embermere/Environment/PrototypeVillage/"
    "SM_EmbermereFenwatchPracticeDummy_01"
)
PLACEMENT_LABEL = "Embermere_FenwatchPracticeDummy_TrainingYard_01"
REPLACED_FAB_LABEL = "FabPass_Village_Crate_C"
EXPECTED_LOCATION = unreal.Vector(-1120.0, -1120.0, 0.0)
EXPECTED_YAW = 45.0
EXPECTED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}


def fail(message):
    unreal.log_error("Embermere Fenwatch practice dummy validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def collision_box_count(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0
    aggregate = body_setup.get_editor_property("agg_geom")
    return len(aggregate.get_editor_property("box_elems"))


def main():
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("saved static mesh is missing")
    if mesh.get_num_triangles(0) != 2572:
        fail("triangle count drifted: {}".format(mesh.get_num_triangles(0)))
    if collision_box_count(mesh) != 2:
        fail("expected two authored collision boxes")

    bounds = mesh.get_bounds()
    full_dimensions = bounds.box_extent * 2.0
    if abs(float(full_dimensions.x) - 252.0) > 1.0:
        fail("mesh width drifted: {}".format(full_dimensions.x))
    if abs(float(full_dimensions.y) - 100.879) > 1.0:
        fail("mesh depth drifted: {}".format(full_dimensions.y))
    if abs(float(full_dimensions.z) - 245.0) > 1.0:
        fail("mesh height drifted: {}".format(full_dimensions.z))
    if abs(float(bounds.origin.z) - 122.5) > 1.0:
        fail("mesh is no longer grounded at local Z zero")

    import_data = mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_data_class))

    assignments = {}
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        material = mesh.get_material(index)
        assignments[slot_name] = material.get_path_name() if material else "None"
    if assignments != EXPECTED_MATERIALS:
        fail("shared material assignments drifted: {}".format(assignments))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    placements = []
    replaced = []
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == PLACEMENT_LABEL:
            placements.append(actor)
        elif label == REPLACED_FAB_LABEL:
            replaced.append(actor)
    if len(placements) != 1:
        fail("expected one saved practice dummy, found {}".format(len(placements)))
    if replaced:
        fail("replaced generic crate remains in the map")

    actor = placements[0]
    if not isinstance(actor, unreal.StaticMeshActor):
        fail("saved placement is not a StaticMeshActor")
    if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("placement location drifted: {}".format(actor.get_actor_location()))
    if abs(float(actor.get_actor_rotation().yaw) - EXPECTED_YAW) > 0.1:
        fail("placement yaw drifted: {}".format(actor.get_actor_rotation().yaw))
    scale = actor.get_actor_scale3d()
    if (scale - unreal.Vector(1.0, 1.0, 1.0)).length() > 0.001:
        fail("placement scale drifted: {}".format(scale))
    if unreal.Name("EmbermereOriginalArt") not in list(actor.get_editor_property("tags")):
        fail("placement lost the EmbermereOriginalArt tag")

    component = actor.get_editor_property("static_mesh_component")
    if component.get_editor_property("static_mesh") != mesh:
        fail("placement references the wrong static mesh")
    if str(component.get_collision_profile_name()) != "BlockAll":
        fail("placement collision profile drifted: {}".format(component.get_collision_profile_name()))

    unreal.log(
        "Embermere Fenwatch practice dummy validation passed: 252 x 100.879 x 245 cm, 2572 triangles, five shared materials, two authored collision boxes, solid base/core, visual-only arms, and one grounded training-yard placement replacing the generic crate"
    )


if __name__ == "__main__":
    main()
