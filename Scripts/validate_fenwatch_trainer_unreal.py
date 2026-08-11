"""Validate Fenwatch trainer data, service separation, and saved placement."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
OFFERINGS_PATH = "/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings"
MESH_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/"
    "SM_EmbermereFenwatchArmsmaster_01"
)
SKELETAL_MESH_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/"
    "SK_EmbermereFenwatchArmsmaster_01"
)
IDLE_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/Animations/"
    "A_EmbermereFenwatchArmsmaster_Idle"
)
SERVICE_LABEL = "Embermere_FenwatchArmsmaster_Service_01"
PRESENTATION_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
EXPECTED_LOCATION = unreal.Vector(-1320.0, -920.0, 0.0)
EXPECTED_YAW = 100.0


def fail(message):
    unreal.log_error("Embermere Fenwatch trainer validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def main():
    offerings = unreal.EditorAssetLibrary.load_asset(OFFERINGS_PATH)
    if not offerings or not isinstance(offerings, unreal.EmbermereTrainerOfferingsData):
        fail("offerings asset is missing or has the wrong class")
    entries = list(offerings.get_editor_property("offerings"))
    if str(offerings.get_editor_property("trainer_name")) != "Fenwatch Training":
        fail("trainer name drifted")
    if len(entries) != 1:
        fail("expected one training offering, found {}".format(len(entries)))
    entry = entries[0]
    if str(entry.get_editor_property("offering_id")) != "CombatDrills":
        fail("offering ID drifted")
    if str(entry.get_editor_property("display_name")) != "Combat Drills":
        fail("offering display name drifted")
    if int(entry.get_editor_property("copper_cost")) != 10:
        fail("offering copper cost drifted")
    if int(entry.get_editor_property("required_level")) != 1:
        fail("offering level requirement drifted")
    if int(entry.get_editor_property("experience_reward")) != 25:
        fail("offering XP reward drifted")

    mesh = unreal.EditorAssetLibrary.load_asset(MESH_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("armsmaster mesh is missing")
    if mesh.get_num_triangles(0) != 2800:
        fail("armsmaster triangle count drifted")
    skeletal_mesh = unreal.EditorAssetLibrary.load_asset(SKELETAL_MESH_PATH)
    idle = unreal.EditorAssetLibrary.load_asset(IDLE_PATH)
    if not skeletal_mesh or not isinstance(skeletal_mesh, unreal.SkeletalMesh):
        fail("rigged armsmaster mesh is missing")
    if not idle or not isinstance(idle, unreal.AnimSequence):
        fail("rigged armsmaster Idle is missing")
    if idle.get_editor_property("skeleton") != skeletal_mesh.get_editor_property("skeleton"):
        fail("rigged armsmaster mesh and Idle do not share one Skeleton")

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    services = []
    presentations = []
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == SERVICE_LABEL:
            services.append(actor)
        elif label == PRESENTATION_LABEL:
            presentations.append(actor)
    if len(services) != 1:
        fail("expected one saved trainer service, found {}".format(len(services)))
    if len(presentations) != 1:
        fail("expected one saved armsmaster presentation, found {}".format(len(presentations)))

    service = services[0]
    presentation = presentations[0]
    if not isinstance(service, unreal.EmbermereTrainerServiceActor):
        fail("saved service uses the wrong native class")
    if not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("saved presentation uses the wrong native class")
    if (service.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("service location drifted: {}".format(service.get_actor_location()))
    if (presentation.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("presentation location drifted: {}".format(presentation.get_actor_location()))
    if abs(float(service.get_actor_rotation().yaw) - EXPECTED_YAW) > 0.1:
        fail("service yaw drifted: {}".format(service.get_actor_rotation().yaw))
    if abs(float(presentation.get_actor_rotation().yaw) - EXPECTED_YAW) > 0.1:
        fail("presentation yaw drifted: {}".format(presentation.get_actor_rotation().yaw))

    tags = {str(tag) for tag in service.get_editor_property("tags")}
    if not {"EmbermereGameplayService", "EmbermereTrainer"}.issubset(tags):
        fail("service tags drifted: {}".format(sorted(tags)))
    trainer = service.get_component_by_class(unreal.EmbermereTrainerComponent)
    interactable = service.get_component_by_class(unreal.EmbermereInteractableComponent)
    if not trainer or trainer.get_editor_property("offerings_data") != offerings:
        fail("service does not reference the reviewed offerings data")
    if not interactable:
        fail("service is not interactable")
    if str(interactable.get_editor_property("display_name")) != "Fenwatch Armsmaster":
        fail("interaction display name drifted")
    if not bool(interactable.get_editor_property("show_world_marker")):
        fail("interaction marker is disabled")
    if abs(float(interactable.get_editor_property("marker_height")) - 238.0) > 0.1:
        fail("interaction marker height drifted")
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("service unexpectedly owns static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("service unexpectedly owns skeletal art")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation unexpectedly owns interaction")
    if presentation.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("presentation unexpectedly owns trainer behavior")
    if presentation.get_editor_property("static_visual_mesh") != mesh:
        fail("presentation does not reference the reviewed armsmaster mesh")
    if presentation.get_editor_property("skeletal_visual_mesh") != skeletal_mesh:
        fail("presentation does not reference the reviewed rigged armsmaster mesh")
    if presentation.get_editor_property("idle_animation") != idle:
        fail("presentation does not reference the reviewed armsmaster Idle")
    if not bool(presentation.get_editor_property("prefer_skeletal_visual")):
        fail("presentation no longer prefers the rigged armsmaster lane")
    if not presentation.is_presentation_collision_disabled():
        fail("presentation collision is enabled")
    if presentation.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.SKELETAL_MESH:
        fail("presentation no longer resolves through the skeletal lane")
    if presentation.get_resolved_animation_mode() != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE:
        fail("presentation no longer resolves the single-node Idle lane")

    unreal.log(
        "Embermere Fenwatch trainer validation passed: one data-driven Combat Drills offering, one saved art-free service, one grounded rigged Idle presentation with static fallback, and trainer/art ownership separation intact"
    )


if __name__ == "__main__":
    main()
