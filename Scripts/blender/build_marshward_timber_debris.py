"""Build three separate grounded broken timbers, without support collision."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardTimberDebris_01'
MATERIALS = {'M_EmbermereTimber':(.24,.105,.035,1),
             'M_MarshwardMoss':(.23,.34,.12,1)}
SPECS = [(-20,-90,10,240,32,12),(25,0,-15,200,36,18),(60,105,30,130,30,14)]


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardTimberDebris')
    scene = bpy.data.scenes.new('MarshwardTimberDebris_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardTimberDebris'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    mats = []
    for name,color in MATERIALS.items():
        mat = bpy.data.materials.get(name)
        if not mat:
            mat = bpy.data.materials.new(name)
            mat.use_nodes = True
            bsdf = mat.node_tree.nodes.get('Principled BSDF')
            bsdf.inputs['Base Color'].default_value = color
            bsdf.inputs['Roughness'].default_value = .9
        mats.append(mat)
    parts = []
    for index,(cx,cy,yaw,length,width,height) in enumerate(SPECS):
        outline = [(-length/2,-width/2),(length/2-18,-width/2),
                   (length/2,-width*.12),(length/2-26,width*.08),
                   (length/2-8,width/2),(-length/2,width/2),
                   (-length/2+15,width*.08),(-length/2-8,-width*.1)]
        angle = math.radians(yaw)
        vertices = [(cx+x*math.cos(angle)-y*math.sin(angle),
                     cy+x*math.sin(angle)+y*math.cos(angle),z)
                    for z in (0,height) for x,y in outline]
        faces = [(i,(i+1)%8,8+(i+1)%8,8+i) for i in range(8)]
        faces += [tuple(reversed(range(8))),tuple(range(8,16))]
        mesh = bpy.data.meshes.new('BrokenTimber_'+str(index))
        mesh.from_pydata(vertices,[],faces)
        mesh.update()
        obj = bpy.data.objects.new('BrokenTimber_'+str(index),mesh)
        scene.collection.objects.link(obj)
        for mat in mats:
            mesh.materials.append(mat)
        for poly in mesh.polygons:
            poly.material_index = 1 if index==1 and poly.index==3 else 0
        parts.append(obj)
    coords = [v.co for obj in parts for v in obj.data.vertices]
    lo = [min(v[i] for v in coords) for i in range(3)]
    hi = [max(v[i] for v in coords) for i in range(3)]
    center = [(lo[0]+hi[0])/2,(lo[1]+hi[1])/2,lo[2]]
    factors = [size/(hi[i]-lo[i]) for i,size in enumerate((380,260,30))]
    def normalized(point):
        return [(point[i]-center[i])*factors[i] for i in range(3)]
    for obj in parts:
        for vertex in obj.data.vertices:
            vertex.co = Vector(normalized(vertex.co))
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
        assert all(edge.is_manifold for edge in bm.edges)
        bm.to_mesh(obj.data)
        bm.free()
    bpy.ops.object.select_all(action='DESELECT')
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    obj = bpy.context.object
    obj.name = NAME
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.context.view_layer.update()
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    remaining = set(bm.verts)
    components = 0
    while remaining:
        stack = [remaining.pop()]
        components += 1
        while stack:
            for edge in stack.pop().link_edges:
                for vertex in edge.verts:
                    if vertex in remaining:
                        remaining.remove(vertex)
                        stack.append(vertex)
    bm.free()
    assert bad_edges==0 and components==3 and len(obj.data.polygons)==84
    assert len(obj.data.uv_layers)==1 and tuple(obj.location)==(0,0,0)
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,(380,260,30)))
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':84,
        'uv_channels':1,'non_manifold_edges':bad_edges,'closed_pieces':components,
        'collision_hulls':0,'materials':list(MATERIALS),'pivot_cm':list(obj.location),
        'base_samples_cm':[normalized((cx,cy,0)) for cx,cy,yaw,l,w,h in SPECS]}}
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,
        object_types={'MESH'},global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',
        axis_forward='-Y',axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'timber_debris_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(600,-750,700))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,10))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 570
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardTimberDebrisPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'timber_debris_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardTimberDebris.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
