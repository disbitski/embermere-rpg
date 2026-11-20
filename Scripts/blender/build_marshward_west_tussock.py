"""Build one flat-footed peat tussock with short closed sedge blades."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector
from mathutils.bvhtree import BVHTree

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardWestTussock_01'
SIZE = (240,170,72)
PALETTE = {'M_MarshwardPeat': (.13,.16,.11,1),
           'M_MarshwardMoss': (.23,.34,.12,1),
           'M_MarshwardReedGreen': (.30,.43,.16,1)}
FOOT_SAMPLES = [(-70,-20,0),(-5,32,0),(69,-17,0)]
BLADES = [
    (-45,-14,170,39,44),(-48,-9,103,26,58),(-40,-16,224,32,49),
    (-4,27,58,34,66),(1,22,125,32,72),(4,26,5,44,52),
    (48,-18,-14,36,54),(45,-24,-72,31,47),(53,-22,39,38,42),
]


def make_geometry():
    vertices,faces,slots = [],[],[]

    def ring(points):
        indices = list(range(len(vertices),len(vertices)+len(points)))
        vertices.extend(points)
        return indices

    def face(indices,slot):
        faces.append(tuple(indices))
        slots.append(slot)

    def stitch(lower,upper,colors):
        for index,vertex in enumerate(lower):
            nxt = (index+1)%len(lower)
            face((vertex,lower[nxt],upper[nxt],upper[index]),colors[index])

    scatter = (1.00,.91,1.05,.96,1.02,.87,1.05,.93,1.01)
    rings = []
    for height,radius in [(0,1),(9,.92),(24,.58)]:
        rings.append(ring([
            (math.cos(index*math.tau/9)*112*uneven*radius + height*.2,
             math.sin(index*math.tau/9)*82*uneven*radius,
             height+((index*5)%7-3 if height else 0))
            for index,uneven in enumerate(scatter)]))
    face(reversed(rings[0]),0)
    stitch(rings[0],rings[1],[0]*9)
    stitch(rings[1],rings[2],[1,0,1,1,0,1,1,0,1])
    crown = ring([(4,2,26)])[0]
    for index,vertex in enumerate(rings[2]):
        face((vertex,rings[2][(index+1)%9],crown),1)

    surface = BVHTree.FromPolygons(vertices,faces)
    for x,y,z in FOOT_SAMPLES:
        hit,_,_,_ = surface.ray_cast(Vector((x,y,-1)),Vector((0,0,1)),2)
        assert hit is not None and abs(hit.z-z)<.001

    for x,y,yaw,reach,top in BLADES:
        angle = math.radians(yaw)
        dx,dy = math.cos(angle),math.sin(angle)
        rings = []
        for run,height,width,depth in [(0,12,3.5,1.3),(.15,top*.58,5.5,1.8),
                                       (.58,top*.92,3.1,1.15),(1,top,.45,.35)]:
            rings.append(ring([
                (x+dx*(reach*run+along)-dy*across,
                 y+dy*(reach*run+along)+dx*across,height)
                for along,across in [(depth,0),(0,width),(-depth,0),(0,-width)]]))
        for index in rings[0]:
            xx,yy,zz = vertices[index]
            hit,_,_,_ = surface.ray_cast(Vector((xx,yy,100)),Vector((0,0,-1)),100)
            assert hit is not None and hit.z>zz+1
        face(reversed(rings[0]),2)
        for lower,upper in zip(rings,rings[1:]):
            stitch(lower,upper,[2]*4)
        face(rings[-1],2)

    lo,hi = [tuple(fn(axis) for axis in zip(*vertices)) for fn in (min,max)]
    center = ((lo[0]+hi[0])/2,(lo[1]+hi[1])/2,lo[2])
    scale = [size/(upper-lower) for size,lower,upper in zip(SIZE,lo,hi)]

    def normalize(point):
        return [(value-pivot)*factor for value,pivot,factor in zip(point,center,scale)]

    return [normalize(point) for point in vertices],faces,slots,[normalize(point) for point in FOOT_SAMPLES]


def main():
    scene = bpy.context.scene
    assert bpy.app.background and {obj.name for obj in scene.objects} <= {'Cube','Camera','Light'}
    for obj in list(scene.objects):
        bpy.data.objects.remove(obj,do_unlink=True)
    scene.name = 'MarshwardWestTussock'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    vertices,faces,slots,feet = make_geometry()
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
    pieces = 0
    while remaining:
        stack = [remaining.pop()]
        pieces += 1
        while stack:
            for edge in stack.pop().link_edges:
                for vertex in edge.verts:
                    if vertex in remaining:
                        remaining.remove(vertex)
                        stack.append(vertex)
    assert bad_edges==0 and pieces==10 and len(bm.faces)==304
    assert all(face.calc_area()>.01 for face in bm.faces)
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
                      'non_manifold_edges': bad_edges, 'closed_pieces': pieces, 'collision_hulls': 0,
                      'materials': list(PALETTE), 'pivot_cm': list(obj.location), 'foot_samples_cm': feet}}
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',
        axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'west_tussock_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')

    bpy.ops.object.camera_add(location=(270,-332,205))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,27))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 310
    camera.data.clip_end = 10000
    scene.camera = camera
    scene.render.engine = 'BLENDER_WORKBENCH'
    scene.display.shading.light = 'STUDIO'
    scene.display.shading.color_type = 'MATERIAL'
    scene.display.shading.show_cavity = True
    scene.render.resolution_x,scene.render.resolution_y = 940,740
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'west_tussock_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardWestTussock.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
