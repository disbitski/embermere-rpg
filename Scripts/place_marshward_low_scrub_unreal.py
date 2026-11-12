"""Add one NoCollision scrub while preserving all preceding actor signatures."""

import json
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
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    preceding = {a:signature(a) for a in subsystem.get_all_level_actors()}
    assert sum(isinstance(a,unreal.StaticMeshActor) for a in preceding)==44
    assert all(signature(a)==before for a,before in preceding.items()), 'Unstable preservation signature'
    rows = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())['placements']
    additions = [row for row in rows if row[1]=='LowScrub']
    assert additions==[['ScrubEast','LowScrub',[3550,2450,-120],0,[1,1,1]]]
    label,kind,pos,yaw,scale = additions[0]
    assert all(a.get_actor_label()!='Marshward_'+label for a in preceding)
    mesh = unreal.load_asset('/Game/Art/Embermere/Environment/MarshwardMarsh/SM_MarshwardLowScrub_01')
    assert mesh
    rotation = unreal.Rotator()
    rotation.yaw = yaw
    actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*pos),rotation)
    assert actor
    try:
        actor.set_actor_label('Marshward_'+label)
        actor.set_actor_scale3d(unreal.Vector(*scale))
        actor.set_folder_path('MarshwardGreybox/MarshTerrain')
        actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(mesh)
        component.set_collision_profile_name('NoCollision')
        assert all(signature(a)==before for a,before in preceding.items()), 'Preceding actor drift'
        runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))['validate_dressing'](list(subsystem.get_all_level_actors()))
        assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    except Exception:
        subsystem.destroy_actor(actor)
        raise
    unreal.log('EMBERMERE_MARSHWARD_LOW_SCRUB_PLACEMENT_SUCCESS: 1 NoCollision clump, prior 44 static placements retained, no route changes')


if __name__ == '__main__':
    main()
