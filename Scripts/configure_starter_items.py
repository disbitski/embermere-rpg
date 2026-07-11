"""Configure tracked starter item assets after item/equipment schema changes.

Run headlessly with UnrealEditor-Cmd -ExecutePythonScript or from the Unreal
Python console. The operation is idempotent.
"""

import unreal


RECRUIT_PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack"


def main():
    recruit_pack = unreal.EditorAssetLibrary.load_asset(RECRUIT_PACK_PATH)
    if not recruit_pack:
        raise RuntimeError(f"Could not load starter item: {RECRUIT_PACK_PATH}")

    recruit_pack.set_editor_property("category", unreal.EmbermereItemCategory.ARMOR)
    recruit_pack.set_editor_property("equipment_slot", unreal.EmbermereEquipmentSlot.BACK)
    recruit_pack.set_editor_property("required_level", 1)

    if not unreal.EditorAssetLibrary.save_loaded_asset(recruit_pack, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save starter item: {RECRUIT_PACK_PATH}")

    unreal.log("Embermere starter item configured: Recruit Pack is level-1 Back armor")


if __name__ == "__main__":
    main()
