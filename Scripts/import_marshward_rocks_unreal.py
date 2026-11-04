"""Fresh classic-FBX import: one original cluster, three authored hulls."""

import unreal

DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
NAME = 'SM_MarshwardRockCluster_01'
STONE = 'M_MarshwardMarshStone'
MOSS = 'M_MarshwardMoss'


def main():
    for name in [NAME,STONE]:
        assert not unreal.EditorAssetLibrary.does_asset_exist(DEST+'/'+name), 'Refusing overwrite: '+name
    moss = unreal.load_asset(DEST+'/'+MOSS)
    assert moss
    stone = unreal.AssetToolsHelpers.get_asset_tools().create_asset(STONE,DEST,unreal.Material,unreal.MaterialFactoryNew())
    color = unreal.MaterialEditingLibrary.create_material_expression(stone,unreal.MaterialExpressionConstant3Vector)
    color.set_editor_property('constant',unreal.LinearColor(.29,.34,.36,1))
    unreal.MaterialEditingLibrary.connect_material_property(color,'',unreal.MaterialProperty.MP_BASE_COLOR)
    roughness = unreal.MaterialEditingLibrary.create_material_expression(stone,unreal.MaterialExpressionConstant)
    roughness.set_editor_property('r',.9)
    unreal.MaterialEditingLibrary.connect_material_property(roughness,'',unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(stone)
    assert unreal.EditorAssetLibrary.save_loaded_asset(stone,False)
    task = unreal.AssetImportTask()
    task.filename = '/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh/'+NAME+'.fbx'
    task.destination_path,task.destination_name = DEST,NAME
    task.automated,task.save = True,False
    ui = unreal.FbxImportUI()
    ui.automated_import_should_detect_type = False
    ui.import_mesh,ui.import_as_skeletal = True,False
    ui.import_materials,ui.import_textures = False,False
    ui.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    ui.original_import_type = unreal.FBXImportType.FBXIT_STATIC_MESH
    ui.static_mesh_import_data.combine_meshes = True
    ui.static_mesh_import_data.auto_generate_collision = False
    ui.static_mesh_import_data.generate_lightmap_u_vs = False
    ui.static_mesh_import_data.one_convex_hull_per_ucx = True
    task.factory,task.options = unreal.FbxFactory(),ui
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.load_asset(DEST+'/'+NAME)
    assert mesh and mesh.get_num_triangles(0)==144
    actual = mesh.get_bounds().box_extent*2
    assert all(abs(a-b)<.1 for a,b in zip((actual.x,actual.y,actual.z),(460,340,260)))
    materials = {STONE:stone,MOSS:moss}
    assigned = []
    for index,slot in enumerate(mesh.get_editor_property('static_materials')):
        name = str(slot.material_slot_name)
        assert name in materials
        mesh.set_material(index,materials[name])
        assigned.append(name)
    assert len(assigned)==2 and set(assigned)==set(materials)
    assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0)==1
    agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
    for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
        assert len(agg.get_editor_property(field))==(3 if field=='convex_elems' else 0)
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)
    unreal.log('EMBERMERE_MARSHWARD_ROCK_IMPORT_SUCCESS: 460x340x260 cm, 144 triangles, 1 UV, 3 convex hulls, 2 materials')


if __name__ == '__main__':
    main()
