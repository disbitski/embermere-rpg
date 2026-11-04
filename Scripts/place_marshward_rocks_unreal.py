"""Place four owned clusters in the initialized map without editing prior art."""

import json
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
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    data = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())
    rows = [row for row in data['placements'] if row[1]=='RockCluster']
    assert len(rows)==4
    labels = {actor.get_actor_label() for actor in subsystem.get_all_level_actors()}
    assert all('Marshward_'+row[0] not in labels for row in rows), 'Refusing duplicate placement'
    mesh = unreal.load_asset(DEST+'/SM_MarshwardRockCluster_01')
    assert mesh
    for label,kind,pos,yaw,scale in rows:
        rotation = unreal.Rotator()
        rotation.yaw = yaw
        actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*pos),rotation)
        assert actor
        actor.set_actor_label('Marshward_'+label)
        actor.set_actor_scale3d(unreal.Vector(*scale))
        actor.set_folder_path('MarshwardGreybox/MarshTerrain')
        actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(mesh)
        component.set_collision_profile_name('BlockAll')
    runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))['validate_dressing'](list(subsystem.get_all_level_actors()))
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    unreal.log('EMBERMERE_MARSHWARD_ROCK_PLACEMENT_SUCCESS: 4 grounded clusters, prior 36 placements and walking corridors retained')


if __name__ == '__main__':
    main()
