"""Build an asymmetric bank variant without rebuilding the accepted kit."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardPeatShelf_01'
MATERIALS = {
    'M_MarshwardPeat': (.13,.16,.11,1),
    'M_MarshwardMoss': (.23,.34,.12,1),
    'M_MarshwardEarth': (.28,.22,.13,1),
}


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardPeatShelf')
    scene = bpy.data.scenes.new('MarshwardPeatShelf_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardPeatShelf'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    vertices,faces,slots = [],[],[]
    stations = [(-1300,150,30),(-950,330,120),(-500,450,205),
                (-100,420,240),(420,370,210),(930,260,110),(1300,110,25)]
    for x,width,height in stations:
        vertices += [(x,-width,0),(x,-.65*width,.65*height),
                     (x,-.09*width,height),(x,.65*width,.55*height),(x,width,0)]
    for station in range(6):
        for edge in range(5):
            nxt = (edge+1)%5
            faces.append((station*5+edge,(station+1)*5+edge,(station+1)*5+nxt,station*5+nxt))
            slots.append(0 if edge in (0,4) else (2 if (station+edge)%4==0 else 1))
    faces += [tuple(reversed(range(5))),tuple(range(30,35))]
    slots += [0,0]
    mesh = bpy.data.meshes.new(NAME+'_Mesh')
    mesh.from_pydata(vertices,[],faces)
    mesh.update()
    obj = bpy.data.objects.new(NAME,mesh)
    scene.collection.objects.link(obj)
    for name,color in MATERIALS.items():
        mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
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
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':len(mesh.polygons),
        'uv_channels':len(mesh.uv_layers),'non_manifold_edges':0,'materials':list(MATERIALS)}}
    assert metrics[NAME]['dimensions_cm']==[2600,900,240]
    assert metrics[NAME]['triangles']==66
    # A separately authored convex envelope keeps the terrain collision simple.
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
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,
        object_types={'MESH'},global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',
        axis_forward='-Y',axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'shelf_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(2100,-2600,1700))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,100))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 3100
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1800))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardShelfPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'shelf_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardPeatShelf.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
