"""Create the standalone map in an initialized editor; never edit Fenwatch."""

import json
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
DEST = '/Game/Art/Embermere/Environment/MarshwardGreybox'
TAG = 'EmbermereMarshwardGreybox'


def rotation(yaw=0, pitch=0):
    value = unreal.Rotator()
    value.yaw = yaw
    value.pitch = pitch
    return value


def main():
    layout = json.loads((ROOT / 'ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages(), 'Unsaved map; refusing switch'
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages(), 'Unsaved content; refusing switch'
    assert not unreal.EditorAssetLibrary.does_asset_exist(layout['map']), 'Refusing to rebuild existing map'
    for name in ['Span','Ramp','Waymarker']:
        assert unreal.load_asset(DEST + '/SM_Marshward' + name + '_Greybox')
    assert unreal.EditorLevelLibrary.new_level(layout['map'])
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    def spawn(cls, label, location, yaw=0, folder='Geometry'):
        actor = actors.spawn_actor_from_class(cls, unreal.Vector(*location), rotation(yaw))
        assert actor
        actor.set_actor_label('Marshward_' + label)
        actor.set_folder_path('MarshwardGreybox/' + folder)
        actor.tags = [unreal.Name(TAG)]
        return actor

    def mesh_actor(label, mesh_path, location, scale=(1,1,1), yaw=0, mat=None):
        actor = spawn(unreal.StaticMeshActor, label, location, yaw)
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(unreal.load_asset(mesh_path))
        actor.set_actor_scale3d(unreal.Vector(*scale))
        component.set_collision_profile_name('BlockAll')
        if mat:
            component.set_material(0, unreal.load_asset(DEST + '/M_MarshwardGreybox' + mat))
        return actor

    for label, location, dimensions, mat in layout['blocks']:
        mesh_actor(label, '/Engine/BasicShapes/Cube', location, tuple(x/100 for x in dimensions), mat=mat)
    for label, kind, location, yaw in layout['kit']:
        mesh_actor(label, DEST + '/SM_Marshward' + kind + '_Greybox', location, yaw=yaw)
    spawn(unreal.PlayerStart, 'PlayerStart', (0,0,100), 90, 'World')
    sun = spawn(unreal.DirectionalLight, 'Sun', (0,0,2500), -35, 'World')
    sun.set_actor_rotation(rotation(-35,-40), False)
    component = sun.get_component_by_class(unreal.DirectionalLightComponent)
    component.set_editor_property('intensity', 7.0)
    component.set_editor_property('atmosphere_sun_light', True)
    component.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
    sky = spawn(unreal.SkyLight, 'SkyLight', (0,0,1000), folder='World')
    sky.get_component_by_class(unreal.SkyLightComponent).set_editor_property('real_time_capture', True)
    spawn(unreal.SkyAtmosphere, 'Atmosphere', (0,0,0), folder='World')
    editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    editor.set_level_viewport_camera_info(unreal.Vector(-5200,-4800,5000), rotation(53,-25))
    assert unreal.EditorLevelLibrary.save_current_level()
    unreal.log('EMBERMERE_MARSHWARD_BUILD_SUCCESS: standalone greybox, 22 static meshes, normal PlayerStart, no gameplay actors')


if __name__ == '__main__':
    main()
