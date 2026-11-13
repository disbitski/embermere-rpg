"""Build a carved stone marker with exact greybox silhouette and body-box parity."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

ROOT = Path('/Users/wizard/Documents/Unreal Game')
OUT = ROOT/'ArtSource/Blender/Environment/MarshwardWaymarker'
NAME = 'SM_MarshwardWaymarker_01'
MATERIAL = 'M_MarshwardGreyboxMarker'


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    assert not (OUT/'MarshwardWaymarker.blend').exists() or args.get('revise_preview',False), 'Refusing source overwrite'
    previous = bpy.data.scenes.get('MarshwardWaymarker')
    scene = bpy.data.scenes.new('MarshwardWaymarker')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardWaymarker'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    material = bpy.data.materials.get(MATERIAL)
    if not material:
        source = ROOT/'ArtSource/Blender/Environment/MarshwardGreybox/MarshwardGreyboxKit.blend'
        with bpy.data.libraries.load(str(source),link=False) as (available,loaded):
            assert MATERIAL in available.materials
            loaded.materials = [MATERIAL]
        material = loaded.materials[0]
    assert material
    parts = []

    def box(name,size,center,bevel=0):
        bpy.ops.mesh.primitive_cube_add(size=1,location=center)
        obj = bpy.context.object
        obj.name = name
        obj.dimensions = size
        bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
        if bevel:
            modifier = obj.modifiers.new('StoneChamfer','BEVEL')
            modifier.width,modifier.segments = bevel,1
            bpy.ops.object.modifier_apply(modifier=modifier.name)
        return obj

    for name,size,center,bevel in [
        ('Body',(120,100,412),(0,0,234),6),
        ('Foot',(120,100,28),(0,0,14),5),
        ('Collar',(136,116,24),(0,0,428),3),
        ('Cap',(180,140,80),(0,0,480),10),
        ('FrontPanel',(78,6,130),(0,-51,310),2),
        ('BackPanel',(78,6,130),(0,51,310),2),
    ]:
        parts.append(box(name,size,center,bevel))
    for side in (-1,1):
        points = [(0,side*54,360),(25,side*54,310),(0,side*54,260),(-25,side*54,310)]
        points += [(x,side*62,z) for x,y,z in points]
        mesh = bpy.data.meshes.new('DiamondRelief')
        mesh.from_pydata(points,[],[(0,3,2,1),(4,5,6,7),
            (0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)])
        mesh.update()
        obj = bpy.data.objects.new('DiamondRelief',mesh)
        scene.collection.objects.link(obj)
        bpy.context.view_layer.objects.active = obj
        modifier = obj.modifiers.new('ReliefChamfer','BEVEL')
        modifier.width,modifier.segments = 2,1
        bpy.ops.object.modifier_apply(modifier=modifier.name)
        parts.append(obj)
    bpy.ops.object.select_all(action='DESELECT')
    for part in parts:
        part.data.materials.append(material)
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    obj = bpy.context.object
    obj.name = NAME
    bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
    bad = sum(not edge.is_manifold for edge in bm.edges)
    assert bad==0
    bm.to_mesh(obj.data)
    bm.free()
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.context.view_layer.update()
    count = len(obj.data.polygons)
    lo = [min(v.co[i] for v in obj.data.vertices) for i in range(3)]
    hi = [max(v.co[i] for v in obj.data.vertices) for i in range(3)]
    assert count<=400 and len(obj.data.uv_layers)==1
    assert lo==[-90,-70,0] and hi==[90,70,520]
    assert tuple(obj.location)==(0,0,0) and tuple(obj.scale)==(1,1,1)
    collider = box('UBX_'+NAME+'_00',(120,100,440),(0,0,220))
    collider.hide_render = True
    collider.display_type = 'WIRE'
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':count,
        'uv_channels':1,'non_manifold_edges':bad,'bounds_min_cm':lo,'bounds_max_cm':hi,
        'pivot_cm':list(obj.location),'materials':[MATERIAL],
        'collision_boxes':[{'dimensions_cm':[120,100,440],'center_cm':[0,0,220],'rotation_deg':[0,0,0]}]}}
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    collider.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'waymarker_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(850,-1050,700))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,260))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 800
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardWaymarkerPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1000,1000
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'waymarker_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardWaymarker.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
