"""Validate live Fenwatch gatewatch-post collision and protected routes."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
POST_LABEL = "Embermere_FenwatchGatewatchPost_EastRoad_01"
POST_LOCATION = (220.0, -650.0)
POST_YAW = math.radians(20.0)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch gatewatch-post trace validation failed: {}".format(
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
    if actual_label != POST_LABEL:
        fail(
            "{} should hit {}, found {}".format(
                description,
                POST_LABEL,
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
        POST_LOCATION[0]
        + local_x * math.cos(POST_YAW)
        - imported_y * math.sin(POST_YAW),
        POST_LOCATION[1]
        + local_x * math.sin(POST_YAW)
        + imported_y * math.cos(POST_YAW),
        z,
    )


def require_vertical_hit(world, description, local_x, authored_local_y, start_z, end_z):
    start = transformed_imported_local(local_x, authored_local_y, start_z)
    end = transformed_imported_local(local_x, authored_local_y, end_z)
    require_hit(world, description, start, end)


def require_vertical_clear(world, description, local_x, authored_local_y, start_z, end_z):
    start = transformed_imported_local(local_x, authored_local_y, start_z)
    end = transformed_imported_local(local_x, authored_local_y, end_z)
    require_clear(world, description, start, end)


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        fail("could not resolve the initialized editor world")

    require_vertical_hit(world, "solid platform", 0.0, 0.0, 100.0, -20.0)
    require_vertical_hit(world, "solid entry step", 0.0, -153.0, 50.0, -10.0)
    require_vertical_hit(world, "solid rear wall", 0.0, 104.0, 260.0, 50.0)
    for description, local_x, local_y in (
        ("solid left-front post", -174.0, -96.0),
        ("solid right-front post", 174.0, -96.0),
        ("solid left-rear post", -174.0, 96.0),
        ("solid right-rear post", 174.0, 96.0),
    ):
        require_vertical_hit(world, description, local_x, local_y, 350.0, 50.0)

    require_vertical_clear(world, "decorative roof", 80.0, -58.0, 405.0, 325.0)
    require_vertical_clear(world, "decorative watch bell", 0.0, -101.0, 280.0, 210.0)
    require_vertical_clear(world, "decorative side railing", 174.0, 0.0, 170.0, 80.0)
    require_vertical_clear(world, "decorative ember crest", 0.0, -159.0, 350.0, 305.0)

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
        "east-road center lane",
        (-450.0, 100.0, 90.15),
        (700.0, 460.0, 90.15),
    )
    require_clear(
        world,
        "gatewatch outer bypass",
        (-150.0, -250.0, 90.15),
        (750.0, 50.0, 90.15),
    )

    unreal.log(
        "Embermere Fenwatch gatewatch-post traces passed: solid platform, "
        "entry step, rear wall, and four structural posts; decorative roof, "
        "bell, side railings, map, horn, spears, trim, and crest excluded from "
        "authored collision; clear PlayerStart-to-Mara, village-to-road, "
        "east-road center, and gatewatch outer-bypass routes"
    )


if __name__ == "__main__":
    main()
