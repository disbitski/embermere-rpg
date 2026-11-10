"""Build two grounded fractured beam fragments in a separate owned scene."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardFallenMasonry_01'
MATERIALS = ['M_MarshwardMarshStone','M_MarshwardMoss']
SPECS = [(-90,-55,-18,130,[(-152,-70),(120,-70),(170,-42),(136,40),
                         (112,70),(-140,70),(-170,45),(-170,-45)]),
         (190,95,32,86,[(-60,-72),(50,-72),(75,-48),(75,50),
                       (48,72),(-48,72),(-75,46),(-75,-50)])]


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardFallenMasonry')
    scene = bpy.data.scenes.new('MarshwardFallenMasonry_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardFallenMasonry'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    mats = [bpy.data.materials.get(name) for name in MATERIALS]
    assert all(mats), 'Use accepted marsh materials; do not recreate them'
    parts,colliders = [],[]
    for index,(cx,cy,yaw,height,outline) in enumerate(SPECS):
        angle = math.radians(yaw)
        vertices,faces,slots = [],[],[]
        for ring,factor in enumerate((.95,1,.88)):
            for x,y in outline:
                lx,ly = x*factor,y*factor
                z = 0 if ring==0 else 12 if ring==1 else height+.07*lx-.035*ly
                vertices.append((cx+lx*math.cos(angle)-ly*math.sin(angle),
                                 cy+lx*math.sin(angle)+ly*math.cos(angle),z))
        for ring in range(2):
            for i in range(8):
                j = (i+1)%8
                faces.append((ring*8+i,ring*8+j,(ring+1)*8+j,(ring+1)*8+i))
                slots.append(1 if ring==1 and i in (3,4) else 0)
        faces.extend([tuple(reversed(range(8))),tuple(range(16,24))])
        slots.extend([0,1 if index==0 else 0])
        mesh = bpy.data.meshes.new('FallenFragment_'+str(index))
        mesh.from_pydata(vertices,[],faces)
        mesh.update()
        obj = bpy.data.objects.new('FallenFragment_'+str(index),mesh)
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
    factors = [size/(hi[i]-lo[i]) for i,size in enumerate((500,300,140))]

    def normalized(point):
        return [(point[i]-center[i])*factors[i] for i in range(3)]

    ground_samples = []
    for index,obj in enumerate(parts):
        cx,cy,yaw,height,outline = SPECS[index]
        ground_samples.append(normalized((cx,min(v.co.y for v in obj.data.vertices)-20,0)))
        for vertex in obj.data.vertices:
            vertex.co = Vector(normalized(vertex.co))
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
        assert all(edge.is_manifold for edge in bm.edges)
        bm.to_mesh(obj.data)
        bm.free()
        hull = bmesh.new()
        for vertex in obj.data.vertices:
            hull.verts.new(vertex.co)
        bmesh.ops.convex_hull(hull,input=list(hull.verts),use_existing_faces=False)
        hull_mesh = bpy.data.meshes.new(NAME+'_Hull_'+str(index))
        hull.to_mesh(hull_mesh)
        hull.free()
        collider = bpy.data.objects.new('UCX_'+NAME+'_'+str(index).zfill(2),hull_mesh)
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
    modifier = obj.modifiers.new('Triangulate','TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    bpy.context.view_layer.update()
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    bm.free()
    assert bad_edges==0 and len(obj.data.polygons)==88
    assert len(obj.data.uv_layers)==1 and tuple(obj.scale)==(1,1,1)
    assert tuple(obj.location)==(0,0,0)
    assert all(abs(a-b)<.001 for a,b in zip(obj.dimensions,(500,300,140)))
    metrics = {NAME:{'dimensions_cm':list(obj.dimensions),'triangles':88,'uv_channels':1,
        'non_manifold_edges':bad_edges,'collision_hulls':2,'materials':MATERIALS,
        'fragment_centers_cm':[normalized((cx,cy,0)) for cx,cy,yaw,h,p in SPECS],
        'fragment_heights_cm':[h*factors[2] for cx,cy,yaw,h,p in SPECS],
        'ground_samples_cm':ground_samples}}
    for collider in colliders:
        collider.select_set(True)
    bpy.ops.export_scene.fbx(filepath=str(OUT/(NAME+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        bake_anim=False,use_mesh_modifiers=True)
    (OUT/'fallen_masonry_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    bpy.ops.object.camera_add(location=(700,-850,650))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,60))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 730
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1000))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardMasonryPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'fallen_masonry_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardFallenMasonry.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
