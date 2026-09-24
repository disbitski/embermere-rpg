"""Protect three single-Prowler pull stances in the initialized editor world."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
AGGRO_RADIUS_CM = 525.0
PLAYER_HALF_WIDTH_CM = 35.0
PLAYER_TRACE_HEIGHTS_CM = (95.0, 155.0)
ENEMIES = {
    "Starter_Enemy_01": (1900.0, 300.0, 95.0),
    "Starter_Enemy_02": (1700.0, 1100.0, 95.0),
    "Starter_Enemy_03": (2500.0, 1300.0, 95.0),
}
LANES = (
    ("Starter_Enemy_01", (1530.0, -50.0), (1350.0, 0.0), (1830.0, 255.0)),
    ("Starter_Enemy_02", (1225.0, 1000.0), (1150.0, 700.0), (1640.0, 1090.0)),
    ("Starter_Enemy_03", (2500.0, 1800.0), (1900.0, 1700.0), (2500.0, 1370.0)),
)


def fail(message):
    unreal.log_error("Embermere Prowler pull-lane validation failed: {}".format(message))
    sys.exit(1)


def label(actor):
    return actor.get_actor_label() if actor else "None"


def trace(world, start, end, actors_to_ignore=None):
    return unreal.SystemLibrary.line_trace_single(
        world,
        unreal.Vector(*start),
        unreal.Vector(*end),
        unreal.TraceTypeQuery.ECC_VISIBILITY,
        False,
        actors_to_ignore or [],
        unreal.DrawDebugTrace.NONE,
        True,
    )


def trace_actor(hit):
    return hit.to_dict().get("hit_actor") if hit else None


def require_player_width_clear(world, description, start, end):
    dx, dy = end[0] - start[0], end[1] - start[1]
    length = math.hypot(dx, dy)
    if length <= 0.0:
        fail("{} has a degenerate segment".format(description))
    normal_x, normal_y = -dy / length, dx / length
    for offset in (-PLAYER_HALF_WIDTH_CM, 0.0, PLAYER_HALF_WIDTH_CM):
        for height in PLAYER_TRACE_HEIGHTS_CM:
            a = (start[0] + normal_x * offset, start[1] + normal_y * offset, height)
            b = (end[0] + normal_x * offset, end[1] + normal_y * offset, height)
            hit = trace(world, a, b)
            if hit:
                fail("{} offset {} height {} hit {} at {}".format(
                    description,
                    offset,
                    height,
                    label(trace_actor(hit)),
                    hit.to_dict().get("impact_point"),
                ))


def require_supported_stance(world, enemy_label, stance):
    hit = trace(world, (stance[0], stance[1], 250.0), (stance[0], stance[1], -40.0))
    if hit and label(trace_actor(hit)) == "FabPass_Wild_Roots_01":
        hit = trace(
            world,
            (stance[0], stance[1], 250.0),
            (stance[0], stance[1], -40.0),
            [trace_actor(hit)],
        )
    if not hit or label(trace_actor(hit)) != "Zone_Ground_Embermere_Glen":
        fail("{} stance has no clear terrain support: {}".format(
            enemy_label, label(trace_actor(hit))
        ))


def main():
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world or world.get_path_name().split(":")[0] != LEVEL_PATH + "." + LEVEL_PATH.rsplit("/", 1)[-1]:
        fail("initialize {} in the live editor before tracing".format(LEVEL_PATH))

    actors = {
        actor.get_actor_label(): actor
        for actor in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    }
    for enemy_label, expected in ENEMIES.items():
        enemy = actors.get(enemy_label)
        if not enemy:
            fail("missing {}".format(enemy_label))
        location = enemy.get_actor_location()
        if math.dist((location.x, location.y, location.z), expected) > 1.0:
            fail("{} home drifted to {}".format(enemy_label, location))
        radius = float(enemy.get_editor_property("aggro_radius"))
        if abs(radius - AGGRO_RADIUS_CM) > 0.01:
            fail("{} aggro radius drifted to {}".format(enemy_label, radius))

    # The staging points are local to the pocket, not a claim that one straight
    # line from PlayerStart can pass through the gate and its existing fences.
    for enemy_label, stance, staging, target_edge in LANES:
        for other_label, home in ENEMIES.items():
            distance = math.dist(stance, home[:2])
            if other_label == enemy_label and not (350.0 <= distance <= AGGRO_RADIUS_CM - 10.0):
                fail("{} is outside its deliberate solo-pull band: {:.1f} cm".format(
                    enemy_label, distance
                ))
            if other_label != enemy_label and distance < AGGRO_RADIUS_CM + 75.0:
                fail("{} stance can aggro {} at {:.1f} cm".format(
                    enemy_label, other_label, distance
                ))
        require_supported_stance(world, enemy_label, stance)
        require_player_width_clear(world, enemy_label + " staging", staging, stance)
        require_player_width_clear(world, enemy_label + " pull sightline", stance, target_edge)

    # A positive control prevents an uninitialized physics scene from making
    # every clearance assertion pass vacuously.
    fence = trace(world, (1200.0, 310.0, 125.0), (1800.0, 310.0, 125.0))
    if not fence or label(trace_actor(fence)) != "Embermere_BoundaryFence_GateSouth_01":
        fail("positive fence control missed: {}".format(label(trace_actor(fence))))

    unreal.log("EMBERMERE_MARSH_PROWLER_PULL_LANES_SUCCESS: three solo stances, six player-width lanes at two heights, grounded supports, and a live collision control")


if __name__ == "__main__":
    main()
