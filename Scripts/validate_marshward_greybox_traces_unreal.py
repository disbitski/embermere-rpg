"""Initialized-world support and player-width clearance for the complete loop."""

import json
import math
from pathlib import Path
import unreal


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    layout = json.loads(Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())

    def trace(a,b):
        return unreal.SystemLibrary.line_trace_single(world, unreal.Vector(*a), unreal.Vector(*b),
            unreal.TraceTypeQuery.ECC_VISIBILITY, False, [], unreal.DrawDebugTrace.NONE, True)

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
        if kind != 'PeatBerm':
            continue
        hit = trace((pos[0],pos[1],pos[2]+500),(pos[0],pos[1],pos[2]-10))
        assert hit and hit.to_dict()['hit_actor'].get_actor_label()=='Marshward_'+label
        berm_hits += 1
    assert berm_hits==6
    unreal.log('EMBERMERE_MARSHWARD_TERRAIN_TRACES_SUCCESS: 6 solid berm support controls')
    unreal.log('EMBERMERE_MARSHWARD_TRACES_SUCCESS: {} support rays, {} clearance rays, positive marker control'.format(supports,clearance))


if __name__ == '__main__':
    main()
