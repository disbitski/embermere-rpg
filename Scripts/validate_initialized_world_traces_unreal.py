"""Run Embermere's collision and route suites in an initialized editor world."""

import importlib
import sys
from pathlib import Path

import unreal


SCRIPT_DIR = Path("/Users/wizard/Documents/Unreal Game/Scripts")
TRACE_MODULES = (
    "validate_road_boundary_traces_unreal",
    "validate_fenwatch_vendor_stall_traces_unreal",
    "validate_fenwatch_cottage_traces_unreal",
    "validate_fenwatch_training_workshop_traces_unreal",
    "validate_fenwatch_notice_board_traces_unreal",
    "validate_fenwatch_communal_well_traces_unreal",
    "validate_fenwatch_handcart_traces_unreal",
    "validate_fenwatch_firewood_rack_traces_unreal",
    "validate_fenwatch_north_cottage_traces_unreal",
    "validate_fenwatch_hitching_trough_traces_unreal",
    "validate_fenwatch_gatewatch_post_traces_unreal",
    "validate_fenwatch_herb_garden_traces_unreal",
    "validate_fenwatch_marshward_shrine_traces_unreal",
    "validate_marsh_prowler_pull_lanes_unreal",
    "validate_fenwatch_interaction_approaches_unreal",
    "validate_ancient_ruin_relief_traces_unreal",
)


def main():
    script_path = str(SCRIPT_DIR)
    if script_path not in sys.path:
        sys.path.insert(0, script_path)

    for module_name in TRACE_MODULES:
        importlib.import_module(module_name).main()

    unreal.log(
        "EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: {} sequential suites".format(
            len(TRACE_MODULES)
        )
    )


if __name__ == "__main__":
    main()
