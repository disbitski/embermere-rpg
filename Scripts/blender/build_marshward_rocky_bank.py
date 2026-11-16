"""Build one independent tapered distant ridge with one purposeful hull."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardRockyBank_01'
MATERIALS = {'M_MarshwardMarshStone':(.29,.34,.36,1),
             'M_MarshwardMoss':(.23,.34,.12,1)}
STATIONS = [(-1100,45,90),(-650,170,420),(-100,200,650),
            (400,180,540),(850,125,320),(1100,35,80)]


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardRockyBank')
    scene = bpy.data.scenes.new('MarshwardRockyBank_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardRockyBank'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    vertices,faces,slots = [],[],[]
    for x,width,height in STATIONS:
        vertices += [(x,-width,0),(x,width,0),(x,.9*width,.35*height),
                     (x,.35*width,height),(x,-.35*width,height),(x,-.9*width,.35*height)]
    for station in range(len(STATIONS)-1):
        for edge in range(6):
            nxt = (edge+1)%6
            faces.append((station*6+edge,(station+1)*6+edge,
                          (station+1)*6+nxt,station*6+nxt))
            slots.append(1 if edge==3 or (edge==2 and station%2==0) else 0)
    faces += [tuple(reversed(range(6))),tuple(range(30,36))]
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
    assert all(edge.is_manifold for edge in bm.edges)
    bm.to_mesh(mesh)
    bm.free()
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    hull = bmesh.new()
    for vertex in mesh.vertices:
        hull.verts.new(vertex.co)
    bmesh.ops.convex_hull(hull,input=list(hull.verts),use_existing_faces=False)
    hull_mesh = bpy.data.meshes.new(NAME+'_Hull')
    hull.to_mesh(hull_mesh)
    hull.free()
    collider = bpy.data.objects.new('UCX_'+NAME+'_00',hull_mesh)
    scene.collection.objects.link(collider)
    collider.hide_render = True
    collider.display_type = 'WIRE'
    collider.select_set(True)
    bpy.context.view_layer.update()
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold = sum(not edge.is_manifold for edge in bm.edges)
    bm.free()
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':len(mesh.polygons),
        'uv_channels':len(mesh.uv_layers),'non_manifold_edges':non_manifold,
        'collision_hulls':1,'materials':list(MATERIALS),'pivot_cm':list(obj.location),
        'crest_samples_cm':[[x,0,height] for x,width,height in STATIONS[1:4]],
        'ground_samples_cm':[[x,y,0] for x in (-650,-100,400) for y in (-230,230)]}}
    assert metrics[NAME]['dimensions_cm']==[2200,400,650]
    assert metrics[NAME]['triangles']==68 and non_manifold==0
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,
        object_types={'MESH'},global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',
        axis_forward='-Y',axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'rocky_bank_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(2300,-2900,1800))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,230))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 2900
    camera.data.clip_end = 20000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1800))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardRockyBankPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'rocky_bank_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardRockyBank.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
