"""Import rigged Mara art and separate it from BP_QuestGiver authority.

The Blueprint keeps interaction, dialogue, marker, quest, and reward behavior.
A colocated AEmbermereNpcPresentationActor owns only the reversible static and
skeletal art lanes. Run in a fresh Unreal process after optional cleanup.
"""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_ROOT = (
    PROJECT_ROOT / "ArtSource/Blender/Characters/NPCs/FenwatchKeeper/Rigged"
)
SKELETAL_SOURCE = SOURCE_ROOT / "SK_EmbermereFenwatchKeeper_Mara_01.fbx"
IDLE_SOURCE = SOURCE_ROOT / "Animations/A_EmbermereFenwatchKeeper_Mara_Idle.fbx"
DESTINATION_PATH = "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper"
STATIC_ASSET_PATH = DESTINATION_PATH + "/SM_EmbermereFenwatchKeeper_Mara_01"
SKELETAL_ASSET_NAME = "SK_EmbermereFenwatchKeeper_Mara_01"
SKELETAL_ASSET_PATH = DESTINATION_PATH + "/" + SKELETAL_ASSET_NAME
SKELETON_ASSET_PATH = SKELETAL_ASSET_PATH + "_Skeleton"
PHYSICS_ASSET_PATH = SKELETAL_ASSET_PATH + "_PhysicsAsset"
IDLE_ASSET_NAME = "A_EmbermereFenwatchKeeper_Mara_Idle"
IDLE_ASSET_PATH = DESTINATION_PATH + "/Animations/" + IDLE_ASSET_NAME
SKIN_MATERIAL_PATH = DESTINATION_PATH + "/M_FenwatchKeeperSkin"
BLUEPRINT_PATH = "/Game/Blueprints/BP_QuestGiver"
QUEST_PATH = "/Game/Data/Quests/DQ_FirstSignsAtTheRuin"
MARA_DISPLAY_NAME = "Mara Fenwatch"
MARA_DIALOGUE = (
    "You picked a lively day to arrive. Something is stirring near the old "
    "stones east of town. Cull a few of those things and come back whole."
)
MARA_MARKER_HEIGHT = 185.0
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
QUEST_ACTOR_LABEL = "Quest_Giver_Mara_Fenwatch"
PRESENTATION_LABEL = "Embermere_FenwatchKeeper_Mara_Presentation_01"
PRESENTATION_LOCATION = (-2050.0, -850.0, 0.0)
PRESENTATION_YAW = 135.0
PRESENTATION_FOLDER = "04_Fab_Zone_Pass/01_Village/Embermere_Originals"
ORIGINAL_TAG = unreal.Name("EmbermereOriginalArt")

SHARED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber",
    "M_FenwatchKeeperSkin": SKIN_MATERIAL_PATH,
}


def fail(message):
    unreal.log_error(
        "Embermere rigged Fenwatch keeper integration failed: {}".format(message)
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


def list_existing_generated_assets():
    return [
        asset_path
        for asset_path in (
            IDLE_ASSET_PATH,
            SKELETAL_ASSET_PATH,
            PHYSICS_ASSET_PATH,
            SKELETON_ASSET_PATH,
        )
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path)
    ]


def import_skeletal_mesh():
    if not SKELETAL_SOURCE.is_file():
        fail("missing skeletal source {}".format(SKELETAL_SOURCE))

    existing_mesh = unreal.EditorAssetLibrary.load_asset(SKELETAL_ASSET_PATH)
    if existing_mesh:
        if not isinstance(existing_mesh, unreal.SkeletalMesh):
            fail("existing skeletal package has the wrong asset type")
        skeleton = existing_mesh.get_editor_property("skeleton")
        if not skeleton:
            fail("existing skeletal mesh has no Skeleton; use cleanup first")
        import_data = existing_mesh.get_editor_property("asset_import_data")
        import_class = import_data.get_class().get_name() if import_data else "None"
        if import_class != "FbxSkeletalMeshImportData":
            fail(
                "existing skeletal mesh has {} provenance; use cleanup first".format(
                    import_class
                )
            )
        return existing_mesh, skeleton, []

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", str(SKELETAL_SOURCE))
    task.set_editor_property("destination_path", DESTINATION_PATH)
    task.set_editor_property("destination_name", SKELETAL_ASSET_NAME)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", False)
    task.set_editor_property("replace_existing_settings", False)
    task.set_editor_property("save", True)

    options = unreal.FbxImportUI()
    options.set_editor_property("automated_import_should_detect_type", False)
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_as_skeletal", True)
    options.set_editor_property("import_animations", False)
    options.set_editor_property("import_materials", False)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_SKELETAL_MESH)
    options.set_editor_property("original_import_type", unreal.FBXImportType.FBXIT_SKELETAL_MESH)
    options.set_editor_property("create_physics_asset", False)

    skeletal_data = options.get_editor_property("skeletal_mesh_import_data")
    skeletal_data.set_editor_property("import_mesh_lo_ds", False)
    skeletal_data.set_editor_property("import_morph_targets", False)
    skeletal_data.set_editor_property("update_skeleton_reference_pose", False)
    skeletal_data.set_editor_property("use_t0_as_ref_pose", False)
    skeletal_data.set_editor_property("preserve_smoothing_groups", True)
    skeletal_data.set_editor_property("import_meshes_in_bone_hierarchy", False)

    task.set_editor_property("factory", unreal.FbxFactory())
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    skeletal_mesh = unreal.EditorAssetLibrary.load_asset(SKELETAL_ASSET_PATH)
    if not skeletal_mesh or not isinstance(skeletal_mesh, unreal.SkeletalMesh):
        fail("FBX did not produce {}".format(SKELETAL_ASSET_PATH))
    skeleton = skeletal_mesh.get_editor_property("skeleton")
    if not skeleton or not isinstance(skeleton, unreal.Skeleton):
        fail("skeletal import did not produce a Skeleton")
    return skeletal_mesh, skeleton, list(task.get_editor_property("imported_object_paths"))


def assign_materials(skeletal_mesh):
    assignments = []
    materials = list(skeletal_mesh.get_editor_property("materials"))
    for index, skeletal_material in enumerate(materials):
        slot_name = str(skeletal_material.get_editor_property("material_slot_name"))
        material_path = SHARED_MATERIALS.get(slot_name)
        if not material_path:
            fail("unexpected skeletal material slot {}".format(slot_name))
        material = unreal.EditorAssetLibrary.load_asset(material_path)
        if not material:
            fail("missing material {} for slot {}".format(material_path, slot_name))
        skeletal_material.set_editor_property("material_interface", material)
        materials[index] = skeletal_material
        assignments.append({"slot": slot_name, "material": material_path})
    if len(assignments) != len(SHARED_MATERIALS):
        fail("expected 6 material slots, found {}".format(len(assignments)))
    skeletal_mesh.set_editor_property("materials", materials)
    if not unreal.EditorAssetLibrary.save_loaded_asset(skeletal_mesh, only_if_is_dirty=False):
        fail("could not save {}".format(SKELETAL_ASSET_PATH))
    return assignments


def import_idle(skeleton):
    if not IDLE_SOURCE.is_file():
        fail("missing Idle source {}".format(IDLE_SOURCE))
    existing_idle = unreal.EditorAssetLibrary.load_asset(IDLE_ASSET_PATH)
    if existing_idle:
        if not isinstance(existing_idle, unreal.AnimSequence):
            fail("existing Idle package has the wrong asset type")
        if existing_idle.get_editor_property("skeleton") != skeleton:
            fail("existing Idle uses another Skeleton; use cleanup first")
        return existing_idle, []

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", str(IDLE_SOURCE))
    task.set_editor_property("destination_path", DESTINATION_PATH + "/Animations")
    task.set_editor_property("destination_name", IDLE_ASSET_NAME)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", False)
    task.set_editor_property("replace_existing_settings", False)
    task.set_editor_property("save", True)

    options = unreal.FbxImportUI()
    options.set_editor_property("automated_import_should_detect_type", False)
    options.set_editor_property("import_mesh", False)
    options.set_editor_property("import_as_skeletal", False)
    options.set_editor_property("import_animations", True)
    options.set_editor_property("import_materials", False)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_ANIMATION)
    options.set_editor_property("original_import_type", unreal.FBXImportType.FBXIT_ANIMATION)
    options.set_editor_property("skeleton", skeleton)
    animation_data = options.get_editor_property("anim_sequence_import_data")
    animation_data.set_editor_property("import_bone_tracks", True)
    animation_data.set_editor_property("import_custom_attribute", False)

    task.set_editor_property("factory", unreal.FbxFactory())
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    idle = unreal.EditorAssetLibrary.load_asset(IDLE_ASSET_PATH)
    if not idle or not isinstance(idle, unreal.AnimSequence):
        fail("FBX did not produce {}".format(IDLE_ASSET_PATH))
    if idle.get_editor_property("skeleton") != skeleton:
        fail("Idle animation does not reference the imported keeper Skeleton")
    if not unreal.EditorAssetLibrary.save_loaded_asset(idle, only_if_is_dirty=False):
        fail("could not save {}".format(IDLE_ASSET_PATH))
    return idle, list(task.get_editor_property("imported_object_paths"))


def validate_assets(skeletal_mesh, skeleton, idle):
    import_data = skeletal_mesh.get_editor_property("asset_import_data")
    import_class = import_data.get_class().get_name() if import_data else "None"
    if import_class != "FbxSkeletalMeshImportData":
        fail("expected classic skeletal FBX import data, found {}".format(import_class))
    bounds = skeletal_mesh.get_bounds()
    size = bounds.box_extent * 2.0
    expected = (107.45, 71.0, 207.5)
    if any(abs(actual - wanted) > 1.0 for actual, wanted in zip(
        (float(size.x), float(size.y), float(size.z)), expected
    )):
        fail("skeletal bounds drifted: {}".format(size))
    if len(list(skeletal_mesh.get_editor_property("materials"))) != 6:
        fail("skeletal mesh material count drifted")
    if unreal.EditorAssetLibrary.does_asset_exist(PHYSICS_ASSET_PATH):
        fail("presentation-only keeper unexpectedly generated a PhysicsAsset")
    play_length = float(idle.get_play_length())
    if abs(play_length - 3.6) > 0.12:
        fail("Idle duration drifted: {} seconds".format(play_length))
    if skeletal_mesh.get_editor_property("skeleton") != skeleton:
        fail("skeletal mesh Skeleton reference drifted")
    if idle.get_editor_property("skeleton") != skeleton:
        fail("Idle Skeleton reference drifted")
    return size, play_length


def get_blueprint_templates():
    blueprint = unreal.EditorAssetLibrary.load_asset(BLUEPRINT_PATH)
    if not blueprint or not isinstance(blueprint, unreal.Blueprint):
        fail("missing quest-giver Blueprint {}".format(BLUEPRINT_PATH))
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
        fail("BP_QuestGiver must retain static fallback and interactable templates")
    return blueprint, static_template, interactable_template


def validate_interactable(interactable, role):
    quest = interactable.get_editor_property("quest_to_offer")
    if not quest or quest.get_path_name() != QUEST_PATH + ".DQ_FirstSignsAtTheRuin":
        fail("{} lost its quest authority: {}".format(role, quest))
    if str(interactable.get_editor_property("display_name")) != MARA_DISPLAY_NAME:
        fail("{} display name drifted".format(role))
    if not bool(interactable.get_editor_property("show_world_marker")):
        fail("{} world marker is disabled".format(role))
    marker_height = float(interactable.get_editor_property("marker_height"))
    if abs(marker_height - MARA_MARKER_HEIGHT) > 0.1:
        fail("{} marker height drifted: {}".format(role, marker_height))
    dialogue = str(interactable.get_editor_property("dialogue_text"))
    if dialogue != MARA_DIALOGUE:
        fail("{} dialogue text drifted: {!r}".format(role, dialogue))


def configure_blueprint_gameplay_owner():
    blueprint, static_template, interactable_template = get_blueprint_templates()
    validate_interactable(interactable_template, "BP_QuestGiver template")
    static_template.modify()
    static_template.set_static_mesh(None)
    static_template.set_collision_profile_name(unreal.Name("NoCollision"))
    static_template.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
    static_template.set_editor_property("can_ever_affect_navigation", False)
    unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
    if not unreal.EditorAssetLibrary.save_loaded_asset(blueprint, only_if_is_dirty=False):
        fail("could not save {}".format(BLUEPRINT_PATH))
    return static_template.get_name(), interactable_template.get_name()


def configure_saved_world(static_mesh, skeletal_mesh, idle):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = list(actor_subsystem.get_all_level_actors())
    quest_actors = [actor for actor in actors if actor_label(actor) == QUEST_ACTOR_LABEL]
    presentations = [actor for actor in actors if actor_label(actor) == PRESENTATION_LABEL]
    if len(quest_actors) != 1:
        fail("expected one saved Mara quest actor, found {}".format(len(quest_actors)))
    if len(presentations) > 1:
        fail("expected at most one Mara presentation, found {}".format(len(presentations)))

    quest_actor = quest_actors[0]
    quest_interactable = quest_actor.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    quest_static = quest_actor.get_component_by_class(unreal.StaticMeshComponent)
    if not quest_interactable or not quest_static:
        fail("saved Mara must retain interactable and legacy static templates")
    validate_interactable(quest_interactable, "saved Mara")
    quest_static.set_static_mesh(None)
    quest_static.set_collision_profile_name(unreal.Name("NoCollision"))
    quest_static.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
    quest_tags = list(quest_actor.get_editor_property("tags"))
    quest_actor.set_editor_property(
        "tags", [tag for tag in quest_tags if tag != ORIGINAL_TAG]
    )

    if presentations:
        presentation = presentations[0]
    else:
        presentation = actor_subsystem.spawn_actor_from_class(
            unreal.EmbermereNpcPresentationActor,
            unreal.Vector(*PRESENTATION_LOCATION),
            make_rotation(PRESENTATION_YAW),
            False,
        )
        if not presentation:
            fail("could not spawn Mara art-only presentation")
        presentation.set_actor_label(PRESENTATION_LABEL)
        presentation.set_folder_path(unreal.Name(PRESENTATION_FOLDER))

    presentation.set_actor_location(unreal.Vector(*PRESENTATION_LOCATION), False, False)
    presentation.set_actor_rotation(make_rotation(PRESENTATION_YAW), False)
    presentation.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
    presentation.set_editor_property("static_visual_mesh", static_mesh)
    presentation.set_editor_property("skeletal_visual_mesh", skeletal_mesh)
    presentation.set_editor_property("animation_class", None)
    presentation.set_editor_property("idle_animation", idle)
    presentation.set_editor_property("loop_idle_animation", True)
    presentation.set_editor_property("idle_animation_play_rate", 1.0)
    presentation.set_editor_property("prefer_skeletal_visual", True)
    presentation.set_editor_property("visual_relative_transform", unreal.Transform())
    presentation.refresh_presentation()
    tags = list(presentation.get_editor_property("tags"))
    if ORIGINAL_TAG not in tags:
        tags.append(ORIGINAL_TAG)
        presentation.set_editor_property("tags", tags)

    if presentation.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.SKELETAL_MESH:
        fail("Mara presentation did not resolve the skeletal lane")
    if presentation.get_resolved_animation_mode() != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE:
        fail("Mara presentation did not resolve single-node Idle")
    if not presentation.is_presentation_collision_disabled():
        fail("Mara presentation collision is enabled")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("Mara presentation unexpectedly owns interaction or quest authority")
    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return actor_label(quest_actor), actor_label(presentation)


def main():
    static_mesh = unreal.EditorAssetLibrary.load_asset(STATIC_ASSET_PATH)
    if not static_mesh or not isinstance(static_mesh, unreal.StaticMesh):
        fail("accepted static fallback is missing")
    existing_before = list_existing_generated_assets()
    skeletal_mesh, skeleton, skeletal_imports = import_skeletal_mesh()
    assignments = assign_materials(skeletal_mesh)
    idle, idle_imports = import_idle(skeleton)
    size, play_length = validate_assets(skeletal_mesh, skeleton, idle)
    for asset in (skeletal_mesh, skeleton, idle):
        if not unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False):
            fail("could not explicitly save {}".format(asset.get_path_name()))
    static_template, interactable_template = configure_blueprint_gameplay_owner()
    quest_actor, presentation = configure_saved_world(static_mesh, skeletal_mesh, idle)
    unreal.log(
        "Embermere rigged Fenwatch keeper integration passed: existing_before={}, "
        "skeletal_imports={}, idle_imports={}, materials={}, dimensions={}, "
        "Idle={:.3f}s, blueprint_static={}, blueprint_interactable={}, quest_actor={}, "
        "art_only_presentation={}, static_fallback={}".format(
            existing_before,
            skeletal_imports,
            idle_imports,
            assignments,
            size,
            play_length,
            static_template,
            interactable_template,
            quest_actor,
            presentation,
            STATIC_ASSET_PATH,
        )
    )


if __name__ == "__main__":
    main()
