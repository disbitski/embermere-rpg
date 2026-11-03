"""Place only the new owned terrain/art through the initialized editor."""

import json
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'
DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith(MAP+'.'), 'Open Causeway before placement'
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    data = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    existing = {a.get_actor_label():a for a in subsystem.get_all_level_actors()}
    assert all('Marshward_'+row[0] not in existing for row in data['placements']), 'Refusing duplicate placement'
    meshes = {kind:unreal.load_asset(DEST+'/SM_Marshward'+kind+'_01') for kind in {row[1] for row in data['placements']}}
    assert all(meshes.values()), 'Import all terrain variants before placement'
    for label,kind,pos,yaw,scale in data['placements']:
        mesh = meshes[kind]
        rotation = unreal.Rotator()
        rotation.yaw = yaw
        actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*pos),rotation)
        actor.set_actor_label('Marshward_'+label)
        actor.set_actor_scale3d(unreal.Vector(*scale))
        actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
        actor.set_folder_path('MarshwardGreybox/MarshTerrain')
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(mesh)
        component.set_collision_profile_name('BlockAll' if kind in ('PeatBerm','PeatShelf') else 'NoCollision')
    validator = runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))
    validator['validate_dressing'](list(subsystem.get_all_level_actors()))
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    unreal.log('EMBERMERE_MARSHWARD_MARSH_PLACEMENT_SUCCESS: 4 berms, 2 shelves, 8 reed clumps, full walking corridors protected')


if __name__ == '__main__':
    main()
