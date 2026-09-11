"""Explicit, scoped material preparation for the Fenwatch classic-FBX lane."""

import unreal


def prepare_skeletal_materials(material_paths):
    """Save missing base usage flags only; never mutate material-instance parents."""
    if "-nullrhi" not in unreal.SystemLibrary.get_command_line().lower().split():
        raise RuntimeError("Prepare Fenwatch materials in a fresh -NullRHI process, before rendering can auto-repair usage flags.")

    pending = []
    for path in sorted(set(material_paths)):
        if not path.startswith("/Game/Art/Embermere/"):
            raise RuntimeError("Refusing non-project material: " + path)
        material = unreal.load_asset(path)
        if not isinstance(material, (unreal.Material, unreal.MaterialInstanceConstant)):
            raise RuntimeError("Missing or unsupported Fenwatch material: " + path)
        enabled = unreal.MaterialEditingLibrary.has_material_usage(
            material, unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH
        )
        if not enabled:
            if not isinstance(material, unreal.Material):
                raise RuntimeError("Material instance requires a separate reviewed usage contract: " + path)
            pending.append(material)

    # Preflight the whole assignment set before modifying any material. The
    # already-compatible iron instance and its engine parent remain read-only.
    saved = []
    for material in pending:
        unreal.MaterialEditingLibrary.set_base_material_usage(
            material, unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH, True
        )
        if not unreal.MaterialEditingLibrary.has_material_usage(
            material, unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH
        ):
            raise RuntimeError("Could not enable SkeletalMesh usage: " + material.get_path_name())
        if not unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False):
            raise RuntimeError("Could not save material usage: " + material.get_path_name())
        saved.append(material.get_path_name())
    return saved
