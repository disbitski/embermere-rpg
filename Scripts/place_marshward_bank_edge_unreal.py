"""Add only the decorative apron in an initialized clean Causeway world."""

import json
import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith('/Game/Maps/L_MarshwardCauseway_Greybox.')
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    rows = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())['placements']
    additions = [row for row in rows if row[1]=='BankEdge']
    assert len(additions)==1
    label,kind,pos,yaw,scale = additions[0]
    assert not any(a.get_actor_label()=='Marshward_'+label for a in subsystem.get_all_level_actors())
    mesh = unreal.load_asset('/Game/Art/Embermere/Environment/MarshwardMarsh/SM_MarshwardBankEdge_01')
    assert mesh
    rotation = unreal.Rotator()
    rotation.yaw = yaw
    actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*pos),rotation)
    assert actor
    try:
        actor.set_actor_label('Marshward_'+label)
        actor.set_actor_scale3d(unreal.Vector(*scale))
        actor.set_folder_path('MarshwardGreybox/MarshTerrain')
        actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(mesh)
        component.set_collision_profile_name('NoCollision')
        runpy.run_path(str(ROOT/'Scripts/validate_marshward_marsh_unreal.py'))['validate_dressing'](list(subsystem.get_all_level_actors()))
        assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    except Exception:
        subsystem.destroy_actor(actor)
        raise
    unreal.log('EMBERMERE_MARSHWARD_BANK_EDGE_PLACEMENT_SUCCESS: 1 no-collision apron, prior 40 placements and walking loop retained')


if __name__ == '__main__':
    main()
