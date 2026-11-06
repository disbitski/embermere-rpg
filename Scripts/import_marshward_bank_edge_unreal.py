"""Fresh classic-FBX apron import; no existing asset or material resaves."""

import unreal

DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
NAME = 'SM_MarshwardBankEdge_01'
MATERIALS = {'M_MarshwardPeat','M_MarshwardMoss','M_MarshwardEarth'}


def main():
    assert not unreal.EditorAssetLibrary.does_asset_exist(DEST+'/'+NAME), 'Refusing overwrite'
    materials = {name:unreal.load_asset(DEST+'/'+name) for name in MATERIALS}
    assert all(materials.values())
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
    task.factory,task.options = unreal.FbxFactory(),ui
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.load_asset(DEST+'/'+NAME)
    assert mesh and mesh.get_num_triangles(0)==46
    size = mesh.get_bounds().box_extent*2
    assert all(abs(a-b)<.1 for a,b in zip((size.x,size.y,size.z),(1000,360,70)))
    assigned = set()
    for index,slot in enumerate(mesh.get_editor_property('static_materials')):
        name = str(slot.material_slot_name)
        assert name in MATERIALS
        mesh.set_material(index,materials[name])
        assigned.add(name)
    assert assigned==MATERIALS
    assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0)==1
    agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
    for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
        assert len(agg.get_editor_property(field))==0
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)
    unreal.log('EMBERMERE_MARSHWARD_BANK_EDGE_IMPORT_SUCCESS: 1000x360x70 cm, 46 triangles, 1 UV, no collision, 3 reused materials')


if __name__ == '__main__':
    main()
