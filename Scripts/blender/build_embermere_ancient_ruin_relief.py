"""Build the decorative carved relief for the Marsh Prowler ruin wall."""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereAncientRuinRelief_01"


def output_paths():
    supplied = globals().get("args", {})
    root = Path(supplied.get("project_root", "/Users/wizard/Documents/Unreal Game"))
    directory = Path(supplied.get("output_dir", root / "ArtSource/Blender/Environment/AncientRuinRelief"))
    directory.mkdir(parents=True, exist_ok=True)
    return tuple(directory / (ASSET_NAME + suffix) for suffix in (".blend", ".fbx", "_preview.png", "_metrics.json"))


def reset_scene():
    if bpy.context.object and bpy.context.object.mode != "OBJECT":
        bpy.ops.object.mode_set(mode="OBJECT")
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)


def material(name, color, roughness=0.9, emission=None):
    mat = bpy.data.materials.new(name=name)
    mat.use_nodes = True
    node = mat.node_tree.nodes.get("Principled BSDF")
    node.inputs["Base Color"].default_value = color
    node.inputs["Roughness"].default_value = roughness
    if emission:
        emit = node.inputs.get("Emission Color") or node.inputs.get("Emission")
        if emit:
            emit.default_value = emission
        strength = node.inputs.get("Emission Strength")
        if strength:
            strength.default_value = 1.25
    return mat


def cube(name, dimensions, location, mat, bevel=0.0, rotation=0.0):
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    obj.rotation_euler.y = math.radians(rotation)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    if bevel:
        modifier = obj.modifiers.new("WeatheredEdge", "BEVEL")
        modifier.width = bevel
        modifier.segments = 2
        modifier.limit_method = "ANGLE"
        bpy.ops.object.modifier_apply(modifier=modifier.name)
    return obj


def shard(name, location, scale, mat):
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=1, radius=1.0, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.scale = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    return obj


def build():
    stone = material("M_Waystone", (0.34, 0.40, 0.37, 1.0))
    moss = material("M_WaystoneMoss", (0.25, 0.39, 0.16, 1.0))
    iron = material("M_EmberLampIron", (0.055, 0.07, 0.075, 1.0), 0.62)
    ember = material("M_WaystoneEmber", (0.58, 0.12, 0.025, 1.0), 0.5, (0.4, 0.06, 0.01, 1.0))
    parts = [
        cube("CarvedBackstone", (202, 22, 216), (0, 0, 108), stone, 5),
        cube("BrokenStoneCrown", (206, 30, 13), (0, 0, 209), stone, 3),
        cube("BottomStoneLip", (202, 28, 12), (0, 0, 8), stone, 2),
    ]
    for side, x in (("Left", -87), ("Right", 87)):
        parts += [
            cube(side + "BrokenPier", (22, 28, 171), (x, 0, 101), stone, 4),
            cube(side + "MossCrown", (29, 33, 19), (x, 0, 191), moss, 3),
            cube(side + "Binding", (25, 30, 8), (x, 0, 74), iron, 1),
        ]

    # Carving appears on both shallow faces so classic FBX's local-Y mirror
    # cannot turn the relief away from the road-facing side of the wall.
    for face, y, sign in (("Front", -14.5, -1), ("Back", 14.5, 1)):
        for side, x in (("Left", -62), ("Right", 62)):
            parts.append(cube(face + side + "CarvedBorder", (8, 8, 128), (x, y, 112), moss, 1.5))
        parts += [
            cube(face + "LowerRune", (96, 8, 9), (0, y, 50), moss, 1.5),
            cube(face + "UpperRune", (84, 8, 9), (0, y, 160), moss, 1.5),
            shard(face + "WornStoneShard", (0, y + sign * 4, 109), (25, 8, 47), stone),
            shard(face + "EmberCore", (0, y + sign * 10, 114), (9, 4, 19), ember),
        ]
        for index, (x, z, tilt) in enumerate(((-38, 78, -25), (-38, 136, 25), (38, 78, 25), (38, 136, -25))):
            parts.append(cube(face + f"Fracture_{index}", (28, 7, 8), (x, y, z), moss, 1, tilt))
        for index, x in enumerate((-46, 0, 46)):
            parts.append(shard(face + f"MossFleck_{index}", (x, y, 189), (9, 4, 5), moss))

    bpy.ops.object.select_all(action="DESELECT")
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    result = bpy.context.object
    result.name = ASSET_NAME
    result.data.name = ASSET_NAME + "_Mesh"
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=0.025)
    bpy.ops.object.mode_set(mode="OBJECT")
    triangulate = result.modifiers.new("ExportTriangles", "TRIANGULATE")
    bpy.ops.object.modifier_apply(modifier=triangulate.name)
    return result


def metrics(obj):
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    non_manifold = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    return {
        "dimensions_cm": [round(value, 3) for value in obj.dimensions],
        "triangles": len(obj.data.polygons),
        "materials": [slot.material.name for slot in obj.material_slots if slot.material],
        "uv_channels": len(obj.data.uv_layers),
        "non_manifold_edges": non_manifold,
        "scale": [round(value, 6) for value in obj.scale],
        "origin_z_cm": round(obj.location.z, 6),
        "authored_collision_boxes": 0,
    }


def validate(result):
    if result["dimensions_cm"] != [206.0, 55.805, 216.0]:
        raise RuntimeError(f"unexpected relief dimensions: {result['dimensions_cm']}")
    if not (500 <= result["triangles"] <= 3500):
        raise RuntimeError(f"unexpected relief triangle count: {result['triangles']}")
    if result["uv_channels"] != 1 or result["non_manifold_edges"] != 0:
        raise RuntimeError(f"invalid relief topology: {result}")
    if result["scale"] != [1.0, 1.0, 1.0] or result["origin_z_cm"] != 0.0:
        raise RuntimeError("relief pivot or applied transform drifted")
    expected = {"M_Waystone", "M_WaystoneMoss", "M_EmberLampIron", "M_WaystoneEmber"}
    if set(result["materials"]) != expected:
        raise RuntimeError(f"relief material contract drifted: {result['materials']}")


def look_at(obj, point):
    obj.rotation_euler = (Vector(point) - obj.location).to_track_quat("-Z", "Y").to_euler()


def preview(obj, path):
    bpy.ops.mesh.primitive_plane_add(size=450, location=(0, 18, 110), rotation=(math.radians(90), 0, 0))
    wall = bpy.context.object
    wall.name = "Preview_RuinWall"
    wall.data.materials.append(material("M_PreviewWall", (0.38, 0.36, 0.3, 1), 1))
    bpy.ops.object.light_add(type="SUN", rotation=(math.radians(30), math.radians(-20), math.radians(-30)))
    bpy.context.object.data.energy = 2.5
    bpy.ops.object.light_add(type="AREA", location=(190, -250, 290))
    bpy.context.object.data.energy = 1300000
    bpy.context.object.data.size = 230
    look_at(bpy.context.object, (0, 0, 115))
    bpy.ops.object.camera_add(location=(255, -395, 220))
    camera = bpy.context.object
    camera.data.lens = 55
    look_at(camera, (0, 0, 110))
    bpy.context.scene.camera = camera
    scene = bpy.context.scene
    scene.render.resolution_x = 900
    scene.render.resolution_y = 900
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    scene.render.filepath = str(path)
    bpy.ops.render.render(write_still=True)


def main():
    blend_path, fbx_path, preview_path, metrics_path = output_paths()
    reset_scene()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 0.01
    scene.unit_settings.length_unit = "CENTIMETERS"
    obj = build()
    result = metrics(obj)
    validate(result)
    metrics_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(filepath=str(fbx_path), use_selection=True, object_types={"MESH"}, global_scale=1.0, apply_unit_scale=True, axis_forward="-Y", axis_up="Z", mesh_smooth_type="FACE", bake_anim=False)
    preview(obj, preview_path)
    bpy.ops.wm.save_as_mainfile(filepath=str(blend_path))
    return {"asset": ASSET_NAME, "blend": str(blend_path), "fbx": str(fbx_path), "preview": str(preview_path), "metrics_file": str(metrics_path), "metrics": result}


__result__ = main()
