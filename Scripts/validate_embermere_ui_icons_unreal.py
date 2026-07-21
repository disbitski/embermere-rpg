"""Validate Embermere's saved UI icon packages in a fresh Unreal process."""

import sys

import unreal


ICON_ROOT = "/Game/UI/Icons"
ICON_SET_PATH = f"{ICON_ROOT}/DA_EmbermereUiIconSet"
RECRUIT_PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack"
MARSH_TONIC_PATH = "/Game/Data/Items/DI_MarshTonic"

SLOT_TEXTURES = {
    unreal.EmbermereEquipmentSlot.MAIN_HAND: "T_Icon_Slot_MainHand",
    unreal.EmbermereEquipmentSlot.OFF_HAND: "T_Icon_Slot_OffHand",
    unreal.EmbermereEquipmentSlot.HEAD: "T_Icon_Slot_Head",
    unreal.EmbermereEquipmentSlot.CHEST: "T_Icon_Slot_Chest",
    unreal.EmbermereEquipmentSlot.HANDS: "T_Icon_Slot_Hands",
    unreal.EmbermereEquipmentSlot.LEGS: "T_Icon_Slot_Legs",
    unreal.EmbermereEquipmentSlot.FEET: "T_Icon_Slot_Feet",
    unreal.EmbermereEquipmentSlot.BACK: "T_Icon_Slot_Back",
    unreal.EmbermereEquipmentSlot.NECK: "T_Icon_Slot_Neck",
    unreal.EmbermereEquipmentSlot.RING: "T_Icon_Slot_Ring",
}

CATEGORY_TEXTURES = {
    unreal.EmbermereItemCategory.MISC: "T_Icon_Item_Missing",
    unreal.EmbermereItemCategory.CONSUMABLE: "T_Icon_Item_MarshTonic",
    unreal.EmbermereItemCategory.WEAPON: "T_Icon_Slot_MainHand",
    unreal.EmbermereItemCategory.ARMOR: "T_Icon_Slot_Chest",
    unreal.EmbermereItemCategory.QUEST: "T_Icon_Item_Missing",
}

ALL_TEXTURE_NAMES = set(SLOT_TEXTURES.values()) | set(CATEGORY_TEXTURES.values()) | {
    "T_Icon_Item_RecruitPack",
    "T_Icon_Slot_Missing",
}


def fail(message):
    unreal.log_error(f"Embermere UI icon validation failed: {message}")
    sys.exit(1)


def load_asset(path, expected_type):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not isinstance(asset, expected_type):
        fail(f"{path} did not load as {expected_type.__name__}")
    return asset


def texture_path(texture):
    if not texture:
        return "None"
    return texture.get_path_name().split(".", 1)[0]


def require_texture(actual, expected_name, context):
    expected_path = f"{ICON_ROOT}/{expected_name}"
    if texture_path(actual) != expected_path:
        fail(f"{context} expected {expected_path}, found {texture_path(actual)}")


def main():
    icon_set = load_asset(ICON_SET_PATH, unreal.EmbermereUiIconSet)
    slot_icons = icon_set.get_editor_property("equipment_slot_icons")
    category_icons = icon_set.get_editor_property("category_fallback_icons")

    if len(slot_icons) != len(SLOT_TEXTURES):
        fail(f"expected 10 slot mappings, found {len(slot_icons)}")
    if len(category_icons) != len(CATEGORY_TEXTURES):
        fail(f"expected 5 category mappings, found {len(category_icons)}")

    for slot, expected_name in SLOT_TEXTURES.items():
        require_texture(slot_icons.get(slot), expected_name, f"slot {slot}")
    for category, expected_name in CATEGORY_TEXTURES.items():
        require_texture(category_icons.get(category), expected_name, f"category {category}")

    require_texture(
        icon_set.get_editor_property("missing_item_icon"),
        "T_Icon_Item_Missing",
        "missing-item fallback",
    )
    require_texture(
        icon_set.get_editor_property("missing_slot_icon"),
        "T_Icon_Slot_Missing",
        "missing-slot fallback",
    )

    for texture_name in sorted(ALL_TEXTURE_NAMES):
        texture = load_asset(f"{ICON_ROOT}/{texture_name}", unreal.Texture2D)
        if texture.blueprint_get_size_x() != 128 or texture.blueprint_get_size_y() != 128:
            fail(
                f"{texture_name} expected 128x128, found "
                f"{texture.blueprint_get_size_x()}x{texture.blueprint_get_size_y()}"
            )
        if texture.get_editor_property("lod_group") != unreal.TextureGroup.TEXTUREGROUP_UI:
            fail(f"{texture_name} is not assigned to TEXTUREGROUP_UI")
        if texture.get_editor_property("mip_gen_settings") != unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS:
            fail(f"{texture_name} does not disable mip generation")

    recruit_pack = load_asset(RECRUIT_PACK_PATH, unreal.EmbermereItemData)
    marsh_tonic = load_asset(MARSH_TONIC_PATH, unreal.EmbermereItemData)
    require_texture(
        recruit_pack.get_editor_property("icon"),
        "T_Icon_Item_RecruitPack",
        "Recruit Pack item icon",
    )
    require_texture(
        marsh_tonic.get_editor_property("icon"),
        "T_Icon_Item_MarshTonic",
        "Marsh Tonic item icon",
    )

    unreal.log(
        "Embermere UI icon validation passed: 14 saved 128x128 UI textures, "
        "10 equipment slots, 5 category fallbacks, and 2 starter-item assignments"
    )


if __name__ == "__main__":
    main()
