"""Run the prototype's saved-package validators sequentially in one commandlet."""

import importlib
import sys
from pathlib import Path

import unreal


SCRIPT_DIR = Path(__file__).resolve().parent
VALIDATOR_MODULES = (
    "validate_embermere_ui_icons_unreal",
    "validate_fenwatch_armsmaster_rig_unreal",
    "validate_fenwatch_communal_well_unreal",
    "validate_fenwatch_cottage_unreal",
    "validate_fenwatch_keeper_greeting_unreal",
    "validate_fenwatch_keeper_rig_unreal",
    "validate_fenwatch_notice_board_unreal",
    "validate_fenwatch_practice_dummy_unreal",
    "validate_fenwatch_practice_target_gameplay_unreal",
    "validate_fenwatch_quartermaster_rig_unreal",
    "validate_fenwatch_rest_presentation_unreal",
    "validate_fenwatch_rest_service_unreal",
    "validate_fenwatch_still_waters_unreal",
    "validate_fenwatch_trainer_unreal",
    "validate_fenwatch_training_workshop_unreal",
    "validate_fenwatch_vendor_stall_unreal",
    "validate_fenwatch_vendor_unreal",
    "validate_level_progression_unreal",
    "validate_fab_zone_pass_unreal",
)


def main():
    script_path = str(SCRIPT_DIR)
    if script_path not in sys.path:
        sys.path.insert(0, script_path)

    completed = []
    for module_name in VALIDATOR_MODULES:
        module = importlib.import_module(module_name)
        module.main()
        completed.append(module_name)

    unreal.log(
        "Embermere saved prototype package validation passed: "
        "validators={}, modules={}".format(len(completed), completed)
    )


if __name__ == "__main__":
    main()
