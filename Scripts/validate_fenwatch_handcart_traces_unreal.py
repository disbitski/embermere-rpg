"""Validate live Fenwatch handcart collision and protected village routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
HANDCART_LABEL = "Embermere_FenwatchHandcart_CottageWest_01"
HANDCART_LOCATION = (-3040.0, -920.0)
HANDCART_YAW = math.radians(38.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch handcart trace validation failed: {}".format(message)
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
    if actual_label != HANDCART_LABEL:
        fail("{} should hit {}, found {}".format(description, HANDCART_LABEL, actual_label))


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
        HANDCART_LOCATION[0]
        + local_x * math.cos(HANDCART_YAW)
        - imported_y * math.sin(HANDCART_YAW),
        HANDCART_LOCATION[1]
        + local_x * math.sin(HANDCART_YAW)
        + imported_y * math.cos(HANDCART_YAW),
        z,
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    bed = transformed_imported_local(0.0, 0.0, 260.0)
    require_hit(world, "solid provision bed", bed, (bed[0], bed[1], 60.0))

    axle = transformed_imported_local(-46.0, 0.0, 180.0)
    require_hit(world, "solid axle and wheel zone", axle, (axle[0], axle[1], -20.0))

    for description, local_y in (("left rest", -38.0), ("right rest", 38.0)):
        point = transformed_imported_local(84.0, local_y, 180.0)
        require_hit(world, description, point, (point[0], point[1], -20.0))

    require_clear(
        world,
        "decorative handles and grips",
        transformed_imported_local(150.0, 0.0, 90.0),
        transformed_imported_local(315.0, 0.0, 90.0),
    )
    require_clear(
        world,
        "decorative supplies and rear crest",
        transformed_imported_local(-140.0, 0.0, 172.0),
        transformed_imported_local(95.0, 0.0, 172.0),
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
        "village-to-road travel lane",
        (-1900.0, -700.0, 90.15),
        (-900.0, -95.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch handcart traces passed: solid provision bed, axle/wheel zone, and two grounded rests; decorative handles, grips, supplies, wheel detail, and crest excluded from authored collision; clear PlayerStart-to-Mara, west-cottage bypass, and village-to-road routes"
    )


if __name__ == "__main__":
    main()
