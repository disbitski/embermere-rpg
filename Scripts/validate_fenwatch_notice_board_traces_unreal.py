"""Validate live Fenwatch notice-board collision and nearby routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
BOARD_LABEL = "Embermere_FenwatchNoticeBoard_Road_01"
BOARD_LOCATION = (-1560.0, -260.0)
BOARD_YAW = math.radians(-35.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch notice board trace validation failed: {}".format(message)
    )
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
    if actual_label != BOARD_LABEL:
        fail("{} should hit {}, found {}".format(description, BOARD_LABEL, actual_label))


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


def transformed_imported_local(local_x, authored_local_y, z):
    # Classic FBX mirrors Blender's authored local Y before world rotation.
    imported_y = -authored_local_y
    return (
        BOARD_LOCATION[0]
        + local_x * math.cos(BOARD_YAW)
        - imported_y * math.sin(BOARD_YAW),
        BOARD_LOCATION[1]
        + local_x * math.sin(BOARD_YAW)
        + imported_y * math.cos(BOARD_YAW),
        z,
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    for description, local_x in (
        ("left grounded support", -94.0),
        ("right grounded support", 94.0),
    ):
        point = transformed_imported_local(local_x, 0.0, 340.0)
        require_hit(world, description, point, (point[0], point[1], -20.0))

    panel = transformed_imported_local(0.0, 0.0, 340.0)
    require_hit(
        world,
        "solid central notice panel",
        panel,
        (panel[0], panel[1], 70.0),
    )

    roof_front = transformed_imported_local(0.0, 80.0, 252.0)
    roof_rear = transformed_imported_local(0.0, -80.0, 252.0)
    require_clear(
        world,
        "decorative roof and crest span",
        roof_front,
        roof_rear,
    )

    require_clear(
        world,
        "PlayerStart-to-Mara traversal",
        (-2450.0, -1210.0, 90.15),
        (-2050.0, -850.0, 90.15),
    )
    require_clear(
        world,
        "village-to-road travel lane",
        (-1900.0, -700.0, 90.15),
        (-900.0, -95.0, 90.15),
    )
    require_clear(
        world,
        "quartermaster service approach",
        (-1530.0, -940.0, 90.15),
        (-1530.0, -1210.0, 90.15),
    )
    require_clear(
        world,
        "armsmaster training-yard approach",
        (-1320.0, -920.0, 90.15),
        (-1185.0, -1055.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch notice board traces passed: solid grounded supports and central panel; decorative roof, notices, pins, trim, and crest excluded from authored collision; clear PlayerStart-to-Mara, village-to-road, quartermaster, and armsmaster routes"
    )


if __name__ == "__main__":
    main()
