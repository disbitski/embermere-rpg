"""Compare actual saved carved-marker geometry and collision with the retained greybox."""

import json
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
DEST = '/Game/Art/Embermere/Environment/MarshwardWaymarker'
NEW = DEST+'/SM_MarshwardWaymarker_01'
OLD = '/Game/Art/Embermere/Environment/MarshwardGreybox/SM_MarshwardWaymarker_Greybox'
MATERIAL = '/Game/Art/Embermere/Environment/MarshwardGreybox/M_MarshwardGreyboxMarker.M_MarshwardGreyboxMarker'


def validate_mesh(mesh=None):
    mesh = mesh or unreal.load_asset(NEW)
    old = unreal.load_asset(OLD)
    assert mesh and old and mesh.get_num_triangles(0)==352 and old.get_num_triangles(0)==36
    require = runpy.run_path(str(ROOT/'Scripts/marshward_waymarker_contract.py'))['require_geometry']
    boxes = []
    for asset in (old,mesh):
        bounds = asset.get_bounds()
        origin,extent = bounds.origin,bounds.box_extent
        body = asset.get_editor_property('body_setup')
        aggregate = body.get_editor_property('agg_geom')
        assert len(aggregate.get_editor_property('box_elems'))==1
        for field in ('convex_elems','sphere_elems','sphyl_elems'):
            assert len(aggregate.get_editor_property(field))==0
        box = aggregate.get_editor_property('box_elems')[0]
        # KBoxElem exposes editor-only fields through reflection, not attributes.
        center,rotation = box.get_editor_property('center'),box.get_editor_property('rotation')
        size = [box.get_editor_property(field) for field in ('x','y','z')]
        require([origin.x-extent.x,origin.y-extent.y,origin.z-extent.z],
                [origin.x+extent.x,origin.y+extent.y,origin.z+extent.z],size,[center.x,center.y,center.z])
        boxes.append((rotation.pitch,rotation.yaw,rotation.roll))
        assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(asset,0)==1
        assert asset.get_editor_property('asset_import_data').get_class().get_name()=='FbxStaticMeshImportData'
        slots = asset.get_editor_property('static_materials')
        assert len(slots)==1 and str(slots[0].material_slot_name)=='M_MarshwardGreyboxMarker'
        assert slots[0].material_interface.get_path_name()==MATERIAL
    assert all(abs((a-b+180)%360-180)<.1 for a,b in zip(*boxes)), boxes
    assert mesh.get_editor_property('body_setup').get_editor_property('collision_trace_flag')==old.get_editor_property('body_setup').get_editor_property('collision_trace_flag')
    unreal.log('EMBERMERE_MARSHWARD_WAYMARKER_PARITY_SUCCESS: 180x140x520 bounds, exact 120x100x440 body box at Z220 and baseline rotation, 352 triangles, 1 UV, reused material')


def marker_roles():
    layout = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())
    rows = [row for row in layout['kit'] if row[1]=='Waymarker']
    assert rows==[['ArrivalMarker','Waymarker',[-750,650,0],0],
                  ['RuinMarker','Waymarker',[-950,6400,120],0],
                  ['ReturnMarker','Waymarker',[3000,5300,0],90]]
    return rows


if __name__ == '__main__':
    validate_mesh()
