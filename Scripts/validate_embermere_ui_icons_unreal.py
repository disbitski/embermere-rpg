"""Validate Embermere's saved UI icon packages in a fresh Unreal process."""

import sys

import unreal


ICON_ROOT = "/Game/UI/Icons"
ABILITY_ICON_ROOT = f"{ICON_ROOT}/Abilities"
ICON_SET_PATH = f"{ICON_ROOT}/DA_EmbermereUiIconSet"
RECRUIT_PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack"
MARSH_TONIC_PATH = "/Game/Data/Items/DI_MarshTonic"
RULES_PATH = "/Game/Data/DA_EmbermereRules"
PAPER_DOLL_TEXTURE_NAME = "T_UI_PaperDoll_Backdrop"

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

ABILITY_TEXTURES = {
    "Strike": "T_Icon_Ability_Strike",
    "Taunt": "T_Icon_Ability_Taunt",
    "ShieldSlam": "T_Icon_Ability_ShieldSlam",
    "BattleShout": "T_Icon_Ability_BattleShout",
    "Smite": "T_Icon_Ability_Smite",
    "LesserHeal": "T_Icon_Ability_LesserHeal",
    "Ward": "T_Icon_Ability_Ward",
    "Judgment": "T_Icon_Ability_Judgment",
    "QuickShot": "T_Icon_Ability_QuickShot",
    "Snare": "T_Icon_Ability_Snare",
    "TwinCut": "T_Icon_Ability_TwinCut",
    "NaturesFocus": "T_Icon_Ability_NaturesFocus",
    "SparkBolt": "T_Icon_Ability_SparkBolt",
    "FrostRoot": "T_Icon_Ability_FrostRoot",
    "ArcaneBurst": "T_Icon_Ability_ArcaneBurst",
    "Meditate": "T_Icon_Ability_Meditate",
}

DEFAULT_DAMAGE = (unreal.EmbermereAbilityEffectType.DAMAGE, 0.0, 1.0)
ABILITY_EFFECTS = {
    "Strike": DEFAULT_DAMAGE,
    "Taunt": DEFAULT_DAMAGE,
    "ShieldSlam": DEFAULT_DAMAGE,
    "BattleShout": (unreal.EmbermereAbilityEffectType.ATTACK_POWER_BUFF, 10.0, 1.0),
    "Smite": DEFAULT_DAMAGE,
    "LesserHeal": (unreal.EmbermereAbilityEffectType.HEAL, 0.0, 1.0),
    "Ward": (unreal.EmbermereAbilityEffectType.ARMOR_BUFF, 10.0, 1.0),
    "Judgment": DEFAULT_DAMAGE,
    "QuickShot": DEFAULT_DAMAGE,
    "Snare": (unreal.EmbermereAbilityEffectType.DAMAGE, 6.0, 0.5),
    "TwinCut": DEFAULT_DAMAGE,
    "NaturesFocus": (unreal.EmbermereAbilityEffectType.ATTACK_POWER_BUFF, 10.0, 1.0),
    "SparkBolt": DEFAULT_DAMAGE,
    "FrostRoot": (unreal.EmbermereAbilityEffectType.DAMAGE, 4.0, 0.0),
    "ArcaneBurst": DEFAULT_DAMAGE,
    "Meditate": (unreal.EmbermereAbilityEffectType.RESTORE_MANA, 0.0, 1.0),
}

BASE_TEXTURE_NAMES = set(SLOT_TEXTURES.values()) | set(CATEGORY_TEXTURES.values()) | {
    "T_Icon_Item_RecruitPack",
    "T_Icon_Slot_Missing",
}
ABILITY_TEXTURE_NAMES = set(ABILITY_TEXTURES.values()) | {"T_Icon_Ability_Missing"}


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
    expected_root = ABILITY_ICON_ROOT if expected_name.startswith("T_Icon_Ability_") else ICON_ROOT
    expected_path = f"{expected_root}/{expected_name}"
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
    require_texture(
        icon_set.get_editor_property("missing_ability_icon"),
        "T_Icon_Ability_Missing",
        "missing-ability fallback",
    )
    require_texture(
        icon_set.get_editor_property("paper_doll_backdrop"),
        PAPER_DOLL_TEXTURE_NAME,
        "paper-doll backdrop",
    )

    for texture_name in sorted(BASE_TEXTURE_NAMES | ABILITY_TEXTURE_NAMES):
        texture_root = ABILITY_ICON_ROOT if texture_name in ABILITY_TEXTURE_NAMES else ICON_ROOT
        texture = load_asset(f"{texture_root}/{texture_name}", unreal.Texture2D)
        if texture.blueprint_get_size_x() != 128 or texture.blueprint_get_size_y() != 128:
            fail(
                f"{texture_name} expected 128x128, found "
                f"{texture.blueprint_get_size_x()}x{texture.blueprint_get_size_y()}"
            )
        if texture.get_editor_property("lod_group") != unreal.TextureGroup.TEXTUREGROUP_UI:
            fail(f"{texture_name} is not assigned to TEXTUREGROUP_UI")
        if texture.get_editor_property("mip_gen_settings") != unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS:
            fail(f"{texture_name} does not disable mip generation")

    paper_doll = load_asset(f"{ICON_ROOT}/{PAPER_DOLL_TEXTURE_NAME}", unreal.Texture2D)
    if paper_doll.blueprint_get_size_x() != 128 or paper_doll.blueprint_get_size_y() != 160:
        fail(
            f"{PAPER_DOLL_TEXTURE_NAME} expected 128x160, found "
            f"{paper_doll.blueprint_get_size_x()}x{paper_doll.blueprint_get_size_y()}"
        )
    if paper_doll.get_editor_property("lod_group") != unreal.TextureGroup.TEXTUREGROUP_UI:
        fail(f"{PAPER_DOLL_TEXTURE_NAME} is not assigned to TEXTUREGROUP_UI")
    if paper_doll.get_editor_property("mip_gen_settings") != unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS:
        fail(f"{PAPER_DOLL_TEXTURE_NAME} does not disable mip generation")

    recruit_pack = load_asset(RECRUIT_PACK_PATH, unreal.EmbermereItemData)
    marsh_tonic = load_asset(MARSH_TONIC_PATH, unreal.EmbermereItemData)
    if str(recruit_pack.get_editor_property("display_name")) != "Recruit Pack":
        fail(
            "Recruit Pack display name must stay compact for inventory and reward UI; "
            f"found {recruit_pack.get_editor_property('display_name')}"
        )
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

    rules = load_asset(RULES_PATH, unreal.EmbermereRulesData)
    abilities = list(rules.get_editor_property("abilities"))
    if len(abilities) != len(ABILITY_TEXTURES):
        fail(f"expected 16 starter abilities, found {len(abilities)}")

    found_ids = set()
    found_icon_paths = set()
    for ability in abilities:
        ability_id = str(ability.get_editor_property("ability_id"))
        expected_name = ABILITY_TEXTURES.get(ability_id)
        if not expected_name:
            fail(f"rules data contains unmapped ability {ability_id}")
        icon = ability.get_editor_property("icon")
        require_texture(icon, expected_name, f"{ability_id} ability icon")

        expected_effect, expected_duration, expected_movement_multiplier = ABILITY_EFFECTS[ability_id]
        actual_effect = ability.get_editor_property("effect_type")
        actual_duration = ability.get_editor_property("duration")
        actual_movement_multiplier = ability.get_editor_property("movement_speed_multiplier")
        if actual_effect != expected_effect:
            fail(f"{ability_id} effect expected {expected_effect}, found {actual_effect}")
        if abs(actual_duration - expected_duration) > 0.001:
            fail(f"{ability_id} duration expected {expected_duration}, found {actual_duration}")
        if abs(actual_movement_multiplier - expected_movement_multiplier) > 0.001:
            fail(
                f"{ability_id} movement multiplier expected "
                f"{expected_movement_multiplier}, found {actual_movement_multiplier}"
            )
        description = str(ability.get_editor_property("description")).lower()
        if "placeholder" in description or "full prototype" in description:
            fail(f"{ability_id} still exposes placeholder behavior copy")

        found_ids.add(ability_id)
        found_icon_paths.add(texture_path(icon))

    if found_ids != set(ABILITY_TEXTURES):
        fail(f"ability ID set mismatch: {sorted(found_ids)}")
    if len(found_icon_paths) != len(ABILITY_TEXTURES):
        fail(f"expected 16 distinct ability icons, found {len(found_icon_paths)}")

    unreal.log(
        "Embermere UI art validation passed: 31 saved 128x128 icon textures, "
        "one saved 128x160 paper-doll backdrop, "
        "10 equipment slots, 5 category fallbacks, 2 starter-item assignments, "
        "16 distinct ability assignments with saved gameplay semantics, and the "
        "compact Recruit Pack display name"
    )


if __name__ == "__main__":
    main()
