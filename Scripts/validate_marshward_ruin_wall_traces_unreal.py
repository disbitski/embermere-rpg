"""Read-only checks for the wall's three courses and its open shoulders."""

import json
from pathlib import Path

import unreal


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())

    def unique(label):
        matches = [actor for actor in actors if actor.get_actor_label()==label]
        assert len(matches)==1,label
        return matches[0]

    wall,basin = unique('Marshward_RuinWallNortheast'),unique('Marshward_BasinCatch')
    comp = wall.get_component_by_class(unreal.StaticMeshComponent)
    assert str(comp.get_collision_profile_name())=='BlockAll'
    assert comp.get_collision_enabled()==unreal.CollisionEnabled.QUERY_AND_PHYSICS
    source = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/ruin_wall_metrics.json'
    spec = json.loads(source.read_text())['SM_MarshwardRuinWall_01']

    def trace(start,end):
        return unreal.SystemLibrary.line_trace_single(world,unreal.Vector(*start),unreal.Vector(*end),
            unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,True)

    def require_height(hit,actor,expected):
        assert hit and hit.to_dict()['hit_actor']==actor
        assert abs(hit.to_dict()['impact_point'].z-expected)<.5

    assert len(spec['crest_samples_cm'])==3
    assert len(spec['ground_samples_cm'])==6
    assert len(spec['air_samples_cm'])==2
    # Classic FBX mirrors local Y at yaw zero.
    for lx,ly,height in spec['crest_samples_cm']:
        x,y = 1900+lx,7600-ly
        require_height(trace((x,y,500),(x,y,-140)),wall,height-120)
    for lx,ly,height in spec['ground_samples_cm']:
        assert height==0
        x,y = 1900+lx,7600-ly
        require_height(trace((x,y,500),(x,y,-140)),basin,-120)
    for lx,height in spec['air_samples_cm']:
        x,z = 1900+lx,height-120
        assert not trace((x,7440,z),(x,7760,z)), (x,z)

    unreal.log('EMBERMERE_MARSHWARD_RUIN_WALL_TRACES_SUCCESS: 3 stepped crest hits, 6 original basin supports, 2 open shoulder rays, no ignored actors')


if __name__ == '__main__':
    main()
