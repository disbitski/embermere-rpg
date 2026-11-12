"""New terrain mesh, exact placement and visible-corridor package contract."""

import json
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
SOURCE = ROOT/'ArtSource/Blender/Environment/MarshwardMarsh'
DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'


def validate_dressing(actors):
    metrics = json.loads((SOURCE/'mesh_metrics.json').read_text())
    metrics.update(json.loads((SOURCE/'shelf_metrics.json').read_text()))
    metrics.update(json.loads((SOURCE/'rock_metrics.json').read_text()))
    metrics.update(json.loads((SOURCE/'bank_edge_metrics.json').read_text()))
    metrics.update(json.loads((SOURCE/'foundation_metrics.json').read_text()))
    metrics.update(json.loads((SOURCE/'fallen_masonry_metrics.json').read_text()))
    metrics.update(json.loads((SOURCE/'tall_reeds_metrics.json').read_text()))
    metrics.update(json.loads((SOURCE/'low_scrub_metrics.json').read_text()))
    hull_counts = {'SM_MarshwardPeatBerm_01':1,'SM_MarshwardPeatShelf_01':1,
                   'SM_MarshwardReedClump_01':0,'SM_MarshwardRockCluster_01':3,
                   'SM_MarshwardBankEdge_01':0,'SM_MarshwardFoundation_01':3,
                   'SM_MarshwardFallenMasonry_01':2,'SM_MarshwardTallReeds_01':0,
                   'SM_MarshwardLowScrub_01':0}
    assert set(metrics)==set(hull_counts)
    for name,spec in metrics.items():
        mesh = unreal.load_asset(DEST+'/'+name)
        assert mesh and mesh.get_num_triangles(0) == spec['triangles']
        assert spec['non_manifold_edges']==0 and spec['uv_channels']==1
        size = mesh.get_bounds().box_extent*2
        assert all(abs(a-b)<.1 for a,b in zip((size.x,size.y,size.z),spec['dimensions_cm']))
        assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0)==1
        assert mesh.get_editor_property('asset_import_data').get_class().get_name()=='FbxStaticMeshImportData'
        slots = list(mesh.get_editor_property('static_materials'))
        assert len(slots)==len(spec['materials'])
        for slot in slots:
            material = str(slot.material_slot_name)
            assert material in spec['materials']
            assert slot.material_interface.get_path_name()==DEST+'/'+material+'.'+material
        agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
        for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
            assert len(agg.get_editor_property(field))==(hull_counts[name] if field=='convex_elems' else 0)
    data = json.loads((SOURCE/'placements.json').read_text())
    route = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())['route']
    assert len(data['corridor_half_widths_cm'])==len(route)-1
    assert data['corridor_half_widths_cm']==[600,250,250,400,400,250,300,600,600]
    assert data['buffer_cm']==75
    overlaps = runpy.run_path(str(ROOT/'Scripts/marshward_route_clearance.py'))['overlaps_corridor']
    owned = {a.get_actor_label():a for a in actors if 'EmbermereMarshwardMarsh' in [str(t) for t in a.tags]}
    assert len(owned)==len(data['placements'])==23
    for label,kind,pos,yaw,scale in data['placements']:
        actor = owned['Marshward_'+label]
        assert isinstance(actor,unreal.StaticMeshActor)
        assert str(actor.get_folder_path())=='MarshwardGreybox/MarshTerrain'
        for actual,expected in [(actor.get_actor_location(),pos),(actor.get_actor_scale3d(),scale)]:
            assert all(abs(a-b)<.1 for a,b in zip((actual.x,actual.y,actual.z),expected))
        rotation = actor.get_actor_rotation()
        assert abs(rotation.pitch)<.1 and abs(rotation.roll)<.1
        assert abs((rotation.yaw-yaw+180)%360-180)<.1
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        name = 'SM_Marshward'+kind+'_01'
        assert component.static_mesh.get_path_name()==DEST+'/'+name+'.'+name
        solid = kind in ('PeatBerm','PeatShelf','RockCluster','Foundation','FallenMasonry')
        expected_profile = 'BlockAll' if solid else 'NoCollision'
        assert str(component.get_collision_profile_name())==expected_profile
        expected_enabled = unreal.CollisionEnabled.QUERY_AND_PHYSICS if solid else unreal.CollisionEnabled.NO_COLLISION
        assert component.get_collision_enabled()==expected_enabled
        if kind in ('Foundation','FallenMasonry','TallReeds','LowScrub'):
            assert list(component.get_materials())==[slot.material_interface for slot in component.static_mesh.get_editor_property('static_materials')]
        center,extent = actor.get_actor_bounds(False)
        for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
            assert not overlaps((center.x,center.y),(extent.x,extent.y),start,end,width,data['buffer_cm']), label+' intrudes on route'
    unreal.log('EMBERMERE_MARSHWARD_MARSH_PACKAGE_SUCCESS: 9 meshes, 4 berms, 2 shelves, 6 original reeds, 2 tall sparse reeds, 4 rock clusters, 1 decorative bank edge, 2 foundation courses, 1 fallen-masonry scatter, 1 low scrub, full-width corridors plus 75 cm clear')


def main():
    assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Maps/L_MarshwardCauseway_Greybox')
    validate_dressing(list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()))


if __name__ == '__main__':
    main()
