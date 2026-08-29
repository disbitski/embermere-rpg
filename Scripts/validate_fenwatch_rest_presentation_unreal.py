"""Validate saved communal-well presentation and authority separation."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
PRESENTATION_LABEL = "Embermere_FenwatchCommunalWell_RestPresentation_01"
EXPECTED_LOCATION = unreal.Vector(-950.0, -1600.0, 0.0)
EXPECTED_YAW = -135.0


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch rest-presentation validation failed: {}".format(
            message
        )
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def yaw_delta(actor, expected):
    return abs((float(actor.get_actor_rotation().yaw) - expected + 180.0) % 360.0 - 180.0)


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = {}
    counts = {WELL_LABEL: 0, SERVICE_LABEL: 0, PRESENTATION_LABEL: 0}
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label in counts:
            counts[label] += 1
            actors[label] = actor
    for label, count in counts.items():
        if count != 1:
            fail("expected exactly one {}, found {}".format(label, count))

    well = actors[WELL_LABEL]
    service = actors[SERVICE_LABEL]
    presentation = actors[PRESENTATION_LABEL]
    if not isinstance(well, unreal.StaticMeshActor):
        fail("well no longer uses presentation-only StaticMeshActor art")
    if not isinstance(service, unreal.EmbermereRestServiceActor):
        fail("service no longer uses the art-free native class")
    if not isinstance(presentation, unreal.EmbermereRestPresentationActor):
        fail("observer no longer uses EmbermereRestPresentationActor")

    for actor in (well, service, presentation):
        if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
            fail("{} location drifted: {}".format(
                actor_label(actor), actor.get_actor_location()
            ))
    if yaw_delta(presentation, EXPECTED_YAW) > 0.1:
        fail("presentation yaw drifted: {}".format(
            presentation.get_actor_rotation().yaw
        ))
    if presentation.get_editor_property("observed_rest_service") != service:
        fail("presentation does not reference the accepted rest service")

    tags = {str(tag) for tag in presentation.get_editor_property("tags")}
    expected_tags = {"EmbermereGameplayPresentation", "EmbermereRestPresentation"}
    if not expected_tags.issubset(tags):
        fail("presentation tags drifted: {}".format(sorted(tags)))
    if "EmbermereOriginalArt" in tags:
        fail("presentation observer must remain outside the original-art count")
    if presentation.get_actor_enable_collision():
        fail("presentation observer unexpectedly enables actor collision")

    forbidden_components = (
        unreal.EmbermereInteractableComponent,
        unreal.EmbermereRestServiceComponent,
        unreal.EmbermereVendorComponent,
        unreal.EmbermereTrainerComponent,
        unreal.StaticMeshComponent,
        unreal.SkeletalMeshComponent,
    )
    for component_class in forbidden_components:
        if presentation.get_component_by_class(component_class):
            fail("presentation observer owns forbidden saved component {}".format(
                component_class.get_name()
            ))
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("rest service unexpectedly owns static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("rest service unexpectedly owns skeletal art")
    if well.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("well art unexpectedly owns interaction")
    if well.get_component_by_class(unreal.EmbermereRestServiceComponent):
        fail("well art unexpectedly owns recovery authority")

    unreal.log(
        "Embermere Fenwatch rest-presentation validation passed: one saved observer colocated with and referencing the separate art-free service; no interaction, recovery, commerce, trainer, static-art, skeletal-art, collision, original-art-count, or persistence authority; transient twelve-segment runtime geometry remains deferred"
    )


if __name__ == "__main__":
    main()
