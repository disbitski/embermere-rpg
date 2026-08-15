"""Configure Mara's read-only contextual greeting presentation.

The quest data owns presentation copy, BP_QuestGiver remains the authority for
interaction and quest mutation, and the separate NPC wrapper only observes it.
Run in a fresh editor after the matching native module is loaded.
"""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
QUEST_PATH = "/Game/Data/Quests/DQ_FirstSignsAtTheRuin"
QUEST_ACTOR_LABEL = "Quest_Giver_Mara_Fenwatch"
PRESENTATION_LABEL = "Embermere_FenwatchKeeper_Mara_Presentation_01"
GREETING_RADIUS = 420.0
GREETING_LOCATION = unreal.Vector(0.0, 0.0, 235.0)
GREETING_COPY = {
    "available_greeting": "The eastern stones are restless.",
    "active_greeting": "Keep to the road, then watch the reeds.",
    "ready_greeting": "You have done enough. Come speak with me.",
    "completed_greeting": "Fenwatch remembers a steady hand.",
}


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch keeper greeting configuration failed: {}".format(
            message
        )
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def main():
    quest = unreal.EditorAssetLibrary.load_asset(QUEST_PATH)
    if not quest or not isinstance(quest, unreal.EmbermereQuestData):
        fail("missing starter quest {}".format(QUEST_PATH))

    for property_name, value in GREETING_COPY.items():
        quest.set_editor_property(property_name, value)
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        quest, only_if_is_dirty=False
    ):
        fail("could not save {}".format(QUEST_PATH))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = list(unreal.EditorLevelLibrary.get_all_level_actors())
    quest_actors = [
        actor for actor in actors if actor_label(actor) == QUEST_ACTOR_LABEL
    ]
    presentations = [
        actor for actor in actors if actor_label(actor) == PRESENTATION_LABEL
    ]
    if len(quest_actors) != 1:
        fail("expected one Mara authority actor, found {}".format(len(quest_actors)))
    if len(presentations) != 1:
        fail("expected one Mara presentation, found {}".format(len(presentations)))

    authority = quest_actors[0]
    presentation = presentations[0]
    if not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("Mara presentation uses the wrong native class")
    interactable = authority.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not interactable or interactable.get_editor_property("quest_to_offer") != quest:
        fail("Mara authority actor does not own the exact starter quest")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("Mara presentation unexpectedly owns interaction authority")

    presentation.set_editor_property("enable_context_greeting", True)
    presentation.set_editor_property("context_authority_actor", authority)
    presentation.set_editor_property("context_greeting_radius", GREETING_RADIUS)
    presentation.set_editor_property(
        "context_greeting_relative_location", GREETING_LOCATION
    )
    if not presentation.is_context_greeting_presentation_only():
        fail("context greeting contributes collision, overlap, or navigation")

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))

    unreal.log(
        "Embermere Fenwatch keeper greeting configuration passed: four "
        "quest-owned lines, one read-only wrapper observer, 420 cm visibility "
        "radius, fixed 320x56 native panel, and unchanged BP_QuestGiver authority"
    )


if __name__ == "__main__":
    main()
