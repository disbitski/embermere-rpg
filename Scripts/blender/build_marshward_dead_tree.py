"""Author one continuous marsh snag in an isolated Blender source file."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector
from mathutils.bvhtree import BVHTree

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardDeadTree_01'
SIZE = (720, 500, 800)
PALETTE = {'M_MarshwardEarth': (.28, .22, .13, 1),
           'M_EmbermereTimber': (.24, .105, .035, 1),
           'M_MarshwardMoss': (.23, .34, .12, 1)}
TRUNK = [(0,0,0,55), (3,2,75,43), (3,9,205,36), (24,19,315,34),
         (12,32,435,28), (-9,57,535,22), (6,72,650,15), (32,85,730,9)]
LIMBS = {
    (2,1): [(-50,105,288,26), (-121,170,338,18), (-155,220,390,11), (-207,265,485,4)],
    (3,4): [(51,-101,365,27), (93,-159,429,18), (126,-205,508,11), (173,-229,536,4)],
    (4,0): [(104,72,490,21), (165,109,554,14), (215,114,609,8), (260,92,673,3)],
    (5,3): [(-78,28,585,16), (-128,2,640,10), (-153,-19,714,4)],
}
FORKS = {
    (2,1): (2,0),
    (3,4): (2,2),
}


def make_tree():
    vertices, faces, slots = [], [], []

    def ring(points):
        indices = list(range(len(vertices), len(vertices)+len(points)))
        vertices.extend(points)
        return indices

    def face(indices, slot=0):
        faces.append(tuple(indices))
        slots.append(slot)

    def sides(lower, upper):
        for index, vertex in enumerate(lower):
            following = (index+1) % len(lower)
            yield index, (vertex, lower[following], upper[following], upper[index])

    def branch(mouth, stations, fork=None):
        center = sum((vertices[index] for index in mouth), Vector((0,0,0)))/4
        prev = center
        guide = vertices[mouth[1]]-vertices[mouth[0]]
        current = mouth
        square = [(-1,-1), (1,-1), (1,1), (-1,1)]
        for level, (x,y,z,radius) in enumerate(stations):
            point = Vector((x,y,z))
            axis = (point-prev).normalized()
            lateral = (guide-axis*guide.dot(axis)).normalized()
            normal = axis.cross(lateral)
            new = ring([point+radius*(u*lateral+v*normal) for u,v in square])
            for column, indices in sides(current,new):
                if (level,column)==fork:
                    corners = [vertices[index] for index in indices]
                    root = sum(corners,Vector((0,0,0)))/4
                    outward = (corners[1]-corners[0]).cross(corners[3]-corners[0]).normalized()
                    twig = [(*tuple(root+outward*reach+Vector((0,0,rise))),width)
                            for reach,rise,width in [(38,5,9),(72,31,6),(98,78,3)]]
                    branch(indices,twig)
                else:
                    face(indices)
            current,prev,guide = new,point,lateral
        face(current,1)

    rings = []
    facets = (1.09,.92,1.04,.89,1.12,.95)
    for level, (x,y,z,radius) in enumerate(TRUNK):
        points = []
        for index, uneven in enumerate(facets):
            angle = index*math.tau/6
            chip = (0,10,22,8,14,4)[index] if level==len(TRUNK)-1 else 0
            points.append(Vector((x+radius*uneven*math.cos(angle),
                                  y+radius*uneven*math.sin(angle)*.86,z-chip)))
        rings.append(ring(points))
    face(reversed(rings[0]))
    for level, (lower,upper) in enumerate(zip(rings,rings[1:])):
        for column, indices in sides(lower,upper):
            key = level,column
            if key in LIMBS:
                branch(indices,LIMBS[key],FORKS.get(key))
            else:
                face(indices,2 if level==0 and column in (0,2,3,5) else int(key in ((1,0),(4,5),(5,5))))
    face(rings[-1],1)

    low, high = [tuple(fn(axis) for axis in zip(*vertices)) for fn in (min,max)]
    origin = ((low[0]+high[0])/2, (low[1]+high[1])/2, low[2])
    scale = [size/(end-start) for size,start,end in zip(SIZE,low,high)]

    def normalize(point):
        return [(value-center)*factor for value,center,factor in zip(point,origin,scale)]

    return [normalize(point) for point in vertices], faces, slots, normalize((0,0,0))


def main():
    scene = bpy.context.scene
    assert bpy.app.background, 'Run with the isolated headless Blender transport'
    assert {obj.name for obj in scene.objects} <= {'Cube','Camera','Light'}, 'Use factory startup'
    for obj in list(scene.objects):
        bpy.data.objects.remove(obj,do_unlink=True)
    scene.name = 'MarshwardDeadTree'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01

    vertices, faces, slots, foot = make_tree()
    mesh = bpy.data.meshes.new(NAME)
    mesh.from_pydata(vertices,[],faces)
    mesh.update()
    for name,color in PALETTE.items():
        material = bpy.data.materials.new(name)
        material.diffuse_color = color
        mesh.materials.append(material)
    for polygon,slot in zip(mesh.polygons,slots):
        polygon.material_index = slot
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
    bmesh.ops.triangulate(bm,faces=list(bm.faces))
    bad_edges = sum(not edge.is_manifold or not edge.is_contiguous for edge in bm.edges)
    remaining = set(bm.verts)
    stack = [remaining.pop()]
    while stack:
        for edge in stack.pop().link_edges:
            for vertex in edge.verts:
                if vertex in remaining:
                    remaining.remove(vertex)
                    stack.append(vertex)
    assert not remaining and bad_edges==0
    assert all(face.calc_area()>.01 for face in bm.faces)
    assert len(bm.faces)==260
    bm.faces.index_update()
    bm.faces.ensure_lookup_table()
    surface = BVHTree.FromBMesh(bm)
    face_vertices = [set(face.verts) for face in bm.faces]
    crossings = [(left,right) for left,right in surface.overlap(surface)
                 if left<right and not face_vertices[left].intersection(face_vertices[right])]
    assert not crossings, [(pair,[tuple(round(value) for value in bm.faces[index].calc_center_median())
                                  for index in pair]) for pair in crossings]
    bm.to_mesh(mesh)
    bm.free()

    obj = bpy.data.objects.new(NAME,mesh)
    scene.collection.objects.link(obj)
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    assert len(mesh.uv_layers)==1 and tuple(obj.location)==(0,0,0)
    assert all(abs(actual-target)<.001 for actual,target in zip(obj.dimensions,SIZE))
    metrics = {NAME: {'dimensions_cm': [round(value,3) for value in obj.dimensions],
                      'triangles': len(mesh.polygons), 'uv_channels': len(mesh.uv_layers),
                      'non_manifold_edges': bad_edges, 'closed_pieces': 1, 'collision_hulls': 0,
                      'materials': list(PALETTE), 'pivot_cm': list(obj.location), 'foot_cm': foot}}
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,
        object_types={'MESH'},global_scale=1,apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'dead_tree_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')

    bpy.ops.object.camera_add(location=(1150,-900,570))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,390))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 970
    camera.data.clip_end = 10000
    scene.camera = camera
    scene.render.engine = 'BLENDER_WORKBENCH'
    scene.display.shading.light = 'STUDIO'
    scene.display.shading.color_type = 'MATERIAL'
    scene.display.shading.show_cavity = True
    scene.render.resolution_x = scene.render.resolution_y = 900
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'dead_tree_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardDeadTree.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
