"""Build a small original greybox kit in a separate Blender scene via MCP."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector


ROOT = Path('/Users/wizard/Documents/Unreal Game')
OUT = ROOT / 'ArtSource/Blender/Environment/MarshwardGreybox'


def box(name, size, center, material=None):
    bpy.ops.mesh.primitive_cube_add(size=1, location=center)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = size
    bpy.ops.object.transform_apply(location=True, rotation=False, scale=True)
    if material:
        obj.data.materials.append(material)
    return obj


def wedge(name, material=None):
    mesh = bpy.data.meshes.new(name + '_Mesh')
    mesh.from_pydata([
        (-300,-250,-40), (300,-250,-40), (300,250,-40), (-300,250,-40),
        (-300,-250,0), (300,-250,120), (300,250,120), (-300,250,0),
    ], [], [(0,3,2,1), (4,5,6,7), (0,1,5,4), (1,2,6,5), (2,3,7,6), (3,0,4,7)])
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.scene.collection.objects.link(obj)
    if material:
        mesh.materials.append(material)
    return obj


def select(objects):
    bpy.ops.object.select_all(action='DESELECT')
    for obj in objects:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = objects[0]


def finish(obj):
    select([obj])
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=0.03)
    bpy.ops.object.mode_set(mode='OBJECT')
    mod = obj.modifiers.new('Triangulate', 'TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=mod.name)
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bad = sum(not edge.is_manifold for edge in bm.edges)
    bm.free()
    assert bad == 0
    return {'triangles': len(obj.data.polygons), 'uv_channels': len(obj.data.uv_layers),
            'non_manifold_edges': bad, 'dimensions_cm': list(obj.dimensions)}


def material(name, color):
    mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
    mat.diffuse_color = color
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get('Principled BSDF')
    bsdf.inputs['Base Color'].default_value = color
    bsdf.inputs['Roughness'].default_value = 0.88
    return mat


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    previous = bpy.data.scenes.get('MarshwardGreyboxKit')
    scene = bpy.data.scenes.new('MarshwardGreyboxKit_Build')
    bpy.context.window.scene = scene
    if previous:
        for obj in list(previous.objects):
            bpy.data.objects.remove(obj, do_unlink=True)
        bpy.data.scenes.remove(previous)
    scene.name = 'MarshwardGreyboxKit'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01
    stone = material('M_MarshwardGreyboxStone', (0.36,0.43,0.46,1))
    marker_mat = material('M_MarshwardGreyboxMarker', (0.67,0.40,0.14,1))
    span_name = 'SM_MarshwardSpan_Greybox'
    span = box(span_name, (600,500,60), (0,0,-30), stone)
    span_collision = box('UBX_' + span_name + '_00', (600,500,60), (0,0,-30))
    ramp_name = 'SM_MarshwardRamp_Greybox'
    ramp = wedge(ramp_name, stone)
    ramp_collision = wedge('UCX_' + ramp_name + '_00')
    marker_name = 'SM_MarshwardWaymarker_Greybox'
    parts = [box('Body', (120,100,440), (0,0,220), marker_mat),
             box('Cap', (180,140,80), (0,0,480), marker_mat),
             box('Inset', (70,12,80), (0,-56,340), marker_mat)]
    select(parts)
    bpy.ops.object.join()
    marker = bpy.context.object
    marker.name = marker_name
    marker_collision = box('UBX_' + marker_name + '_00', (120,100,440), (0,0,220))
    metrics = {}
    for obj, collider in [(span, span_collision), (ramp,ramp_collision), (marker,marker_collision)]:
        metrics[obj.name] = finish(obj)
        collider.hide_render = True
        collider.display_type = 'WIRE'
        select([obj,collider])
        bpy.ops.export_scene.fbx(filepath=str(OUT / (obj.name + '.fbx')),
            use_selection=True, object_types={'MESH'}, global_scale=1,
            apply_unit_scale=True, apply_scale_options='FBX_SCALE_NONE',
            axis_forward='-Y', axis_up='Z', bake_anim=False, use_mesh_modifiers=True)
    (OUT / 'kit_metrics.json').write_text(json.dumps(metrics, indent=2) + '\n')
    # Display offsets belong to the preview scene only; exports above use local pivots.
    span.location = (-420,0,80)
    ramp.location = (300,0,80)
    marker.location = (850,0,0)
    bpy.ops.object.camera_add(location=(1450,-1850,1350))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((200,0,150))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 2200
    camera.data.clip_end = 10000
    scene.camera = camera
    bpy.ops.object.light_add(type='SUN', location=(0,-500,1500))
    bpy.context.object.rotation_euler = (0.5,-0.4,-0.4)
    bpy.context.object.data.energy = 3
    scene.world = bpy.data.worlds.new('MarshwardKitWorld')
    scene.world.color = (0.3,0.3,0.3)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x = 1200
    scene.render.resolution_y = 800
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT / 'kit_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT / 'MarshwardGreyboxKit.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
