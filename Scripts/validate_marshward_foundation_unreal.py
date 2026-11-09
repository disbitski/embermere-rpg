"""Independent read-only persisted foundation mesh gate; no reimport or save."""

import unreal

DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
NAME = 'SM_MarshwardFoundation_01'


def main():
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    mesh = unreal.load_asset(DEST+'/'+NAME)
    assert mesh and mesh.get_num_triangles(0)==180
    size = mesh.get_bounds().box_extent*2
    assert all(abs(a-b)<.1 for a,b in zip((size.x,size.y,size.z),(600,260,150)))
    assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0)==1
    assert mesh.get_editor_property('asset_import_data').get_class().get_name()=='FbxStaticMeshImportData'
    slots = list(mesh.get_editor_property('static_materials'))
    assert len(slots)==2
    assert {str(slot.material_slot_name) for slot in slots}=={'M_MarshwardMarshStone','M_MarshwardMoss'}
    for slot in slots:
        material = str(slot.material_slot_name)
        assert slot.material_interface.get_path_name()==DEST+'/'+material+'.'+material
    agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
    for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
        assert len(agg.get_editor_property(field))==(3 if field=='convex_elems' else 0)
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    unreal.log('EMBERMERE_MARSHWARD_FOUNDATION_PACKAGE_SUCCESS: 600x260x150 cm, 180 triangles, 1 UV, 3 convex hulls, 2 reused materials, no dirty packages or saves')


if __name__ == '__main__':
    main()
