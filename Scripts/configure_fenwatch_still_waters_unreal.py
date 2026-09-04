"""Create Still Waters data and place its art-free notice-board quest owner."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
QUEST_PATH = "/Game/Data/Quests/DQ_FenwatchStillWaters"
QUEST_SERVICE_LABEL = "Embermere_FenwatchNoticeBoard_StillWatersService_01"
NOTICE_BOARD_LABEL = "Embermere_FenwatchNoticeBoard_Road_01"
REST_SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
REST_PRESENTATION_LABEL = "Embermere_FenwatchCommunalWell_RestPresentation_01"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
SERVICE_LOCATION = (-1560.0, -260.0, 0.0)
SERVICE_YAW = -35.0
SERVICE_FOLDER = "01_Village/NPCs/Services"
SERVICE_TAGS = [
    unreal.Name("EmbermereGameplayService"),
    unreal.Name("EmbermereQuestService"),
    unreal.Name("EmbermereStillWaters"),
]

AVAILABLE_COPY = (
    "A fresh notice asks travelers to test the emberlit water at "
    "Fenwatch's communal well."
)
ACTIVE_COPY = (
    "Still Waters: Complete one successful rest at the Fenwatch communal well."
)
READY_COPY = "The well's emberlit water answered. Mark the notice complete."
COMPLETED_COPY = "The Still Waters notice bears your completed mark."
OBJECTIVE_INSTRUCTIONS = "Complete a rest at the communal well."


def fail(message):
    unreal.log_error(
        "Embermere Still Waters integration failed: {}".format(message)
    )
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


def load_or_create_quest():
    quest = (
        unreal.EditorAssetLibrary.load_asset(QUEST_PATH)
        if unreal.EditorAssetLibrary.does_asset_exist(QUEST_PATH)
        else None
    )
    if quest and not isinstance(quest, unreal.EmbermereQuestData):
        fail("{} exists with the wrong class".format(QUEST_PATH))
    if not quest:
        package_path, asset_name = QUEST_PATH.rsplit("/", 1)
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.EmbermereQuestData)
        quest = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            asset_name,
            package_path,
            unreal.EmbermereQuestData,
            factory,
        )
    if not quest:
        fail("could not create {}".format(QUEST_PATH))

    quest.set_editor_property("quest_id", unreal.Name("FenwatchStillWaters"))
    quest.set_editor_property("title", "Still Waters")
    quest.set_editor_property(
        "description",
        "Rest once at the Fenwatch communal well, then return to the notice board.",
    )
    quest.set_editor_property("objective_instructions", OBJECTIVE_INSTRUCTIONS)
    quest.set_editor_property("available_greeting", AVAILABLE_COPY)
    quest.set_editor_property("active_greeting", ACTIVE_COPY)
    quest.set_editor_property("ready_greeting", READY_COPY)
    quest.set_editor_property("completed_greeting", COMPLETED_COPY)
    quest.set_editor_property(
        "objective_id", unreal.Name("FenwatchRestCompleted")
    )
    quest.set_editor_property("required_objective_count", 1)
    quest.set_editor_property("reward_experience", 50)
    quest.set_editor_property("reward_copper", 10)
    quest.set_editor_property("reward_item", None)
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        quest, only_if_is_dirty=False
    ):
        fail("could not save {}".format(QUEST_PATH))
    return quest


def place_service(quest):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    notice_board = None
    rest_service = None
    rest_presentation = None
    well = None
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        label = actor_label(actor)
        if label == QUEST_SERVICE_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(label)
        elif label == NOTICE_BOARD_LABEL:
            notice_board = actor
        elif label == REST_SERVICE_LABEL:
            rest_service = actor
        elif label == REST_PRESENTATION_LABEL:
            rest_presentation = actor
        elif label == WELL_LABEL:
            well = actor

    if not notice_board or not isinstance(notice_board, unreal.StaticMeshActor):
        fail("accepted notice-board art is missing")
    if not rest_service or not isinstance(
        rest_service, unreal.EmbermereRestServiceActor
    ):
        fail("accepted communal-well rest service is missing")
    if not rest_presentation or not isinstance(
        rest_presentation, unreal.EmbermereRestPresentationActor
    ):
        fail("accepted communal-well rest presentation is missing")
    if not well or not isinstance(well, unreal.StaticMeshActor):
        fail("accepted communal-well art is missing")

    quest_service = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermereRestQuestServiceActor,
        unreal.Vector(*SERVICE_LOCATION),
        make_rotation(SERVICE_YAW),
        False,
    )
    if not quest_service:
        fail("could not spawn {}".format(QUEST_SERVICE_LABEL))

    quest_service.set_actor_label(QUEST_SERVICE_LABEL)
    quest_service.set_folder_path(unreal.Name(SERVICE_FOLDER))
    quest_service.set_editor_property("tags", SERVICE_TAGS)
    quest_service.set_actor_enable_collision(False)
    interactable = quest_service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    router = quest_service.get_component_by_class(
        unreal.EmbermereRestQuestObjectiveRouterComponent
    )
    if not interactable or not router:
        fail("quest service is missing interaction or objective routing")

    interactable.set_editor_property("display_name", "Fenwatch Notice Board")
    interactable.set_editor_property("dialogue_text", AVAILABLE_COPY)
    interactable.set_editor_property("quest_to_offer", quest)
    interactable.set_editor_property("use_quest_state_dialogue", True)
    interactable.set_editor_property("show_world_marker", True)
    interactable.set_editor_property("marker_height", 305.0)
    router.set_editor_property("observed_rest_service", rest_service)
    router.set_editor_property("quest_id", unreal.Name("FenwatchStillWaters"))
    router.set_editor_property(
        "objective_id", unreal.Name("FenwatchRestCompleted")
    )

    forbidden_art = (
        unreal.StaticMeshComponent,
        unreal.SkeletalMeshComponent,
    )
    for component_class in forbidden_art:
        if quest_service.get_component_by_class(component_class):
            fail("quest service owns forbidden art {}".format(
                component_class.get_name()
            ))
    if quest_service.get_component_by_class(unreal.EmbermereRestServiceComponent):
        fail("quest service unexpectedly owns rest authority")

    quest_authority_components = (
        unreal.EmbermereInteractableComponent,
        unreal.EmbermereRestQuestObjectiveRouterComponent,
    )
    for art_actor in (notice_board, well, rest_presentation):
        for component_class in quest_authority_components:
            if art_actor.get_component_by_class(component_class):
                fail("{} unexpectedly owns quest authority".format(
                    actor_label(art_actor)
                ))
    if rest_service.get_component_by_class(
        unreal.EmbermereRestQuestObjectiveRouterComponent
    ):
        fail("rest service unexpectedly owns quest routing")
    rest_interactable = rest_service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not rest_interactable:
        fail("rest service lost its standard interaction")
    if rest_interactable.get_editor_property("quest_to_offer"):
        fail("rest service unexpectedly owns quest data")

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return removed


def main():
    quest = load_or_create_quest()
    removed = place_service(quest)
    unreal.log(
        "Embermere Still Waters integration passed: quest={}, service={}, "
        "quest_id=FenwatchStillWaters, objective_id=FenwatchRestCompleted, "
        "requirement=1 committed rest Success, rewards=50 XP and 10 copper, "
        "no item, four-state quest copy, art/rest/presentation separation "
        "intact, original_art_count_unchanged, removed={}".format(
            QUEST_PATH,
            QUEST_SERVICE_LABEL,
            removed,
        )
    )


if __name__ == "__main__":
    main()
