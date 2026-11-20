"""Verify the original basin beneath the visual-only peat cushion."""

import json
import math
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

    tussock,basin = unique('Marshward_WestTussock'),unique('Marshward_BasinCatch')
    component = tussock.get_component_by_class(unreal.StaticMeshComponent)
    assert str(component.get_collision_profile_name())=='NoCollision'
    assert component.get_collision_enabled()==unreal.CollisionEnabled.NO_COLLISION
    source = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/west_tussock_metrics.json'
    samples = json.loads(source.read_text())['SM_MarshwardWestTussock_01']['foot_samples_cm']
    assert len(samples)==3 and len({tuple(sample) for sample in samples})==3
    for foot_x,foot_y,foot_z in samples:
        assert foot_z==0 and math.isfinite(foot_x) and math.isfinite(foot_y)
        # Classic FBX mirrors authored local Y at this yaw-zero placement.
        x,y = -2290+foot_x,3480-foot_y
        hit = unreal.SystemLibrary.line_trace_single(world,unreal.Vector(x,y,50),unreal.Vector(x,y,-140),
            unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,True)
        assert hit and hit.to_dict()['hit_actor']==basin
        assert abs(hit.to_dict()['impact_point'].z+120)<.5
    unreal.log('EMBERMERE_MARSHWARD_WEST_TUSSOCK_TRACES_SUCCESS: 3 unfiltered basin supports, 1 NoCollision tussock, no new support authority')


if __name__ == '__main__':
    main()
