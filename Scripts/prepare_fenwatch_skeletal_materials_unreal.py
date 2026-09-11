"""Repair only missing project-owned rig material flags; do not reimport art."""

from pathlib import Path
import sys

import unreal

sys.path.insert(0, str(Path(__file__).resolve().parent))

from fenwatch_skeletal_materials_unreal import prepare_skeletal_materials
from import_embermere_fenwatch_armsmaster_rigged_unreal import SHARED_MATERIALS as ARMSMASTER
from import_embermere_fenwatch_keeper_rigged_unreal import SHARED_MATERIALS as KEEPER
from import_embermere_fenwatch_quartermaster_rigged_unreal import SHARED_MATERIALS as QUARTERMASTER


def main():
    paths = set(ARMSMASTER.values()) | set(KEEPER.values()) | set(QUARTERMASTER.values())
    saved = prepare_skeletal_materials(paths)
    unreal.log("EMBERMERE_FENWATCH_SKELETAL_MATERIAL_PREPARATION_SUCCESS: saved={}".format(saved))


if __name__ == "__main__":
    main()
