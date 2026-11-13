"""Fresh classic-FBX carved marker import, retaining original packages untouched."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
DEST = '/Game/Art/Embermere/Environment/MarshwardWaymarker'
NAME = 'SM_MarshwardWaymarker_01'


def main():
    assert not unreal.EditorAssetLibrary.does_asset_exist(DEST+'/'+NAME), 'Refusing overwrite'
    material = unreal.load_asset('/Game/Art/Embermere/Environment/MarshwardGreybox/M_MarshwardGreyboxMarker')
    assert material
    task = unreal.AssetImportTask()
    task.filename = str(ROOT/'ArtSource/Blender/Environment/MarshwardWaymarker'/(NAME+'.fbx'))
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
    assert mesh
    slots = mesh.get_editor_property('static_materials')
    assert len(slots)==1 and str(slots[0].material_slot_name)=='M_MarshwardGreyboxMarker'
    mesh.set_material(0,material)
    runpy.run_path(str(ROOT/'Scripts/validate_marshward_waymarker_unreal.py'))['validate_mesh'](mesh)
    assert unreal.EditorAssetLibrary.save_loaded_asset(mesh,False)
    unreal.log('EMBERMERE_MARSHWARD_WAYMARKER_IMPORT_SUCCESS: fresh carved marker, exact saved-greybox bounds and box parity, one reused material')


if __name__ == '__main__':
    main()
