"""Stage the pool for visual review, then save only on explicit acceptance."""

import json
from pathlib import Path
import runpy
import unreal

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = ROOT/'Scripts'
CONTRACT = runpy.run_path(str(SCRIPTS/'validate_marshward_still_pool_unreal.py'))
SIGNATURE = runpy.run_path(str(SCRIPTS/'place_marshward_foundation_unreal.py'))['signature']


def stage():
    editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    assert editor.get_editor_world().get_path_name().startswith(CONTRACT['MAP']+'.')
    assert not editor.get_game_world()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    preceding = {actor: SIGNATURE(actor) for actor in subsystem.get_all_level_actors()}
    assert sum(isinstance(actor, unreal.StaticMeshActor) for actor in preceding) == 51
    assert all(SIGNATURE(actor) == before for actor,before in preceding.items())
    rows = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())['placements']
    additions = [row for row in rows if row[1] == 'StillPool']
    assert additions == [['StillPoolWest', 'StillPool', list(CONTRACT['POS']), 0, [1,1,1]]]
    assert all(a.get_actor_label() != CONTRACT['LABEL'] for a in preceding)
    mesh = unreal.load_asset(CONTRACT['DEST']+'/'+CONTRACT['NAME'])
    assert mesh
    actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(*CONTRACT['POS']), unreal.Rotator())
    assert actor
    try:
        actor.set_actor_label(CONTRACT['LABEL'])
        actor.set_folder_path('MarshwardGreybox/MarshTerrain')
        actor.tags = [unreal.Name('EmbermereMarshwardMarsh')]
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        assert component.set_static_mesh(mesh)
        component.set_collision_profile_name('NoCollision')
        component.set_editor_property('generate_overlap_events', False)
        component.set_editor_property('can_ever_affect_navigation', False)
        component.set_editor_property('cast_shadow', False)
        verify(preceding, actor)
    except Exception:
        subsystem.destroy_actor(actor)
        raise
    unreal.log('EMBERMERE_MARSHWARD_STILL_POOL_STAGED: original 51 static placements retained, map not saved')
    return preceding, actor


def verify(preceding, pool):
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = list(subsystem.get_all_level_actors())
    assert set(actors) == set(preceding) | {pool}
    assert all(SIGNATURE(actor) == before for actor,before in preceding.items()), 'Preceding actor drift'
    runpy.run_path(str(SCRIPTS/'validate_marshward_marsh_unreal.py'))['validate_dressing'](actors)
    CONTRACT['main']()


def accept(preceding, pool):
    verify(preceding, pool)
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()
    dirty = unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    assert [package.get_path_name() for package in dirty] == [CONTRACT['MAP']]
    assert unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
    unreal.log('EMBERMERE_MARSHWARD_STILL_POOL_PLACEMENT_SUCCESS: 1 visual study, prior 51 static placements retained')
