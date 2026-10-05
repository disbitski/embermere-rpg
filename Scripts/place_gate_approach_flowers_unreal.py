"""Keep the existing decorative gate flowers off the road opening."""

import math
import sys

import unreal


LEVEL_NAME = "L_Embermere_Prototype"
ACTOR_LABEL = "FabPass_Road_Flowers_02"
OLD_LOCATION = (1010.0, 570.0, 0.0)
SHOULDER_LOCATION = (500.0, 0.0, 0.0)
FLOWER_YAW = -20.0
FLOWER_SCALE = 1.2


def fail(message):
    unreal.log_error("Embermere gate flower placement failed: {}".format(message))
    sys.exit(1)


def near(location, expected):
    return math.dist(
        (location.x, location.y, location.z), expected
    ) <= 2.0


def main():
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world or world.get_name() != LEVEL_NAME:
        fail("open {} in the editor before running".format(LEVEL_NAME))

    matches = [
        actor for actor in unreal.EditorLevelLibrary.get_all_level_actors()
        if actor.get_actor_label() == ACTOR_LABEL
    ]
    if len(matches) != 1:
        fail("expected exactly one {}, found {}".format(ACTOR_LABEL, len(matches)))

    actor = matches[0]
    if not any(near(actor.get_actor_location(), location) for location in (OLD_LOCATION, SHOULDER_LOCATION)):
        fail("{} has an unexpected location; preserve the local placement".format(ACTOR_LABEL))
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if not component:
        fail("{} has no static mesh component".format(ACTOR_LABEL))

    actor.modify()
    component.modify()
    actor.set_actor_location(unreal.Vector(*SHOULDER_LOCATION), False, False)
    actor.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=FLOWER_YAW, roll=0.0), False)
    actor.set_actor_scale3d(unreal.Vector(FLOWER_SCALE, FLOWER_SCALE, FLOWER_SCALE))
    component.set_collision_profile_name("NoCollision", True)
    if component.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("placed flowers did not accept NoCollision")
    if abs(actor.get_actor_rotation().yaw - FLOWER_YAW) > 0.1 or abs(actor.get_actor_scale3d().x - FLOWER_SCALE) > 0.001:
        fail("placed flowers did not accept the authored orientation and scale")
    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save the current level")
    unreal.log("EMBERMERE_GATE_APPROACH_FLOWERS_SUCCESS: shoulder placement and NoCollision saved")


if __name__ == "__main__":
    main()
