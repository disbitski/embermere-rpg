"""Fresh-process classic-FBX import; no placement or map changes here."""

import json
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
SOURCE = ROOT/'ArtSource/Blender/Environment/MarshwardMarsh'
DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
COLORS = {
    'M_MarshwardPeat': (.13,.16,.11), 'M_MarshwardMoss': (.23,.34,.12),
    'M_MarshwardEarth': (.28,.22,.13), 'M_MarshwardReedGreen': (.30,.43,.16),
    'M_MarshwardReedSeed': (.26,.13,.07),
}


def main():
    metrics = json.loads((SOURCE/'mesh_metrics.json').read_text())
    for name in [*COLORS,*metrics]:
        assert not unreal.EditorAssetLibrary.does_asset_exist(DEST+'/'+name), 'Refusing overwrite: '+name
    mats = {}
    for name,color in COLORS.items():
        mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name,DEST,unreal.Material,unreal.MaterialFactoryNew())
        node = unreal.MaterialEditingLibrary.create_material_expression(mat,unreal.MaterialExpressionConstant3Vector)
        node.set_editor_property('constant',unreal.LinearColor(*color,1))
        unreal.MaterialEditingLibrary.connect_material_property(node,'',unreal.MaterialProperty.MP_BASE_COLOR)
        roughness = unreal.MaterialEditingLibrary.create_material_expression(mat,unreal.MaterialExpressionConstant)
        roughness.set_editor_property('r',.9)
        unreal.MaterialEditingLibrary.connect_material_property(roughness,'',unreal.MaterialProperty.MP_ROUGHNESS)
        unreal.MaterialEditingLibrary.recompile_material(mat)
        assert unreal.EditorAssetLibrary.save_loaded_asset(mat,False)
        mats[name] = mat
    for name,expected in metrics.items():
        task = unreal.AssetImportTask()
        task.filename = str(SOURCE/(name+'.fbx'))
        task.destination_path,task.destination_name = DEST,name
        task.automated = True
        task.save = False
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
        mesh = unreal.load_asset(DEST+'/'+name)
        assert mesh and mesh.get_num_triangles(0) == expected['triangles']
        assignments = []
        for i,slot in enumerate(mesh.get_editor_property('static_materials')):
            slot_name = str(slot.material_slot_name)
            assert slot_name in expected['materials']
            mesh.set_material(i,mats[slot_name])
            assignments.append(slot_name)
        assert set(assignments) == set(expected['materials'])
        actual = mesh.get_bounds().box_extent*2
        assert all(abs(a-b)<.1 for a,b in zip((actual.x,actual.y,actual.z),expected['dimensions_cm']))
        assert unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem).get_num_uv_channels(mesh,0) == 1
        agg = mesh.get_editor_property('body_setup').get_editor_property('agg_geom')
        for field in ['box_elems','convex_elems','sphere_elems','sphyl_elems']:
            assert len(agg.get_editor_property(field)) == (1 if 'PeatBerm' in name and field=='convex_elems' else 0)
        assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)
    unreal.log('EMBERMERE_MARSHWARD_MARSH_IMPORT_SUCCESS: 2 original meshes, 96/616 triangles, 1 convex berm, no reed collision')


if __name__ == '__main__':
    main()
