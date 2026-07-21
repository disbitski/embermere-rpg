"""Import, configure, and validate Embermere's deterministic UI icon family."""

from pathlib import Path

import unreal


PROJECT_ROOT = Path(__file__).resolve().parents[1]
SOURCE_DIR = PROJECT_ROOT / "ArtSource" / "UI" / "Icons"
DESTINATION_PATH = "/Game/UI/Icons"
ICON_SET_PATH = f"{DESTINATION_PATH}/DA_EmbermereUiIconSet"
RECRUIT_PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack"
MARSH_TONIC_PATH = "/Game/Data/Items/DI_MarshTonic"

TEXTURE_NAMES = (
    "T_Icon_Item_RecruitPack",
    "T_Icon_Item_MarshTonic",
    "T_Icon_Slot_MainHand",
    "T_Icon_Slot_OffHand",
    "T_Icon_Slot_Head",
    "T_Icon_Slot_Chest",
    "T_Icon_Slot_Hands",
    "T_Icon_Slot_Legs",
    "T_Icon_Slot_Feet",
    "T_Icon_Slot_Back",
    "T_Icon_Slot_Neck",
    "T_Icon_Slot_Ring",
    "T_Icon_Item_Missing",
    "T_Icon_Slot_Missing",
)


def import_textures():
    tasks = []
    for texture_name in TEXTURE_NAMES:
        source_path = SOURCE_DIR / f"{texture_name}.png"
        if not source_path.is_file():
            raise RuntimeError(f"Missing generated icon source: {source_path}")

        task = unreal.AssetImportTask()
        task.set_editor_property("filename", str(source_path))
        task.set_editor_property("destination_path", DESTINATION_PATH)
        task.set_editor_property("destination_name", texture_name)
        task.set_editor_property("automated", True)
        task.set_editor_property("replace_existing", True)
        task.set_editor_property("save", False)
        task.set_editor_property("factory", unreal.TextureFactory())
        tasks.append(task)

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)

    textures = {}
    for texture_name in TEXTURE_NAMES:
        asset_path = f"{DESTINATION_PATH}/{texture_name}"
        texture = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not isinstance(texture, unreal.Texture2D):
            raise RuntimeError(f"Icon did not import as Texture2D: {asset_path}")

        texture.set_editor_property(
            "compression_settings",
            unreal.TextureCompressionSettings.TC_EDITOR_ICON,
        )
        texture.set_editor_property(
            "mip_gen_settings",
            unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS,
        )
        texture.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
        texture.set_editor_property("srgb", True)
        texture.modify()
        if not unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False):
            raise RuntimeError(f"Could not save icon texture: {asset_path}")
        textures[texture_name] = texture

    return textures


def load_or_create_icon_set():
    icon_set = unreal.EditorAssetLibrary.load_asset(ICON_SET_PATH)
    if icon_set:
        return icon_set

    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", unreal.EmbermereUiIconSet)
    icon_set = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "DA_EmbermereUiIconSet",
        DESTINATION_PATH,
        unreal.EmbermereUiIconSet,
        factory,
    )
    if not icon_set:
        raise RuntimeError(f"Could not create UI icon set: {ICON_SET_PATH}")
    return icon_set


def configure_icon_set(icon_set, textures):
    icon_set.set_editor_property(
        "equipment_slot_icons",
        {
            unreal.EmbermereEquipmentSlot.MAIN_HAND: textures["T_Icon_Slot_MainHand"],
            unreal.EmbermereEquipmentSlot.OFF_HAND: textures["T_Icon_Slot_OffHand"],
            unreal.EmbermereEquipmentSlot.HEAD: textures["T_Icon_Slot_Head"],
            unreal.EmbermereEquipmentSlot.CHEST: textures["T_Icon_Slot_Chest"],
            unreal.EmbermereEquipmentSlot.HANDS: textures["T_Icon_Slot_Hands"],
            unreal.EmbermereEquipmentSlot.LEGS: textures["T_Icon_Slot_Legs"],
            unreal.EmbermereEquipmentSlot.FEET: textures["T_Icon_Slot_Feet"],
            unreal.EmbermereEquipmentSlot.BACK: textures["T_Icon_Slot_Back"],
            unreal.EmbermereEquipmentSlot.NECK: textures["T_Icon_Slot_Neck"],
            unreal.EmbermereEquipmentSlot.RING: textures["T_Icon_Slot_Ring"],
        },
    )
    icon_set.set_editor_property(
        "category_fallback_icons",
        {
            unreal.EmbermereItemCategory.MISC: textures["T_Icon_Item_Missing"],
            unreal.EmbermereItemCategory.CONSUMABLE: textures["T_Icon_Item_MarshTonic"],
            unreal.EmbermereItemCategory.WEAPON: textures["T_Icon_Slot_MainHand"],
            unreal.EmbermereItemCategory.ARMOR: textures["T_Icon_Slot_Chest"],
            unreal.EmbermereItemCategory.QUEST: textures["T_Icon_Item_Missing"],
        },
    )
    icon_set.set_editor_property("missing_item_icon", textures["T_Icon_Item_Missing"])
    icon_set.set_editor_property("missing_slot_icon", textures["T_Icon_Slot_Missing"])
    icon_set.modify()
    if not unreal.EditorAssetLibrary.save_loaded_asset(icon_set, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save UI icon set: {ICON_SET_PATH}")


def assign_item_icon(item_path, texture):
    item = unreal.EditorAssetLibrary.load_asset(item_path)
    if not item:
        raise RuntimeError(f"Missing starter item for icon assignment: {item_path}")
    item.set_editor_property("icon", texture)
    item.modify()
    if not unreal.EditorAssetLibrary.save_loaded_asset(item, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save starter item icon assignment: {item_path}")


def main():
    textures = import_textures()
    icon_set = load_or_create_icon_set()
    configure_icon_set(icon_set, textures)
    assign_item_icon(RECRUIT_PACK_PATH, textures["T_Icon_Item_RecruitPack"])
    assign_item_icon(MARSH_TONIC_PATH, textures["T_Icon_Item_MarshTonic"])
    unreal.log(
        "Embermere UI icons imported: 14 textures, 10 equipment slots, "
        "5 category fallbacks, and 2 starter-item assignments"
    )


if __name__ == "__main__":
    main()
