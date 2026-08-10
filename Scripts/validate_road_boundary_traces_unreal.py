"""Validate traversable routes and authored original-art collision."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
GATE_CENTER = (1080.0, 540.0)
GATE_YAW = 20.0
FENCE_CENTERS = {
    "Embermere_BoundaryFence_GateSouth_01": (1206.55, 192.35),
    "Embermere_BoundaryFence_GateNorth_01": (953.45, 887.65),
}
BOUNDARY_STONE_LOCAL_Y = {
    "Embermere_BoundaryStone_GateSouth_01": -570.0,
    "Embermere_BoundaryStone_GateNorth_01": 570.0,
}
SHELTER_CENTER = (-1740.0, -700.0)
SHELTER_YAW = -64.0
SHELTER_SUPPORTS = (
    (-160.0, -88.0),
    (160.0, -88.0),
    (-160.0, 88.0),
    (160.0, 88.0),
)
PRACTICE_DUMMY_LABEL = "Embermere_FenwatchPracticeDummy_TrainingYard_01"
PRACTICE_DUMMY_CENTER = (-1120.0, -1120.0)
PRACTICE_DUMMY_YAW = 45.0


def fail(message):
    unreal.log_error("Embermere road boundary trace validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    if not actor:
        return "None"
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def world_point_yaw(center, yaw_degrees, local_x, local_y, z):
    yaw = math.radians(yaw_degrees)
    return unreal.Vector(
        center[0] + local_x * math.cos(yaw) - local_y * math.sin(yaw),
        center[1] + local_x * math.sin(yaw) + local_y * math.cos(yaw),
        z,
    )


def world_point(center, local_x, local_y, z):
    return world_point_yaw(center, GATE_YAW, local_x, local_y, z)


def trace_local_x(world, center, local_y, z, half_length):
    start = world_point(center, -half_length, local_y, z)
    end = world_point(center, half_length, local_y, z)
    return unreal.SystemLibrary.line_trace_single(
        world,
        start,
        end,
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        False,
        [],
        unreal.DrawDebugTrace.NONE,
        True,
    )


def trace_local_y(world, center, yaw_degrees, local_x, z, half_length):
    start = world_point_yaw(center, yaw_degrees, local_x, -half_length, z)
    end = world_point_yaw(center, yaw_degrees, local_x, half_length, z)
    return unreal.SystemLibrary.line_trace_single(
        world,
        start,
        end,
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        False,
        [],
        unreal.DrawDebugTrace.NONE,
        True,
    )


def hit_actor(hit_result):
    # UE 5.8 protects direct component access but exposes resolved hit objects
    # through the struct's public dictionary representation.
    return hit_result.to_dict().get("hit_actor")


def require_clear(world, description, center, local_y, z, half_length):
    result = trace_local_x(world, center, local_y, z, half_length)
    if result:
        hit_data = result.to_dict()
        fail("{} should be clear but hit {} at {}".format(
            description,
            actor_label(hit_actor(result)),
            hit_data.get("impact_point"),
        ))


def require_hit(world, description, expected_label, center, local_y, z, half_length):
    result = trace_local_x(world, center, local_y, z, half_length)
    if not result:
        fail("{} should block but the trace was clear".format(description))
    actual_label = actor_label(hit_actor(result))
    if actual_label != expected_label:
        fail("{} should hit {}, found {}".format(description, expected_label, actual_label))


def require_local_y_clear(
    world,
    description,
    center,
    yaw_degrees,
    local_x,
    z,
    half_length,
):
    result = trace_local_y(world, center, yaw_degrees, local_x, z, half_length)
    if result:
        hit_data = result.to_dict()
        fail("{} should be clear but hit {} at {}".format(
            description,
            actor_label(hit_actor(result)),
            hit_data.get("impact_point"),
        ))


def require_local_y_hit(
    world,
    description,
    expected_label,
    center,
    yaw_degrees,
    local_x,
    z,
    half_length,
):
    result = trace_local_y(world, center, yaw_degrees, local_x, z, half_length)
    if not result:
        fail("{} should block but the trace was clear".format(description))
    actual_label = actor_label(hit_actor(result))
    if actual_label != expected_label:
        fail("{} should hit {}, found {}".format(description, expected_label, actual_label))


def require_vertical_hit(world, description, expected_label, x, y, start_z, end_z):
    result = unreal.SystemLibrary.line_trace_single(
        world,
        unreal.Vector(x, y, start_z),
        unreal.Vector(x, y, end_z),
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        False,
        [],
        unreal.DrawDebugTrace.NONE,
        True,
    )
    if not result:
        fail("{} should block but the trace was clear".format(description))
    actual_label = actor_label(hit_actor(result))
    if actual_label != expected_label:
        fail("{} should hit {}, found {}".format(description, expected_label, actual_label))


def require_world_clear(world, description, start, end):
    result = unreal.SystemLibrary.line_trace_single(
        world,
        unreal.Vector(*start),
        unreal.Vector(*end),
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        False,
        [],
        unreal.DrawDebugTrace.NONE,
        True,
    )
    if result:
        hit_data = result.to_dict()
        fail("{} should be clear but hit {} at {}".format(
            description,
            actor_label(hit_actor(result)),
            hit_data.get("impact_point"),
        ))


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the editor world")

    for local_y in (-100.0, 0.0, 100.0):
        require_clear(
            world,
            "gate opening at local Y {}".format(local_y),
            GATE_CENTER,
            local_y,
            140.0,
            180.0,
        )

    require_hit(
        world,
        "gate south support",
        "Embermere_RoadGate_01",
        GATE_CENTER,
        -170.0,
        140.0,
        180.0,
    )

    for label, center in FENCE_CENTERS.items():
        require_hit(
            world,
            "{} center rails".format(label),
            label,
            center,
            0.0,
            118.0,
            130.0,
        )

    for label, local_y in BOUNDARY_STONE_LOCAL_Y.items():
        center = (
            GATE_CENTER[0] - local_y * math.sin(math.radians(GATE_YAW)),
            GATE_CENTER[1] + local_y * math.cos(math.radians(GATE_YAW)),
        )
        require_hit(
            world,
            "{} core".format(label),
            label,
            center,
            0.0,
            125.0,
            90.0,
        )

    require_vertical_hit(
        world,
        "village supply chest lid",
        "Embermere_SupplyChest_Vendor_01",
        -1740.0,
        -1180.0,
        250.0,
        -50.0,
    )

    require_world_clear(
        world,
        "spawn-to-Mara autorun corridor",
        (-2400.0, -1200.0, 90.15),
        (-1350.0, -750.0, 90.15),
    )

    shelter_center_start = world_point_yaw(SHELTER_CENTER, SHELTER_YAW, -105.0, 0.0, 90.15)
    shelter_center_end = world_point_yaw(SHELTER_CENTER, SHELTER_YAW, 105.0, 0.0, 90.15)
    require_world_clear(
        world,
        "Fenwatch shelter center opening",
        (shelter_center_start.x, shelter_center_start.y, shelter_center_start.z),
        (shelter_center_end.x, shelter_center_end.y, shelter_center_end.z),
    )

    for index, (local_x, local_y) in enumerate(SHELTER_SUPPORTS):
        support = world_point_yaw(SHELTER_CENTER, SHELTER_YAW, local_x, local_y, 0.0)
        require_vertical_hit(
            world,
            "Fenwatch shelter support {}".format(index + 1),
            "Embermere_FenwatchShelter_Mara_01",
            support.x,
            support.y,
            330.0,
            -20.0,
        )

    require_local_y_hit(
        world,
        "Fenwatch practice dummy torso",
        PRACTICE_DUMMY_LABEL,
        PRACTICE_DUMMY_CENTER,
        PRACTICE_DUMMY_YAW,
        0.0,
        151.0,
        100.0,
    )
    require_local_y_hit(
        world,
        "Fenwatch practice dummy base",
        PRACTICE_DUMMY_LABEL,
        PRACTICE_DUMMY_CENTER,
        PRACTICE_DUMMY_YAW,
        0.0,
        11.0,
        100.0,
    )
    for side, local_x in (("left", -115.0), ("right", 115.0)):
        require_local_y_clear(
            world,
            "Fenwatch practice dummy {} arm".format(side),
            PRACTICE_DUMMY_CENTER,
            PRACTICE_DUMMY_YAW,
            local_x,
            125.0,
            100.0,
        )

    unreal.log(
        "Embermere road boundary traces passed: clear spawn autorun corridor, clear Fenwatch shelter center, 4 solid shelter supports, 3 clear gate lanes, 1 solid gate support, 2 solid fence centers, 2 solid boundary stones, 1 solid supply chest, and a solid-core Fenwatch practice dummy with 2 clear arms"
    )


if __name__ == "__main__":
    main()
