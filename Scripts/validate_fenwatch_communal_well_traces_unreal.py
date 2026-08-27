"""Validate live Fenwatch communal-well collision and protected routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
WELL_LOCATION = (-950.0, -1600.0)
WELL_YAW = math.radians(-135.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch communal well trace validation failed: {}".format(message)
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
    if actual_label != WELL_LABEL:
        fail("{} should hit {}, found {}".format(description, WELL_LABEL, actual_label))


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
        WELL_LOCATION[0]
        + local_x * math.cos(WELL_YAW)
        - imported_y * math.sin(WELL_YAW),
        WELL_LOCATION[1]
        + local_x * math.sin(WELL_YAW)
        + imported_y * math.cos(WELL_YAW),
        z,
    )


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    for description, local_x, local_y in (
        ("north curb", 0.0, 88.0),
        ("south curb", 0.0, -88.0),
        ("east curb", 88.0, 0.0),
        ("west curb", -88.0, 0.0),
    ):
        point = transformed_imported_local(local_x, local_y, 380.0)
        require_hit(world, description, point, (point[0], point[1], -20.0))

    for description, local_x in (
        ("left roof upright", -132.0),
        ("right roof upright", 132.0),
    ):
        point = transformed_imported_local(local_x, 0.0, 380.0)
        require_hit(world, description, point, (point[0], point[1], -20.0))

    require_clear(
        world,
        "open well shaft",
        transformed_imported_local(-55.0, 0.0, 80.0),
        transformed_imported_local(55.0, 0.0, 80.0),
    )
    require_clear(
        world,
        "decorative axle, rope, and bucket",
        transformed_imported_local(0.0, 0.0, 245.0),
        transformed_imported_local(0.0, 0.0, 60.0),
    )
    require_clear(
        world,
        "decorative roof, trim, and crest",
        transformed_imported_local(0.0, -108.0, 286.0),
        transformed_imported_local(0.0, 108.0, 286.0),
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
        "vendor stall east bypass",
        (-1260.0, -1530.0, 90.15),
        (-1260.0, -1190.0, 90.15),
    )
    require_clear(
        world,
        "armsmaster training-yard approach",
        (-1320.0, -920.0, 90.15),
        (-1185.0, -1055.0, 90.15),
    )
    require_clear(
        world,
        "workshop south circulation lane",
        (-1240.0, -1380.0, 90.15),
        (-620.0, -1380.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch communal well traces passed: four solid curb sections and two solid uprights; open shaft plus decorative axle, rope, bucket, roof, trim, and crest excluded from collision; clear PlayerStart-to-Mara, village-to-road, quartermaster, vendor-bypass, armsmaster, and workshop circulation routes"
    )


if __name__ == "__main__":
    main()
