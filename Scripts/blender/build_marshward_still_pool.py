"""Build the independent flat west-basin still-water study."""

import json
from pathlib import Path

import bmesh
import bpy

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardStillPool_01'
MATERIAL = 'M_MarshwardStillPool'
RIM = [
    (420, -15), (385, 91), (293, 171), (171, 220),
    (31, 260), (-115, 238), (-238, 190), (-362, 122),
    (-420, 33), (-395, -82), (-302, -164), (-205, -219),
    (-47, -260), (97, -240), (248, -203), (360, -122),
]


def main():
    scene = bpy.context.scene
    assert bpy.app.background and {obj.name for obj in scene.objects} <= {'Cube', 'Camera', 'Light'}
    for obj in list(scene.objects):
        bpy.data.objects.remove(obj, do_unlink=True)
    scene.name = 'MarshwardStillPool'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01

    count = len(RIM)
    vertices = ([(x, y, 0) for x, y in RIM] +
                [(x*.74, y*.74, 0) for x, y in RIM] +
                [(x, y, -3) for x, y in RIM] +
                [(0, 0, 0), (0, 0, -3)])
    faces = []
    for i in range(count):
        j = (i+1) % count
        faces.extend([(3*count, count+i, count+j),
                      (count+i, i, j), (count+i, j, count+j),
                      (i, 2*count+i, 2*count+j), (i, 2*count+j, j),
                      (3*count+1, 2*count+j, 2*count+i)])
    mesh = bpy.data.meshes.new(NAME)
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    uv = mesh.uv_layers.new(name='UVMap')
    color = mesh.color_attributes.new(name='WaterCore', type='BYTE_COLOR', domain='CORNER')
    for face in mesh.polygons:
        side = face.index % 6 in (3, 4)
        i = face.index // 6
        j = (i+1) % count
        for loop_index in face.loop_indices:
            vertex = mesh.loops[loop_index].vertex_index
            x, y, z = vertices[vertex]
            if side:
                uv.uv[loop_index].vector = ((i if vertex % count == i else i+1)/count, .02 if z else .06)
                assert vertex % count in (i, j)
            else:
                uv.uv[loop_index].vector = (.5+x/900, .5+y/620)
            core = float(count <= vertex < 2*count or vertex == 3*count)
            color.data[loop_index].color = (core, core, core, 1)

    obj = bpy.data.objects.new(NAME, mesh)
    scene.collection.objects.link(obj)
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    material = bpy.data.materials.new(MATERIAL)
    material.diffuse_color = (.032, .075, .074, 1)
    mesh.materials.append(material)

    bm = bmesh.new()
    bm.from_mesh(mesh)
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    volume = bm.calc_volume(signed=True)
    bm.free()
    assert bad_edges == 0 and volume > 0 and len(mesh.polygons) == 96
    assert tuple(obj.dimensions) == (840, 520, 3) and len(mesh.uv_layers) == 1
    metrics = {NAME: {'dimensions_cm': list(obj.dimensions), 'triangles': 96, 'uv_channels': 1,
                      'non_manifold_edges': bad_edges, 'collision_hulls': 0, 'materials': [MATERIAL],
                      'pivot_cm': [0, 0, 0], 'top_plane_cm': 0, 'core_ring_fraction': .74,
                      'rim_xy_cm': RIM}}
    bpy.ops.export_scene.fbx(filepath=str(OUT / (NAME+'.fbx')), use_selection=True,
        object_types={'MESH'}, global_scale=1, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_NONE', axis_forward='-Y', axis_up='Z',
        bake_anim=False, use_mesh_modifiers=True, colors_type='LINEAR')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT / 'MarshwardStillPool.blend'))
    (OUT / 'still_pool_metrics.json').write_text(json.dumps(metrics, indent=2)+'\n')
    return metrics


__result__ = main()
