"""Protect the relief's empty collision volume and the original solid wall."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
RELIEF_LABEL = "Embermere_AncientRuinRelief_WallA_01"
WALL_LABEL = "FabPass_Ruin_Wall_A"


def fail(message):
    unreal.log_error("Embermere ancient ruin relief trace validation failed: {}".format(message))
    sys.exit(1)


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


def label(hit):
    actor = hit.to_dict().get("hit_actor") if hit else None
    return actor.get_actor_label() if actor else "None"


def main():
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world or world.get_path_name().split(":")[0] != LEVEL_PATH + ".L_Embermere_Prototype":
        fail("initialize the saved prototype world before native traces")
    actors = {
        actor.get_actor_label(): actor
        for actor in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    }
    relief = actors.get(RELIEF_LABEL)
    wall = actors.get(WALL_LABEL)
    if not relief or not wall:
        fail("relief or backing wall is missing")
    component = relief.get_editor_property("static_mesh_component")
    if str(component.get_collision_profile_name()) != "NoCollision":
        fail("relief profile is not NoCollision")
    for height in (75.0, 120.0, 175.0):
        clear = trace(world, (2200.0, 350.0, height), (2290.0, 350.0, height))
        if clear:
            fail("decorative relief blocks at height {}: {}".format(height, label(clear)))
        wall_hit = trace(world, (2200.0, 350.0, height), (2400.0, 350.0, height))
        if not wall_hit or label(wall_hit) != WALL_LABEL:
            fail("backing wall lost solid collision at height {}: {}".format(height, label(wall_hit)))
    unreal.log("EMBERMERE_ANCIENT_RUIN_RELIEF_TRACES_SUCCESS: three decorative-clear rays, three positive wall hits")


if __name__ == "__main__":
    main()
