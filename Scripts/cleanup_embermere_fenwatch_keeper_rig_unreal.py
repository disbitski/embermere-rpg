"""Remove only generated Fenwatch keeper rig packages before a rebuild."""

import sys

import unreal


GENERATED_ASSET_PATHS = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/Animations/A_EmbermereFenwatchKeeper_Mara_Idle",
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/SK_EmbermereFenwatchKeeper_Mara_01_PhysicsAsset",
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/SK_EmbermereFenwatchKeeper_Mara_01",
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/SK_EmbermereFenwatchKeeper_Mara_01_Skeleton",
)


def fail(message):
    unreal.log_error("Embermere Fenwatch keeper rig cleanup failed: {}".format(message))
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
        "Embermere Fenwatch keeper rig cleanup passed: removed={}; static "
        "fallback, skin/materials, BP_QuestGiver, quest data, and map were not "
        "modified".format(removed)
    )


if __name__ == "__main__":
    main()
