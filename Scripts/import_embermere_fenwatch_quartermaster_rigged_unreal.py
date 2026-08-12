"""Import the rigged Fenwatch quartermaster and switch its art-only wrapper.

Run in a fresh Unreal commandlet or a stopped editor world. The script uses the
classic FBX factory, explicitly saves every generated package, keeps the
accepted static mesh as a reversible fallback, and leaves all vendor authority
on the separate Fenwatch service actor.
"""

from pathlib import Path
import sys

import unreal


PROJECT_ROOT = Path("/Users/wizard/Documents/Unreal Game")
SOURCE_ROOT = (
    PROJECT_ROOT
    / "ArtSource/Blender/Characters/NPCs/FenwatchQuartermaster/Rigged"
)
SKELETAL_SOURCE = SOURCE_ROOT / "SK_EmbermereFenwatchQuartermaster_01.fbx"
IDLE_SOURCE = (
    SOURCE_ROOT / "Animations/A_EmbermereFenwatchQuartermaster_Idle.fbx"
)
DESTINATION_PATH = "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster"
STATIC_ASSET_PATH = DESTINATION_PATH + "/SM_EmbermereFenwatchQuartermaster_01"
SKELETAL_ASSET_NAME = "SK_EmbermereFenwatchQuartermaster_01"
SKELETAL_ASSET_PATH = DESTINATION_PATH + "/" + SKELETAL_ASSET_NAME
SKELETON_ASSET_PATH = SKELETAL_ASSET_PATH + "_Skeleton"
PHYSICS_ASSET_PATH = SKELETAL_ASSET_PATH + "_PhysicsAsset"
IDLE_ASSET_NAME = "A_EmbermereFenwatchQuartermaster_Idle"
IDLE_ASSET_PATH = DESTINATION_PATH + "/Animations/" + IDLE_ASSET_NAME
SKIN_MATERIAL_PATH = DESTINATION_PATH + "/M_FenwatchQuartermasterSkin"
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
PRESENTATION_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
SERVICE_LABEL = "Embermere_FenwatchQuartermaster_Service_01"

SHARED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber",
    "M_FenwatchQuartermasterSkin": SKIN_MATERIAL_PATH,
}


def fail(message):
    unreal.log_error(
        "Embermere rigged Fenwatch quartermaster integration failed: {}".format(
            message
        )
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def list_existing_generated_assets():
    existing = []
    for asset_path in (
        IDLE_ASSET_PATH,
        SKELETAL_ASSET_PATH,
        PHYSICS_ASSET_PATH,
        SKELETON_ASSET_PATH,
    ):
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            existing.append(asset_path)
    return existing


def import_skeletal_mesh():
    if not SKELETAL_SOURCE.is_file():
        fail("missing skeletal source {}".format(SKELETAL_SOURCE))

    existing_mesh = (
        unreal.EditorAssetLibrary.load_asset(SKELETAL_ASSET_PATH)
        if unreal.EditorAssetLibrary.does_asset_exist(SKELETAL_ASSET_PATH)
        else None
    )
    existing_skeleton = (
        existing_mesh.get_editor_property("skeleton")
        if isinstance(existing_mesh, unreal.SkeletalMesh)
        else None
    )
    if existing_mesh and not existing_skeleton:
        fail(
            "existing skeletal mesh has no valid Skeleton; remove the incomplete "
            "generated package in a separate process before retrying"
        )
    if existing_mesh:
        import_data = existing_mesh.get_editor_property("asset_import_data")
        import_data_class = (
            import_data.get_class().get_name() if import_data else "None"
        )
        if import_data_class != "FbxSkeletalMeshImportData":
            fail(
                "existing skeletal mesh has {} provenance; remove the generated "
                "skeletal packages in a separate process before retrying".format(
                    import_data_class
                )
            )
        return existing_mesh, existing_skeleton, []

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
    options.set_editor_property(
        "mesh_type_to_import", unreal.FBXImportType.FBXIT_SKELETAL_MESH
    )
    options.set_editor_property(
        "original_import_type", unreal.FBXImportType.FBXIT_SKELETAL_MESH
    )
    options.set_editor_property("create_physics_asset", False)
    options.set_editor_property("skeleton", existing_skeleton)

    skeletal_data = options.get_editor_property("skeletal_mesh_import_data")
    skeletal_data.set_editor_property("import_mesh_lods", False)
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
    return (
        skeletal_mesh,
        skeleton,
        list(task.get_editor_property("imported_object_paths")),
    )


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
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        skeletal_mesh, only_if_is_dirty=False
    ):
        fail("could not save {}".format(SKELETAL_ASSET_PATH))
    return assignments


def import_idle(skeleton):
    if not IDLE_SOURCE.is_file():
        fail("missing Idle source {}".format(IDLE_SOURCE))

    existing_idle = (
        unreal.EditorAssetLibrary.load_asset(IDLE_ASSET_PATH)
        if unreal.EditorAssetLibrary.does_asset_exist(IDLE_ASSET_PATH)
        else None
    )
    if existing_idle:
        if not isinstance(existing_idle, unreal.AnimSequence):
            fail("existing Idle package has the wrong asset type")
        if existing_idle.get_editor_property("skeleton") != skeleton:
            fail(
                "existing Idle references a different Skeleton; remove the "
                "generated animation package in a separate process before retrying"
            )
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
    options.set_editor_property(
        "mesh_type_to_import", unreal.FBXImportType.FBXIT_ANIMATION
    )
    options.set_editor_property(
        "original_import_type", unreal.FBXImportType.FBXIT_ANIMATION
    )
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
        fail("Idle animation does not reference the imported quartermaster Skeleton")
    if not unreal.EditorAssetLibrary.save_loaded_asset(idle, only_if_is_dirty=False):
        fail("could not save {}".format(IDLE_ASSET_PATH))
    return idle, list(task.get_editor_property("imported_object_paths"))


def validate_assets(skeletal_mesh, skeleton, idle):
    import_data = skeletal_mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxSkeletalMeshImportData":
        fail("expected classic skeletal FBX import data, found {}".format(import_data_class))

    bounds = skeletal_mesh.get_bounds()
    size = bounds.box_extent * 2.0
    if not all(
        (
            abs(float(size.x) - 120.842) <= 1.0,
            abs(float(size.y) - 93.0) <= 1.0,
            abs(float(size.z) - 217.0) <= 1.0,
        )
    ):
        fail(
            "skeletal bounds drifted: origin={}, extent={}".format(
                bounds.origin,
                bounds.box_extent,
            )
        )
    if len(list(skeletal_mesh.get_editor_property("materials"))) != 6:
        fail("skeletal mesh material count drifted")
    if unreal.EditorAssetLibrary.does_asset_exist(PHYSICS_ASSET_PATH):
        fail("presentation-only quartermaster unexpectedly generated a PhysicsAsset")

    play_length = float(idle.get_play_length())
    if abs(play_length - 4.0) > 0.12:
        fail("Idle duration drifted: {} seconds".format(play_length))
    if skeletal_mesh.get_editor_property("skeleton") != skeleton:
        fail("skeletal mesh Skeleton reference drifted")
    if idle.get_editor_property("skeleton") != skeleton:
        fail("Idle Skeleton reference drifted")
    return size, play_length


def configure_saved_presentation(static_mesh, skeletal_mesh, idle):
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
        fail("expected one quartermaster presentation, found {}".format(len(presentations)))
    if len(services) != 1:
        fail("expected one vendor service, found {}".format(len(services)))

    presentation = presentations[0]
    service = services[0]
    if not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("saved quartermaster uses the wrong presentation class")
    if not isinstance(service, unreal.EmbermereVendorServiceActor):
        fail("saved vendor service uses the wrong gameplay class")

    presentation.set_editor_property("static_visual_mesh", static_mesh)
    presentation.set_editor_property("skeletal_visual_mesh", skeletal_mesh)
    presentation.set_editor_property("animation_class", None)
    presentation.set_editor_property("idle_animation", idle)
    presentation.set_editor_property("loop_idle_animation", True)
    presentation.set_editor_property("idle_animation_play_rate", 1.0)
    presentation.set_editor_property("prefer_skeletal_visual", True)
    presentation.refresh_presentation()

    if (
        presentation.get_resolved_visual_mode()
        != unreal.EmbermereNpcVisualMode.SKELETAL_MESH
    ):
        fail("saved quartermaster did not resolve through the skeletal lane")
    if (
        presentation.get_resolved_animation_mode()
        != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE
    ):
        fail("saved quartermaster did not resolve the single-node Idle lane")
    if not presentation.is_presentation_collision_disabled():
        fail("quartermaster presentation collision is enabled")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation unexpectedly owns interaction behavior")
    if presentation.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("presentation unexpectedly owns vendor behavior")
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("vendor service unexpectedly owns static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("vendor service unexpectedly owns skeletal art")

    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return actor_label(presentation), actor_label(service)


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
        if not unreal.EditorAssetLibrary.save_loaded_asset(
            asset, only_if_is_dirty=False
        ):
            fail("could not explicitly save {}".format(asset.get_path_name()))
    presentation_label, service_label = configure_saved_presentation(
        static_mesh,
        skeletal_mesh,
        idle,
    )

    unreal.log(
        "Embermere rigged Fenwatch quartermaster integration passed: existing_before={}, skeletal_imports={}, idle_imports={}, materials={}, dimensions={}, Idle={:.3f}s, presentation={}, service={}, static_fallback={}".format(
            existing_before,
            skeletal_imports,
            idle_imports,
            assignments,
            size,
            play_length,
            presentation_label,
            service_label,
            STATIC_ASSET_PATH,
        )
    )


if __name__ == "__main__":
    main()
