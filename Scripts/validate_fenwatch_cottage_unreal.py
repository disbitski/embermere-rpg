"""Validate the saved Fenwatch cottage asset and map integration."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ASSET_PATH = (
    "/Game/Art/Embermere/Environment/PrototypeVillage/"
    "SM_EmbermereFenwatchCottage_01"
)
PLACEMENT_LABEL = "Embermere_FenwatchCottage_West_01"
REPLACED_FAB_LABEL = "FabPass_Village_Fence_02"
MARA_LABEL = "Quest_Giver_Mara_Fenwatch"
PLAYER_START_LABEL = "PlayerStart_Embermere_Village"
EXPECTED_LOCATION = unreal.Vector(-2480.0, -260.0, 0.0)
EXPECTED_YAW = 38.0
EXPECTED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}


def fail(message):
    unreal.log_error("Embermere Fenwatch cottage validation failed: {}".format(message))
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


def distance_to_segment_2d(point, start, end):
    segment_x = end[0] - start[0]
    segment_y = end[1] - start[1]
    length_squared = segment_x * segment_x + segment_y * segment_y
    projection = (
        (point[0] - start[0]) * segment_x
        + (point[1] - start[1]) * segment_y
    ) / length_squared
    projection = max(0.0, min(1.0, projection))
    closest = (
        start[0] + projection * segment_x,
        start[1] + projection * segment_y,
    )
    return math.hypot(point[0] - closest[0], point[1] - closest[1])


def main():
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("saved static mesh is missing")
    if mesh.get_num_triangles(0) != 6616:
        fail("triangle count drifted: {}".format(mesh.get_num_triangles(0)))
    if collision_box_count(mesh) != 2:
        fail("expected two authored collision boxes")

    bounds = mesh.get_bounds()
    full_dimensions = bounds.box_extent * 2.0
    expected_dimensions = (580.0, 422.0, 503.0)
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
    if abs(float(bounds.origin.z) - 251.5) > 1.0:
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
    context = {}
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == PLACEMENT_LABEL:
            placements.append(actor)
        elif label == REPLACED_FAB_LABEL:
            replaced.append(actor)
        elif label in {MARA_LABEL, PLAYER_START_LABEL}:
            context[label] = actor
    if len(placements) != 1:
        fail("expected one saved cottage, found {}".format(len(placements)))
    if replaced:
        fail("replaced generic Fab fence remains in the map")
    for required in {MARA_LABEL, PLAYER_START_LABEL}:
        if required not in context:
            fail("missing village context actor {}".format(required))

    actor = placements[0]
    if not isinstance(actor, unreal.StaticMeshActor):
        fail("saved placement is not a StaticMeshActor")
    if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("placement location drifted: {}".format(actor.get_actor_location()))
    yaw_delta = abs((float(actor.get_actor_rotation().yaw) - EXPECTED_YAW + 180.0) % 360.0 - 180.0)
    if yaw_delta > 0.1:
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

    spawn = context[PLAYER_START_LABEL].get_actor_location()
    mara = context[MARA_LABEL].get_actor_location()
    route_clearance = distance_to_segment_2d(
        (EXPECTED_LOCATION.x, EXPECTED_LOCATION.y),
        (spawn.x, spawn.y),
        (mara.x, mara.y),
    )
    if route_clearance < 650.0:
        fail("cottage encroaches on the PlayerStart-to-Mara route: {:.1f} cm".format(route_clearance))
    mara_distance = math.hypot(
        EXPECTED_LOCATION.x - mara.x,
        EXPECTED_LOCATION.y - mara.y,
    )
    if mara_distance < 700.0:
        fail("cottage moved too close to Mara's marker/greeting lane: {:.1f} cm".format(mara_distance))

    unreal.log(
        "Embermere Fenwatch cottage validation passed: 580 x 422 x 503 cm, 6616 triangles, five shared materials, two authored body/step collision boxes, one grounded west-village placement, removed generic fence, {:.1f} cm spawn-route clearance, and {:.1f} cm Mara separation".format(
            route_clearance,
            mara_distance,
        )
    )


if __name__ == "__main__":
    main()
