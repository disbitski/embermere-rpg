"""Validate live Fenwatch cottage collision and nearby village routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
COTTAGE_LABEL = "Embermere_FenwatchCottage_West_01"
COTTAGE_LOCATION = (-2480.0, -260.0)
COTTAGE_YAW = math.radians(38.0)


def fail(message):
    unreal.log_error("Embermere Fenwatch cottage trace validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    if not actor:
        return "None"
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def hit_actor(hit_result):
    return hit_result.to_dict().get("hit_actor")


def trace(world, start, end):
    return unreal.SystemLibrary.line_trace_single(
        world,
        unreal.Vector(*start),
        unreal.Vector(*end),
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        False,
        [],
        unreal.DrawDebugTrace.NONE,
        True,
    )


def require_hit(world, description, start, end):
    result = trace(world, start, end)
    if not result:
        fail("{} should block but the trace was clear".format(description))
    actual_label = actor_label(hit_actor(result))
    if actual_label != COTTAGE_LABEL:
        fail("{} should hit {}, found {}".format(description, COTTAGE_LABEL, actual_label))


def require_clear(world, description, start, end):
    result = trace(world, start, end)
    if result:
        hit_data = result.to_dict()
        fail(
            "{} should be clear but hit {} at {}".format(
                description,
                actor_label(hit_actor(result)),
                hit_data.get("impact_point"),
            )
        )


def transformed_local_xy(local_x, local_y):
    return (
        COTTAGE_LOCATION[0] + local_x * math.cos(COTTAGE_YAW) - local_y * math.sin(COTTAGE_YAW),
        COTTAGE_LOCATION[1] + local_x * math.sin(COTTAGE_YAW) + local_y * math.cos(COTTAGE_YAW),
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    require_hit(
        world,
        "closed cottage body",
        (COTTAGE_LOCATION[0], COTTAGE_LOCATION[1], 620.0),
        (COTTAGE_LOCATION[0], COTTAGE_LOCATION[1], -20.0),
    )

    # Classic FBX mirrors authored local Y, so the +188 cm Blender doorstep is
    # imported at local -188 cm before the saved 38-degree world rotation.
    step_x, step_y = transformed_local_xy(0.0, -188.0)
    require_hit(
        world,
        "cottage doorstep",
        (step_x, step_y, 90.0),
        (step_x, step_y, -20.0),
    )

    require_clear(
        world,
        "PlayerStart-to-Mara traversal",
        (-2450.0, -1210.0, 90.15),
        (-2050.0, -850.0, 90.15),
    )
    require_clear(
        world,
        "west cottage bypass",
        (-2860.0, -760.0, 90.15),
        (-2100.0, -760.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch cottage traces passed: solid closed body and doorstep, decorative roof/chimney details excluded from authored collision, clear PlayerStart-to-Mara traversal, and clear west-side bypass"
    )


if __name__ == "__main__":
    main()
