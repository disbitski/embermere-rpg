"""Create the opaque study material and import just the new mesh."""

from pathlib import Path
import runpy
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    contract = runpy.run_path(str(ROOT/'Scripts/validate_marshward_still_pool_unreal.py'))
    dest,name,material_name = (contract[key] for key in ('DEST', 'NAME', 'MATERIAL'))
    for asset in (name,material_name):
        assert not unreal.EditorAssetLibrary.does_asset_exist(dest+'/'+asset), 'Refusing overwrite'
    material = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        material_name, dest, unreal.Material, unreal.MaterialFactoryNew())
    lib = unreal.MaterialEditingLibrary

    def node(kind, label, **settings):
        result = lib.create_material_expression(material, getattr(unreal, 'MaterialExpression'+kind))
        result.set_editor_property('desc', label)
        for prop,value in settings.items():
            result.set_editor_property(prop, value)
        return result

    nodes = {'CoreMask': node('VertexColor', 'CoreMask')}
    for label,rgb in contract['COLORS'].items():
        nodes[label] = node('Constant3Vector', label, constant=unreal.LinearColor(*rgb, 1))
    for label,value in contract['SCALARS'].items():
        nodes[label] = node('Constant', label, r=value)
    for label,inputs in contract['LINKS'].items():
        nodes[label] = node('LinearInterpolate', label)
        for source,pin in zip(inputs, ('A', 'B', 'Alpha')):
            assert lib.connect_material_expressions(nodes[source], 'R' if source == 'CoreMask' else '',
                                                    nodes[label], pin)
    for prop,label in contract['OUTPUTS'].items():
        assert lib.connect_material_property(nodes[label], '', getattr(unreal.MaterialProperty, prop))
    lib.layout_material_expressions(material)
    lib.recompile_material(material)
    contract['validate_material'](material)

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
    options.vertex_color_import_option = unreal.VertexColorImportOption.REPLACE
    task.factory,task.options = unreal.FbxFactory(),ui
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.load_asset(dest+'/'+name)
    assert mesh and len(mesh.get_editor_property('static_materials')) == 1
    mesh.set_material(0, material)
    assert contract['validate_mesh']() == material
    assert unreal.EditorAssetLibrary.save_loaded_asset(material, False)
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh, False)
    unreal.log('EMBERMERE_MARSHWARD_STILL_POOL_IMPORT_SUCCESS: 96 triangles, 1 UV, no simple shapes, 1 opaque vertex-color material')


if __name__ == '__main__':
    main()
