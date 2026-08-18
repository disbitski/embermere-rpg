"""Place the art-free Fenwatch practice-target gameplay actor."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
TARGET_LABEL = "Embermere_FenwatchPracticeTarget_Gameplay_01"
TARGET_LOCATION = (-1120.0, -1120.0, 0.0)
TARGET_YAW = 45.0
TARGET_FOLDER = "03_Gameplay/PracticeTargets"
TARGET_TAGS = [unreal.Name("PracticeTarget"), unreal.Name("EmbermereGameplay")]


def fail(message):
    unreal.log_error("Embermere Fenwatch practice-target placement failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def make_rotation(yaw):
    rotation = unreal.Rotator()
    rotation.pitch = 0.0
    rotation.yaw = float(yaw)
    rotation.roll = 0.0
    return rotation


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        if actor_label(actor) == TARGET_LABEL or isinstance(
            actor, unreal.EmbermerePracticeTargetActor
        ):
            removed.append(actor_label(actor))
            actor_subsystem.destroy_actor(actor)

    target = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermerePracticeTargetActor,
        unreal.Vector(*TARGET_LOCATION),
        make_rotation(TARGET_YAW),
        False,
    )
    if not target:
        fail("could not spawn {}".format(TARGET_LABEL))

    target.set_actor_label(TARGET_LABEL)
    target.set_folder_path(unreal.Name(TARGET_FOLDER))
    target.set_editor_property("tags", TARGET_TAGS)
    target.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
    if unreal.Name("EmbermereOriginalArt") in list(target.tags):
        fail("gameplay target must not be counted as original art")
    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))

    unreal.log(
        "Embermere Fenwatch practice-target placement passed: "
        "label={}, location={}, yaw={}, removed={}".format(
            TARGET_LABEL,
            TARGET_LOCATION,
            TARGET_YAW,
            removed,
        )
    )


if __name__ == "__main__":
    main()
