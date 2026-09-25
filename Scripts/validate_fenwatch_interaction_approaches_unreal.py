"""Protect reachable Fenwatch F-interaction points in the initialized world."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
PLAYER_CENTER_Z = 95.0
PLAYER_HALF_WIDTH = 35.0
TRACE_HEIGHTS = (95.0, 155.0)
OWNERS = (
    (
        "Quest_Giver_Mara_Fenwatch",
        (-2050.0, -850.0, 140.0),
        (-2450.0, -1210.0),
        (-2230.0, -1010.0),
    ),
    (
        "Embermere_FenwatchQuartermaster_Service_01",
        (-1530.0, -1190.0, 0.0),
        (-1700.0, -950.0),
        (-1610.0, -1020.0),
    ),
    (
        "Embermere_FenwatchArmsmaster_Service_01",
        (-1320.0, -920.0, 0.0),
        (-950.0, -550.0),
        (-1200.0, -760.0),
    ),
    (
        "Embermere_FenwatchNoticeBoard_StillWatersService_01",
        (-1560.0, -260.0, 0.0),
        (-1150.0, -300.0),
        (-1460.0, -450.0),
    ),
    (
        "Embermere_FenwatchCommunalWell_RestService_01",
        (-950.0, -1600.0, 0.0),
        (-1400.0, -1600.0),
        (-1180.0, -1600.0),
    ),
)
WELL_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch interaction approach validation failed: {}".format(message)
    )
    sys.exit(1)


def label(actor):
    return actor.get_actor_label() if actor else "None"


def xyz(vector):
    return vector.x, vector.y, vector.z


def hit_actor(hit):
    return hit.to_dict().get("hit_actor") if hit else None


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


def require_ground(world, description, point):
    hit = trace(world, (point[0], point[1], 250.0), (point[0], point[1], -40.0))
    if label(hit_actor(hit)) != "Zone_Ground_Embermere_Glen":
        fail("{} rests on {} rather than zone ground".format(
            description, label(hit_actor(hit))
        ))


def require_width_clear(world, description, start, end):
    dx, dy = end[0] - start[0], end[1] - start[1]
    length = math.hypot(dx, dy)
    if length <= 0.0:
        fail("{} has a degenerate approach".format(description))
    normal_x, normal_y = -dy / length, dx / length
    for offset in (-PLAYER_HALF_WIDTH, 0.0, PLAYER_HALF_WIDTH):
        for height in TRACE_HEIGHTS:
            a = (start[0] + normal_x * offset, start[1] + normal_y * offset, height)
            b = (end[0] + normal_x * offset, end[1] + normal_y * offset, height)
            hit = trace(world, a, b)
            if hit:
                fail("{} offset {} height {} hit {} at {}".format(
                    description,
                    offset,
                    height,
                    label(hit_actor(hit)),
                    hit.to_dict().get("impact_point"),
                ))


def main():
    world = unreal.EditorLevelLibrary.get_editor_world()
    expected_world = LEVEL_PATH + "." + LEVEL_PATH.rsplit("/", 1)[-1]
    if not world or world.get_path_name().split(":")[0] != expected_world:
        fail("initialize {} in the live editor before tracing".format(LEVEL_PATH))

    controller = unreal.get_default_object(unreal.EmbermerePlayerController)
    global_radius = float(controller.get_editor_property("interaction_radius"))
    rest_data = unreal.EditorAssetLibrary.load_asset(
        "/Game/Data/Services/DA_FenwatchCommunalWellRest"
    )
    if not rest_data or abs(global_radius - 350.0) > 0.01:
        fail("missing rest data or unexpected global F radius: {}".format(global_radius))
    rest_radius = float(rest_data.get_editor_property("interaction_radius"))
    if abs(rest_radius - 300.0) > 0.01:
        fail("well service radius drifted to {}".format(rest_radius))

    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    interactables = {
        actor.get_actor_label(): actor
        for actor in actors
        if actor.get_component_by_class(unreal.EmbermereInteractableComponent)
    }
    if set(interactables) != {entry[0] for entry in OWNERS}:
        fail("F owner set drifted: {}".format(sorted(interactables)))

    for owner_label, expected_home, staging, stance in OWNERS:
        location = interactables[owner_label].get_actor_location()
        actual_home = xyz(location)
        if math.dist(actual_home, expected_home) > 1.0:
            fail("{} moved to {}".format(owner_label, actual_home))

        player_location = (stance[0], stance[1], PLAYER_CENTER_Z)
        distances = sorted(
            (math.dist(player_location, xyz(actor.get_actor_location())), other_label)
            for other_label, actor in interactables.items()
        )
        nearest_distance, nearest_label = distances[0]
        if nearest_label != owner_label or nearest_distance > global_radius - 20.0:
            fail("{} stance would select {} at {:.1f} cm".format(
                owner_label, nearest_label, nearest_distance
            ))
        if distances[1][0] - nearest_distance < 50.0:
            fail("{} has ambiguous F ownership: {}".format(owner_label, distances[:2]))
        if owner_label == WELL_LABEL and nearest_distance > rest_radius - 20.0:
            fail("well stance is outside its service-owned range: {:.1f} cm".format(
                nearest_distance
            ))

        require_ground(world, owner_label + " staging", staging)
        require_ground(world, owner_label + " stance", stance)
        require_width_clear(world, owner_label, staging, stance)

    # A known solid hit ensures that clear approaches are not passing in an
    # uninitialized physics scene.
    fence = trace(world, (1200.0, 310.0, 125.0), (1800.0, 310.0, 125.0))
    if label(hit_actor(fence)) != "Embermere_BoundaryFence_GateSouth_01":
        fail("positive fence control missed: {}".format(label(hit_actor(fence))))

    unreal.log(
        "EMBERMERE_FENWATCH_INTERACTION_APPROACHES_SUCCESS: five exact F owners, "
        "five unambiguous in-range stances, ten terrain supports, thirty "
        "player-width rays, and a live collision control"
    )


if __name__ == "__main__":
    main()
