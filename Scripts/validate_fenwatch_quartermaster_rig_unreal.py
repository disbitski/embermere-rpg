"""Validate saved rigged Fenwatch quartermaster packages and ownership."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ART_ROOT = "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster"
STATIC_PATH = ART_ROOT + "/SM_EmbermereFenwatchQuartermaster_01"
SKELETAL_PATH = ART_ROOT + "/SK_EmbermereFenwatchQuartermaster_01"
IDLE_PATH = ART_ROOT + "/Animations/A_EmbermereFenwatchQuartermaster_Idle"
PHYSICS_PATH = SKELETAL_PATH + "_PhysicsAsset"
PRESENTATION_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
SERVICE_LABEL = "Embermere_FenwatchQuartermaster_Service_01"
EXPECTED_LOCATION = unreal.Vector(-1530.0, -1190.0, 0.0)
EXPECTED_YAW = 100.0

EXPECTED_MATERIALS = {
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/M_FenwatchQuartermasterSkin.M_FenwatchQuartermasterSkin",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
}


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch quartermaster rig validation failed: {}".format(message)
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def main():
    static_mesh = unreal.EditorAssetLibrary.load_asset(STATIC_PATH)
    skeletal_mesh = unreal.EditorAssetLibrary.load_asset(SKELETAL_PATH)
    idle = unreal.EditorAssetLibrary.load_asset(IDLE_PATH)
    if not static_mesh or not isinstance(static_mesh, unreal.StaticMesh):
        fail("accepted static fallback is missing")
    if not skeletal_mesh or not isinstance(skeletal_mesh, unreal.SkeletalMesh):
        fail("rigged skeletal mesh is missing")
    if not idle or not isinstance(idle, unreal.AnimSequence):
        fail("rigged Idle animation is missing")

    import_data = skeletal_mesh.get_editor_property("asset_import_data")
    import_class = import_data.get_class().get_name() if import_data else "None"
    if import_class != "FbxSkeletalMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_class))
    if unreal.EditorAssetLibrary.does_asset_exist(PHYSICS_PATH):
        fail("presentation-only rig unexpectedly has a PhysicsAsset")

    skeleton = skeletal_mesh.get_editor_property("skeleton")
    if not skeleton or idle.get_editor_property("skeleton") != skeleton:
        fail("skeletal mesh and Idle do not share one Skeleton")
    if abs(float(idle.get_play_length()) - 4.0) > 0.12:
        fail("Idle duration drifted: {}".format(idle.get_play_length()))

    bounds = skeletal_mesh.get_bounds()
    size = bounds.box_extent * 2.0
    expected_size = (120.842, 93.0, 217.0)
    if any(
        abs(actual - expected) > 1.0
        for actual, expected in zip(
            (float(size.x), float(size.y), float(size.z)), expected_size
        )
    ):
        fail("skeletal bounds drifted: {}".format(size))

    material_paths = set()
    for skeletal_material in list(skeletal_mesh.get_editor_property("materials")):
        material = skeletal_material.get_editor_property("material_interface")
        if material:
            material_paths.add(material.get_path_name())
    if material_paths != EXPECTED_MATERIALS:
        fail("skeletal material assignments drifted: {}".format(sorted(material_paths)))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    presentations = []
    services = []
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == PRESENTATION_LABEL:
            presentations.append(actor)
        elif label == SERVICE_LABEL:
            services.append(actor)
    if len(presentations) != 1:
        fail("expected one saved presentation, found {}".format(len(presentations)))
    if len(services) != 1:
        fail("expected one saved vendor service, found {}".format(len(services)))

    presentation = presentations[0]
    service = services[0]
    if not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("saved presentation uses the wrong native class")
    if not isinstance(service, unreal.EmbermereVendorServiceActor):
        fail("saved vendor service uses the wrong native class")
    for actor, role in ((presentation, "presentation"), (service, "service")):
        if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
            fail("{} location drifted: {}".format(role, actor.get_actor_location()))
        if abs(float(actor.get_actor_rotation().yaw) - EXPECTED_YAW) > 0.1:
            fail("{} yaw drifted: {}".format(role, actor.get_actor_rotation().yaw))

    if presentation.get_editor_property("static_visual_mesh") != static_mesh:
        fail("accepted static fallback reference is missing")
    if presentation.get_editor_property("skeletal_visual_mesh") != skeletal_mesh:
        fail("saved presentation does not reference the rigged mesh")
    if presentation.get_editor_property("idle_animation") != idle:
        fail("saved presentation does not reference the reviewed Idle")
    if not bool(presentation.get_editor_property("prefer_skeletal_visual")):
        fail("saved presentation no longer prefers the skeletal lane")
    if not bool(presentation.get_editor_property("loop_idle_animation")):
        fail("saved presentation no longer loops Idle")
    if abs(float(presentation.get_editor_property("idle_animation_play_rate")) - 1.0) > 0.001:
        fail("saved Idle play rate drifted")
    if (
        presentation.get_resolved_visual_mode()
        != unreal.EmbermereNpcVisualMode.SKELETAL_MESH
    ):
        fail("saved presentation does not resolve through the skeletal lane")
    if (
        presentation.get_resolved_animation_mode()
        != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE
    ):
        fail("saved presentation does not resolve single-node Idle")
    if not presentation.is_presentation_collision_disabled():
        fail("saved presentation collision is enabled")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation unexpectedly owns interaction")
    if presentation.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("presentation unexpectedly owns vendor authority")
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("vendor service unexpectedly owns static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("vendor service unexpectedly owns skeletal art")

    unreal.log(
        "Embermere Fenwatch quartermaster rig validation passed: classic skeletal FBX packages, shared Skeleton and 4.0-second Idle, exact static fallback, one saved art-only skeletal presentation, and separate vendor authority"
    )


if __name__ == "__main__":
    main()
