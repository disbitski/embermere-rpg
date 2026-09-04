"""Validate the two saved quest-owned objective-display contracts."""

import sys

import unreal


EXPECTED_QUESTS = {
    "/Game/Data/Quests/DQ_FirstSignsAtTheRuin": {
        "quest_id": "FirstSignsAtTheRuin",
        "objective_id": "StarterEnemyDefeated",
        "required_objective_count": 3,
        "objective_instructions": "Defeat 3 Marsh Prowlers.",
    },
    "/Game/Data/Quests/DQ_FenwatchStillWaters": {
        "quest_id": "FenwatchStillWaters",
        "objective_id": "FenwatchRestCompleted",
        "required_objective_count": 1,
        "objective_instructions": "Complete a rest at the communal well.",
    },
}


def fail(message):
    unreal.log_error(
        "Embermere quest objective-display validation failed: {}".format(
            message
        )
    )
    sys.exit(1)


def main():
    validated = []
    for asset_path, expected in EXPECTED_QUESTS.items():
        quest = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not quest or not isinstance(quest, unreal.EmbermereQuestData):
            fail("{} is missing or uses the wrong class".format(asset_path))

        for property_name in ("quest_id", "objective_id"):
            actual = str(quest.get_editor_property(property_name))
            if actual != expected[property_name]:
                fail(
                    "{} {} drifted: {}".format(
                        asset_path, property_name, actual
                    )
                )
        actual_requirement = int(
            quest.get_editor_property("required_objective_count")
        )
        if actual_requirement != expected["required_objective_count"]:
            fail(
                "{} required_objective_count drifted: {}".format(
                    asset_path, actual_requirement
                )
            )
        actual_instructions = str(
            quest.get_editor_property("objective_instructions")
        )
        if actual_instructions != expected["objective_instructions"]:
            fail(
                "{} objective_instructions drifted: {!r}".format(
                    asset_path, actual_instructions
                )
            )
        if actual_instructions in {
            str(quest.get_editor_property("title")),
            str(quest.get_editor_property("description")),
            str(quest.get_editor_property("available_greeting")),
            str(quest.get_editor_property("active_greeting")),
            str(quest.get_editor_property("ready_greeting")),
            str(quest.get_editor_property("completed_greeting")),
        }:
            fail("{} objective instructions duplicate other copy".format(asset_path))
        validated.append(quest.get_name())

    if len(set(
        expected["objective_instructions"]
        for expected in EXPECTED_QUESTS.values()
    )) != len(EXPECTED_QUESTS):
        fail("authored quest objectives are not distinct")

    unreal.log(
        "Embermere quest objective-display validation passed: {} retain "
        "distinct authored instructions, stable IDs, and numeric requirements"
        .format(", ".join(validated))
    )


if __name__ == "__main__":
    main()
