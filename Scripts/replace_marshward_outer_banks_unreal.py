"""Change only two owned mesh references in the initialized Causeway map."""

import runpy
import unreal

DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    by_label = {a.get_actor_label():a for a in actors}
    replacement = unreal.load_asset(DEST+'/SM_MarshwardPeatShelf_01')
    assert replacement
    targets = []
    for label in ['Marshward_BermWestFar','Marshward_BermEast']:
        actor = by_label[label]
        assert 'EmbermereMarshwardMarsh' in [str(tag) for tag in actor.tags]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.static_mesh.get_path_name()==DEST+'/SM_MarshwardPeatBerm_01.SM_MarshwardPeatBerm_01'
        assert str(component.get_collision_profile_name())=='BlockAll'
        targets.append((actor,component))
    for actor,component in targets:
        actor.modify()
        component.modify()
        assert component.set_static_mesh(replacement)
    runpy.run_path('/Users/wizard/Documents/Unreal Game/Scripts/validate_marshward_marsh_unreal.py')['validate_dressing'](actors)
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    unreal.log('EMBERMERE_MARSHWARD_SHELF_REPLACEMENT_SUCCESS: two mesh references only, actor transforms/count and route retained')


if __name__ == '__main__':
    main()
