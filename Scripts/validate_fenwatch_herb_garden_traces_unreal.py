"""Validate live Fenwatch herb-garden collision and north-commons routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
GARDEN_LABEL = "Embermere_FenwatchHerbGarden_NorthCommons_01"
GARDEN_LOCATION = (-1500.0, 1400.0)
GARDEN_YAW = math.radians(-15.0)


def fail(message):
    unreal.log_error("Embermere Fenwatch herb-garden trace validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    if not actor:
        return "None"
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


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
        fail("{} should block but was clear".format(description))
    hit = result.to_dict().get("hit_actor")
    if actor_label(hit) != GARDEN_LABEL:
        fail("{} should hit {}, found {}".format(description, GARDEN_LABEL, actor_label(hit)))


def require_clear(world, description, start, end):
    result = trace(world, start, end)
    if result:
        hit = result.to_dict()
        fail("{} should be clear but hit {} at {}".format(description, actor_label(hit.get("hit_actor")), hit.get("impact_point")))


def world_point(local_x, authored_local_y, z):
    imported_y = -authored_local_y
    return (
        GARDEN_LOCATION[0] + local_x * math.cos(GARDEN_YAW) - imported_y * math.sin(GARDEN_YAW),
        GARDEN_LOCATION[1] + local_x * math.sin(GARDEN_YAW) + imported_y * math.cos(GARDEN_YAW),
        z,
    )


def vertical_hit(world, description, x, y, top, bottom):
    require_hit(world, description, world_point(x, y, top), world_point(x, y, bottom))


def vertical_clear(world, description, x, y, top, bottom):
    require_clear(world, description, world_point(x, y, top), world_point(x, y, bottom))


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve initialized editor world")

    vertical_hit(world, "left raised bed", -92.0, 0.0, 90.0, -10.0)
    vertical_hit(world, "right raised bed", 92.0, 0.0, 90.0, -10.0)
    vertical_hit(world, "left trellis post", -160.0, 82.0, 185.0, 20.0)
    vertical_hit(world, "right trellis post", 160.0, 82.0, 185.0, 20.0)
    vertical_clear(world, "central tending aisle", 0.0, 0.0, 100.0, 5.0)
    vertical_clear(world, "decorative trellis beam", 0.0, 82.0, 190.0, 135.0)
    vertical_clear(world, "decorative watering pail", 0.0, -84.0, 60.0, 5.0)

    require_clear(world, "north-commons south lane", (-2250.0, 950.0, 90.15), (-800.0, 950.0, 90.15))
    require_clear(world, "north-commons perimeter lane", (-2600.0, 1800.0, 90.15), (-800.0, 1800.0, 90.15))
    # Begin outside the cottage's closed body; an endpoint inside a purposeful
    # collider is not a traversal assertion.
    require_clear(world, "north-cottage village approach", (-2020.0, 700.0, 90.15), (-1700.0, 300.0, 90.15))
    require_clear(world, "village-to-road travel lane", (-1900.0, -700.0, 90.15), (-900.0, -95.0, 90.15))

    unreal.log(
        "EMBERMERE_FENWATCH_HERB_GARDEN_TRACES_SUCCESS: solid left/right beds "
        "and trellis posts; clear central aisle, decorative beam and pail; "
        "clear north-commons south/perimeter, cottage approach, and village-road routes"
    )


if __name__ == "__main__":
    main()
