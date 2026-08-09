"""Create Fenwatch training data and place its art-free gameplay service."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
OFFERINGS_PATH = "/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings"
SERVICE_LABEL = "Embermere_FenwatchArmsmaster_Service_01"
PRESENTATION_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
SERVICE_LOCATION = (-1320.0, -920.0, 0.0)
SERVICE_YAW = 100.0
SERVICE_FOLDER = "01_Village/NPCs/Services"
SERVICE_TAGS = [unreal.Name("EmbermereGameplayService"), unreal.Name("EmbermereTrainer")]


def fail(message):
    unreal.log_error("Embermere Fenwatch trainer integration failed: {}".format(message))
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


def load_or_create_offerings():
    offerings = (
        unreal.EditorAssetLibrary.load_asset(OFFERINGS_PATH)
        if unreal.EditorAssetLibrary.does_asset_exist(OFFERINGS_PATH)
        else None
    )
    if offerings and not isinstance(offerings, unreal.EmbermereTrainerOfferingsData):
        fail("{} exists with the wrong class".format(OFFERINGS_PATH))
    if not offerings:
        package_path, asset_name = OFFERINGS_PATH.rsplit("/", 1)
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.EmbermereTrainerOfferingsData)
        offerings = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            asset_name,
            package_path,
            unreal.EmbermereTrainerOfferingsData,
            factory,
        )
    if not offerings:
        fail("could not create {}".format(OFFERINGS_PATH))

    combat_drills = unreal.EmbermereTrainerOffering()
    combat_drills.set_editor_property("offering_id", unreal.Name("CombatDrills"))
    combat_drills.set_editor_property("display_name", "Combat Drills")
    combat_drills.set_editor_property(
        "description",
        "Practice the Fenwatch guard forms and earn 25 experience.",
    )
    combat_drills.set_editor_property("copper_cost", 10)
    combat_drills.set_editor_property("required_level", 1)
    combat_drills.set_editor_property("experience_reward", 25)

    offerings.set_editor_property("trainer_name", "Fenwatch Training")
    offerings.set_editor_property("offerings", [combat_drills])
    if not unreal.EditorAssetLibrary.save_loaded_asset(offerings, only_if_is_dirty=False):
        fail("could not save {}".format(OFFERINGS_PATH))
    return offerings


def place_service(offerings):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    presentation = None
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        label = actor_label(actor)
        if label == SERVICE_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(label)
        elif label == PRESENTATION_LABEL:
            presentation = actor

    if not presentation or not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("accepted armsmaster presentation is missing")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("armsmaster presentation unexpectedly owns interaction")
    if presentation.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("armsmaster presentation unexpectedly owns trainer behavior")

    service = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermereTrainerServiceActor,
        unreal.Vector(*SERVICE_LOCATION),
        make_rotation(SERVICE_YAW),
        False,
    )
    if not service:
        fail("could not spawn {}".format(SERVICE_LABEL))

    service.set_actor_label(SERVICE_LABEL)
    service.set_folder_path(unreal.Name(SERVICE_FOLDER))
    service.set_editor_property("tags", SERVICE_TAGS)
    trainer = service.get_component_by_class(unreal.EmbermereTrainerComponent)
    interactable = service.get_component_by_class(unreal.EmbermereInteractableComponent)
    if not trainer or not interactable:
        fail("service actor is missing its native components")
    trainer.set_editor_property("offerings_data", offerings)
    interactable.set_editor_property("display_name", "Fenwatch Armsmaster")
    interactable.set_editor_property("dialogue_text", "")
    interactable.set_editor_property("show_world_marker", True)
    interactable.set_editor_property("marker_height", 238.0)

    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("service actor must not own static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("service actor must not own skeletal art")
    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return removed


def main():
    offerings = load_or_create_offerings()
    removed = place_service(offerings)
    unreal.log(
        "Embermere Fenwatch trainer integration passed: offerings={}, entries=1, service={}, removed={}".format(
            OFFERINGS_PATH,
            SERVICE_LABEL,
            removed,
        )
    )


if __name__ == "__main__":
    main()
