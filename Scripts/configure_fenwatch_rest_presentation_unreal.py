"""Place the removable communal-well rest presentation observer."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
WELL_LABEL = "Embermere_FenwatchCommunalWell_SouthCommons_01"
SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
PRESENTATION_LABEL = "Embermere_FenwatchCommunalWell_RestPresentation_01"
PRESENTATION_LOCATION = (-950.0, -1600.0, 0.0)
PRESENTATION_YAW = -135.0
PRESENTATION_FOLDER = "01_Village/Presentation"
PRESENTATION_TAGS = [
    unreal.Name("EmbermereGameplayPresentation"),
    unreal.Name("EmbermereRestPresentation"),
]


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch rest-presentation integration failed: {}".format(
            message
        )
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


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    well = None
    service = None
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        label = actor_label(actor)
        if label == PRESENTATION_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(label)
        elif label == WELL_LABEL:
            well = actor
        elif label == SERVICE_LABEL:
            service = actor

    if not well or not isinstance(well, unreal.StaticMeshActor):
        fail("accepted communal-well art is missing")
    if not service or not isinstance(service, unreal.EmbermereRestServiceActor):
        fail("accepted communal-well rest service is missing")

    presentation = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermereRestPresentationActor,
        unreal.Vector(*PRESENTATION_LOCATION),
        make_rotation(PRESENTATION_YAW),
        False,
    )
    if not presentation:
        fail("could not spawn {}".format(PRESENTATION_LABEL))

    presentation.set_actor_label(PRESENTATION_LABEL)
    presentation.set_folder_path(unreal.Name(PRESENTATION_FOLDER))
    presentation.set_editor_property("tags", PRESENTATION_TAGS)
    presentation.set_editor_property("observed_rest_service", service)
    presentation.set_actor_enable_collision(False)

    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation observer unexpectedly owns interaction")
    if presentation.get_component_by_class(unreal.EmbermereRestServiceComponent):
        fail("presentation observer unexpectedly owns recovery authority")
    if presentation.get_component_by_class(unreal.StaticMeshComponent):
        fail("saved presentation observer must defer transient visual segments")
    if presentation.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("presentation observer unexpectedly owns skeletal art")
    if well.get_component_by_class(unreal.EmbermereRestServiceComponent):
        fail("well art unexpectedly owns recovery authority")
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("rest service unexpectedly owns presentation art")

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))

    unreal.log(
        "Embermere Fenwatch rest-presentation integration passed: observer={}, service={}, location={}, yaw={}, tags={}, deferred_segments=12, original_art_count_unchanged, removed={}".format(
            PRESENTATION_LABEL,
            SERVICE_LABEL,
            PRESENTATION_LOCATION,
            PRESENTATION_YAW,
            [str(tag) for tag in PRESENTATION_TAGS],
            removed,
        )
    )


if __name__ == "__main__":
    main()
