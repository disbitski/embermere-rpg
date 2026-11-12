"""Build one low broad-leaf scrub in a separate scene using accepted materials."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardLowScrub_01'
MATERIALS = ['M_MarshwardReedGreen','M_MarshwardEarth']
BASES = [(-60,-20,0),(0,30,0),(65,-15,0)]


def main():
    assert not (OUT/'MarshwardLowScrub.blend').exists(), 'Refusing source overwrite'
    scene = bpy.data.scenes.new('MarshwardLowScrub')
    bpy.context.window.scene = scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    mats = [bpy.data.materials.get(name) for name in MATERIALS]
    assert all(mats), 'Reuse existing materials without editing them'
    parts = []

    def branch(a,b,radius):
        a,b = Vector(a),Vector(b)
        bpy.ops.mesh.primitive_cone_add(vertices=6,radius1=radius,radius2=radius*.55,
            depth=(b-a).length,location=(a+b)/2)
        obj = bpy.context.object
        obj.rotation_euler = (b-a).to_track_quat('Z','Y').to_euler()
        obj.data.materials.append(mats[1])
        parts.append(obj)

    for i,base in enumerate(BASES):
        top = (base[0]+(-12+i*12),base[1]+12,60+i*7)
        # Vertical grounded starts keep every branch base on the same plane.
        branch(base,(base[0],base[1],top[2]),3)
        for side in (-1,1):
            tip = (top[0]+side*32,top[1]+side*24,top[2]-8)
            branch((base[0],base[1],25),tip,2)
            for k in range(3):
                bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=1,radius=1,
                    location=(tip[0]+side*(k-1)*17,tip[1]+(k-1)*19,tip[2]+k*5))
                leaf = bpy.context.object
                leaf.scale = (29,15,6)
                leaf.rotation_euler = (.15*side,.1*k,i*.7+side*.5+k*.5)
                leaf.data.materials.append(mats[0])
                parts.append(leaf)
    bpy.ops.object.select_all(action='DESELECT')
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    obj = bpy.context.object
    obj.name = NAME
    bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    lo = [min(v.co[i] for v in obj.data.vertices) for i in range(3)]
    hi = [max(v.co[i] for v in obj.data.vertices) for i in range(3)]
    center = [(lo[0]+hi[0])/2,(lo[1]+hi[1])/2,lo[2]]
    factors = [size/(hi[i]-lo[i]) for i,size in enumerate((240,180,95))]

    def normalized(point):
        return [(point[i]-center[i])*factors[i] for i in range(3)]

    for vertex in obj.data.vertices:
        vertex.co = Vector(normalized(vertex.co))
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
    assert count<=600 and len(obj.data.uv_layers)==1
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,(240,180,95)))
    assert tuple(obj.location)==(0,0,0) and tuple(obj.scale)==(1,1,1)
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':count,
        'uv_channels':1,'non_manifold_edges':bad,'collision_hulls':0,
        'materials':MATERIALS,'stem_count':3,'leaf_count':18,
        'base_samples_cm':[normalized(base) for base in BASES]}}
    assert all(abs(p[2])<.001 for p in metrics[NAME]['base_samples_cm'])
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'low_scrub_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(420,-500,330))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,45))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 390
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardLowScrubPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'low_scrub_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardLowScrub.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
