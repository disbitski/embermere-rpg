"""Validate the saved Fenwatch communal-well asset and map integration."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ASSET_PATH = (
    "/Game/Art/Embermere/Environment/PrototypeVillage/"
    "SM_EmbermereFenwatchCommunalWell_01"
)
PLACEMENT_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
MARA_LABEL = "Quest_Giver_Mara_Fenwatch"
QUARTERMASTER_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
ARMSMASTER_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
PLAYER_START_LABEL = "PlayerStart_Embermere_Village"
PRACTICE_DUMMY_LABEL = "Embermere_FenwatchPracticeDummy_TrainingYard_01"
TRAINING_WORKSHOP_LABEL = "Embermere_FenwatchTrainingWorkshop_Armsmaster_01"
VENDOR_STALL_LABEL = "Embermere_FenwatchVendorStall_Quartermaster_01"
EXPECTED_LOCATION = unreal.Vector(-950.0, -1600.0, 0.0)
EXPECTED_YAW = -135.0
EXPECTED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch communal well validation failed: {}".format(message)
    )
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


def horizontal_distance(left, right):
    return math.hypot(left.x - right.x, left.y - right.y)


def main():
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("saved static mesh is missing")
    if mesh.get_num_triangles(0) != 6760:
        fail("triangle count drifted: {}".format(mesh.get_num_triangles(0)))
    if collision_box_count(mesh) != 6:
        fail("expected six authored curb/upright collision boxes")

    bounds = mesh.get_bounds()
    full_dimensions = bounds.box_extent * 2.0
    expected_dimensions = (348.0, 220.0, 322.0)
    actual_dimensions = (
        float(full_dimensions.x),
        float(full_dimensions.y),
        float(full_dimensions.z),
    )
    if any(
        abs(actual_dimensions[index] - expected_dimensions[index]) > 1.0
        for index in range(3)
    ):
        fail("mesh dimensions drifted: {}".format(actual_dimensions))
    if abs(float(bounds.origin.z) - 161.0) > 1.0:
        fail("mesh is no longer grounded at local Z zero")

    import_data = mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_data_class))

    assignments = {}
    for index, static_material in enumerate(
        list(mesh.get_editor_property("static_materials"))
    ):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        material = mesh.get_material(index)
        assignments[slot_name] = material.get_path_name() if material else "None"
    if assignments != EXPECTED_MATERIALS:
        fail("shared material assignments drifted: {}".format(assignments))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    placements = []
    context = {}
    required_context = {
        MARA_LABEL,
        QUARTERMASTER_LABEL,
        ARMSMASTER_LABEL,
        PLAYER_START_LABEL,
        PRACTICE_DUMMY_LABEL,
        TRAINING_WORKSHOP_LABEL,
        VENDOR_STALL_LABEL,
    }
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == PLACEMENT_LABEL:
            placements.append(actor)
        elif label in required_context:
            context[label] = actor
    if len(placements) != 1:
        fail("expected one saved communal well, found {}".format(len(placements)))
    for required in required_context:
        if required not in context:
            fail("missing village context actor {}".format(required))

    actor = placements[0]
    if not isinstance(actor, unreal.StaticMeshActor):
        fail("saved placement is not a StaticMeshActor")
    if actor.get_class().get_name() != "StaticMeshActor":
        fail("saved placement must use the art-only StaticMeshActor class")
    if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("placement location drifted: {}".format(actor.get_actor_location()))
    yaw_delta = abs(
        (float(actor.get_actor_rotation().yaw) - EXPECTED_YAW + 180.0) % 360.0
        - 180.0
    )
    if yaw_delta > 0.1:
        fail("placement yaw drifted: {}".format(actor.get_actor_rotation().yaw))
    scale = actor.get_actor_scale3d()
    if (scale - unreal.Vector(1.0, 1.0, 1.0)).length() > 0.001:
        fail("placement scale drifted: {}".format(scale))
    if unreal.Name("EmbermereOriginalArt") not in list(
        actor.get_editor_property("tags")
    ):
        fail("placement lost the EmbermereOriginalArt tag")

    component = actor.get_editor_property("static_mesh_component")
    if component.get_editor_property("static_mesh") != mesh:
        fail("placement references the wrong static mesh")
    if str(component.get_collision_profile_name()) != "BlockAll":
        fail(
            "placement collision profile drifted: {}".format(
                component.get_collision_profile_name()
            )
        )

    distances = {
        label: horizontal_distance(EXPECTED_LOCATION, context[label].get_actor_location())
        for label in required_context
    }
    minimums = {
        MARA_LABEL: 1250.0,
        QUARTERMASTER_LABEL: 675.0,
        ARMSMASTER_LABEL: 725.0,
        PLAYER_START_LABEL: 1450.0,
        PRACTICE_DUMMY_LABEL: 475.0,
        TRAINING_WORKSHOP_LABEL: 575.0,
        VENDOR_STALL_LABEL: 575.0,
    }
    for label, minimum in minimums.items():
        if distances[label] < minimum:
            fail(
                "communal well moved too close to {}: {:.1f} cm".format(
                    label,
                    distances[label],
                )
            )

    unreal.log(
        "Embermere Fenwatch communal well validation passed: 348 x 220 x 322 cm, 6760 triangles, five shared materials, six authored curb/upright collision boxes, one grounded project-owned south-commons placement, presentation-only static art, and clear Mara, quartermaster, armsmaster, PlayerStart, practice-dummy, workshop, and vendor-stall spacing"
    )


if __name__ == "__main__":
    main()
