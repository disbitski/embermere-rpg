"""Configure Embermere's saved starter-ability behavior.

Run after ability-schema changes with UnrealEditor-Cmd -ExecutePythonScript or
from the Unreal Python console. The operation is idempotent and preserves each
ability's project-owned icon reference.
"""

import unreal


RULES_PATH = "/Game/Data/DA_EmbermereRules"

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

UPDATED_DESCRIPTIONS = {
    "BattleShout": "Raises Attack Power by 8 for 10 seconds.",
    "Ward": "Raises Armor by 10 for 10 seconds.",
    "Snare": "Deals light damage and slows the target by 50% for 6 seconds.",
    "NaturesFocus": "Raises Attack Power by 8 for 10 seconds.",
    "FrostRoot": "Deals light damage and roots the target for 4 seconds.",
    "Meditate": "Restores 18 mana.",
}


def main():
    rules = unreal.EditorAssetLibrary.load_asset(RULES_PATH)
    if not isinstance(rules, unreal.EmbermereRulesData):
        raise RuntimeError(f"Missing Embermere rules data: {RULES_PATH}")

    abilities = list(rules.get_editor_property("abilities"))
    configured_ids = set()
    for ability in abilities:
        ability_id = str(ability.get_editor_property("ability_id"))
        semantics = ABILITY_EFFECTS.get(ability_id)
        if not semantics:
            raise RuntimeError(f"Unexpected starter ability in rules data: {ability_id}")

        effect_type, duration, movement_multiplier = semantics
        ability.set_editor_property("effect_type", effect_type)
        ability.set_editor_property("duration", duration)
        ability.set_editor_property("movement_speed_multiplier", movement_multiplier)
        if ability_id in UPDATED_DESCRIPTIONS:
            ability.set_editor_property("description", UPDATED_DESCRIPTIONS[ability_id])
        configured_ids.add(ability_id)

    missing_ids = set(ABILITY_EFFECTS) - configured_ids
    if missing_ids:
        raise RuntimeError(f"Rules data is missing starter abilities: {sorted(missing_ids)}")

    rules.set_editor_property("abilities", abilities)
    rules.modify()
    if not unreal.EditorAssetLibrary.save_loaded_asset(rules, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save starter ability behavior: {RULES_PATH}")

    unreal.log(
        "Embermere starter abilities configured: 16 primary effects, "
        "Battle Shout/Nature's Focus power buffs, Ward armor, Snare slow, "
        "Frost Root control, and Meditate mana recovery"
    )


if __name__ == "__main__":
    main()
