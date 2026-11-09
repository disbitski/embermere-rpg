"""Place only two owned foundation courses, comparing stable property values."""

import json
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def signature(actor):
    location,rotation,scale = actor.get_actor_location(),actor.get_actor_rotation(),actor.get_actor_scale3d()
    result = [(location.x,location.y,location.z),(rotation.pitch,rotation.yaw,rotation.roll),
              (scale.x,scale.y,scale.z),str(actor.get_folder_path()),[str(t) for t in actor.tags]]
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if component:
        result += [component.static_mesh.get_path_name(),str(component.get_collision_profile_name()),
                   str(component.get_collision_enabled()),[m.get_path_name() if m else None for m in component.get_materials()]]
    return result


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    preceding = {a:signature(a) for a in subsystem.get_all_level_actors()}
    assert sum(isinstance(a,unreal.StaticMeshActor) for a in preceding)==41
    assert all(signature(a)==before for a,before in preceding.items()), 'Unstable preservation signature'
    source = ROOT/'ArtSource/Blender/Environment/MarshwardMarsh'
    additions = json.loads((source/'foundation_placements.json').read_text())['placements']
    installed = json.loads((source/'placements.json').read_text())['placements']
    assert additions==[row for row in installed if row[1]=='Foundation'] and len(additions)==2
    assert all(a.get_actor_label() not in ['Marshward_'+r[0] for r in additions] for a in preceding)
    mesh = unreal.load_asset('/Game/Art/Embermere/Environment/MarshwardMarsh/SM_MarshwardFoundation_01')
    assert mesh
    created = []
    try:
        for label,kind,pos,yaw,scale in additions:
            rotation = unreal.Rotator()
            rotation.yaw = yaw
            actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*pos),rotation)
            assert actor
            created.append(actor)
            actor.set_actor_label('Marshward_'+label)
            actor.set_actor_scale3d(unreal.Vector(*scale))
            actor.set_folder_path('MarshwardGreybox/MarshTerrain')
            actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
            component = actor.get_component_by_class(unreal.StaticMeshComponent)
            assert component.set_static_mesh(mesh)
            component.set_collision_profile_name('BlockAll')
        assert all(signature(a)==before for a,before in preceding.items()), 'Preceding actor drift'
        runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))['validate_dressing'](list(subsystem.get_all_level_actors()))
        assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    except Exception:
        for actor in created:
            subsystem.destroy_actor(actor)
        raise
    unreal.log('EMBERMERE_MARSHWARD_FOUNDATION_PLACEMENT_SUCCESS: 2 foundation courses, prior 41 static placements retained, no route changes')


if __name__ == '__main__':
    main()
