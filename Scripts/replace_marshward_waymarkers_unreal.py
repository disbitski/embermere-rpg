"""Replace only three existing marker mesh paths after saved-package parity."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    signature = runpy.run_path(str(ROOT/'Scripts/place_marshward_foundation_unreal.py'))['signature']
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    before = {a:signature(a) for a in actors}
    assert sum(isinstance(a,unreal.StaticMeshActor) for a in actors)==45
    assert all(signature(a)==value for a,value in before.items()), 'Unstable signature'
    by_label = {a.get_actor_label():a for a in actors}
    contract = runpy.run_path(str(ROOT/'Scripts/validate_marshward_waymarker_unreal.py'))
    contract['validate_mesh']()
    replacement = unreal.load_asset(contract['NEW'])
    targets = []
    expected = {a:list(value) for a,value in before.items()}
    for label,kind,pos,yaw in contract['marker_roles']():
        actor = by_label['Marshward_'+label]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        old = component.static_mesh
        assert old.get_path_name()==contract['OLD']+'.'+contract['OLD'].rsplit('/',1)[1]
        assert before[actor][6]=='BlockAll'
        assert before[actor][8]==[contract['MATERIAL']]
        expected[actor][5] = replacement.get_path_name()
        targets.append((actor,component,old))
    try:
        for actor,component,old in targets:
            actor.modify()
            component.modify()
            assert component.set_static_mesh(replacement)
        assert all(signature(a)==value for a,value in expected.items()), 'Unexpected actor drift'
        assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    except Exception:
        for actor,component,old in targets:
            component.set_static_mesh(old)
        raise
    unreal.log('EMBERMERE_MARSHWARD_WAYMARKER_REPLACEMENT_SUCCESS: 3 mesh references only, 45 placements and all other properties retained')


if __name__ == '__main__':
    main()
