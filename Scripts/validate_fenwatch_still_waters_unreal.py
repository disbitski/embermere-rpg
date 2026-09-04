"""Validate Still Waters data, saved owner/router, and authority separation."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
QUEST_PATH = "/Game/Data/Quests/DQ_FenwatchStillWaters"
QUEST_SERVICE_LABEL = "Embermere_FenwatchNoticeBoard_StillWatersService_01"
NOTICE_BOARD_LABEL = "Embermere_FenwatchNoticeBoard_Road_01"
REST_SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
REST_PRESENTATION_LABEL = "Embermere_FenwatchCommunalWell_RestPresentation_01"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
EXPECTED_LOCATION = unreal.Vector(-1560.0, -260.0, 0.0)
EXPECTED_YAW = -35.0

EXPECTED_TEXT = {
    "title": "Still Waters",
    "description": (
        "Rest once at the Fenwatch communal well, then return to the notice board."
    ),
    "objective_instructions": "Complete a rest at the communal well.",
    "available_greeting": (
        "A fresh notice asks travelers to test the emberlit water at "
        "Fenwatch's communal well."
    ),
    "active_greeting": (
        "Still Waters: Complete one successful rest at the Fenwatch communal well."
    ),
    "ready_greeting": (
        "The well's emberlit water answered. Mark the notice complete."
    ),
    "completed_greeting": (
        "The Still Waters notice bears your completed mark."
    ),
}


def fail(message):
    unreal.log_error(
        "Embermere Still Waters validation failed: {}".format(message)
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def yaw_delta(actor, expected):
    return abs(
        (float(actor.get_actor_rotation().yaw) - expected + 180.0) % 360.0
        - 180.0
    )


def main():
    quest = unreal.EditorAssetLibrary.load_asset(QUEST_PATH)
    if not quest or not isinstance(quest, unreal.EmbermereQuestData):
        fail("quest asset is missing or has the wrong class")
    if str(quest.get_editor_property("quest_id")) != "FenwatchStillWaters":
        fail("quest ID drifted")
    if str(quest.get_editor_property("objective_id")) != "FenwatchRestCompleted":
        fail("objective ID drifted")
    for property_name, expected in EXPECTED_TEXT.items():
        if str(quest.get_editor_property(property_name)) != expected:
            fail("{} drifted".format(property_name))
    if int(quest.get_editor_property("required_objective_count")) != 1:
        fail("objective requirement drifted")
    if int(quest.get_editor_property("reward_experience")) != 50:
        fail("XP reward drifted")
    if int(quest.get_editor_property("reward_copper")) != 10:
        fail("copper reward drifted")
    if quest.get_editor_property("reward_item"):
        fail("Still Waters must not grant an item")

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    labels = {
        QUEST_SERVICE_LABEL,
        NOTICE_BOARD_LABEL,
        REST_SERVICE_LABEL,
        REST_PRESENTATION_LABEL,
        WELL_LABEL,
    }
    actors = {}
    counts = {label: 0 for label in labels}
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label in counts:
            counts[label] += 1
            actors[label] = actor
    for label, count in counts.items():
        if count != 1:
            fail("expected exactly one {}, found {}".format(label, count))

    quest_service = actors[QUEST_SERVICE_LABEL]
    notice_board = actors[NOTICE_BOARD_LABEL]
    rest_service = actors[REST_SERVICE_LABEL]
    rest_presentation = actors[REST_PRESENTATION_LABEL]
    well = actors[WELL_LABEL]
    if not isinstance(quest_service, unreal.EmbermereRestQuestServiceActor):
        fail("saved quest owner uses the wrong native class")
    if not isinstance(notice_board, unreal.StaticMeshActor):
        fail("notice-board art no longer uses StaticMeshActor")
    if not isinstance(rest_service, unreal.EmbermereRestServiceActor):
        fail("rest service uses the wrong native class")
    if not isinstance(rest_presentation, unreal.EmbermereRestPresentationActor):
        fail("rest presentation uses the wrong native class")
    if not isinstance(well, unreal.StaticMeshActor):
        fail("well art no longer uses StaticMeshActor")

    if (quest_service.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("quest service location drifted: {}".format(
            quest_service.get_actor_location()
        ))
    if yaw_delta(quest_service, EXPECTED_YAW) > 0.1:
        fail("quest service yaw drifted: {}".format(
            quest_service.get_actor_rotation().yaw
        ))
    if (notice_board.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("quest owner is no longer colocated with notice-board art")

    tags = {str(tag) for tag in quest_service.get_editor_property("tags")}
    expected_tags = {
        "EmbermereGameplayService",
        "EmbermereQuestService",
        "EmbermereStillWaters",
    }
    if not expected_tags.issubset(tags):
        fail("quest service tags drifted: {}".format(sorted(tags)))
    if "EmbermereOriginalArt" in tags:
        fail("quest service must remain outside the original-art count")
    if quest_service.get_actor_enable_collision():
        fail("quest service unexpectedly enables actor collision")

    interactable = quest_service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    router = quest_service.get_component_by_class(
        unreal.EmbermereRestQuestObjectiveRouterComponent
    )
    if not interactable or not router:
        fail("quest owner is missing interaction or objective routing")
    if interactable.get_editor_property("quest_to_offer") != quest:
        fail("quest owner does not reference Still Waters")
    if str(interactable.get_editor_property("display_name")) != "Fenwatch Notice Board":
        fail("notice-board interaction name drifted")
    if str(interactable.get_editor_property("dialogue_text")) != EXPECTED_TEXT[
        "available_greeting"
    ]:
        fail("notice-board fallback dialogue drifted")
    if not bool(interactable.get_editor_property("use_quest_state_dialogue")):
        fail("four-state quest dialogue is disabled")
    if not bool(interactable.get_editor_property("show_world_marker")):
        fail("quest marker is disabled")
    if abs(float(interactable.get_editor_property("marker_height")) - 305.0) > 0.1:
        fail("quest marker height drifted")
    if router.get_editor_property("observed_rest_service") != rest_service:
        fail("objective router does not observe the accepted rest service")
    if str(router.get_editor_property("quest_id")) != "FenwatchStillWaters":
        fail("router quest ID drifted")
    if str(router.get_editor_property("objective_id")) != "FenwatchRestCompleted":
        fail("router objective ID drifted")

    forbidden_service_components = (
        unreal.StaticMeshComponent,
        unreal.SkeletalMeshComponent,
        unreal.EmbermereRestServiceComponent,
        unreal.EmbermereVendorComponent,
        unreal.EmbermereTrainerComponent,
    )
    for component_class in forbidden_service_components:
        if quest_service.get_component_by_class(component_class):
            fail("quest service owns forbidden component {}".format(
                component_class.get_name()
            ))

    for art_actor in (notice_board, well, rest_presentation):
        if art_actor.get_component_by_class(unreal.EmbermereInteractableComponent):
            fail("{} unexpectedly owns quest interaction".format(
                actor_label(art_actor)
            ))
        if art_actor.get_component_by_class(
            unreal.EmbermereRestQuestObjectiveRouterComponent
        ):
            fail("{} unexpectedly owns quest routing".format(
                actor_label(art_actor)
            ))
    if rest_service.get_component_by_class(
        unreal.EmbermereRestQuestObjectiveRouterComponent
    ):
        fail("rest service unexpectedly owns quest routing")
    rest_interactable = rest_service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not rest_interactable or rest_interactable.get_editor_property(
        "quest_to_offer"
    ):
        fail("rest service unexpectedly owns quest data")

    unreal.log(
        "Embermere Still Waters validation passed: one saved data-driven "
        "FenwatchStillWaters quest with FenwatchRestCompleted requirement 1, "
        "50 XP and 10 copper rewards, no item; one collision-free art-free "
        "notice-board owner with four-state dialogue and one exact committed-"
        "rest router; notice board, well, rest service, and rest presentation "
        "remain quest-free; original-art count unchanged"
    )


if __name__ == "__main__":
    main()
