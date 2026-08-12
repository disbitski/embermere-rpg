"""Remove only generated Fenwatch quartermaster rig packages before rebuild.

Run this in its own Unreal commandlet process, then run the reviewed rig
importer in a fresh process. Separating cleanup from import prevents stale
SkeletalMesh/Skeleton UObject references and stale Interchange provenance.
The accepted static fallback, materials, map, and vendor service are untouched.
"""

import sys

import unreal


GENERATED_ASSET_PATHS = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/Animations/A_EmbermereFenwatchQuartermaster_Idle",
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/SK_EmbermereFenwatchQuartermaster_01_PhysicsAsset",
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/SK_EmbermereFenwatchQuartermaster_01",
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/SK_EmbermereFenwatchQuartermaster_01_Skeleton",
)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch quartermaster rig cleanup failed: {}".format(message)
    )
    sys.exit(1)


def main():
    removed = []
    for asset_path in GENERATED_ASSET_PATHS:
        if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            continue
        if not unreal.EditorAssetLibrary.delete_asset(asset_path):
            fail("could not remove generated asset {}".format(asset_path))
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            fail("generated asset still exists after deletion: {}".format(asset_path))
        removed.append(asset_path)

    unreal.log(
        "Embermere Fenwatch quartermaster rig cleanup passed: removed={}; "
        "static fallback, materials, map, and vendor service were not modified".format(
            removed
        )
    )


if __name__ == "__main__":
    main()
