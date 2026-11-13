"""Unfiltered body-entry and decorative-clear controls at all three saved marker roles."""

import math
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    contract = runpy.run_path(str(ROOT/'Scripts/validate_marshward_waymarker_unreal.py'))
    actors = {a.get_actor_label():a for a in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()}
    solids = clear = 0
    for label,kind,pos,yaw in contract['marker_roles']():
        actor = actors['Marshward_'+label]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.static_mesh.get_path_name()==contract['NEW']+'.'+contract['NEW'].rsplit('/',1)[1]
        angle = math.radians(yaw)

        def point(local):
            x,y,z = local
            return unreal.Vector(pos[0]+x*math.cos(angle)-y*math.sin(angle),
                                 pos[1]+x*math.sin(angle)+y*math.cos(angle),pos[2]+z)

        def trace(a,b):
            return unreal.SystemLibrary.line_trace_single(world,point(a),point(b),
                unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,True)

        for start,end,entry in [((-150,0,200),(150,0,200),(-60,0,200)),
                                ((0,-150,200),(0,150,200),(0,-50,200))]:
            hit = trace(start,end)
            assert hit and hit.to_dict()['hit_actor']==actor
            actual,expected = hit.to_dict()['impact_point'],point(entry)
            assert math.dist((actual.x,actual.y,actual.z),(expected.x,expected.y,expected.z))<.5
            solids += 1
        for start,end in [((75,-100,480),(75,100,480)),((-100,60,310),(100,60,310))]:
            assert not trace(start,end), label+' decorative geometry gained collision'
            clear += 1
    assert solids==clear==6
    unreal.log('EMBERMERE_MARSHWARD_WAYMARKER_TRACES_SUCCESS: 6 exact body entries, 6 decorative-clear rays, 3 saved markers, no ignored actors')


if __name__ == '__main__':
    main()
