"""Check saved rig material usage in a fresh -NullRHI process, before rendering."""

from pathlib import Path
import sys

import unreal

sys.path.insert(0, str(Path(__file__).resolve().parent))

from import_embermere_fenwatch_armsmaster_rigged_unreal import (
    SHARED_MATERIALS as ARMSMASTER_MATERIALS,
    SKELETAL_ASSET_PATH as ARMSMASTER_MESH,
)
from import_embermere_fenwatch_keeper_rigged_unreal import (
    SHARED_MATERIALS as KEEPER_MATERIALS,
    SKELETAL_ASSET_PATH as KEEPER_MESH,
)
from import_embermere_fenwatch_quartermaster_rigged_unreal import (
    SHARED_MATERIALS as QUARTERMASTER_MATERIALS,
    SKELETAL_ASSET_PATH as QUARTERMASTER_MESH,
)

RIG_MATERIALS = (
    (ARMSMASTER_MESH, ARMSMASTER_MATERIALS),
    (KEEPER_MESH, KEEPER_MATERIALS),
    (QUARTERMASTER_MESH, QUARTERMASTER_MATERIALS),
)


def main():
    if "-nullrhi" not in unreal.SystemLibrary.get_command_line().lower().split():
        raise RuntimeError("Saved material validation requires a fresh -NullRHI process; editor rendering can auto-repair flags in memory.")

    # Read materials before meshes. HasMaterialUsage is a non-mutating query,
    # unlike the render-time CheckMaterialUsage path that can auto-enable flags.
    paths = sorted({path for _, mapping in RIG_MATERIALS for path in mapping.values()})
    missing = []
    materials = {}
    for path in paths:
        material = unreal.load_asset(path)
        if not isinstance(material, (unreal.Material, unreal.MaterialInstanceConstant)):
            raise RuntimeError("Missing or unsupported Fenwatch material: " + path)
        materials[path] = material
        enabled = unreal.MaterialEditingLibrary.has_material_usage(
            material, unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH
        )
        unreal.log("Fenwatch saved SkeletalMesh usage: {} = {}".format(path, enabled))
        if not enabled:
            missing.append(path)
    if missing:
        raise RuntimeError("Missing saved SkeletalMesh usage: " + ", ".join(missing))

    for mesh_path, mapping in RIG_MATERIALS:
        mesh = unreal.load_asset(mesh_path)
        if not isinstance(mesh, unreal.SkeletalMesh):
            raise RuntimeError("Missing Fenwatch skeletal mesh: " + mesh_path)
        slots = list(mesh.get_editor_property("materials"))
        actual = {}
        for slot in slots:
            name = str(slot.get_editor_property("material_slot_name"))
            actual[name] = slot.get_editor_property("material_interface")
        expected = {name: materials[path] for name, path in mapping.items()}
        if len(slots) != len(expected) or actual != expected:
            raise RuntimeError("Fenwatch skeletal material slot assignments drifted: " + mesh_path)

    unreal.log("EMBERMERE_FENWATCH_SKELETAL_MATERIAL_VALIDATION_SUCCESS: 8 materials, 3 rigs, 18 slots")


if __name__ == "__main__":
    main()
