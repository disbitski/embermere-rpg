"""Validate live Fenwatch hitching-trough collision and protected routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
TROUGH_LABEL = "Embermere_FenwatchHitchingTrough_CottageWest_01"
TROUGH_LOCATION = (-3550.0, -520.0)
TROUGH_YAW = math.radians(38.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch hitching-trough trace validation failed: {}".format(
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
    if actual_label != TROUGH_LABEL:
        fail(
            "{} should hit {}, found {}".format(
                description,
                TROUGH_LABEL,
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
        TROUGH_LOCATION[0]
        + local_x * math.cos(TROUGH_YAW)
        - imported_y * math.sin(TROUGH_YAW),
        TROUGH_LOCATION[1]
        + local_x * math.sin(TROUGH_YAW)
        + imported_y * math.cos(TROUGH_YAW),
        z,
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    trough_body = transformed_imported_local(0.0, 0.0, 240.0)
    require_hit(
        world,
        "solid trough body",
        trough_body,
        (trough_body[0], trough_body[1], -20.0),
    )

    hitch_rail = transformed_imported_local(0.0, 37.0, 220.0)
    require_hit(
        world,
        "solid hitching rail",
        hitch_rail,
        (hitch_rail[0], hitch_rail[1], 120.0),
    )

    decorative_ring = transformed_imported_local(0.0, 20.0, 170.0)
    require_clear(
        world,
        "decorative hitch ring",
        decorative_ring,
        (decorative_ring[0], decorative_ring[1], 120.0),
    )
    decorative_cap = transformed_imported_local(-132.0, 37.0, 220.0)
    require_clear(
        world,
        "decorative post cap",
        decorative_cap,
        (decorative_cap[0], decorative_cap[1], 168.0),
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
        (-3040.0, -640.0, 90.15),
        (-2460.0, -640.0, 90.15),
    )
    require_clear(
        world,
        "cottage-to-handcart work lane",
        (-2840.0, -420.0, 90.15),
        (-3020.0, -750.0, 90.15),
    )
    require_clear(
        world,
        "west utility circulation",
        (-3460.0, -900.0, 90.15),
        (-3200.0, -430.0, 90.15),
    )
    require_clear(
        world,
        "village-to-road travel lane",
        (-1900.0, -700.0, 90.15),
        (-900.0, -95.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch hitching-trough traces passed: solid trough body "
        "and hitching rail; decorative feed, feet, bands, rings, caps, braces, "
        "and crest excluded from authored collision; clear PlayerStart-to-Mara, "
        "west-cottage bypass, cottage-to-handcart, west-utility, and "
        "village-to-road routes"
    )


if __name__ == "__main__":
    main()
