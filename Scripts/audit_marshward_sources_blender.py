"""Read retained Causeway sources in an isolated Blender process; never save."""

import hashlib
import json
import math
from pathlib import Path

ROOT = Path('/Users/wizard/Documents/Unreal Game')
SOURCE = ROOT/'ArtSource/Blender/Environment'


def main(output='/tmp/embermere-marshward-source-quality.json'):
    import bpy

    assert bpy.app.background, 'Use headless transport, not the active Blender scene'
    pairs = [('MarshwardGreybox/MarshwardGreyboxKit.blend', 'MarshwardGreybox/kit_metrics.json'),
             ('MarshwardWaymarker/MarshwardWaymarker.blend', 'MarshwardWaymarker/waymarker_metrics.json')]
    for blend, metric in (
        ('MarshAssets', 'mesh'), ('PeatShelf', 'shelf'), ('RockCluster', 'rock'),
        ('BankEdge', 'bank_edge'), ('Foundation', 'foundation'),
        ('FallenMasonry', 'fallen_masonry'), ('TallReeds', 'tall_reeds'),
        ('LowScrub', 'low_scrub'), ('RockyBank', 'rocky_bank'),
        ('TimberDebris', 'timber_debris'), ('MooringPosts', 'mooring_posts'),
        ('DeadTree', 'dead_tree'), ('WestTussock', 'west_tussock'),
        ('RuinWall', 'ruin_wall'), ('StillPool', 'still_pool'), ('PoolShore', 'pool_shore')):
        pairs.append(('MarshwardMarsh/Marshward'+blend+'.blend',
                      'MarshwardMarsh/'+metric+'_metrics.json'))
    specs = {}
    owned = {}
    for blend, metric in pairs:
        owned[blend] = json.loads((SOURCE/metric).read_text())
        for name, spec in owned[blend].items():
            assert name not in specs, name
            specs[name] = spec
    rows = {}
    for blend, _ in pairs:
        file = SOURCE/blend
        names = set(owned[blend])
        with bpy.data.libraries.load(str(file), link=False) as (available, loaded):
            assert names <= set(available.objects), (file, names-set(available.objects))
            # Some scenes retain donor objects; only this file's declared exports own rows.
            loaded.objects = [name for name in available.objects if name in names or
                              any(name.startswith(prefix+owner) for owner in names
                                  for prefix in ('UCX_', 'UBX_'))]
        objects = [obj for obj in loaded.objects if obj is not None]
        for obj in objects:
            if not obj.name.startswith('SM_Marshward'):
                continue
            name = obj.name
            assert name in specs and name not in rows, name
            assert obj.type == 'MESH' and not obj.modifiers, name
            assert all(abs(obj.matrix_world[i][j] - (1 if i == j else 0)) < 1e-6
                       for i in range(4) for j in range(4)), name+' transform not applied'
            mesh = obj.data
            mesh.calc_loop_triangles()
            low = [min(v.co[i] for v in mesh.vertices) for i in range(3)]
            high = [max(v.co[i] for v in mesh.vertices) for i in range(3)]
            edge_faces = {tuple(sorted(edge.vertices)): 0 for edge in mesh.edges}
            for polygon in mesh.polygons:
                for edge in polygon.edge_keys:
                    edge_faces[tuple(sorted(edge))] += 1
            materials = [material.name if material else None for material in mesh.materials]
            uv_values = [float(value) for layer in mesh.uv_layers
                         for loop in layer.data for value in loop.uv]
            assert uv_values and all(math.isfinite(value) for value in uv_values), name
            row = {'source_file': str(file),
                   'source_sha256': hashlib.sha256(file.read_bytes()).hexdigest(),
                   'triangles': len(mesh.loop_triangles), 'uv_channels': len(mesh.uv_layers),
                   'bounds_min': low, 'bounds_max': high,
                   'non_manifold_edges': sum(count != 2 for count in edge_faces.values()),
                   'uv_range': [min(uv_values), max(uv_values)],
                   'materials': materials,
                   'simple_shapes': {
                       'box_elems': sum(o.name.startswith('UBX_'+name) for o in objects),
                       'convex_elems': sum(o.name.startswith('UCX_'+name) for o in objects)}}
            spec = specs[name]
            assert row['triangles'] == spec['triangles'], name
            assert row['uv_channels'] == spec['uv_channels'] == 1, name
            assert row['non_manifold_edges'] == spec['non_manifold_edges'] == 0, name
            assert all(abs((hi-lo)-want) < .1
                       for lo, hi, want in zip(low, high, spec['dimensions_cm'])), name
            assert materials and all(materials), name
            if 'materials' in spec:
                assert sorted(materials) == sorted(spec['materials']), (name, materials, spec['materials'])
            rows[name] = row
        for obj in objects:
            bpy.data.objects.remove(obj, do_unlink=True)
        bpy.data.orphans_purge(do_recursive=True)
    assert set(rows) == set(specs), (set(specs)-set(rows), set(rows)-set(specs))
    report = {'source_files': len(pairs), 'meshes': dict(sorted(rows.items()))}
    Path(output).write_text(json.dumps(report, indent=2)+'\n')
    print('EMBERMERE_MARSHWARD_SOURCE_QUALITY_SUCCESS: '
          f'{len(pairs)} retained files, {len(rows)} meshes, exact metrics, applied transforms, '
          'one UV each, closed edges, no source saves')
    return report


if __name__ == '__main__':
    main()
