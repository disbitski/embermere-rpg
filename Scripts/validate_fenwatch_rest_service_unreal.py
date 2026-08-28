"""Validate Fenwatch communal-well rest data, service, and art separation."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
REST_DATA_PATH = "/Game/Data/Services/DA_FenwatchCommunalWellRest"
SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
EXPECTED_LOCATION = unreal.Vector(-950.0, -1600.0, 0.0)
EXPECTED_YAW = -135.0


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch rest-service validation failed: {}".format(message)
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def main():
    rest_data = unreal.EditorAssetLibrary.load_asset(REST_DATA_PATH)
    if not rest_data or not isinstance(rest_data, unreal.EmbermereRestServiceData):
        fail("rest definition is missing or has the wrong class")

    expected_data = {
        "service_id": "FenwatchCommunalWellRest",
        "display_name": "Fenwatch Communal Well",
        "prompt_text": "Cool emberlit water gathers beneath the old stone.",
        "resting_text": "Resting at Fenwatch Communal Well. Remain still.",
    }
    for property_name, expected in expected_data.items():
        if str(rest_data.get_editor_property(property_name)) != expected:
            fail("{} drifted".format(property_name))
    expected_numbers = {
        "interaction_radius": 300.0,
        "channel_duration_seconds": 1.5,
        "cooldown_seconds": 12.0,
        "movement_interrupt_distance": 35.0,
    }
    for property_name, expected in expected_numbers.items():
        actual = float(rest_data.get_editor_property(property_name))
        if abs(actual - expected) > 0.001:
            fail("{} drifted: {}".format(property_name, actual))
    if not bool(rest_data.get_editor_property("restore_health")):
        fail("health recovery is disabled")
    if not bool(rest_data.get_editor_property("restore_mana")):
        fail("mana recovery is disabled")

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    services = []
    wells = []
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == SERVICE_LABEL:
            services.append(actor)
        elif label == WELL_LABEL:
            wells.append(actor)
    if len(services) != 1:
        fail("expected one saved rest service, found {}".format(len(services)))
    if len(wells) != 1:
        fail("expected one saved communal well, found {}".format(len(wells)))

    service = services[0]
    well = wells[0]
    if not isinstance(service, unreal.EmbermereRestServiceActor):
        fail("saved service uses the wrong native class")
    if not isinstance(well, unreal.StaticMeshActor):
        fail("saved well no longer uses art-only StaticMeshActor")
    if (service.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("service location drifted: {}".format(service.get_actor_location()))
    yaw_delta = abs(
        (float(service.get_actor_rotation().yaw) - EXPECTED_YAW + 180.0)
        % 360.0
        - 180.0
    )
    if yaw_delta > 0.1:
        fail("service yaw drifted: {}".format(service.get_actor_rotation().yaw))
    if (well.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("art and service are no longer colocated")

    tags = {str(tag) for tag in service.get_editor_property("tags")}
    if not {"EmbermereGameplayService", "EmbermereRestService"}.issubset(tags):
        fail("service tags drifted: {}".format(sorted(tags)))
    if "EmbermereOriginalArt" in tags:
        fail("gameplay service must remain outside the original-art count")
    if service.get_actor_enable_collision():
        fail("art-free rest service unexpectedly enables actor collision")

    rest_service = service.get_component_by_class(
        unreal.EmbermereRestServiceComponent
    )
    interactable = service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not rest_service or not interactable:
        fail("service is missing rest or interaction authority")
    if rest_service.get_editor_property("rest_data") != rest_data:
        fail("service does not reference the reviewed rest definition")
    if str(interactable.get_editor_property("display_name")) != expected_data[
        "display_name"
    ]:
        fail("interaction display name drifted")
    if str(interactable.get_editor_property("dialogue_text")) != expected_data[
        "prompt_text"
    ]:
        fail("interaction prompt drifted")
    if not bool(interactable.get_editor_property("show_world_marker")):
        fail("interaction marker is disabled")
    if abs(float(interactable.get_editor_property("marker_height")) - 355.0) > 0.1:
        fail("interaction marker height drifted")
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("service unexpectedly owns static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("service unexpectedly owns skeletal art")
    if service.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("rest service unexpectedly owns vendor authority")
    if service.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("rest service unexpectedly owns trainer authority")

    if well.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("well art unexpectedly owns interaction")
    if well.get_component_by_class(unreal.EmbermereRestServiceComponent):
        fail("well art unexpectedly owns recovery authority")

    unreal.log(
        "Embermere Fenwatch rest-service validation passed: one saved data-driven art-free service colocated with presentation-only communal-well art; exact 300 cm range, 1.5 second channel, 35 cm movement interruption, 12 second session cooldown, full health/mana recovery, native interaction copy, and ownership separation intact"
    )


if __name__ == "__main__":
    main()
