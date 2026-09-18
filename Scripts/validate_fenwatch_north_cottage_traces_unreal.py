"""Validate live Fenwatch north-cottage collision and nearby village routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
COTTAGE_LABEL = "Embermere_FenwatchCottage_North_01"
COTTAGE_LOCATION = (-2500.0, 680.0)
COTTAGE_YAW = math.radians(28.0)


def fail(message):
    unreal.log_error("Embermere Fenwatch north cottage trace validation failed: {}".format(message))
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

    # Classic FBX mirrors authored local Y before the saved world rotation.
    deck_x, deck_y = transformed_local_xy(100.0, -210.0)
    require_hit(
        world,
        "covered porch deck",
        (deck_x, deck_y, 100.0),
        (deck_x, deck_y, -20.0),
    )

    require_hit(
        world,
        "middle porch step",
        (*transformed_local_xy(-320.0, -220.0), 32.0),
        (*transformed_local_xy(40.0, -220.0), 32.0),
    )

    lower_x, lower_y = transformed_local_xy(-95.0, -285.0)
    require_hit(
        world,
        "lower porch step",
        (lower_x, lower_y, 55.0),
        (lower_x, lower_y, -20.0),
    )

    for description, local_x in (("left porch post", -180.0), ("right porch post", 180.0)):
        post_x, post_y = transformed_local_xy(local_x, -232.0)
        require_hit(
            world,
            description,
            (post_x, post_y, 330.0),
            (post_x, post_y, 80.0),
        )

    require_clear(
        world,
        "decorative roof ridge",
        (*transformed_local_xy(-300.0, 0.0), 430.0),
        (*transformed_local_xy(300.0, 0.0), 430.0),
    )

    require_clear(
        world,
        "PlayerStart-to-Mara traversal",
        (-2450.0, -1210.0, 90.15),
        (-2050.0, -850.0, 90.15),
    )
    require_clear(
        world,
        "lane between the two cottages",
        (-2920.0, 210.0, 90.15),
        (-2080.0, 210.0, 90.15),
    )
    require_clear(
        world,
        "north cottage to village road approach",
        (-2160.0, 370.0, 90.15),
        (-1740.0, -180.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch north cottage traces passed: solid body, porch deck, two steps, and two posts; decorative roof excluded from authored collision; clear PlayerStart-to-Mara traversal, cottage-pair lane, and village-road approach"
    )


if __name__ == "__main__":
    main()
