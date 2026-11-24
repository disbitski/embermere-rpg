"""Validate the persisted mud strip and its original basin support."""

import json
from pathlib import Path
import runpy
import unreal

ROOT = Path(__file__).resolve().parents[1]
DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'
NAME = 'SM_MarshwardPoolShore_01'
MATERIAL = 'M_MarshwardShoreMud'
LABEL = 'Marshward_PoolShoreWest'
POS = (-1320,3340,-118)


def spec():
    path = ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/pool_shore_metrics.json'
    return json.loads(path.read_text())[NAME]


def validate_mesh(mesh, material):
    assert mesh and mesh.get_num_triangles(0) == 92
    assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0) == 1
    assert mesh.get_editor_property('asset_import_data').get_class().get_name() == 'FbxStaticMeshImportData'
    assert not mesh.get_editor_property('nanite_settings').get_editor_property('enabled')
    body = mesh.get_editor_property('body_setup')
    assert body.get_editor_property('collision_trace_flag') != unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE
    agg = body.get_editor_property('agg_geom')
    for field in ('box_elems','convex_elems','sphere_elems','sphyl_elems','tapered_capsule_elems',
                  'level_set_elems','skinned_level_set_elems','ml_level_set_elems','skinned_triangle_mesh_elems'):
        assert not agg.get_editor_property(field), field
    origin,extent = mesh.get_bounds().origin,mesh.get_bounds().box_extent
    axes = spec()['bounds_cm']
    # The classic FBX transform mirrors local Y for yaw-zero placements.
    want = [axes[0], [-axes[1][1],-axes[1][0]], axes[2]]
    for mid,half,(lo,hi) in zip((origin.x,origin.y,origin.z),(extent.x,extent.y,extent.z),want):
        assert abs(mid-half-lo)<.1 and abs(mid+half-hi)<.1, (mid,half,lo,hi)
    slots = list(mesh.get_editor_property('static_materials'))
    assert len(slots)==1 and str(slots[0].material_slot_name)==MATERIAL
    assert slots[0].material_interface==material
    assert material.get_path_name()==DEST+'/'+MATERIAL+'.'+MATERIAL
    runpy.run_path(str(ROOT/'Scripts/validate_marshward_wet_peat_unreal.py'))['validate_graph'](material)


def validate_placement(actors):
    def unique(label):
        found, = [actor for actor in actors if actor.get_actor_label()==label]
        return found

    shore = unique(LABEL)
    pool = unique('Marshward_StillPoolWest')
    component = shore.get_component_by_class(unreal.StaticMeshComponent)
    assert component.static_mesh.get_path_name()==DEST+'/'+NAME+'.'+NAME
    material = unreal.load_asset(DEST+'/'+MATERIAL)
    validate_mesh(component.static_mesh, material)
    assert list(component.get_materials())==[material]
    owners = [a for a in actors if isinstance(a,unreal.StaticMeshActor) and
              material in a.get_component_by_class(unreal.StaticMeshComponent).get_materials()]
    assert owners==[shore]
    assert str(component.get_collision_profile_name())=='NoCollision'
    assert component.get_collision_enabled()==unreal.CollisionEnabled.NO_COLLISION
    for name in ('cast_shadow','generate_overlap_events','can_ever_affect_navigation'):
        assert not component.get_editor_property(name), name
    assert not component.is_simulating_physics()
    center,extent = shore.get_actor_bounds(False)
    pc,pe = pool.get_actor_bounds(False)
    assert abs(pc.x-center.x)<pe.x+extent.x and abs(pc.y-center.y)<pe.y+extent.y
    assert center.z+extent.z < -118.3 and center.z-extent.z < -120.5


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith(MAP+'.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    validate_placement(actors)
    basin, = [actor for actor in actors if actor.get_actor_label()=='Marshward_BasinCatch']
    top = spec()['top_rows_cm']
    for i in range(1,7):
        inside,crest,toe = top[3*i:3*i+3]
        for lx,ly,_ in (crest,tuple((a+b)/2 for a,b in zip(crest,toe))):
            x,y = POS[0]+lx,POS[1]-ly
            hit = unreal.SystemLibrary.line_trace_single(
                world,unreal.Vector(x,y,200),unreal.Vector(x,y,-140),
                unreal.TraceTypeQuery.ECC_VISIBILITY,False,[],unreal.DrawDebugTrace.NONE,True)
            assert hit and hit.to_dict()['hit_actor']==basin, (x,y)
            assert abs(hit.to_dict()['impact_point'].z+120)<.5
    unreal.log('EMBERMERE_MARSHWARD_POOL_SHORE_TRACES_SUCCESS: 12 unfiltered basin hits across the visual crest and toe')


if __name__=='__main__':
    main()
