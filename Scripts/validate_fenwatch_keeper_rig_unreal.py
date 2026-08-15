"""Validate rigged Mara packages and the Blueprint-backed ownership boundary."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
BLUEPRINT_PATH = "/Game/Blueprints/BP_QuestGiver"
QUEST_PATH = "/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"
ART_ROOT = "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper"
STATIC_PATH = ART_ROOT + "/SM_EmbermereFenwatchKeeper_Mara_01"
SKELETAL_PATH = ART_ROOT + "/SK_EmbermereFenwatchKeeper_Mara_01"
IDLE_PATH = ART_ROOT + "/Animations/A_EmbermereFenwatchKeeper_Mara_Idle"
PHYSICS_PATH = SKELETAL_PATH + "_PhysicsAsset"
QUEST_ACTOR_LABEL = "Quest_Giver_Mara_Fenwatch"
PRESENTATION_LABEL = "Embermere_FenwatchKeeper_Mara_Presentation_01"
QUEST_LOCATION = unreal.Vector(-2050.0, -850.0, 140.0)
QUEST_YAW = 35.0
PRESENTATION_LOCATION = unreal.Vector(-2050.0, -850.0, 0.0)
PRESENTATION_YAW = 135.0
MARA_DISPLAY_NAME = "Mara Fenwatch"
MARA_DIALOGUE = (
    "You picked a lively day to arrive. Something is stirring near the old "
    "stones east of town. Cull a few of those things and come back whole."
)
MARA_MARKER_HEIGHT = 185.0
GREETING_RADIUS = 420.0
GREETING_LOCATION = unreal.Vector(0.0, 0.0, 235.0)
GREETING_COPY = {
    "available_greeting": "The eastern stones are restless.",
    "active_greeting": "Keep to the road, then watch the reeds.",
    "ready_greeting": "You have done enough. Come speak with me.",
    "completed_greeting": "Fenwatch remembers a steady hand.",
}
ORIGINAL_TAG = unreal.Name("EmbermereOriginalArt")

EXPECTED_MATERIALS = {
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/M_FenwatchKeeperSkin.M_FenwatchKeeperSkin",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
}


def fail(message):
    unreal.log_error("Embermere Fenwatch keeper rig validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def validate_transform(actor, location, yaw, role):
    if (actor.get_actor_location() - location).length() > 0.1:
        fail("{} location drifted: {}".format(role, actor.get_actor_location()))
    rotation = actor.get_actor_rotation()
    if (
        abs(float(rotation.pitch)) > 0.1
        or abs(float(rotation.yaw) - yaw) > 0.1
        or abs(float(rotation.roll)) > 0.1
    ):
        fail("{} rotation drifted: {}".format(role, rotation))
    scale = actor.get_actor_scale3d()
    if (scale - unreal.Vector(1.0, 1.0, 1.0)).length() > 0.001:
        fail("{} scale drifted: {}".format(role, scale))


def validate_interactable(interactable, role, quest):
    if not interactable:
        fail("{} lost its interactable component".format(role))
    if str(interactable.get_editor_property("display_name")) != MARA_DISPLAY_NAME:
        fail("{} display name drifted".format(role))
    if str(interactable.get_editor_property("dialogue_text")) != MARA_DIALOGUE:
        fail("{} dialogue drifted".format(role))
    if interactable.get_editor_property("quest_to_offer") != quest:
        fail("{} lost the exact starter quest".format(role))
    if not bool(interactable.get_editor_property("show_world_marker")):
        fail("{} marker is disabled".format(role))
    if abs(float(interactable.get_editor_property("marker_height")) - MARA_MARKER_HEIGHT) > 0.1:
        fail("{} marker height drifted".format(role))


def blueprint_templates():
    blueprint = unreal.EditorAssetLibrary.load_asset(BLUEPRINT_PATH)
    if not blueprint or not isinstance(blueprint, unreal.Blueprint):
        fail("quest-giver Blueprint is missing")
    subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    static_template = None
    interactable_template = None
    for handle in subsystem.k2_gather_subobject_data_for_blueprint(blueprint):
        data = subsystem.k2_find_subobject_data_from_handle(handle)
        candidate = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
        if isinstance(candidate, unreal.StaticMeshComponent):
            static_template = candidate
        elif isinstance(candidate, unreal.EmbermereInteractableComponent):
            interactable_template = candidate
    if not static_template or not interactable_template:
        fail("quest-giver Blueprint lost its expected component templates")
    return static_template, interactable_template


def main():
    static_mesh = unreal.EditorAssetLibrary.load_asset(STATIC_PATH)
    skeletal_mesh = unreal.EditorAssetLibrary.load_asset(SKELETAL_PATH)
    idle = unreal.EditorAssetLibrary.load_asset(IDLE_PATH)
    quest = unreal.EditorAssetLibrary.load_asset(QUEST_PATH)
    if not static_mesh or not isinstance(static_mesh, unreal.StaticMesh):
        fail("accepted static fallback is missing")
    if not skeletal_mesh or not isinstance(skeletal_mesh, unreal.SkeletalMesh):
        fail("rigged skeletal mesh is missing")
    if not idle or not isinstance(idle, unreal.AnimSequence):
        fail("rigged Idle animation is missing")
    if not quest or not isinstance(quest, unreal.EmbermereQuestData):
        fail("starter quest data is missing")
    for property_name, expected in GREETING_COPY.items():
        if str(quest.get_editor_property(property_name)) != expected:
            fail("starter quest {} drifted".format(property_name))

    import_data = skeletal_mesh.get_editor_property("asset_import_data")
    import_class = import_data.get_class().get_name() if import_data else "None"
    if import_class != "FbxSkeletalMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_class))
    if unreal.EditorAssetLibrary.does_asset_exist(PHYSICS_PATH):
        fail("presentation-only keeper unexpectedly has a PhysicsAsset")
    skeleton = skeletal_mesh.get_editor_property("skeleton")
    if not skeleton or idle.get_editor_property("skeleton") != skeleton:
        fail("skeletal mesh and Idle do not share one Skeleton")
    if abs(float(idle.get_play_length()) - 3.6) > 0.12:
        fail("Idle duration drifted: {}".format(idle.get_play_length()))

    bounds = skeletal_mesh.get_bounds()
    size = bounds.box_extent * 2.0
    expected_size = (107.45, 71.0, 207.5)
    if any(
        abs(actual - expected) > 1.0
        for actual, expected in zip(
            (float(size.x), float(size.y), float(size.z)), expected_size
        )
    ):
        fail("skeletal bounds drifted: {}".format(size))
    material_paths = {
        material.get_path_name()
        for entry in list(skeletal_mesh.get_editor_property("materials"))
        for material in [entry.get_editor_property("material_interface")]
        if material
    }
    if material_paths != EXPECTED_MATERIALS:
        fail("skeletal material assignments drifted: {}".format(sorted(material_paths)))

    static_template, interactable_template = blueprint_templates()
    if static_template.get_editor_property("static_mesh"):
        fail("quest-giver Blueprint still embeds render art")
    if static_template.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("dormant Blueprint visual template collision is enabled")
    relative_location = static_template.get_editor_property("relative_location")
    relative_rotation = static_template.get_editor_property("relative_rotation")
    relative_scale = static_template.get_editor_property("relative_scale3d")
    if (relative_location - unreal.Vector(0.0, 0.0, -140.0)).length() > 0.1:
        fail("Blueprint fallback location drifted: {}".format(relative_location))
    if abs(float(relative_rotation.yaw) - 100.0) > 0.1:
        fail("Blueprint fallback yaw drifted: {}".format(relative_rotation.yaw))
    if (relative_scale - unreal.Vector(1.0, 1.0, 1.0)).length() > 0.001:
        fail("Blueprint fallback scale drifted: {}".format(relative_scale))
    validate_interactable(interactable_template, "Blueprint template", quest)

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = list(unreal.EditorLevelLibrary.get_all_level_actors())
    quest_actors = [actor for actor in actors if actor_label(actor) == QUEST_ACTOR_LABEL]
    presentations = [actor for actor in actors if actor_label(actor) == PRESENTATION_LABEL]
    if len(quest_actors) != 1:
        fail("expected one Mara gameplay actor, found {}".format(len(quest_actors)))
    if len(presentations) != 1:
        fail("expected one Mara presentation actor, found {}".format(len(presentations)))

    quest_actor = quest_actors[0]
    presentation = presentations[0]
    validate_transform(quest_actor, QUEST_LOCATION, QUEST_YAW, "Mara gameplay actor")
    validate_transform(presentation, PRESENTATION_LOCATION, PRESENTATION_YAW, "Mara presentation")
    validate_interactable(
        quest_actor.get_component_by_class(unreal.EmbermereInteractableComponent),
        "saved Mara gameplay actor",
        quest,
    )
    quest_static = quest_actor.get_component_by_class(unreal.StaticMeshComponent)
    if not quest_static or quest_static.get_editor_property("static_mesh"):
        fail("saved Mara gameplay actor must retain an empty static template")
    if quest_static.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("saved Mara gameplay actor's dormant art collision is enabled")
    if ORIGINAL_TAG in list(quest_actor.get_editor_property("tags")):
        fail("Mara gameplay actor is still counted as original art")

    if not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("saved Mara presentation uses the wrong native class")
    if presentation.get_editor_property("static_visual_mesh") != static_mesh:
        fail("accepted static fallback reference is missing")
    if presentation.get_editor_property("skeletal_visual_mesh") != skeletal_mesh:
        fail("saved presentation does not reference the rigged mesh")
    if presentation.get_editor_property("idle_animation") != idle:
        fail("saved presentation does not reference the reviewed Idle")
    if presentation.get_editor_property("animation_class"):
        fail("saved presentation unexpectedly uses an animation class")
    if not bool(presentation.get_editor_property("prefer_skeletal_visual")):
        fail("saved presentation no longer prefers the skeletal lane")
    if not bool(presentation.get_editor_property("loop_idle_animation")):
        fail("saved presentation no longer loops Idle")
    if abs(float(presentation.get_editor_property("idle_animation_play_rate")) - 1.0) > 0.001:
        fail("saved Idle play rate drifted")
    if presentation.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.SKELETAL_MESH:
        fail("saved presentation does not resolve through the skeletal lane")
    if presentation.get_resolved_animation_mode() != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE:
        fail("saved presentation does not resolve single-node Idle")
    if not presentation.is_presentation_collision_disabled():
        fail("saved presentation collision is enabled")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("Mara art presentation unexpectedly owns interaction or quest authority")
    if not bool(presentation.get_editor_property("enable_context_greeting")):
        fail("Mara context greeting is disabled")
    if presentation.get_editor_property("context_authority_actor") != quest_actor:
        fail("Mara context greeting lost its exact authority reference")
    if abs(float(presentation.get_editor_property("context_greeting_radius")) - GREETING_RADIUS) > 0.01:
        fail("Mara context greeting radius drifted")
    greeting_location = presentation.get_editor_property("context_greeting_relative_location")
    if (greeting_location - GREETING_LOCATION).length() > 0.01:
        fail("Mara context greeting location drifted: {}".format(greeting_location))
    if not presentation.is_context_greeting_presentation_only():
        fail("Mara context greeting contributes collision, overlap, or navigation")
    if ORIGINAL_TAG not in list(presentation.get_editor_property("tags")):
        fail("Mara art presentation lost the original-art tag")

    unreal.log(
        "Embermere Fenwatch keeper rig validation passed: exact Blueprint and "
        "saved-actor quest authority, classic skeletal FBX packages, shared "
        "Skeleton and 3.6-second Idle, one art-only skeletal presentation, "
        "NoCollision, exact reversible static fallback, and a read-only "
        "four-state contextual greeting"
    )


if __name__ == "__main__":
    main()
