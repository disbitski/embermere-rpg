"""Persisted package contract; run in a fresh process independently of build."""

import json
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'
DEST = '/Game/Art/Embermere/Environment/MarshwardGreybox'


def close_vector(actual, expected):
    assert all(abs(a-b)<0.1 for a,b in zip((actual.x,actual.y,actual.z),expected)), str(actual)


def main():
    specs = [('Span',(600,500,60),12,'box_elems'), ('Ramp',(600,500,160),12,'convex_elems'),
             ('Waymarker',(180,140,520),36,'box_elems')]
    subsystem = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
    for kind, size, count, shape in specs:
        mesh = unreal.load_asset(DEST + '/SM_Marshward' + kind + '_Greybox')
        assert mesh and mesh.get_num_triangles(0) == count
        close_vector(mesh.get_bounds().box_extent*2, size)
        assert subsystem.get_num_uv_channels(mesh,0) == 1
        assert mesh.get_editor_property('asset_import_data').get_class().get_name() == 'FbxStaticMeshImportData'
        mats = mesh.get_editor_property('static_materials')
        assert len(mats) == 1
        suffix = 'Marker' if kind == 'Waymarker' else 'Stone'
        assert mats[0].material_interface.get_path_name() == DEST + '/M_MarshwardGreybox' + suffix + '.M_MarshwardGreybox' + suffix
        agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
        for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
            assert len(agg.get_editor_property(field)) == (1 if field == shape else 0)
    assert unreal.EditorLoadingAndSavingUtils.load_map(MAP)
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith(MAP + '.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    by_label = {a.get_actor_label():a for a in actors}
    layout = json.loads((ROOT / 'ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())
    expected = {}
    for label, pos, dimensions, mat in layout['blocks']:
        expected[label] = (pos, tuple(x/100 for x in dimensions), 0, '/Engine/BasicShapes/Cube.Cube')
    for label, kind, pos, yaw in layout['kit']:
        path = DEST + '/SM_Marshward' + kind + '_Greybox'
        expected[label] = (pos, (1,1,1), yaw, path + '.' + path.rsplit('/',1)[1])
    meshes = [a for a in actors if isinstance(a, unreal.StaticMeshActor)]
    assert len(expected) == 22 and len(meshes) == 36
    for label,(pos,scale,yaw,path) in expected.items():
        actor = by_label['Marshward_' + label]
        assert 'EmbermereMarshwardGreybox' in [str(t) for t in actor.tags]
        assert str(actor.get_folder_path()) == 'MarshwardGreybox/Geometry'
        close_vector(actor.get_actor_location(),pos)
        close_vector(actor.get_actor_scale3d(),scale)
        rot = actor.get_actor_rotation()
        assert abs(rot.pitch)<0.1 and abs(rot.roll)<0.1
        assert abs((rot.yaw-yaw+180)%360-180)<0.1
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.static_mesh.get_path_name() == path
        assert str(component.get_collision_profile_name()) == 'BlockAll'
        assert component.get_collision_enabled() == unreal.CollisionEnabled.QUERY_AND_PHYSICS
    starts = [a for a in actors if isinstance(a,unreal.PlayerStart)]
    assert len(starts)==1
    close_vector(starts[0].get_actor_location(),(0,0,100))
    assert abs(starts[0].get_actor_rotation().yaw-90)<0.1
    forbidden = ['Quest','Enemy','Vendor','Trainer','RestService','Save','Trigger']
    assert not any(any(word in a.get_class().get_name() for word in forbidden) for a in actors)
    runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))['validate_dressing'](actors)
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    unreal.log('EMBERMERE_MARSHWARD_PACKAGE_SUCCESS: 5 meshes, 22 original plus 14 terrain/art placements, standalone map, no quest/service/save authority')


if __name__ == '__main__':
    main()
