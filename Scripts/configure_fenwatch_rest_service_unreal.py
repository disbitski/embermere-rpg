"""Create Fenwatch rest data and place its art-free communal-well service."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
REST_DATA_PATH = "/Game/Data/Services/DA_FenwatchCommunalWellRest"
SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
SERVICE_LOCATION = (-950.0, -1600.0, 0.0)
SERVICE_YAW = -135.0
SERVICE_FOLDER = "01_Village/NPCs/Services"
SERVICE_TAGS = [
    unreal.Name("EmbermereGameplayService"),
    unreal.Name("EmbermereRestService"),
]


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch rest-service integration failed: {}".format(message)
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


def load_or_create_rest_data():
    rest_data = (
        unreal.EditorAssetLibrary.load_asset(REST_DATA_PATH)
        if unreal.EditorAssetLibrary.does_asset_exist(REST_DATA_PATH)
        else None
    )
    if rest_data and not isinstance(rest_data, unreal.EmbermereRestServiceData):
        fail("{} exists with the wrong class".format(REST_DATA_PATH))
    if not rest_data:
        package_path, asset_name = REST_DATA_PATH.rsplit("/", 1)
        factory = unreal.DataAssetFactory()
        factory.set_editor_property(
            "data_asset_class", unreal.EmbermereRestServiceData
        )
        rest_data = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            asset_name,
            package_path,
            unreal.EmbermereRestServiceData,
            factory,
        )
    if not rest_data:
        fail("could not create {}".format(REST_DATA_PATH))

    rest_data.set_editor_property(
        "service_id", unreal.Name("FenwatchCommunalWellRest")
    )
    rest_data.set_editor_property("display_name", "Fenwatch Communal Well")
    rest_data.set_editor_property(
        "prompt_text", "Cool emberlit water gathers beneath the old stone."
    )
    rest_data.set_editor_property(
        "resting_text", "Resting at Fenwatch Communal Well. Remain still."
    )
    rest_data.set_editor_property("interaction_radius", 300.0)
    rest_data.set_editor_property("channel_duration_seconds", 1.5)
    rest_data.set_editor_property("cooldown_seconds", 12.0)
    rest_data.set_editor_property("movement_interrupt_distance", 35.0)
    rest_data.set_editor_property("restore_health", True)
    rest_data.set_editor_property("restore_mana", True)
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        rest_data, only_if_is_dirty=False
    ):
        fail("could not save {}".format(REST_DATA_PATH))
    return rest_data


def place_service(rest_data):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    well = None
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        label = actor_label(actor)
        if label == SERVICE_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(label)
        elif label == WELL_LABEL:
            well = actor

    if not well or not isinstance(well, unreal.StaticMeshActor):
        fail("accepted communal-well art is missing")
    if well.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("communal-well art unexpectedly owns interaction")
    if well.get_component_by_class(unreal.EmbermereRestServiceComponent):
        fail("communal-well art unexpectedly owns recovery authority")

    service = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermereRestServiceActor,
        unreal.Vector(*SERVICE_LOCATION),
        make_rotation(SERVICE_YAW),
        False,
    )
    if not service:
        fail("could not spawn {}".format(SERVICE_LABEL))

    service.set_actor_label(SERVICE_LABEL)
    service.set_folder_path(unreal.Name(SERVICE_FOLDER))
    service.set_editor_property("tags", SERVICE_TAGS)
    service.set_actor_enable_collision(False)
    rest_service = service.get_component_by_class(
        unreal.EmbermereRestServiceComponent
    )
    interactable = service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not rest_service or not interactable:
        fail("service actor is missing its native components")
    rest_service.set_editor_property("rest_data", rest_data)
    interactable.set_editor_property("display_name", "Fenwatch Communal Well")
    interactable.set_editor_property(
        "dialogue_text", "Cool emberlit water gathers beneath the old stone."
    )
    interactable.set_editor_property("show_world_marker", True)
    interactable.set_editor_property("marker_height", 355.0)

    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("service actor must not own static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("service actor must not own skeletal art")
    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return removed


def main():
    rest_data = load_or_create_rest_data()
    removed = place_service(rest_data)
    unreal.log(
        "Embermere Fenwatch rest-service integration passed: data={}, service={}, channel=1.5s, cooldown=12s, radius=300cm, movement_interrupt=35cm, full health/mana recovery, art separation intact, removed={}".format(
            REST_DATA_PATH,
            SERVICE_LABEL,
            removed,
        )
    )


if __name__ == "__main__":
    main()
