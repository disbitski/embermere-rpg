"""Import the new shore with an independent copy of the basin shader graph."""

from pathlib import Path
import runpy
import unreal

ROOT = Path(__file__).resolve().parents[1]


def main():
    contract = runpy.run_path(str(ROOT/'Scripts/validate_marshward_pool_shore_unreal.py'))
    dest,name,mat_name = (contract[key] for key in ('DEST','NAME','MATERIAL'))
    for asset in (name,mat_name):
        assert not unreal.EditorAssetLibrary.does_asset_exist(dest+'/'+asset), 'Refusing overwrite'
    basin = unreal.load_asset(dest+'/M_MarshwardWetPeat')
    assert isinstance(basin,unreal.Material)
    material = unreal.AssetToolsHelpers.get_asset_tools().duplicate_asset(mat_name,dest,basin)
    assert material and material != basin

    task = unreal.AssetImportTask()
    task.filename = str(ROOT/'ArtSource/Blender/Environment/MarshwardMarsh'/(name+'.fbx'))
    task.destination_path,task.destination_name = dest,name
    task.automated,task.save = True,False
    ui = unreal.FbxImportUI()
    ui.automated_import_should_detect_type = False
    ui.import_mesh,ui.import_as_skeletal = True,False
    ui.import_materials,ui.import_textures = False,False
    ui.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    ui.original_import_type = unreal.FBXImportType.FBXIT_STATIC_MESH
    options = ui.static_mesh_import_data
    options.combine_meshes = True
    options.auto_generate_collision = False
    options.generate_lightmap_u_vs = False
    task.factory,task.options = unreal.FbxFactory(),ui
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.load_asset(dest+'/'+name)
    assert mesh and len(mesh.get_editor_property('static_materials'))==1
    mesh.set_material(0,material)
    contract['validate_mesh'](mesh,material)
    unreal.MaterialEditingLibrary.recompile_material(material)
    assert unreal.EditorAssetLibrary.save_loaded_asset(material,False)
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)
    unreal.log('EMBERMERE_MARSHWARD_POOL_SHORE_IMPORT_SUCCESS: 92 triangles, one UV, exact bounds, no simple shapes, independent mud shader')


if __name__=='__main__':
    main()
