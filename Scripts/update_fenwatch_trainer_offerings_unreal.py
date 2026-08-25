"""Update only the saved Fenwatch offerings asset without resaving the map."""

import sys
from pathlib import Path

import unreal


SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from configure_fenwatch_trainer_unreal import OFFERINGS_PATH, load_or_create_offerings


def main():
    offerings = load_or_create_offerings()
    entries = list(offerings.get_editor_property("offerings"))
    if len(entries) != 2:
        unreal.log_error(
            "Embermere Fenwatch trainer offerings update failed: expected 2 entries, found {}".format(
                len(entries)
            )
        )
        sys.exit(1)
    unreal.log(
        "Embermere Fenwatch trainer offerings update passed: asset={}, entries=2".format(
            OFFERINGS_PATH
        )
    )


if __name__ == "__main__":
    main()
