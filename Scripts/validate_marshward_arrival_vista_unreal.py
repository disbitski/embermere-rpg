"""Read-only initialized arrival landmark sightlines; no content mutation."""

import unreal

LANDMARKS = {
    'Marshward_RuinPillarWest': (-500,6800,570),
    'Marshward_RuinPillarEast': (500,6800,470),
    'Marshward_BrokenLintel': (-280,6800,1050),
}


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    actors = {a.get_actor_label():a for a in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()}
    for label,expected in LANDMARKS.items():
        actual = actors[label].get_actor_location()
        assert all(abs(a-b)<.1 for a,b in zip((actual.x,actual.y,actual.z),expected)), label

    def trace(start,end,labels):
        hit = unreal.SystemLibrary.line_trace_single(world,unreal.Vector(*start),unreal.Vector(*end),
            unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,False)
        assert hit and hit.to_dict()['hit_actor'] in [actors[label] for label in labels], (start,end,labels)
        return hit

    vistas = supports = 0
    for x in (0,-500,500):
        origin = (x,-450,260)
        for label,point in LANDMARKS.items():
            trace(origin,point,[label])
            vistas += 1
        # The positive-X pose is above the original coplanar island/return join.
        labels = ['Marshward_ArrivalIsland','Marshward_SouthReturn'] if x==500 else ['Marshward_ArrivalIsland']
        support = trace(origin,(x,-450,-10),labels)
        assert abs(support.to_dict()['impact_point'].z)<.5
        supports += 1
    assert vistas==9 and supports==3
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    unreal.log('EMBERMERE_MARSHWARD_ARRIVAL_VISTA_SUCCESS: 3 arrival origins, 9 positive landmark sightlines, 3 arrival-ground supports, no ignored actors or content mutation')


if __name__ == '__main__':
    main()
