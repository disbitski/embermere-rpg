"""Initialized-world support and player-width clearance for the complete loop."""

import json
import math
import runpy
from pathlib import Path
import unreal


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    layout = json.loads(Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())

    def trace(a,b,ignore=()):
        return unreal.SystemLibrary.line_trace_single(world, unreal.Vector(*a), unreal.Vector(*b),
            unreal.TraceTypeQuery.ECC_VISIBILITY, False, list(ignore), unreal.DrawDebugTrace.NONE, True)

    supports = clearance = 0
    for start,end in zip(layout['route'],layout['route'][1:]):
        length = math.dist(start,end)
        count = math.ceil(length/100)
        dx,dy = end[0]-start[0], end[1]-start[1]
        horizontal = math.hypot(dx,dy)
        normal = (-dy/horizontal, dx/horizontal)
        for offset in [-50,0,50]:
            previous = None
            for index in range(count+1):
                t = index/count
                x = start[0] + dx*t + normal[0]*offset
                y = start[1] + dy*t + normal[1]*offset
                z = start[2] + (end[2]-start[2])*t
                hit = trace((x,y,z+300),(x,y,z-20))
                assert hit, 'Missing route support: ' + str((x,y,z))
                actual = hit.to_dict()['impact_point']
                assert abs(actual.z-z)<2, 'Unexpected support: ' + str(actual)
                supports += 1
                if previous:
                    for height in [95,155]:
                        assert not trace((previous[0],previous[1],previous[2]+height),(x,y,z+height)), 'Route blocked: ' + str((x,y,z))
                        clearance += 1
                previous = (x,y,z)
    control = trace((-1000,650,200),(-500,650,200))
    assert control and control.to_dict()['hit_actor'].get_actor_label() == 'Marshward_ArrivalMarker'
    dressing = json.loads(Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())
    berm_hits = 0
    for label,kind,pos,yaw,scale in dressing['placements']:
        if kind not in ('PeatBerm','PeatShelf'):
            continue
        hit = trace((pos[0],pos[1],pos[2]+500),(pos[0],pos[1],pos[2]-10))
        assert hit and hit.to_dict()['hit_actor'].get_actor_label()=='Marshward_'+label
        berm_hits += 1
    assert berm_hits==6
    metrics = json.loads(Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh/rock_metrics.json').read_text())['SM_MarshwardRockCluster_01']
    rock_hits = ground_hits = 0
    actors = {a.get_actor_label():a for a in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()}
    for label,kind,pos,yaw,scale in dressing['placements']:
        if kind != 'RockCluster':
            continue
        actor = actors['Marshward_'+label]
        angle = math.radians(yaw)
        for local in metrics['stone_centers_cm']:
            # Classic FBX mirrors authored local Y; route rays above ignore nothing.
            lx,ly = local[0]*scale[0],-local[1]*scale[1]
            x = pos[0]+lx*math.cos(angle)-ly*math.sin(angle)
            y = pos[1]+lx*math.sin(angle)+ly*math.cos(angle)
            hit = trace((x,y,pos[2]+500),(x,y,pos[2]-10))
            assert hit and hit.to_dict()['hit_actor']==actor
            rock_hits += 1
            support = trace((x,y,pos[2]+10),(x,y,pos[2]-30),[actor])
            assert support and support.to_dict()['hit_actor'].get_actor_label()=='Marshward_BasinCatch'
            assert abs(support.to_dict()['impact_point'].z-pos[2])<.5
            ground_hits += 1
    assert rock_hits==ground_hits==12
    edge = actors['Marshward_BankEdgeEast']
    component = edge.get_component_by_class(unreal.StaticMeshComponent)
    assert component.get_collision_enabled()==unreal.CollisionEnabled.NO_COLLISION
    samples = json.loads(Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh/bank_edge_metrics.json').read_text())['SM_MarshwardBankEdge_01']
    pos,angle = edge.get_actor_location(),math.radians(edge.get_actor_rotation().yaw)
    edge_ground = edge_join = 0
    for field,expected in [('ground_samples_cm','Marshward_BasinCatch'),('join_samples_cm','Marshward_BermEast')]:
        for lx,ly,_ in samples[field]:
            # Classic FBX local-Y mirroring, with no ignored actors in either control.
            x = pos.x+lx*math.cos(angle)+ly*math.sin(angle)
            y = pos.y+lx*math.sin(angle)-ly*math.cos(angle)
            hit = trace((x,y,pos.z+400),(x,y,pos.z-20))
            assert hit and hit.to_dict()['hit_actor'].get_actor_label()==expected, (field,x,y)
            if field=='ground_samples_cm':
                assert abs(hit.to_dict()['impact_point'].z-pos.z)<.5
                edge_ground += 1
            else:
                edge_join += 1
    assert edge_ground==9 and edge_join==2
    runpy.run_path(str(Path(__file__).with_name('validate_marshward_arrival_vista_unreal.py')),run_name='__main__')
    unreal.log('EMBERMERE_MARSHWARD_BANK_EDGE_TRACES_SUCCESS: 9 unfiltered basin supports, 2 existing-shelf join controls, decorative apron has no collision')
    unreal.log('EMBERMERE_MARSHWARD_ROCK_TRACES_SUCCESS: 12 stone controls, 12 underlying basin supports; only support diagnostics ignore own cluster')
    unreal.log('EMBERMERE_MARSHWARD_TERRAIN_TRACES_SUCCESS: 6 solid terrain support controls including 2 shelves')
    unreal.log('EMBERMERE_MARSHWARD_TRACES_SUCCESS: {} support rays, {} clearance rays, positive marker control'.format(supports,clearance))


if __name__ == '__main__':
    main()
