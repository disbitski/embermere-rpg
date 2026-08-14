"""Validate live Fenwatch vendor-stall collision and service approach routes."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
STALL_LABEL = "Embermere_FenwatchVendorStall_Quartermaster_01"
STALL_CENTER = (-1530.0, -1430.0)
SUPPORT_OFFSETS = (
    (-155.0, 78.0),
    (155.0, 78.0),
    (-155.0, -78.0),
    (155.0, -78.0),
)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch vendor stall trace validation failed: {}".format(message)
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
    if actual_label != STALL_LABEL:
        fail("{} should hit {}, found {}".format(description, STALL_LABEL, actual_label))


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


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    for index, (offset_x, offset_y) in enumerate(SUPPORT_OFFSETS, start=1):
        x = STALL_CENTER[0] + offset_x
        y = STALL_CENTER[1] + offset_y
        require_hit(
            world,
            "vendor stall support {}".format(index),
            (x, y, 340.0),
            (x, y, -20.0),
        )

    require_hit(
        world,
        "vendor stall counter",
        (STALL_CENTER[0], STALL_CENTER[1] + 68.0, 200.0),
        (STALL_CENTER[0], STALL_CENTER[1] + 68.0, 20.0),
    )

    # The quartermaster and service share the point at (-1530,-1190). Their
    # customer-side approach must remain open while the counter blocks behind.
    require_clear(
        world,
        "quartermaster service approach",
        (-1530.0, -940.0, 90.15),
        (-1530.0, -1210.0, 90.15),
    )
    require_clear(
        world,
        "vendor stall east bypass",
        (-1260.0, -1530.0, 90.15),
        (-1260.0, -1190.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch vendor stall traces passed: four solid supports, "
        "one solid counter, a clear quartermaster service approach, and a clear "
        "east-side bypass"
    )


if __name__ == "__main__":
    main()
