"""Compare loaded Causeway mesh packages with a fresh isolated Blender report."""

import hashlib
import json
import math
from pathlib import Path
import runpy

_budget = runpy.run_path(str(Path(__file__).with_name('audit_marshward_budget_unreal.py')))
MAP, RETAINED, SHAPES = (_budget[key] for key in ('MAP', 'RETAINED', 'SHAPES'))


def compare_mesh(source, imported):
    for key in ('triangles', 'uv_channels', 'materials'):
        assert source[key] == imported[key], (key, source[key], imported[key])
    # Classic FBX mirrors local Y; asymmetric pivots must compare both endpoints.
    low, high = source['bounds_min'], source['bounds_max']
    assert len(low) == len(high) == 3
    expected = ([low[0], -high[1], low[2]], [high[0], -low[1], high[2]])
    for key, want in zip(('bounds_min', 'bounds_max'), expected):
        actual = imported[key]
        assert len(actual) == 3
        assert all(math.isfinite(a) and math.isfinite(b) and abs(a-b) < .1
                   for a, b in zip(actual, want)), (key, actual, want)
    expected_shapes = {k: v for k, v in source['simple_shapes'].items() if v}
    actual_shapes = {k: v for k, v in imported['simple_shapes'].items() if v}
    assert expected_shapes == actual_shapes, (expected_shapes, actual_shapes)
    assert not imported['complex_as_simple'], 'Authored simple collision must stay authoritative'


def main(report_path='/tmp/embermere-marshward-source-quality.json'):
    import unreal

    def clean():
        assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
        assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()

    clean()
    editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    assert editor.get_editor_world().get_path_name() == MAP+'.L_MarshwardCauseway_Greybox'
    assert not editor.get_game_world()
    report = json.loads(Path(report_path).read_text())
    specs = report['meshes']
    for source in specs.values():
        assert hashlib.sha256(Path(source['source_file']).read_bytes()).hexdigest() == source['source_sha256']
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    assets = {}
    for actor in actors:
        if isinstance(actor, unreal.StaticMeshActor):
            mesh = actor.get_component_by_class(unreal.StaticMeshComponent).static_mesh
            if mesh.get_path_name().startswith('/Game/'):
                assets[mesh.get_name()] = mesh
    retained = unreal.load_asset(RETAINED)
    assert retained.get_name() not in assets
    assets[retained.get_name()] = retained
    assert set(assets) == set(specs)
    subsystem = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
    for name, mesh in sorted(assets.items()):
        bound = mesh.get_bounds()
        center, extent = bound.origin, bound.box_extent
        body = mesh.get_editor_property('body_setup')
        agg = body.get_editor_property('agg_geom')
        slots = mesh.get_editor_property('static_materials')
        assert all(slot.material_interface is not None for slot in slots), name
        for slot in slots:
            material = slot.material_interface
            material_name = material.get_name()
            folder = ('PrototypeVillage' if material_name == 'M_EmbermereTimber' else
                      'MarshwardGreybox' if material_name.startswith('M_MarshwardGreybox') else
                      'MarshwardMarsh')
            assert material.get_path_name() == ('/Game/Art/Embermere/Environment/'+folder+
                                                '/'+material_name+'.'+material_name), name
        imported = {
            'triangles': mesh.get_num_triangles(0),
            'uv_channels': subsystem.get_num_uv_channels(mesh, 0),
            'bounds_min': [center.x-extent.x, center.y-extent.y, center.z-extent.z],
            'bounds_max': [center.x+extent.x, center.y+extent.y, center.z+extent.z],
            'materials': [slot.material_interface.get_name() for slot in slots],
            'simple_shapes': {field: len(agg.get_editor_property(field)) for field in SHAPES},
            'complex_as_simple': body.get_editor_property('collision_trace_flag') == unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE,
        }
        compare_mesh(specs[name], imported)
        print('MARSHWARD_ASSET_QUALITY_MESH', json.dumps({
            'name': name, **imported, 'lods': mesh.get_num_lods(),
            'material_paths': [slot.material_interface.get_path_name() for slot in slots],
            'nanite': mesh.get_editor_property('nanite_settings').get_editor_property('enabled')}, sort_keys=True))
    clean()
    print('EMBERMERE_MARSHWARD_ASSET_QUALITY_SUCCESS: '
          f'{len(assets)} packages match retained source UVs, bounds/pivots, materials, '
          'triangles and authored simple-shape counts, no dirty packages')


if __name__ == '__main__':
    main()
