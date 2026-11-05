"""Initialized-editor basin-only material override; no geometry edits."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    basin = [a for a in actors if a.get_actor_label()=='Marshward_BasinCatch']
    assert len(basin)==1
    component = basin[0].get_component_by_class(unreal.StaticMeshComponent)
    original = component.get_material(0)
    assert original.get_name()=='M_MarshwardGreyboxBasin'
    mat = unreal.load_asset('/Game/Art/Embermere/Environment/MarshwardMarsh/M_MarshwardWetPeat')
    assert mat
    component.set_material(0,mat)
    try:
        runpy.run_path(str(ROOT/'Scripts/validate_marshward_wet_peat_unreal.py'))['validate_material'](actors)
        assert unreal.EditorLevelLibrary.save_current_level()
    except Exception:
        component.set_material(0,original)
        raise
    unreal.log('EMBERMERE_MARSHWARD_WET_PEAT_APPLY_SUCCESS: only basin slot-zero override saved')


if __name__ == '__main__':
    main()
