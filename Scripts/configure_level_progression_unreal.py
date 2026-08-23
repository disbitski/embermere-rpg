"""Persist Embermere's first data-driven level progression rules.

Run after the progression schema is compiled. The operation is idempotent and
only updates the project-owned DA_EmbermereRules package.
"""

import unreal


RULES_PATH = "/Game/Data/DA_EmbermereRules"
THRESHOLDS = [0, 100, 250, 450, 700]
RACE_GROWTH = {
    "Human": (2.0, 2.0, 0.5, 0.5, 0.5, 0.5),
    "Elf": (1.0, 4.0, 0.25, 0.5, 0.75, 1.0),
    "Dwarf": (5.0, 1.0, 1.0, 0.5, 0.25, 0.25),
    "Gnome": (1.0, 5.0, 0.25, 0.75, 0.5, 1.0),
    "Dark Elf": (2.0, 4.0, 0.5, 0.5, 0.75, 0.75),
    "Lizardman": (4.0, 1.0, 0.75, 0.25, 0.75, 0.25),
    "Ogre": (6.0, 0.0, 1.25, 0.25, 0.25, 0.0),
    "Bullywug": (3.0, 2.0, 0.5, 0.75, 0.5, 0.25),
}
CLASS_GROWTH = {
    "Warrior": (8.0, 1.0, 1.5, 0.5, 0.75, 0.25),
    "Cleric": (5.0, 6.0, 0.5, 1.5, 0.5, 1.0),
    "Ranger": (6.0, 3.0, 1.0, 0.5, 1.5, 0.5),
    "Wizard": (3.0, 8.0, 0.25, 1.0, 0.5, 1.75),
}


def make_growth(values):
    growth = unreal.EmbermereAttributeGrowth()
    for field, value in zip(
        ("max_health", "max_mana", "strength", "spirit", "agility", "intellect"),
        values,
    ):
        growth.set_editor_property(field, value)
    return growth


def configure_definitions(definitions, expected_growth, label):
    seen = set()
    for definition in definitions:
        name = str(definition.get_editor_property("display_name"))
        values = expected_growth.get(name)
        if values is None:
            raise RuntimeError(f"Unexpected {label} definition in rules data: {name}")
        definition.set_editor_property("level_growth", make_growth(values))
        seen.add(name)
    missing = set(expected_growth) - seen
    if missing:
        raise RuntimeError(f"Rules data is missing {label} definitions: {sorted(missing)}")


def main():
    rules = unreal.EditorAssetLibrary.load_asset(RULES_PATH)
    if not isinstance(rules, unreal.EmbermereRulesData):
        raise RuntimeError(f"Missing Embermere rules data: {RULES_PATH}")

    races = list(rules.get_editor_property("races"))
    classes = list(rules.get_editor_property("classes"))
    configure_definitions(races, RACE_GROWTH, "race")
    configure_definitions(classes, CLASS_GROWTH, "class")
    rules.set_editor_property("races", races)
    rules.set_editor_property("classes", classes)
    rules.set_editor_property("experience_thresholds", THRESHOLDS)
    rules.modify()
    if not unreal.EditorAssetLibrary.save_loaded_asset(rules, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save level progression rules: {RULES_PATH}")

    unreal.log(
        "EMBERMERE_LEVEL_PROGRESSION_CONFIGURED: "
        "levels=5 thresholds=0,100,250,450,700 races=8 classes=4"
    )


if __name__ == "__main__":
    main()
