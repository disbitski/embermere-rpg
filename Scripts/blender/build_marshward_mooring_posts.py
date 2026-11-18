"""Build an independent, visual-only pair of weathered mooring posts."""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector

OUT = Path('/Users/wizard/Documents/Unreal Game/ArtSource/Blender/Environment/MarshwardMarsh')
NAME = 'SM_MarshwardMooringPosts_01'
SIZE = (280, 190, 235)
MATERIALS = {'M_EmbermereTimber': (.24, .105, .035, 1),
             'M_MarshwardMoss': (.23, .34, .12, 1)}
RIM = [(-.5, -.32), (-.32, -.5), (.3, -.5), (.5, -.3),
       (.5, .32), (.29, .5), (-.32, .5), (-.5, .29)]
POSTS = [(-77, -48, 52, 49, 235, -13, 5),
         (78, 49, 43, 56, 145, 16, -9)]


def main():
    scene = bpy.context.scene
    assert bpy.app.background, 'Run with the isolated headless Blender transport'
    assert {obj.name for obj in scene.objects} <= {'Cube', 'Camera', 'Light'}, 'Use factory startup'
    for obj in list(scene.objects):
        bpy.data.objects.remove(obj, do_unlink=True)
    scene.name = 'MarshwardMooringPosts'
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = .01

    vertices, faces, moss_faces = [], [], set()
    for cx, cy, width, depth, height, lean_x, lean_y in POSTS:
        offset = len(vertices)
        chips = (0, 5, 13, 19, 16, 8, 3, 2)
        for fraction, taper in [(0, 1), (.18, 1), (.86, .93), (1, .86)]:
            for index, (x, y) in enumerate(RIM):
                z = height * fraction - (chips[index] if fraction == 1 else 0)
                vertices.append((cx+x*width*taper+fraction*lean_x,
                                 cy+y*depth*taper+fraction*lean_y, z))
        for ring in range(3):
            for index in range(8):
                base = offset + ring*8
                following = (index+1)%8
                if ring == 0 and index in (1, 2, 3, 6):
                    moss_faces.add(len(faces))
                faces.append((base+index, base+following, base+8+following, base+8+index))
        faces.extend((tuple(offset+i for i in reversed(range(8))),
                      tuple(offset+24+i for i in range(8))))

    low, high = [tuple(fn(axis) for axis in zip(*vertices)) for fn in (min, max)]
    center = ((low[0]+high[0])/2, (low[1]+high[1])/2, low[2])
    factors = [target/(upper-lower) for target, lower, upper in zip(SIZE, low, high)]

    def normalize(point):
        return [(value-origin)*factor for value, origin, factor in zip(point, center, factors)]

    mesh = bpy.data.meshes.new(NAME)
    mesh.from_pydata([normalize(point) for point in vertices], [], faces)
    mesh.update()
    obj = bpy.data.objects.new(NAME, mesh)
    scene.collection.objects.link(obj)
    for name, color in MATERIALS.items():
        material = bpy.data.materials.new(name)
        material.diffuse_color = color
        mesh.materials.append(material)
    for index in moss_faces:
        mesh.polygons[index].material_index = 1
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(island_margin=.025)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Triangulate', 'TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=modifier.name)

    bm = bmesh.new()
    bm.from_mesh(mesh)
    bad_edges = sum(not edge.is_manifold for edge in bm.edges)
    remaining = set(bm.verts)
    pieces = 0
    while remaining:
        stack = [remaining.pop()]
        pieces += 1
        while stack:
            for edge in stack.pop().link_edges:
                for vertex in edge.verts:
                    if vertex in remaining:
                        remaining.remove(vertex)
                        stack.append(vertex)
    bm.free()
    assert bad_edges == 0 and pieces == 2 and len(mesh.polygons) == 120
    assert len(mesh.uv_layers) == 1 and tuple(obj.location) == (0, 0, 0)
    assert all(abs(actual-expected) < .001 for actual, expected in zip(obj.dimensions, SIZE))
    metrics = {NAME: {'dimensions_cm': list(obj.dimensions), 'triangles': len(mesh.polygons),
                      'uv_channels': len(mesh.uv_layers), 'non_manifold_edges': bad_edges,
                      'closed_pieces': pieces, 'collision_hulls': 0,
                      'materials': list(MATERIALS), 'pivot_cm': list(obj.location),
                      'base_samples_cm': [normalize((post[0], post[1], 0)) for post in POSTS]}}
    bpy.ops.export_scene.fbx(filepath=str(OUT / (NAME+'.fbx')), use_selection=True,
        object_types={'MESH'}, global_scale=1, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_NONE', axis_forward='-Y', axis_up='Z',
        bake_anim=False, use_mesh_modifiers=True)
    (OUT / 'mooring_posts_metrics.json').write_text(json.dumps(metrics, indent=2)+'\n')

    bpy.ops.object.camera_add(location=(420, -600, 300))
    camera = bpy.context.object
    camera.rotation_euler = (Vector((0, 0, 100))-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.type = 'ORTHO'
    camera.data.ortho_scale = 420
    camera.data.clip_end = 10000
    scene.camera = camera
    scene.render.engine = 'BLENDER_WORKBENCH'
    scene.display.shading.light = 'STUDIO'
    scene.display.shading.color_type = 'MATERIAL'
    scene.display.shading.show_cavity = True
    scene.render.resolution_x, scene.render.resolution_y = 960, 720
    scene.render.resolution_percentage = 100
    scene.render.filepath = str(OUT / 'mooring_posts_preview.png')
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT / 'MarshwardMooringPosts.blend'))
    bpy.ops.render.render(write_still=True)
    return metrics


__result__ = main()
