"""Validate Mara's quest-owned, presentation-only contextual greeting."""

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
        "Embermere Fenwatch keeper greeting validation failed: {}".format(message)
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
        fail("starter quest data is missing")
    for property_name, expected in GREETING_COPY.items():
        actual = str(quest.get_editor_property(property_name))
        if actual != expected:
            fail("{} drifted: {!r}".format(property_name, actual))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = list(unreal.EditorLevelLibrary.get_all_level_actors())
    quest_actors = [
        actor for actor in actors if actor_label(actor) == QUEST_ACTOR_LABEL
    ]
    presentations = [
        actor for actor in actors if actor_label(actor) == PRESENTATION_LABEL
    ]
    if len(quest_actors) != 1 or len(presentations) != 1:
        fail(
            "expected one authority and one presentation, found {} and {}".format(
                len(quest_actors), len(presentations)
            )
        )

    authority = quest_actors[0]
    presentation = presentations[0]
    interactable = authority.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not interactable:
        fail("Mara authority lost its interactable component")
    if interactable.get_editor_property("quest_to_offer") != quest:
        fail("Mara authority lost the exact starter quest")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation unexpectedly owns interaction or quest authority")
    if presentation.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("presentation unexpectedly owns vendor authority")
    if presentation.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("presentation unexpectedly owns trainer authority")

    if not bool(presentation.get_editor_property("enable_context_greeting")):
        fail("context greeting is not enabled")
    if presentation.get_editor_property("context_authority_actor") != authority:
        fail("context greeting does not observe the exact Mara authority actor")
    if abs(
        float(presentation.get_editor_property("context_greeting_radius"))
        - GREETING_RADIUS
    ) > 0.01:
        fail("context greeting radius drifted")
    location = presentation.get_editor_property(
        "context_greeting_relative_location"
    )
    if (location - GREETING_LOCATION).length() > 0.01:
        fail("context greeting location drifted: {}".format(location))
    if not presentation.is_context_greeting_presentation_only():
        fail("context greeting contributes collision, overlap, or navigation")

    widget = presentation.get_editor_property("context_greeting_widget")
    if not widget or not isinstance(widget, unreal.WidgetComponent):
        fail("presentation lost its native widget component")
    draw_size = widget.get_editor_property("draw_size")
    if int(draw_size.x) != 320 or int(draw_size.y) != 56:
        fail("widget draw size drifted: {}".format(draw_size))
    widget_class = widget.get_editor_property("widget_class")
    if not widget_class or widget_class.get_name() != "EmbermereNpcGreetingWidget":
        fail("widget class drifted: {}".format(widget_class))
    if widget.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("widget collision is enabled")

    unreal.log(
        "Embermere Fenwatch keeper greeting validation passed: exact four-state "
        "quest copy, one explicit authority reference, fixed 320x56 native "
        "presentation, NoCollision, and no dialogue, quest, reward, vendor, or "
        "trainer authority on the wrapper"
    )


if __name__ == "__main__":
    main()
