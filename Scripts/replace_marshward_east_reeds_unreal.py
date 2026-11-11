"""Replace two reed mesh paths while preserving every other actor property."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    signature = runpy.run_path(str(ROOT/'Scripts/place_marshward_foundation_unreal.py'))['signature']
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    before = {a:signature(a) for a in actors}
    assert sum(isinstance(a,unreal.StaticMeshActor) for a in actors)==44
    assert all(signature(a)==value for a,value in before.items()), 'Unstable signature'
    by_label = {a.get_actor_label():a for a in actors}
    replacement = unreal.load_asset(DEST+'/SM_MarshwardTallReeds_01')
    assert replacement
    targets = []
    expected = {a:list(value) for a,value in before.items()}
    for label in ['Marshward_ReedsEastNear','Marshward_ReedsEastFar']:
        actor = by_label[label]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        old = component.static_mesh
        assert old.get_path_name()==DEST+'/SM_MarshwardReedClump_01.SM_MarshwardReedClump_01'
        assert str(component.get_collision_profile_name())=='NoCollision'
        assert component.get_collision_enabled()==unreal.CollisionEnabled.NO_COLLISION
        assert before[actor][8]==[slot.material_interface.get_path_name() for slot in replacement.get_editor_property('static_materials')]
        expected[actor][5] = replacement.get_path_name()
        targets.append((actor,component,old))
    try:
        for actor,component,old in targets:
            actor.modify()
            component.modify()
            assert component.set_static_mesh(replacement)
        assert all(signature(a)==value for a,value in expected.items()), 'Unexpected actor drift'
        runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))['validate_dressing'](actors)
        assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    except Exception:
        for actor,component,old in targets:
            component.set_static_mesh(old)
        raise
    unreal.log('EMBERMERE_MARSHWARD_TALL_REEDS_REPLACEMENT_SUCCESS: 2 mesh references only, 44 placements and all other properties retained')


if __name__ == '__main__':
    main()
