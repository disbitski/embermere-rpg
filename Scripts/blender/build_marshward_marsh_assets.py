"""Build original peat terrain and closed-geometry reeds through Blender MCP."""

import json
import math
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
COLORS = {
    'M_MarshwardPeat': (0.13,0.16,0.11,1),
    'M_MarshwardMoss': (0.23,0.34,0.12,1),
    'M_MarshwardEarth': (0.28,0.22,0.13,1),
    'M_MarshwardReedGreen': (0.30,0.43,0.16,1),
    'M_MarshwardReedSeed': (0.26,0.13,0.07,1),
}


def select(objects):
    bpy.ops.object.select_all(action='DESELECT')
    for obj in objects:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = objects[0]


def normalize(obj, target):
    bpy.context.view_layer.update()
    coords = [vertex.co for vertex in obj.data.vertices]
    lo = [min(v[i] for v in coords) for i in range(3)]
    hi = [max(v[i] for v in coords) for i in range(3)]
    center = [(lo[0]+hi[0])/2, (lo[1]+hi[1])/2, lo[2]]
    for vertex in obj.data.vertices:
        vertex.co = Vector([(vertex.co[i]-center[i])*target[i]/(hi[i]-lo[i]) for i in range(3)])
    obj.data.update()
    bpy.context.view_layer.update()


def finish(obj):
    select([obj])
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    bad = sum(not e.is_manifold for e in bm.edges)
    assert bad == 0
    bm.to_mesh(obj.data)
    bm.free()
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=0.02)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate', 'TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    return {'dimensions_cm':list(obj.dimensions), 'triangles':len(obj.data.polygons),
            'uv_channels':len(obj.data.uv_layers), 'non_manifold_edges':bad,
            'materials':[mat.name for mat in obj.data.materials]}


def terrain(materials):
    vertices = []
    for ring,(radius,z) in enumerate([(1,0),(.72,90),(.30,155)]):
        for i in range(16):
            theta = math.tau*i/16
            variation = 1 + .06*math.sin(3*theta) + .025*math.cos(5*theta)
            vertices.append((900*radius*variation*math.cos(theta),500*radius*math.sin(theta),z))
    vertices += [(0,0,180),(0,0,0)]
    faces, slots = [], []
    for ring in range(2):
        for i in range(16):
            j = (i+1)%16
            faces += [(ring*16+i,ring*16+j,(ring+1)*16+j,(ring+1)*16+i)]
            slots += [0 if ring==0 else (2 if i%4==0 else 1)]
    for i in range(16):
        faces += [(32+i,32+(i+1)%16,48),(i,49,(i+1)%16)]
        slots += [1 if i%3 else 2,0]
    mesh = bpy.data.meshes.new('PeatBerm_Mesh')
    mesh.from_pydata(vertices,[],faces)
    mesh.update()
    obj = bpy.data.objects.new('SM_MarshwardPeatBerm_01',mesh)
    bpy.context.scene.collection.objects.link(obj)
    for name in ['M_MarshwardPeat','M_MarshwardMoss','M_MarshwardEarth']:
        mesh.materials.append(materials[name])
    for polygon,index in zip(mesh.polygons,slots):
        polygon.material_index = index
    normalize(obj,(1800,1000,180))
    return obj


def reeds(materials):
    parts = []
    for i,(x,y) in enumerate([(-75,-30),(-45,45),(0,0),(55,-25),(70,45),(10,70),(-15,-65)]):
        h = 140 + (i*17)%65
        bpy.ops.mesh.primitive_cone_add(vertices=6,radius1=3,radius2=2,depth=h,location=(x,y,h/2))
        stem = bpy.context.object
        stem.data.materials.append(materials['M_MarshwardReedGreen'])
        parts.append(stem)
        bpy.ops.mesh.primitive_cone_add(vertices=6,radius1=6,radius2=4,depth=28,location=(x,y,h+8))
        seed = bpy.context.object
        seed.data.materials.append(materials['M_MarshwardReedSeed'])
        parts.append(seed)
        for k in range(2):
            theta = i*2.4+k*2.8
            bpy.ops.mesh.primitive_uv_sphere_add(segments=6,ring_count=3,radius=1,
                location=(x+20*math.cos(theta),y+20*math.sin(theta),55+k*35))
            leaf = bpy.context.object
            leaf.scale = (3,12,50)
            leaf.rotation_euler = (.55,0,theta)
            leaf.data.materials.append(materials['M_MarshwardReedGreen'])
            parts.append(leaf)
    select(parts)
    bpy.ops.object.join()
    obj = bpy.context.object
    obj.name = 'SM_MarshwardReedClump_01'
    bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    normalize(obj,(260,220,210))
    return obj


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardMarshAssets')
    scene = bpy.data.scenes.new('MarshwardMarshAssets_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj,do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardMarshAssets'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01
    materials = {}
    for name,color in COLORS.items():
        mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
        mat.use_nodes = True
        node = mat.node_tree.nodes.get('Principled BSDF')
        node.inputs['Base Color'].default_value = color
        node.inputs['Roughness'].default_value = .9
        materials[name] = mat
    bank, clump = terrain(materials), reeds(materials)
    metrics = {}
    for obj in [bank,clump]:
        metrics[obj.name] = finish(obj)
        objects = [obj]
        if obj == bank:
            collider = obj.copy()
            collider.data = obj.data.copy()
            collider.name = 'UCX_' + obj.name + '_00'
            scene.collection.objects.link(collider)
            collider.hide_render = True
            collider.display_type = 'WIRE'
            objects.append(collider)
        select(objects)
        bpy.ops.export_scene.fbx(filepath=str(OUT/(obj.name+'.fbx')),use_selection=True,
            object_types={'MESH'},global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',
            axis_forward='-Y',axis_up='Z',bake_anim=False,use_mesh_modifiers=True)
    (OUT/'mesh_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    clump.location = (0,-150,180)
    bpy.ops.object.camera_add(location=(1900,-2400,1500))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0,0,80))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 2300
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN',location=(0,-500,1500))
    bpy.context.object.rotation_euler = (.5,-.4,-.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardMarshPreview')
    scene.world.color = (.25,.25,.25)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x,scene.render.resolution_y = 1200,800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT/'terrain_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarshwardMarshAssets.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
