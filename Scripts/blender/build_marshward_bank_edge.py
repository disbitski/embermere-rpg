"""Build only the decorative bank apron; preserve accepted source scenes."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardBankEdge_01'
MATERIALS = {'M_MarshwardPeat':(.13,.16,.11,1),
             'M_MarshwardMoss':(.23,.34,.12,1),
             'M_MarshwardEarth':(.28,.22,.13,1)}


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardBankEdge')
    scene = bpy.data.scenes.new('MarshwardBankEdge_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardBankEdge'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    vertices,faces,slots = [],[],[]
    stations = [(-500,100,15),(-300,-120,50),(0,-180,70),
                (300,-160,50),(500,60,10)]
    for x,outer,height in stations:
        vertices += [(x,outer,0),(x,outer+.2*(120-outer),.18*height),
                     (x,120,height),(x,180,.9*height),(x,180,0)]
    for station in range(4):
        for edge in range(5):
            nxt = (edge+1)%5
            faces.append((station*5+edge,(station+1)*5+edge,
                          (station+1)*5+nxt,station*5+nxt))
            slots.append(1 if edge==2 else (2 if edge==1 and station%2 else 0))
    faces += [tuple(reversed(range(5))),tuple(range(20,25))]
    slots += [0,0]
    mesh = bpy.data.meshes.new(NAME+'_Mesh')
    mesh.from_pydata(vertices,[],faces)
    mesh.update()
    obj = bpy.data.objects.new(NAME,mesh)
    scene.collection.objects.link(obj)
    for name,color in MATERIALS.items():
        mat = bpy.data.materials.get(name)
        if not mat:
            mat = bpy.data.materials.new(name)
            mat.use_nodes = True
            bsdf = mat.node_tree.nodes.get('Principled BSDF')
            bsdf.inputs['Base Color'].default_value = color
            bsdf.inputs['Roughness'].default_value = .9
        mesh.materials.append(mat)
    for poly,index in zip(mesh.polygons,slots):
        poly.material_index = index
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
    assert all(e.is_manifold for e in bm.edges)
    bm.to_mesh(mesh)
    bm.free()
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.02)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.context.view_layer.update()
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold = sum(not edge.is_manifold for edge in bm.edges)
    bm.free()
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':len(mesh.polygons),
        'uv_channels':len(mesh.uv_layers),'non_manifold_edges':non_manifold,
        'collision_hulls':0,'materials':list(MATERIALS),
        'ground_samples_cm':[(x,y,0) for x in (-250,0,250) for y in (-100,-40,0)],
        'join_samples_cm':[(x,140,0) for x in (0,250)]}}
    assert metrics[NAME]['dimensions_cm']==[1000,360,70]
    assert metrics[NAME]['triangles']==46 and non_manifold==0
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,
        object_types={'MESH'},global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',
        axis_forward='-Y',axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'bank_edge_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(1000,-1300,900))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,25))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 1300
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1800))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardBankEdgePreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'bank_edge_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardBankEdge.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
