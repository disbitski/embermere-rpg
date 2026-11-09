"""Build one original three-block foundation course in its own Blender scene."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardFoundation_01'
MATERIALS = ['M_MarshwardMarshStone', 'M_MarshwardMoss']
SPECS = [(-300,-108,0,130,150), (-98,96,-12,110,128), (106,300,4,118,140)]


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardFoundation')
    scene = bpy.data.scenes.new('MarshwardFoundation_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardFoundation'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    mats = [bpy.data.materials.get(name) for name in MATERIALS]
    assert all(mats), 'Open the accepted marsh source before building; do not recreate shared materials'
    parts,colliders = [],[]
    for index,(left,right,cy,depth,height) in enumerate(SPECS):
        vertices,faces,slots = [],[],[]
        for inset,z in [(6,0),(0,12),(0,height-16),(14,height)]:
            x0,x1 = left+inset,right-inset
            y0,y1 = cy-depth+inset,cy+depth-inset
            cut = 20+index*3
            vertices.extend([(x0+cut,y0,z),(x1-cut,y0,z),(x1,y0+cut,z),
                             (x1,y1-cut,z),(x1-cut,y1,z),(x0+cut,y1,z),
                             (x0,y1-cut,z),(x0,y0+cut,z)])
        for ring in range(3):
            for i in range(8):
                j = (i+1)%8
                faces.append((ring*8+i,ring*8+j,(ring+1)*8+j,(ring+1)*8+i))
                slots.append(1 if ring==2 and (i+index)%4==0 else 0)
        faces.extend([tuple(reversed(range(8))),tuple(range(24,32))])
        slots.extend([0,1 if index!=1 else 0])
        mesh = bpy.data.meshes.new('FoundationBlock_'+str(index))
        mesh.from_pydata(vertices,[],faces)
        mesh.update()
        obj = bpy.data.objects.new('FoundationBlock_'+str(index),mesh)
        scene.collection.objects.link(obj)
        for mat in mats:
            mesh.materials.append(mat)
        for poly,slot in zip(mesh.polygons,slots):
            poly.material_index = slot
        bm = bmesh.new()
        bm.from_mesh(mesh)
        bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
        assert all(edge.is_manifold for edge in bm.edges)
        bm.to_mesh(mesh)
        bm.free()
        hull = bmesh.new()
        for vertex in mesh.vertices:
            hull.verts.new(vertex.co)
        bmesh.ops.convex_hull(hull,input=list(hull.verts),use_existing_faces=False)
        hull_mesh = bpy.data.meshes.new(NAME+'_Hull_'+str(index))
        hull.to_mesh(hull_mesh)
        hull.free()
        collider = bpy.data.objects.new('UCX_'+NAME+'_'+str(index).zfill(2),hull_mesh)
        scene.collection.objects.link(collider)
        collider.hide_render = True
        collider.display_type = 'WIRE'
        parts.append(obj)
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
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.context.view_layer.update()
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    bm.free()
    assert bad_edges==0 and len(obj.data.polygons)==180
    assert len(obj.data.uv_layers)==1
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,(600,260,150)))
    assert tuple(obj.location)==(0,0,0) and tuple(obj.scale)==(1,1,1)
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':180,
        'uv_channels':1,'non_manifold_edges':bad_edges,'collision_hulls':3,
        'materials':MATERIALS,'block_centers_cm':[[(left+right)/2,cy,0] for left,right,cy,depth,h in SPECS],
        'block_heights_cm':[h for left,right,cy,depth,h in SPECS],
        'ground_samples_cm':[[(left+right)/2,cy-depth-20,0] for left,right,cy,depth,h in SPECS]}}
    for collider in colliders:
        collider.select_set(True)
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'foundation_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(850,-1050,680))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,65))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 850
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardFoundationPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'foundation_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardFoundation.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
