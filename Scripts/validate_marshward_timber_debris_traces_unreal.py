"""Three unfiltered original-basin supports beneath visual-only debris."""

import json
from pathlib import Path
import unreal


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    def unique(label):
        found = [a for a in actors if a.get_actor_label()==label]
        assert len(found)==1,label
        return found[0]
    debris,basin = unique('Marshward_TimberDebrisInner'),unique('Marshward_BasinCatch')
    component = debris.get_component_by_class(unreal.StaticMeshComponent)
    assert str(component.get_collision_profile_name())=='NoCollision'
    assert component.get_collision_enabled()==unreal.CollisionEnabled.NO_COLLISION
    source = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/timber_debris_metrics.json'
    spec = json.loads(source.read_text())['SM_MarshwardTimberDebris_01']
    samples = spec['base_samples_cm']
    assert len(samples)==3 and len({tuple(p) for p in samples})==3
    for lx,ly,z in samples:
        assert z==0
        # Classic FBX mirrors authored local Y at this yaw-zero placement.
        x,y = 1200+lx,3300-ly
        hit = unreal.SystemLibrary.line_trace_single(world,unreal.Vector(x,y,100),unreal.Vector(x,y,-140),
            unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,True)
        assert hit and hit.to_dict()['hit_actor']==basin
        assert abs(hit.to_dict()['impact_point'].z+120)<.5
    unreal.log('EMBERMERE_MARSHWARD_TIMBER_DEBRIS_TRACES_SUCCESS: 3 unfiltered basin supports, 1 NoCollision scatter, no new support authority')


if __name__ == '__main__':
    main()
