"""Validate live Fenwatch firewood-rack collision and protected village routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
FIREWOOD_RACK_LABEL = "Embermere_FenwatchFirewoodRack_CottageWest_01"
FIREWOOD_RACK_LOCATION = (-3050.0, -120.0)
FIREWOOD_RACK_YAW = math.radians(38.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch firewood-rack trace validation failed: {}".format(message)
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
    if actual_label != FIREWOOD_RACK_LABEL:
        fail(
            "{} should hit {}, found {}".format(
                description,
                FIREWOOD_RACK_LABEL,
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
        FIREWOOD_RACK_LOCATION[0]
        + local_x * math.cos(FIREWOOD_RACK_YAW)
        - imported_y * math.sin(FIREWOOD_RACK_YAW),
        FIREWOOD_RACK_LOCATION[1]
        + local_x * math.sin(FIREWOOD_RACK_YAW)
        + imported_y * math.cos(FIREWOOD_RACK_YAW),
        z,
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    log_stack = transformed_imported_local(-18.0, 8.0, 190.0)
    require_hit(
        world,
        "solid firewood stack",
        log_stack,
        (log_stack[0], log_stack[1], 20.0),
    )

    for description, local_x in (
        ("left structural side", -126.0),
        ("right structural side", 90.0),
    ):
        point = transformed_imported_local(local_x, 4.0, 260.0)
        require_hit(world, description, point, (point[0], point[1], -20.0))

    chopping_block = transformed_imported_local(174.0, -24.0, 150.0)
    require_hit(
        world,
        "solid chopping block",
        chopping_block,
        (chopping_block[0], chopping_block[1], -20.0),
    )

    require_clear(
        world,
        "decorative moss roof",
        transformed_imported_local(-150.0, -65.0, 218.0),
        transformed_imported_local(130.0, -65.0, 218.0),
    )
    require_clear(
        world,
        "decorative axe",
        transformed_imported_local(160.0, -24.0, 150.0),
        transformed_imported_local(225.0, -24.0, 150.0),
    )
    require_clear(
        world,
        "decorative crest",
        transformed_imported_local(-18.0, -95.0, 185.0),
        transformed_imported_local(-18.0, -55.0, 185.0),
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
        "village-to-road travel lane",
        (-1900.0, -700.0, 90.15),
        (-900.0, -95.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch firewood-rack traces passed: solid log stack, two structural sides, and chopping block; decorative roof, individual detail, axe, trim, and crest excluded from authored collision; clear PlayerStart-to-Mara, west-cottage bypass, cottage-to-handcart work lane, and village-to-road routes"
    )


if __name__ == "__main__":
    main()
