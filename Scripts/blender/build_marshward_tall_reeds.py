"""Build a sparse three-stem variant without modifying accepted reed assets."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardTallReeds_01'
MATERIALS = ['M_MarshwardReedGreen','M_MarshwardReedSeed']
STEMS = [(-65,-20,245),(55,35,270),(5,-55,225)]


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardTallReeds')
    scene = bpy.data.scenes.new('MarshwardTallReeds_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardTallReeds'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    mats = [bpy.data.materials.get(name) for name in MATERIALS]
    assert all(mats), 'Reuse accepted reed materials; do not recreate them'
    parts = []
    for i,(x,y,height) in enumerate(STEMS):
        bpy.ops.mesh.primitive_cone_add(vertices=6,radius1=2.5,radius2=1.6,
            depth=height,location=(x,y,height/2))
        stem = bpy.context.object
        stem.data.materials.append(mats[0])
        parts.append(stem)
        bpy.ops.mesh.primitive_cone_add(vertices=6,radius1=5,radius2=3.5,
            depth=34,location=(x,y,height+10))
        seed = bpy.context.object
        seed.data.materials.append(mats[1])
        parts.append(seed)
        for k in range(2):
            theta = i*2.4+k*2.8
            bpy.ops.mesh.primitive_uv_sphere_add(segments=6,ring_count=3,radius=1,
                location=(x+25*math.cos(theta),y+25*math.sin(theta),95+k*55))
            leaf = bpy.context.object
            leaf.scale = (2.5,9,60)
            leaf.rotation_euler = (.55,0,theta)
            leaf.data.materials.append(mats[0])
            parts.append(leaf)
    bpy.ops.object.select_all(action='DESELECT')
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    obj = bpy.context.object
    obj.name = NAME
    bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    coords = [v.co for v in obj.data.vertices]
    lo = [min(v[i] for v in coords) for i in range(3)]
    hi = [max(v[i] for v in coords) for i in range(3)]
    center = [(lo[0]+hi[0])/2,(lo[1]+hi[1])/2,lo[2]]
    factors = [size/(hi[i]-lo[i]) for i,size in enumerate((220,180,300))]

    def normalized(point):
        return [(point[i]-center[i])*factors[i] for i in range(3)]

    for vertex in obj.data.vertices:
        vertex.co = Vector(normalized(vertex.co))
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    assert bad_edges==0
    bm.to_mesh(obj.data)
    bm.free()
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.context.view_layer.update()
    assert len(obj.data.polygons)==264 and len(obj.data.uv_layers)==1
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,(220,180,300)))
    assert tuple(obj.location)==(0,0,0) and tuple(obj.scale)==(1,1,1)
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':264,
        'uv_channels':1,'non_manifold_edges':bad_edges,'collision_hulls':0,
        'materials':MATERIALS,'stem_count':3,'leaf_count':6,
        'base_samples_cm':[normalized((x,y,0)) for x,y,h in STEMS]}}
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'tall_reeds_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(500,-650,420))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,140))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 520
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardTallReedsPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'tall_reeds_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardTallReeds.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
