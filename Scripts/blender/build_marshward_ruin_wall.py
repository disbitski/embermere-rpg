"""Build staggered broken masonry over three inset collision cores."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector
from mathutils.bvhtree import BVHTree

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardRuinWall_01'
SIZE = (900,220,420)
PALETTE = {'M_MarshwardMarshStone': (.32,.35,.34,1), 'M_MarshwardMoss': (.23,.34,.12,1)}
COURSES = [
    [(-450,-237,0,110,0,137),(-229,-12,-4,99,0,135),
     (-5,213,5,99,0,143),(221,450,-6,100,0,139)],
    [(-344,-108,7,89,129,269),(-101,135,-1,96,129,275),
     (142,340,6,84,130,268)],
    [(-179,28,-6,84,263,420),(38,227,8,77,261,374)],
]
CORES = [(-425,423,82,0,122),(-317,312,70,122,244),(-152,178,57,244,333)]
CRESTS = [(-390,0,122),(-230,0,244),(0,0,333)]
GROUND = [(x,y,0) for x in (-390,0,390) for y in (-160,160)]
AIR = [(-390,210),(390,210)]


def block(spec,index,course):
    left,right,y,half_depth,bottom,top = spec
    vertices,faces,slots = [],[],[]
    cuts = (0,8,17,22,14,4,11,16)
    for level,(inset,z) in enumerate([(8,bottom),(0,bottom+16),(0,top-26),(13,top)]):
        x0,x1 = left+inset,right-inset
        y0,y1 = y-half_depth+inset,y+half_depth-inset
        cut = 15+(index*7)%10
        outline = [(x0+cut,y0),(x1-cut,y0),(x1,y0+cut),(x1,y1-cut),
                   (x1-cut,y1),(x0+cut,y1),(x0,y1-cut),(x0,y0+cut)]
        for corner,(xx,yy) in enumerate(outline):
            nick = cuts[(corner+index)%8] if level==3 and course>0 else 0
            vertices.append((xx,yy,z-nick))
    for level in range(3):
        for side in range(8):
            lower = level*8
            nxt = (side+1)%8
            faces.append((lower+side,lower+nxt,lower+8+nxt,lower+8+side))
            slots.append(int((level==0 and (side+index)%5==0) or
                             (level==2 and course<2 and (side+2*index)%5==0)))
    faces.extend([tuple(reversed(range(8))),tuple(range(24,32))])
    slots.extend([0,int(index in (1,3,4,6,8))])
    return vertices,faces,slots


def make_mesh(name,vertices,faces):
    mesh = bpy.data.meshes.new(name)
    mesh.from_pydata(vertices,[],faces)
    mesh.update()
    obj = bpy.data.objects.new(name,mesh)
    bpy.context.scene.collection.objects.link(obj)
    return obj


def main():
    scene = bpy.context.scene
    assert bpy.app.background and {obj.name for obj in scene.objects} <= {'Cube','Camera','Light'}
    for obj in list(scene.objects):
        bpy.data.objects.remove(obj,do_unlink=True)
    scene.name = 'MarshwardRuinWall'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    vertices,faces,slots = [],[],[]
    stone_count = 0
    for course,specs in enumerate(COURSES):
        for spec in specs:
            points,polys,colors = block(spec,stone_count,course)
            faces.extend(tuple(index+len(vertices) for index in poly) for poly in polys)
            vertices.extend(points)
            slots.extend(colors)
            stone_count += 1
    obj = make_mesh(NAME,vertices,faces)
    mesh = obj.data
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
    assert bad_edges==0 and len(bm.faces)==540
    assert all(face.calc_area()>.01 for face in bm.faces)
    bm.to_mesh(mesh)
    bm.free()
    surface = BVHTree.FromPolygons([vertex.co for vertex in mesh.vertices],
                                   [tuple(face.vertices) for face in mesh.polygons],all_triangles=True)
    for x,y,z in CRESTS:
        hit,_,_,_ = surface.ray_cast(Vector((x,y,450)),Vector((0,0,-1)),450)
        assert hit is not None and hit.z>z
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    assert len(mesh.uv_layers)==1 and tuple(obj.location)==(0,0,0)
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,SIZE))

    for index,(left,right,depth,bottom,top) in enumerate(CORES):
        points = [(x,y,z) for z in (bottom,top)
                  for x,y in [(left,-depth),(right,-depth),(right,depth),(left,depth)]]
        collider = make_mesh(f'UCX_{NAME}_{index:02}',points,
            [(3,2,1,0),(4,5,6,7),(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)])
        collider.hide_render = True
        collider.display_type = 'WIRE'
        collider.select_set(True)
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':len(mesh.polygons),
                     'uv_channels':len(mesh.uv_layers),'non_manifold_edges':bad_edges,
                     'stone_count':stone_count,'collision_hulls':len(CORES),
                     'materials':list(PALETTE),'pivot_cm':list(obj.location),
                     'cores_cm':CORES,'crest_samples_cm':CRESTS,'ground_samples_cm':GROUND,
                     'air_samples_cm':AIR}}
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',
        axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'ruin_wall_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')

    bpy.ops.object.camera_add(location=(760,-1260,570))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,172))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 1110
    camera.data.clip_end = 10000
    scene.camera = camera
    scene.render.engine = 'BLENDER_WORKBENCH'
    scene.display.shading.light = 'STUDIO'
    scene.display.shading.color_type = 'MATERIAL'
    scene.display.shading.show_cavity = True
    scene.render.resolution_x,scene.render.resolution_y = 1000,680
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'ruin_wall_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardRuinWall.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
