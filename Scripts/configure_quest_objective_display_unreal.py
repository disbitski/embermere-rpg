"""Author the player-facing objective instructions on both Fenwatch quests."""

import sys

import unreal


QUEST_OBJECTIVES = {
    "/Game/Data/Quests/DQ_FirstSignsAtTheRuin": "Defeat 3 Marsh Prowlers.",
    "/Game/Data/Quests/DQ_FenwatchStillWaters": (
        "Complete a rest at the communal well."
    ),
}


def fail(message):
    unreal.log_error(
        "Embermere quest objective-display configuration failed: {}".format(
            message
        )
    )
    sys.exit(1)


def main():
    configured = []
    for asset_path, objective_instructions in QUEST_OBJECTIVES.items():
        quest = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not quest or not isinstance(quest, unreal.EmbermereQuestData):
            fail("{} is missing or uses the wrong class".format(asset_path))
        quest.set_editor_property(
            "objective_instructions", objective_instructions
        )
        if not unreal.EditorAssetLibrary.save_loaded_asset(
            quest, only_if_is_dirty=False
        ):
            fail("could not save {}".format(asset_path))
        configured.append(quest.get_name())

    unreal.log(
        "Embermere quest objective-display configuration passed: {} own "
        "distinct authored instructions".format(", ".join(configured))
    )


if __name__ == "__main__":
    main()
