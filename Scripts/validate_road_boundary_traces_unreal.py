"""Validate traversable gate and solid boundary collision in the saved map."""

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


def world_point(center, local_x, local_y, z):
    yaw = math.radians(GATE_YAW)
    return unreal.Vector(
        center[0] + local_x * math.cos(yaw) - local_y * math.sin(yaw),
        center[1] + local_x * math.sin(yaw) + local_y * math.cos(yaw),
        z,
    )


def trace_local_x(world, center, local_y, z, half_length):
    start = world_point(center, -half_length, local_y, z)
    end = world_point(center, half_length, local_y, z)
    return unreal.SystemLibrary.line_trace_single(
        world,
        start,
        end,
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        True,
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

    unreal.log(
        "Embermere road boundary traces passed: 3 clear gate lanes, 1 solid gate support, and 2 solid fence centers"
    )


if __name__ == "__main__":
    main()
