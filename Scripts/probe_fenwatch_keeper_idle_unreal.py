"""Log one read-only live-PIE sample of Mara's Fenwatch keeper Idle clock."""

import sys

import unreal


PRESENTATION_LABEL = "Embermere_FenwatchKeeper_Mara_Presentation_01"
QUEST_ACTOR_LABEL = "Quest_Giver_Mara_Fenwatch"
EXPECTED_IDLE_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/Animations/"
    "A_EmbermereFenwatchKeeper_Mara_Idle.A_EmbermereFenwatchKeeper_Mara_Idle"
)


def fail(message):
    unreal.log_error("Embermere keeper Idle clock probe failed: {}".format(message))
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
    presentations = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(
            world, unreal.EmbermereNpcPresentationActor
        )
        if actor_label(actor) == PRESENTATION_LABEL
    ]
    quest_actors = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
        if actor_label(actor) == QUEST_ACTOR_LABEL
    ]
    if len(presentations) != 1:
        fail("expected one live keeper presentation, found {}".format(len(presentations)))
    if len(quest_actors) != 1:
        fail("expected one live Mara gameplay actor, found {}".format(len(quest_actors)))

    presentation = presentations[0]
    quest_actor = quest_actors[0]
    skeletal_component = presentation.get_component_by_class(unreal.SkeletalMeshComponent)
    if not skeletal_component:
        fail("live presentation has no SkeletalMeshComponent")
    if presentation.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.SKELETAL_MESH:
        fail("live presentation is not using the skeletal lane")
    if presentation.get_resolved_animation_mode() != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE:
        fail("live presentation is not using single-node Idle")
    animation = presentation.get_editor_property("idle_animation")
    animation_path = animation.get_path_name() if animation else "None"
    if animation_path != EXPECTED_IDLE_PATH:
        fail("live animation drifted: {}".format(animation_path))
    position = float(skeletal_component.get_position())
    if position < 0.0 or position > 3.61:
        fail("live playback position is outside the Idle cycle: {}".format(position))
    if not skeletal_component.is_playing():
        fail("live Idle is not playing")
    if not presentation.is_presentation_collision_disabled():
        fail("live animated presentation collision is enabled")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("live presentation unexpectedly owns interaction")
    if not quest_actor.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("live Mara gameplay actor lost interaction authority")

    unreal.log(
        "Embermere keeper Idle clock sample: position={:.6f}, playing=true, "
        "animation={}, mode=SingleNodeIdle, collision=NoCollision, "
        "quest_authority=BP_QuestGiver".format(position, animation_path)
    )


if __name__ == "__main__":
    main()
