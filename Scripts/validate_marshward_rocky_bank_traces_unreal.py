"""Read-only ridge and adjacent-ground controls in the settled saved world."""

import json
from pathlib import Path
import unreal


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    def unique(label):
        found = [a for a in actors if a.get_actor_label()==label]
        assert len(found)==1, label
        return found[0]
    ridge,basin = unique('Marshward_RockyBankWest'),unique('Marshward_BasinCatch')
    component = ridge.get_component_by_class(unreal.StaticMeshComponent)
    assert str(component.get_collision_profile_name())=='BlockAll'
    spec = json.loads((Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/rocky_bank_metrics.json').read_text())['SM_MarshwardRockyBank_01']
    def trace(x,y,top,bottom):
        return unreal.SystemLibrary.line_trace_single(world,unreal.Vector(x,y,top),unreal.Vector(x,y,bottom),
            unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,True)
    solid = ground = 0
    # At yaw90, classic-FBX local-Y mirroring maps (lx,ly) to (+ly,+lx).
    for lx,ly,height in spec['crest_samples_cm']:
        hit = trace(-3850+ly,3300+lx,800,-140)
        assert hit and hit.to_dict()['hit_actor']==ridge
        assert abs(hit.to_dict()['impact_point'].z-(-120+height))<.5
        solid += 1
    for lx,ly,_ in spec['ground_samples_cm']:
        hit = trace(-3850+ly,3300+lx,800,-140)
        assert hit and hit.to_dict()['hit_actor']==basin
        assert abs(hit.to_dict()['impact_point'].z+120)<.5
        ground += 1
    assert solid==3 and ground==6
    unreal.log('EMBERMERE_MARSHWARD_ROCKY_BANK_TRACES_SUCCESS: 3 solid crest controls, 6 adjacent basin supports at Z-120, no ignored actors')


if __name__ == '__main__':
    main()
