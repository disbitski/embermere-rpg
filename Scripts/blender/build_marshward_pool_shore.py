"""Build a thin closed mud strip from the accepted pool rim stations."""

import json
from math import hypot
from pathlib import Path

import bmesh
import bpy

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardPoolShore_01'
MATERIAL = 'M_MarshwardShoreMud'
STATIONS = (12, 13, 14, 15, 0, 1, 2, 3)
TOE_OFFSETS = (12, 38, 52, 58, 64, 48, 34, 12)


def main():
    scene = bpy.context.scene
    assert bpy.app.background and {obj.name for obj in scene.objects} <= {'Cube', 'Camera', 'Light'}
    for obj in list(scene.objects):
        bpy.data.objects.remove(obj, do_unlink=True)
    scene.name = 'MarshwardPoolShore'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01

    rim = json.loads((OUT/'still_pool_metrics.json').read_text())['SM_MarshwardStillPool_01']['rim_xy_cm']
    top = []
    for i,(index,width) in enumerate(zip(STATIONS, TOE_OFFSETS)):
        x,y = rim[index]
        length = hypot(x,y)
        z = -2.5 if i in (0, len(STATIONS)-1) else -.4
        top.extend([(x+x*offset/length, y+y*offset/length, h)
                    for offset,h in ((-9,z), (2,z), (width,-2.5))])
    size = len(top)
    triangles = []
    boundary = {}
    for station in range(len(STATIONS)-1):
        for lane in range(2):
            i = 3*station+lane
            triangles.extend(((i,i+1,i+4), (i,i+4,i+3)))
    for tri in triangles:
        for a,b in zip(tri, (*tri[1:],tri[0])):
            if (b,a) in boundary:
                del boundary[b,a]
            else:
                boundary[a,b] = None
    faces = triangles + [tuple(i+size for i in reversed(tri)) for tri in triangles]
    for a,b in boundary:
        faces.extend(((a,a+size,b+size), (a,b+size,b)))
    vertices = top + [(x,y,-4) for x,y,_ in top]
    mesh = bpy.data.meshes.new(NAME)
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(NAME, mesh)
    scene.collection.objects.link(obj)
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    material = bpy.data.materials.new(MATERIAL)
    material.diffuse_color = (.0725,.07,.0425,1)
    mesh.materials.append(material)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.02)
    bpy.ops.object.mode_set(mode='OBJECT')

    bm = bmesh.new()
    bm.from_mesh(mesh)
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    volume = bm.calc_volume(signed=True)
    bm.free()
    assert bad_edges == 0 and volume > 0 and len(mesh.polygons) == 92
    assert len(mesh.uv_layers) == 1 and all(p.area > 0 for p in mesh.polygons)
    bounds = [[min(axis),max(axis)] for axis in zip(*vertices)]
    metrics = {NAME: {'dimensions_cm': [high-low for low,high in bounds],
                      'bounds_cm': bounds, 'triangles': 92, 'uv_channels': 1,
                      'non_manifold_edges': bad_edges, 'collision_hulls': 0,
                      'materials': [MATERIAL], 'pivot_cm': [0,0,0],
                      'rim_stations': list(STATIONS), 'top_rows_cm': top}}
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')), use_selection=True,
        object_types={'MESH'}, global_scale=1, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_NONE', axis_forward='-Y', axis_up='Z',
        bake_anim=False, use_mesh_modifiers=True)
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardPoolShore.blend'))
    (OUT/'pool_shore_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    return metrics


__result__ = main()
