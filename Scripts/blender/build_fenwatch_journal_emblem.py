"""Build an original journal for a 48px quest-update emblem.

Reviewed scene-resetting script: use factory-clean headless Blender MCP only.
The live bridge scene and its Safe Mode configuration are not changed.
"""

import json
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector


ROOT = Path(globals().get("args", {}).get("project_root", "/Users/wizard/Documents/Unreal Game"))
OUTPUT = ROOT / "ArtSource/Blender/UI/FenwatchJournal"
NAME = "SM_FenwatchJournalEmblem"


def material(name, color, metallic=0.0, roughness=0.65):
    mat = bpy.data.materials.new(name)
    mat.diffuse_color = (*color, 1.0)
    mat.use_nodes = True
    shader = mat.node_tree.nodes.get("Principled BSDF")
    shader.inputs["Base Color"].default_value = (*color, 1.0)
    shader.inputs["Metallic"].default_value = metallic
    shader.inputs["Roughness"].default_value = roughness
    return mat


def box(name, size, location, mat, bevel=0.18):
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = size
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    if bevel:
        modifier = obj.modifiers.new("Soft authored edges", "BEVEL")
        modifier.width = bevel
        modifier.segments = 2
        bpy.ops.object.modifier_apply(modifier=modifier.name)
    return obj


def aim(obj, target):
    obj.rotation_euler = (Vector(target) - obj.location).to_track_quat("-Z", "Y").to_euler()


def build():
    OUTPUT.mkdir(parents=True, exist_ok=True)
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 0.01
    scene.unit_settings.length_unit = "CENTIMETERS"
    moss = material("Journal_MossLeather", (0.09, 0.26, 0.17))
    pages = material("Journal_PalePages", (0.76, 0.71, 0.51))
    iron = material("Journal_Iron", (0.075, 0.09, 0.09), 0.65, 0.35)
    gold = material("Journal_Brass", (0.55, 0.32, 0.09), 0.7, 0.3)
    ember = material("Journal_EmberSeal", (0.93, 0.31, 0.045), 0.25, 0.35)
    parts = [
        box("BackCover", (22, 28, 0.8), (0, 0, 0.4), moss, 0.35),
        box("FrontCover", (22, 28, 0.8), (0, 0, 4.8), moss, 0.35),
        box("BoundSpine", (1.7, 28, 5.2), (-10.3, 0, 2.6), moss, 0.55),
    ]
    for index in range(10):
        parts.append(box(f"PageLayer_{index:02}", (20.0, 26.5, 0.3), (0.2, 0, 1.05 + index * 0.335), pages, 0.08))
    for x in (-8.8, 8.8):
        parts.append(box("CoverBorder", (0.35, 24, 0.22), (x, 0, 5.24), gold, 0.1))
    for y in (-12, 12):
        parts.append(box("CoverBorder", (17.9, 0.35, 0.22), (0, y, 5.24), gold, 0.1))
    for y in (-9.7, 9.7):
        parts.append(box("SpineBand", (2.3, 1.7, 5.4), (-10.1, y, 2.7), iron, 0.28))
        parts.append(box("ForeEdgeClasp", (4.4, 2.3, 0.6), (9.0, y, 5.3), iron, 0.2))
        parts.append(box("ClaspRivet", (0.7, 0.7, 0.24), (8.1, y, 5.65), gold, 0.16))
    bpy.ops.mesh.primitive_cylinder_add(vertices=12, radius=4.9, depth=0.5, location=(0, 0, 5.55))
    medallion = bpy.context.object
    medallion.name = "SealIronRim"
    medallion.data.materials.append(iron)
    parts.append(medallion)
    bpy.ops.mesh.primitive_cylinder_add(vertices=12, radius=4.15, depth=0.3, location=(0, 0, 5.9))
    inset = bpy.context.object
    inset.name = "SealBrassInset"
    inset.data.materials.append(gold)
    parts.append(inset)
    mesh = bpy.data.meshes.new("FacetedEmberDiamond")
    mesh.from_pydata([(0, -3.4, 6.2), (2.35, 0, 6.2), (0, 3.4, 6.2), (-2.35, 0, 6.2), (0, 0, 7.45), (0, 0, 5.95)], [],
                    [(0, 1, 4), (1, 2, 4), (2, 3, 4), (3, 0, 4), (1, 0, 5), (2, 1, 5), (3, 2, 5), (0, 3, 5)])
    gem = bpy.data.objects.new("EmberDiamond", mesh)
    scene.collection.objects.link(gem)
    mesh.materials.append(ember)
    parts.append(gem)

    bpy.ops.object.select_all(action="DESELECT")
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    model = bpy.context.object
    model.name = NAME
    scene.cursor.location = (0, 0, 0)
    bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(island_margin=0.025)
    bpy.ops.object.mode_set(mode="OBJECT")
    bm = bmesh.new()
    bm.from_mesh(model.data)
    metrics = {
        "dimensions_cm": [round(v, 3) for v in model.dimensions],
        "triangles": sum(len(p.vertices) - 2 for p in model.data.polygons),
        "non_manifold_edges": sum(not e.is_manifold for e in bm.edges),
        "uv_channels": len(model.data.uv_layers),
        "material_slots": len(model.material_slots),
        "purpose": "presentation-only UI render source; no map actor or gameplay",
    }
    bm.free()
    assert metrics["non_manifold_edges"] == 0, metrics
    assert metrics["triangles"] < 6000, metrics
    assert metrics["material_slots"] == 5 and metrics["uv_channels"] == 1, metrics
    bpy.ops.export_scene.fbx(filepath=str(OUTPUT / f"{NAME}.fbx"), use_selection=True,
                             object_types={"MESH"}, axis_forward="-Y", axis_up="Z",
                             apply_unit_scale=True, bake_anim=False, add_leaf_bones=False)

    bpy.ops.object.camera_add(location=(23, -34, 66))
    camera = bpy.context.object
    camera.name = "JournalIconCamera"
    camera.data.type = "ORTHO"
    camera.data.ortho_scale = 41
    aim(camera, (0, 0, 2))
    scene.camera = camera
    for name, location, energy, color, size in [
        ("Key", (-24, -25, 50), 22000, (1.0, 0.92, 0.8), 35),
        ("Fill", (25, 0, 30), 14000, (0.72, 0.85, 1.0), 30),
        ("Rim", (0, 35, 25), 17000, (1.0, 0.7, 0.34), 20),
    ]:
        bpy.ops.object.light_add(type="AREA", location=location)
        light = bpy.context.object
        light.name = name
        light.data.energy = energy
        light.data.color = color
        light.data.shape = "DISK"
        light.data.size = size
        aim(light, (0, 0, 2))
    scene.render.engine = "BLENDER_EEVEE"
    scene.render.resolution_x = 512
    scene.render.resolution_y = 512
    scene.render.resolution_percentage = 100
    scene.render.film_transparent = True
    scene.render.image_settings.file_format = "PNG"
    scene.render.image_settings.color_mode = "RGBA"
    scene.view_settings.view_transform = "AgX"
    scene.render.filepath = str(OUTPUT / "T_FenwatchJournalEmblem.png")
    bpy.ops.wm.save_as_mainfile(filepath=str(OUTPUT / f"{NAME}.blend"))
    (OUTPUT / f"{NAME}_metrics.json").write_text(json.dumps(metrics, indent=2) + "\n")
    print("FENWATCH_JOURNAL_SOURCE_OK " + json.dumps(metrics))
    return metrics


__result__ = build()
