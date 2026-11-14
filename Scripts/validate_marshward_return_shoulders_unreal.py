"""Read-only initialized-world near-edge support and sampled shoulder clearance."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    plan = runpy.run_path(str(ROOT/'Scripts/marshward_return_review.py'))
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    targets = {}
    for label in ('Marshward_EastReturn','Marshward_BasinCatch'):
        matches = [actor for actor in actors if actor.get_actor_label()==label]
        assert len(matches)==1, 'Ambiguous support owner: '+label
        targets[label] = matches[0]

    def trace(start,end):
        return unreal.SystemLibrary.line_trace_single(world,unreal.Vector(*start),unreal.Vector(*end),
            unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,False)

    platform = basin = clearance = 0
    for x,y,expected,height in plan['support_samples']():
        hit = trace((x,y,300),(x,y,-150))
        assert hit, 'Missing return review support: '+str((x,y))
        data = hit.to_dict()
        assert data['hit_actor']==targets[expected], 'Unexpected first-hit actor'
        plan['require_support'](data['hit_actor'].get_actor_label(),data['impact_point'].z,expected,height)
        if expected=='Marshward_EastReturn':
            platform += 1
        else:
            basin += 1
    for start,end in plan['clearance_segments']():
        assert not trace(start,end), 'Sampled return lane blocked: '+str((start,end))
        clearance += 1
    assert (platform,basin,clearance)==(15,10,24)
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    unreal.log('EMBERMERE_MARSHWARD_RETURN_SHOULDERS_SUCCESS: 15 platform supports at Z0, 10 basin supports at Z-120, 24 sampled clearance rays, no ignored actors or content mutation')


if __name__ == '__main__':
    main()
