"""Validate live Fenwatch training-workshop collision and nearby routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
WORKSHOP_LABEL = "Embermere_FenwatchTrainingWorkshop_Armsmaster_01"
WORKSHOP_LOCATION = (-690.0, -1030.0)
WORKSHOP_YAW = math.radians(-100.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch training workshop trace validation failed: {}".format(
            message
        )
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
    if actual_label != WORKSHOP_LABEL:
        fail(
            "{} should hit {}, found {}".format(
                description,
                WORKSHOP_LABEL,
                actual_label,
            )
        )


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
        WORKSHOP_LOCATION[0]
        + local_x * math.cos(WORKSHOP_YAW)
        - imported_y * math.sin(WORKSHOP_YAW),
        WORKSHOP_LOCATION[1]
        + local_x * math.sin(WORKSHOP_YAW)
        + imported_y * math.cos(WORKSHOP_YAW),
        z,
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    for description, local_x in (
        ("left open-front support", -184.0),
        ("right open-front support", 184.0),
    ):
        point = transformed_imported_local(local_x, 82.0, 400.0)
        require_hit(
            world,
            description,
            point,
            (point[0], point[1], -20.0),
        )

    rear = transformed_imported_local(0.0, -105.0, 420.0)
    require_hit(
        world,
        "solid rear work wall",
        rear,
        (rear[0], rear[1], -20.0),
    )
    bench = transformed_imported_local(0.0, -64.0, 300.0)
    require_hit(
        world,
        "solid rear workbench",
        bench,
        (bench[0], bench[1], -20.0),
    )

    opening_start = transformed_imported_local(0.0, 190.0, 100.0)
    opening_end = transformed_imported_local(0.0, 5.0, 100.0)
    require_clear(
        world,
        "open-front center approach",
        opening_start,
        opening_end,
    )

    overhead_left = transformed_imported_local(-145.0, 120.0, 190.0)
    overhead_right = transformed_imported_local(145.0, 120.0, 190.0)
    require_clear(
        world,
        "clear player-height training bay",
        overhead_left,
        overhead_right,
    )

    require_clear(
        world,
        "armsmaster-to-practice-dummy approach",
        (-1320.0, -920.0, 90.15),
        (-1185.0, -1055.0, 90.15),
    )
    require_clear(
        world,
        "road-side east bypass",
        (-1040.0, -430.0, 90.15),
        (-560.0, -430.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch training workshop traces passed: solid front supports, rear wall, and workbench; clear open-front center and player-height bay; clear armsmaster-to-dummy approach; decorative roof, trim, crest, and tools excluded from authored collision; and clear road-side east bypass"
    )


if __name__ == "__main__":
    main()
