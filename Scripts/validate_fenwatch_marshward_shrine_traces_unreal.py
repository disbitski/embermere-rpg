"""Validate live Fenwatch marshward-shrine collision and north-road routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
SHRINE_LABEL = "Embermere_FenwatchMarshwardShrine_NorthRoad_01"
SHRINE_LOCATION = (300.0, 1450.0)
SHRINE_YAW = math.radians(-90.0)


def fail(message):
    unreal.log_error("Embermere Fenwatch marshward-shrine trace validation failed: {}".format(message))
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
    if actor_label(hit) != SHRINE_LABEL:
        fail("{} should hit {}, found {}".format(description, SHRINE_LABEL, actor_label(hit)))


def require_clear(world, description, start, end):
    result = trace(world, start, end)
    if result:
        hit = result.to_dict()
        fail("{} should be clear but hit {} at {}".format(description, actor_label(hit.get("hit_actor")), hit.get("impact_point")))


def world_point(local_x, authored_local_y, z):
    imported_y = -authored_local_y
    return (
        SHRINE_LOCATION[0] + local_x * math.cos(SHRINE_YAW) - imported_y * math.sin(SHRINE_YAW),
        SHRINE_LOCATION[1] + local_x * math.sin(SHRINE_YAW) + imported_y * math.cos(SHRINE_YAW),
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

    vertical_hit(world, "stone plinth", 0.0, -45.0, 70.0, -10.0)
    vertical_hit(world, "ward backstone", 0.0, 50.0, 170.0, 50.0)
    vertical_hit(world, "left shrine post", -94.0, 40.0, 210.0, 20.0)
    vertical_hit(world, "right shrine post", 94.0, 40.0, 210.0, 20.0)
    vertical_clear(world, "decorative hanging ward", 0.0, 0.0, 230.0, 150.0)
    vertical_clear(world, "decorative offering bowl", 0.0, -45.0, 100.0, 50.0)
    vertical_clear(world, "open front approach", 0.0, -150.0, 120.0, 10.0)

    # These segments begin beyond the existing north gate fence collider.
    require_clear(world, "north-road centerline", (1100.0, 1100.0, 90.15), (1100.0, 2000.0, 90.15))
    require_clear(world, "north-road west shoulder", (650.0, 1100.0, 90.15), (650.0, 1900.0, 90.15))
    require_clear(world, "north-commons south lane", (-2250.0, 950.0, 90.15), (-800.0, 950.0, 90.15))

    unreal.log(
        "EMBERMERE_FENWATCH_MARSHWARD_SHRINE_TRACES_SUCCESS: solid plinth, "
        "backstone, and paired posts; clear hanging ward, offering bowl, and "
        "front approach; clear north-road centerline/shoulder and "
        "north-commons routes"
    )


if __name__ == "__main__":
    main()
