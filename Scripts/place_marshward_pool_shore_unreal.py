"""Stage the new mud strip for review; save only on explicit acceptance."""

import json
from pathlib import Path
import runpy
import unreal

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = ROOT/'Scripts'
CONTRACT = runpy.run_path(str(SCRIPTS/'validate_marshward_pool_shore_unreal.py'))
SIGNATURE = runpy.run_path(str(SCRIPTS/'place_marshward_foundation_unreal.py'))['signature']


def stage():
    editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    assert editor.get_editor_world().get_path_name().startswith(CONTRACT['MAP']+'.')
    assert not editor.get_game_world()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    preceding = {actor:SIGNATURE(actor) for actor in subsystem.get_all_level_actors()}
    assert sum(isinstance(actor,unreal.StaticMeshActor) for actor in preceding)==52
    assert all(SIGNATURE(actor)==before for actor,before in preceding.items())
    source = ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json'
    rows = json.loads(source.read_text())['placements']
    assert [row for row in rows if row[1]=='PoolShore']==[
        ['PoolShoreWest','PoolShore',list(CONTRACT['POS']),0,[1,1,1]]]
    assert all(a.get_actor_label()!=CONTRACT['LABEL'] for a in preceding)
    mesh = unreal.load_asset(CONTRACT['DEST']+'/'+CONTRACT['NAME'])
    assert mesh
    actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor,unreal.Vector(*CONTRACT['POS']),unreal.Rotator())
    assert actor
    try:
        actor.set_actor_label(CONTRACT['LABEL'])
        actor.set_folder_path('MarshwardGreybox/MarshTerrain')
        actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(mesh)
        component.set_collision_profile_name('NoCollision')
        for name in ('cast_shadow','generate_overlap_events','can_ever_affect_navigation'):
            component.set_editor_property(name,False)
        verify(preceding,actor)
    except Exception:
        subsystem.destroy_actor(actor)
        raise
    unreal.log('EMBERMERE_MARSHWARD_POOL_SHORE_STAGED: original 52 static placements retained, map not saved')
    return preceding,actor


def verify(preceding,shore):
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = list(subsystem.get_all_level_actors())
    assert set(actors)==set(preceding)|{shore}
    assert all(SIGNATURE(actor)==before for actor,before in preceding.items()), 'Preceding actor drift'
    runpy.run_path(str(SCRIPTS/'validate_marshward_marsh_unreal.py'))['validate_dressing'](actors)
    CONTRACT['main']()


def accept(preceding,shore):
    verify(preceding,shore)
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert [package.get_path_name() for package in dirty]==[CONTRACT['MAP']]
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    unreal.log('EMBERMERE_MARSHWARD_POOL_SHORE_PLACEMENT_SUCCESS: one visual strip, prior 52 static placements retained')
