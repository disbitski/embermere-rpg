"""Validate Embermere's saved level progression data asset."""

import math
import unreal


RULES_PATH = "/Game/Data/DA_EmbermereRules"
EXPECTED_THRESHOLDS = [0, 100, 250, 450, 700]
EXPECTED_RACE_NAMES = {
    "Human", "Elf", "Dwarf", "Gnome", "Dark Elf", "Lizardman", "Ogre", "Bullywug"
}
EXPECTED_CLASS_NAMES = {"Warrior", "Cleric", "Ranger", "Wizard"}


def validate_growth(definition, label):
    growth = definition.get_editor_property("level_growth")
    values = [
        float(growth.get_editor_property(field))
        for field in ("max_health", "max_mana", "strength", "spirit", "agility", "intellect")
    ]
    if any(not math.isfinite(value) or value < 0.0 for value in values):
        raise RuntimeError(f"{label} has invalid level growth: {values}")
    if not any(value > 0.0 for value in values):
        raise RuntimeError(f"{label} has no authored level growth")


def main():
    rules = unreal.EditorAssetLibrary.load_asset(RULES_PATH)
    if not isinstance(rules, unreal.EmbermereRulesData):
        raise RuntimeError(f"Missing Embermere rules data: {RULES_PATH}")

    thresholds = list(rules.get_editor_property("experience_thresholds"))
    if thresholds != EXPECTED_THRESHOLDS:
        raise RuntimeError(f"Unexpected XP thresholds: {thresholds}")

    races = list(rules.get_editor_property("races"))
    classes = list(rules.get_editor_property("classes"))
    race_names = {str(entry.get_editor_property("display_name")) for entry in races}
    class_names = {str(entry.get_editor_property("display_name")) for entry in classes}
    if race_names != EXPECTED_RACE_NAMES:
        raise RuntimeError(f"Unexpected race definitions: {sorted(race_names)}")
    if class_names != EXPECTED_CLASS_NAMES:
        raise RuntimeError(f"Unexpected class definitions: {sorted(class_names)}")
    for entry in races:
        validate_growth(entry, f"Race {entry.get_editor_property('display_name')}")
    for entry in classes:
        validate_growth(entry, f"Class {entry.get_editor_property('display_name')}")

    unreal.log(
        "EMBERMERE_LEVEL_PROGRESSION_VALIDATION_SUCCESS: "
        "levels=5 thresholds=0,100,250,450,700 races=8 classes=4"
    )


if __name__ == "__main__":
    main()
