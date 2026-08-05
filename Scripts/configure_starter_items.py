"""Configure tracked starter item assets after item/equipment schema changes.

Run headlessly with UnrealEditor-Cmd -ExecutePythonScript or from the Unreal
Python console. The operation is idempotent.
"""

import unreal


RECRUIT_PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack"
MARSH_TONIC_PATH = "/Game/Data/Items/DI_MarshTonic"
STARTER_QUEST_PATH = "/Game/Data/Quests/DQ_FirstSignsAtTheRuin"
RECRUIT_PACK_ICON_PATH = "/Game/UI/Icons/T_Icon_Item_RecruitPack"
MARSH_TONIC_ICON_PATH = "/Game/UI/Icons/T_Icon_Item_MarshTonic"


def load_or_create_item(asset_path):
    item = unreal.EditorAssetLibrary.load_asset(asset_path)
    if item:
        return item

    package_path, asset_name = asset_path.rsplit("/", 1)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", unreal.EmbermereItemData)
    item = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name,
        package_path,
        unreal.EmbermereItemData,
        factory,
    )
    if not item:
        raise RuntimeError(f"Could not create starter item: {asset_path}")
    return item


def main():
    recruit_pack = load_or_create_item(RECRUIT_PACK_PATH)

    recruit_pack.set_editor_property("item_id", "RecruitPack")
    recruit_pack.set_editor_property("display_name", "Recruit Pack")
    recruit_pack.set_editor_property(
        "description",
        "A simple bundle of village supplies for a new defender of Embermere.",
    )
    recruit_pack.set_editor_property("category", unreal.EmbermereItemCategory.ARMOR)
    recruit_pack.set_editor_property("equipment_slot", unreal.EmbermereEquipmentSlot.BACK)
    recruit_pack.set_editor_property("required_level", 1)
    recruit_pack.set_editor_property("sell_value_copper", 12)

    bonuses = unreal.EmbermereItemStatBonuses()
    bonuses.set_editor_property("max_health", 5.0)
    bonuses.set_editor_property("armor", 1.0)
    recruit_pack.set_editor_property("stat_bonuses", bonuses)

    recruit_pack_icon = unreal.EditorAssetLibrary.load_asset(RECRUIT_PACK_ICON_PATH)
    if recruit_pack_icon:
        recruit_pack.set_editor_property("icon", recruit_pack_icon)

    if not unreal.EditorAssetLibrary.save_loaded_asset(recruit_pack, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save starter item: {RECRUIT_PACK_PATH}")

    marsh_tonic = load_or_create_item(MARSH_TONIC_PATH)
    marsh_tonic.set_editor_property("item_id", "MarshTonic")
    marsh_tonic.set_editor_property("display_name", "Marsh Tonic")
    marsh_tonic.set_editor_property("description", "A sharp herbal tonic brewed from reeds gathered near the Embermere ruins.")
    marsh_tonic.set_editor_property("max_stack", 5)
    marsh_tonic.set_editor_property("category", unreal.EmbermereItemCategory.CONSUMABLE)
    marsh_tonic.set_editor_property("sell_value_copper", 3)

    effects = unreal.EmbermereConsumableEffects()
    effects.set_editor_property("heal_health", 25.0)
    effects.set_editor_property("restore_mana", 10.0)
    marsh_tonic.set_editor_property("consumable_effects", effects)

    marsh_tonic_icon = unreal.EditorAssetLibrary.load_asset(MARSH_TONIC_ICON_PATH)
    if marsh_tonic_icon:
        marsh_tonic.set_editor_property("icon", marsh_tonic_icon)

    if not unreal.EditorAssetLibrary.save_loaded_asset(marsh_tonic, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save starter item: {MARSH_TONIC_PATH}")

    starter_quest = unreal.EditorAssetLibrary.load_asset(STARTER_QUEST_PATH)
    if not starter_quest or not isinstance(starter_quest, unreal.EmbermereQuestData):
        raise RuntimeError(f"Could not load starter quest: {STARTER_QUEST_PATH}")
    starter_quest.set_editor_property("reward_copper", 20)
    if not unreal.EditorAssetLibrary.save_loaded_asset(starter_quest, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save starter quest: {STARTER_QUEST_PATH}")

    unreal.log(
        "Embermere starter economy configured: Recruit Pack and Marsh Tonic sell values plus starter-quest copper"
    )


if __name__ == "__main__":
    main()
