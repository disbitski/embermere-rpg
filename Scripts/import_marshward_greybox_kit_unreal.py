"""Import the Blender-authored kit in a fresh GUI-down Unreal process."""

from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
DEST = '/Game/Art/Embermere/Environment/MarshwardGreybox'
SPECS = {
    'SM_MarshwardSpan_Greybox': ((600,500,60),12,'box_elems'),
    'SM_MarshwardRamp_Greybox': ((600,500,160),12,'convex_elems'),
    'SM_MarshwardWaymarker_Greybox': ((180,140,520),36,'box_elems'),
}


def main():
    for name in SPECS:
        assert not unreal.EditorAssetLibrary.does_asset_exist(DEST + '/' + name), 'Refusing overwrite: ' + name
    materials = {}
    for suffix, color in [('Stone',(0.36,0.43,0.46)), ('Marker',(0.67,0.40,0.14)),
                          ('Ground',(0.23,0.31,0.22)), ('Basin',(0.10,0.22,0.24))]:
        name = 'M_MarshwardGreybox' + suffix
        assert not unreal.EditorAssetLibrary.does_asset_exist(DEST + '/' + name)
        mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, DEST, unreal.Material, unreal.MaterialFactoryNew())
        node = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector)
        node.set_editor_property('constant', unreal.LinearColor(*color, 1))
        unreal.MaterialEditingLibrary.connect_material_property(node, '', unreal.MaterialProperty.MP_BASE_COLOR)
        unreal.MaterialEditingLibrary.recompile_material(mat)
        assert unreal.EditorAssetLibrary.save_loaded_asset(mat, False)
        materials[suffix] = mat
    for name, (size, triangles, shape) in SPECS.items():
        task = unreal.AssetImportTask()
        task.filename = str(ROOT / 'ArtSource/Blender/Environment/MarshwardGreybox' / (name + '.fbx'))
        task.destination_path = DEST
        task.destination_name = name
        task.automated = True
        task.save = False
        ui = unreal.FbxImportUI()
        ui.automated_import_should_detect_type = False
        ui.import_mesh = True
        ui.import_as_skeletal = False
        ui.import_materials = False
        ui.import_textures = False
        ui.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
        ui.original_import_type = unreal.FBXImportType.FBXIT_STATIC_MESH
        data = ui.static_mesh_import_data
        data.combine_meshes = True
        data.auto_generate_collision = False
        data.generate_lightmap_u_vs = False
        data.one_convex_hull_per_ucx = True
        task.factory = unreal.FbxFactory()
        task.options = ui
        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
        mesh = unreal.load_asset(DEST + '/' + name)
        assert mesh and mesh.get_num_triangles(0) == triangles
        mesh.set_material(0, materials['Marker' if 'Waymarker' in name else 'Stone'])
        actual = mesh.get_bounds().box_extent * 2
        assert all(abs(v-e)<0.1 for v,e in zip((actual.x,actual.y,actual.z),size)), str(actual)
        agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
        for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
            assert len(agg.get_editor_property(field)) == (1 if field == shape else 0), name + '/' + field
        assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0) == 1
        assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)
    unreal.log('EMBERMERE_MARSHWARD_KIT_IMPORT_SUCCESS: 3 classic-FBX meshes, 2 boxes, 1 convex, 1 UV each')


if __name__ == '__main__':
    main()
