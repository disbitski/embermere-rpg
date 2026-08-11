"""Log one read-only live-PIE sample of the Fenwatch armsmaster Idle clock."""

import sys

import unreal


PRESENTATION_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
EXPECTED_IDLE_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/Animations/"
    "A_EmbermereFenwatchArmsmaster_Idle.A_EmbermereFenwatchArmsmaster_Idle"
)


def fail(message):
    unreal.log_error("Embermere armsmaster Idle clock probe failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def main():
    world = unreal.EditorLevelLibrary.get_game_world()
    if not world:
        fail("no live PIE world is available")

    matches = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(
            world,
            unreal.EmbermereNpcPresentationActor,
        )
        if actor_label(actor) == PRESENTATION_LABEL
    ]
    if len(matches) != 1:
        fail("expected one live armsmaster presentation, found {}".format(len(matches)))

    presentation = matches[0]
    skeletal_component = presentation.get_component_by_class(
        unreal.SkeletalMeshComponent
    )
    if not skeletal_component:
        fail("live presentation has no SkeletalMeshComponent")
    if (
        presentation.get_resolved_visual_mode()
        != unreal.EmbermereNpcVisualMode.SKELETAL_MESH
    ):
        fail("live presentation is not using the skeletal lane")
    if (
        presentation.get_resolved_animation_mode()
        != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE
    ):
        fail("live presentation is not using single-node Idle")

    animation = presentation.get_editor_property("idle_animation")
    animation_path = animation.get_path_name() if animation else "None"
    if animation_path != EXPECTED_IDLE_PATH:
        fail("live animation drifted: {}".format(animation_path))
    position = float(skeletal_component.get_position())
    if position < 0.0 or position > 3.21:
        fail("live playback position is outside the Idle cycle: {}".format(position))
    if not skeletal_component.is_playing():
        fail("live Idle is not playing")
    if not presentation.is_presentation_collision_disabled():
        fail("live animated presentation collision is enabled")

    unreal.log(
        "Embermere armsmaster Idle clock sample: position={:.6f}, playing=true, "
        "animation={}, mode=SingleNodeIdle, collision=NoCollision".format(
            position,
            animation_path,
        )
    )


if __name__ == "__main__":
    main()
