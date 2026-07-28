"""Build a visual-only stylized marsh reed cluster for Embermere.

Run this reviewed script through Blender MCP. It creates a compact low-poly
cluster with solid tapered leaves, cattail-like seed heads, a terrain-blended
footprint, shared Embermere material slot names, and no gameplay collision.
"""

from __future__ import annotations

import bmesh
import json
import math
import random
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereMarshReedCluster_01"
RANDOM_SEED = 271828


def get_parameters() -> dict:
    supplied = globals().get("args", {})
    return supplied if isinstance(supplied, dict) else {}


def resolve_output_paths(parameters: dict) -> tuple[Path, Path, Path, Path]:
    project_root = Path(
        parameters.get("project_root", "/Users/wizard/Documents/Unreal Game")
    ).expanduser()
    output_dir = Path(
        parameters.get(
            "output_dir",
            project_root / "ArtSource/Blender/Environment/MarshReeds",
        )
    ).expanduser()
    output_dir.mkdir(parents=True, exist_ok=True)
    return (
        output_dir / f"{ASSET_NAME}.blend",
        output_dir / f"{ASSET_NAME}.fbx",
        output_dir / f"{ASSET_NAME}_preview.png",
        output_dir / f"{ASSET_NAME}_metrics.json",
    )


def reset_scene() -> None:
    if bpy.context.object and bpy.context.object.mode != "OBJECT":
        bpy.ops.object.mode_set(mode="OBJECT")
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)

    for datablocks in (
        bpy.data.meshes,
        bpy.data.materials,
        bpy.data.cameras,
        bpy.data.lights,
    ):
        for datablock in list(datablocks):
            if datablock.users == 0:
                datablocks.remove(datablock)


def configure_scene_units() -> None:
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 0.01
    scene.unit_settings.length_unit = "CENTIMETERS"
    scene.render.resolution_x = 900
    scene.render.resolution_y = 900
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    try:
        scene.view_settings.look = "AgX - Medium High Contrast"
    except TypeError:
        pass

    engines = {
        item.identifier for item in scene.render.bl_rna.properties["engine"].enum_items
    }
    if "BLENDER_EEVEE_NEXT" in engines:
        scene.render.engine = "BLENDER_EEVEE_NEXT"


def make_material(
    name: str,
    base_color: tuple[float, float, float, float],
    roughness: float,
) -> bpy.types.Material:
    material = bpy.data.materials.new(name=name)
    material.use_nodes = True
    node = material.node_tree.nodes.get("Principled BSDF")
    node.inputs["Base Color"].default_value = base_color
    node.inputs["Roughness"].default_value = roughness
    return material


def add_cylinder(
    name: str,
    radius: float,
    depth: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    vertices: int = 8,
    scale: tuple[float, float, float] = (1.0, 1.0, 1.0),
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cylinder_add(
        vertices=vertices,
        radius=radius,
        depth=depth,
        location=location,
    )
    obj = bpy.context.object
    obj.name = name
    obj.scale = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    return obj


def add_tapered_leaf(
    name: str,
    base: tuple[float, float],
    angle: float,
    width: float,
    height: float,
    lean: float,
    material: bpy.types.Material,
) -> bpy.types.Object:
    right = Vector((math.cos(angle), math.sin(angle), 0.0))
    forward = Vector((-math.sin(angle), math.cos(angle), 0.0))
    base_center = Vector((base[0], base[1], 5.0))
    mid_center = base_center + forward * (lean * 0.35) + Vector((0.0, 0.0, height * 0.58))
    tip_center = base_center + forward * lean + Vector((0.0, 0.0, height))

    vertices = []
    for center, ring_width, thickness in (
        (base_center, width * 0.5, 1.8),
        (mid_center, width * 0.34, 1.2),
        (tip_center, width * 0.025, 0.35),
    ):
        vertices.extend(
            [
                tuple(center - right * ring_width - forward * thickness),
                tuple(center + right * ring_width - forward * thickness),
                tuple(center + right * ring_width + forward * thickness),
                tuple(center - right * ring_width + forward * thickness),
            ]
        )

    faces = [
        (0, 1, 2, 3),
        (0, 4, 5, 1),
        (1, 5, 6, 2),
        (2, 6, 7, 3),
        (3, 7, 4, 0),
        (4, 8, 9, 5),
        (5, 9, 10, 6),
        (6, 10, 11, 7),
        (7, 11, 8, 4),
        (8, 11, 10, 9),
    ]
    mesh = bpy.data.meshes.new(f"{name}_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    return obj


def join_render_mesh(objects: list[bpy.types.Object]) -> bpy.types.Object:
    bpy.ops.object.select_all(action="DESELECT")
    for obj in objects:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = objects[0]
    bpy.ops.object.join()
    result = bpy.context.object
    result.name = ASSET_NAME
    result.data.name = f"{ASSET_NAME}_Mesh"
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66.0), island_margin=0.025)
    bpy.ops.object.mode_set(mode="OBJECT")

    triangulate = result.modifiers.new(name="ExportTriangulation", type="TRIANGULATE")
    bpy.context.view_layer.objects.active = result
    bpy.ops.object.modifier_apply(modifier=triangulate.name)
    return result


def build_reed_cluster() -> bpy.types.Object:
    moss = make_material("M_WaystoneMoss", (0.18, 0.34, 0.12, 1.0), 0.95)
    timber = make_material("M_EmbermereTimber", (0.20, 0.075, 0.022, 1.0), 0.92)
    stone = make_material("M_Waystone", (0.17, 0.22, 0.16, 1.0), 1.0)
    ground = make_material("M_EmbermereGround", (0.12, 0.20, 0.09, 1.0), 1.0)

    parts = [
        add_cylinder(
            "MarshGroundFootprint",
            84.0,
            3.0,
            (0.0, 0.0, 1.5),
            ground,
            vertices=12,
            scale=(1.0, 0.78, 1.0),
        ),
        add_cylinder(
            "MarshMossPatch",
            70.0,
            3.0,
            (4.0, -2.0, 3.5),
            moss,
            vertices=12,
            scale=(1.0, 0.72, 1.0),
        ),
    ]
    for index, (x, y, radius, scale) in enumerate(
        (
            (-58.0, 10.0, 8.0, (1.25, 0.75, 0.55)),
            (53.0, -18.0, 7.0, (0.85, 1.2, 0.5)),
            (30.0, 38.0, 5.5, (1.3, 0.7, 0.45)),
        )
    ):
        parts.append(
            add_cylinder(
                f"MarshEdgeStone_{index:02d}",
                radius,
                5.0,
                (x, y, 3.0),
                stone,
                vertices=8,
                scale=scale,
            )
        )

    rng = random.Random(RANDOM_SEED)
    for index in range(30):
        angle = rng.uniform(0.0, math.tau)
        radius = math.sqrt(rng.random()) * 70.0
        base = (math.cos(angle) * radius, math.sin(angle) * radius * 0.72)
        parts.append(
            add_tapered_leaf(
                f"MarshLeaf_{index:02d}",
                base,
                angle + rng.uniform(-0.45, 0.45),
                rng.uniform(6.5, 12.5),
                rng.uniform(55.0, 145.0),
                rng.uniform(10.0, 42.0),
                moss,
            )
        )

    cattail_locations = (
        (-35.0, -14.0, 136.0),
        (-8.0, 24.0, 156.0),
        (20.0, -20.0, 126.0),
        (43.0, 12.0, 145.0),
        (4.0, -38.0, 112.0),
    )
    for index, (x, y, height) in enumerate(cattail_locations):
        parts.append(
            add_cylinder(
                f"MarshStalk_{index:02d}",
                2.3,
                height,
                (x, y, 5.0 + height * 0.5),
                timber,
                vertices=6,
            )
        )
        parts.append(
            add_cylinder(
                f"MarshSeedHead_{index:02d}",
                6.5,
                24.0,
                (x, y, height + 18.0),
                timber,
                vertices=8,
                scale=(1.0, 1.0, 1.15),
            )
        )

    return join_render_mesh(parts)


def mesh_metrics(obj: bpy.types.Object) -> dict:
    mesh = obj.data
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold_edges = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    return {
        "dimensions_cm": [round(value, 3) for value in obj.dimensions],
        "triangles": sum(max(1, len(poly.vertices) - 2) for poly in mesh.polygons),
        "vertices": len(mesh.vertices),
        "materials": [slot.material.name for slot in obj.material_slots if slot.material],
        "uv_channels": len(mesh.uv_layers),
        "non_manifold_edges": non_manifold_edges,
        "scale": [round(value, 6) for value in obj.scale],
        "origin_z_cm": round(obj.location.z, 6),
        "collision_shapes": 0,
    }


def validate_asset(render_mesh: bpy.types.Object) -> dict:
    metrics = mesh_metrics(render_mesh)
    errors = []
    depth, width, height = metrics["dimensions_cm"]
    if not (155.0 <= depth <= 230.0 and 120.0 <= width <= 210.0 and 145.0 <= height <= 200.0):
        errors.append(f"Unexpected dimensions: {metrics['dimensions_cm']}")
    if metrics["triangles"] > 2500:
        errors.append(f"Triangle budget exceeded: {metrics['triangles']}")
    if metrics["uv_channels"] < 1:
        errors.append("Render mesh has no UV channel")
    if metrics["non_manifold_edges"] != 0:
        errors.append(f"Non-manifold edges found: {metrics['non_manifold_edges']}")
    if metrics["scale"] != [1.0, 1.0, 1.0]:
        errors.append(f"Transforms are not applied: {metrics['scale']}")
    if abs(metrics["origin_z_cm"]) > 0.001:
        errors.append(f"Origin must remain at ground level: {metrics['origin_z_cm']}")
    expected_materials = {
        "M_EmbermereGround",
        "M_WaystoneMoss",
        "M_EmbermereTimber",
        "M_Waystone",
    }
    if set(metrics["materials"]) != expected_materials:
        errors.append(f"Unexpected material slots: {metrics['materials']}")
    if errors:
        raise RuntimeError("Embermere marsh reed validation failed: " + "; ".join(errors))
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(render_mesh: bpy.types.Object, preview_path: Path) -> None:
    ground_material = make_material("M_PreviewGround", (0.055, 0.11, 0.055, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=700.0, location=(0.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_material)

    bpy.ops.object.light_add(
        type="SUN",
        rotation=(math.radians(35.0), math.radians(-25.0), math.radians(-35.0)),
    )
    sun = bpy.context.object
    sun.name = "Preview_Sun"
    sun.data.energy = 2.5
    sun.data.angle = math.radians(18.0)

    bpy.ops.object.light_add(type="AREA", location=(-280.0, -300.0, 250.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 2400000.0
    key.data.size = 240.0
    look_at(key, (0.0, 0.0, 75.0))

    bpy.ops.object.light_add(type="AREA", location=(240.0, 120.0, 190.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 1000000.0
    fill.data.color = (0.35, 0.5, 0.75)
    fill.data.size = 200.0
    look_at(fill, (0.0, 0.0, 80.0))

    bpy.ops.object.camera_add(location=(-310.0, -350.0, 215.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 62.0
    look_at(camera, (0.0, 0.0, 78.0))
    bpy.context.scene.camera = camera

    world = bpy.context.scene.world or bpy.data.worlds.new("EmbermerePreviewWorld")
    bpy.context.scene.world = world
    world.use_nodes = True
    background = world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.02, 0.045, 0.055, 1.0)
    background.inputs["Strength"].default_value = 0.9

    render_mesh.hide_render = False
    bpy.context.scene.render.filepath = str(preview_path)
    bpy.ops.render.render(write_still=True)


def export_fbx(render_mesh: bpy.types.Object, fbx_path: Path) -> None:
    bpy.ops.object.select_all(action="DESELECT")
    render_mesh.select_set(True)
    bpy.context.view_layer.objects.active = render_mesh
    bpy.ops.export_scene.fbx(
        filepath=str(fbx_path),
        use_selection=True,
        object_types={"MESH"},
        global_scale=1.0,
        apply_unit_scale=True,
        axis_forward="-Y",
        axis_up="Z",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        bake_anim=False,
    )


def main() -> dict:
    parameters = get_parameters()
    blend_path, fbx_path, preview_path, metrics_path = resolve_output_paths(parameters)
    reset_scene()
    configure_scene_units()
    render_mesh = build_reed_cluster()
    metrics = validate_asset(render_mesh)
    metrics_path.write_text(json.dumps(metrics, indent=2) + "\n", encoding="utf-8")
    export_fbx(render_mesh, fbx_path)
    add_preview_scene(render_mesh, preview_path)
    bpy.ops.wm.save_as_mainfile(filepath=str(blend_path))
    return {
        "asset": ASSET_NAME,
        "blend": str(blend_path),
        "fbx": str(fbx_path),
        "preview": str(preview_path),
        "metrics_file": str(metrics_path),
        "metrics": metrics,
    }


__result__ = main()
