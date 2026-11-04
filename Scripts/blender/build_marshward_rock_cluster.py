"""Build three grounded closed stones and one explicit hull per stone."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardRockCluster_01'
MATERIALS = {'M_MarshwardMarshStone':(.29,.34,.36,1),'M_MarshwardMoss':(.23,.34,.12,1)}


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardRockCluster')
    scene = bpy.data.scenes.new('MarshwardRockCluster_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardRockCluster'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    mats = []
    for name,color in MATERIALS.items():
        mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
        mat.use_nodes = True
        bsdf = mat.node_tree.nodes.get('Principled BSDF')
        bsdf.inputs['Base Color'].default_value = color
        bsdf.inputs['Roughness'].default_value = .9
        mats.append(mat)
    specs = [(-85,-5,135,130,260),(105,-65,115,110,165),(80,115,90,85,110)]
    parts,colliders = [],[]
    for stone,(cx,cy,rx,ry,height) in enumerate(specs):
        vertices,faces,slots = [],[],[]
        for radius,z in [(.82,0),(1,.5*height),(.62,.85*height)]:
            for i in range(8):
                theta = math.tau*i/8
                vertices.append((cx+rx*radius*math.cos(theta),cy+ry*radius*math.sin(theta),z))
        vertices += [(cx+.1*rx,cy-.1*ry,height),(cx,cy,0)]
        for ring in range(2):
            for i in range(8):
                j = (i+1)%8
                faces.append((ring*8+i,ring*8+j,(ring+1)*8+j,(ring+1)*8+i))
                slots.append(1 if ring==1 and (i+stone)%3==0 else 0)
        for i in range(8):
            faces += [(16+i,16+(i+1)%8,24),(i,25,(i+1)%8)]
            slots += [1 if (i+stone)%3==0 else 0,0]
        mesh = bpy.data.meshes.new('MarshStone_'+str(stone))
        mesh.from_pydata(vertices,[],faces)
        mesh.update()
        obj = bpy.data.objects.new('MarshStone_'+str(stone),mesh)
        scene.collection.objects.link(obj)
        for mat in mats:
            mesh.materials.append(mat)
        for poly,slot in zip(mesh.polygons,slots):
            poly.material_index = slot
        parts.append(obj)
    coords = [v.co for obj in parts for v in obj.data.vertices]
    lo = [min(v[i] for v in coords) for i in range(3)]
    hi = [max(v[i] for v in coords) for i in range(3)]
    center = [(lo[0]+hi[0])/2,(lo[1]+hi[1])/2,lo[2]]
    factors = [size/(hi[i]-lo[i]) for i,size in enumerate((460,340,260))]
    def normalized(point):
        return [(point[i]-center[i])*factors[i] for i in range(3)]
    for stone,obj in enumerate(parts):
        for vertex in obj.data.vertices:
            vertex.co = Vector(normalized(vertex.co))
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
        assert all(e.is_manifold for e in bm.edges)
        bm.to_mesh(obj.data)
        bm.free()
        hull = bmesh.new()
        for vertex in obj.data.vertices:
            hull.verts.new(vertex.co)
        bmesh.ops.convex_hull(hull,input=list(hull.verts),use_existing_faces=False)
        hull_mesh = bpy.data.meshes.new(NAME+'_Hull_'+str(stone))
        hull.to_mesh(hull_mesh)
        hull.free()
        collider = bpy.data.objects.new('UCX_'+NAME+'_'+str(stone).zfill(2),hull_mesh)
        scene.collection.objects.link(collider)
        collider.hide_render = True
        collider.display_type = 'WIRE'
        colliders.append(collider)
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
    mod = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=mod.name)
    bpy.context.view_layer.update()
    assert len(obj.data.polygons)==144
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,(460,340,260)))
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':144,'uv_channels':len(obj.data.uv_layers),
        'non_manifold_edges':0,'collision_hulls':3,'materials':list(MATERIALS),
        'stone_centers_cm':[normalized((cx,cy,0)) for cx,cy,rx,ry,h in specs]}}
    for collider in colliders:
        collider.select_set(True)
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'rock_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(700,-850,550))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,90))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 680
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardRocksPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'rock_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardRockCluster.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
