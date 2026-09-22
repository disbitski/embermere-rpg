"""Build Embermere's project-owned Fenwatch north-commons herb garden."""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereFenwatchHerbGarden_01"
COLLISION_PREFIX = f"UBX_{ASSET_NAME}"


def parameters() -> dict:
    supplied = globals().get("args", {})
    return supplied if isinstance(supplied, dict) else {}


def output_paths(values: dict) -> tuple[Path, Path, Path, Path]:
    root = Path(values.get("project_root", "/Users/wizard/Documents/Unreal Game"))
    directory = Path(
        values.get(
            "output_dir",
            root / "ArtSource/Blender/Environment/FenwatchHerbGarden",
        )
    )
    directory.mkdir(parents=True, exist_ok=True)
    return (
        directory / f"{ASSET_NAME}.blend",
        directory / f"{ASSET_NAME}.fbx",
        directory / f"{ASSET_NAME}_preview.png",
        directory / f"{ASSET_NAME}_metrics.json",
    )


def reset_scene() -> None:
    if bpy.context.object and bpy.context.object.mode != "OBJECT":
        bpy.ops.object.mode_set(mode="OBJECT")
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for collection in (
        bpy.data.meshes,
        bpy.data.curves,
        bpy.data.materials,
        bpy.data.cameras,
        bpy.data.lights,
    ):
        for datablock in list(collection):
            if datablock.users == 0:
                collection.remove(datablock)


def configure_scene() -> None:
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 0.01
    scene.unit_settings.length_unit = "CENTIMETERS"
    scene.render.resolution_x = 1000
    scene.render.resolution_y = 1000
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


def material(
    name: str,
    color: tuple[float, float, float, float],
    roughness: float,
    metallic: float = 0.0,
    emission: tuple[float, float, float, float] | None = None,
) -> bpy.types.Material:
    result = bpy.data.materials.new(name=name)
    result.use_nodes = True
    node = result.node_tree.nodes.get("Principled BSDF")
    node.inputs["Base Color"].default_value = color
    node.inputs["Roughness"].default_value = roughness
    node.inputs["Metallic"].default_value = metallic
    if emission:
        emission_input = node.inputs.get("Emission Color") or node.inputs.get("Emission")
        if emission_input:
            emission_input.default_value = emission
        if node.inputs.get("Emission Strength"):
            node.inputs["Emission Strength"].default_value = 6.0
    return result


def bevel(obj: bpy.types.Object, amount: float, segments: int = 2) -> None:
    modifier = obj.modifiers.new("StylizedBevel", "BEVEL")
    modifier.width = amount
    modifier.segments = segments
    modifier.limit_method = "ANGLE"
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)


def cube(
    name: str,
    dimensions: tuple[float, float, float],
    location: tuple[float, float, float],
    mat: bpy.types.Material,
    bevel_amount: float = 0.0,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    if bevel_amount:
        bevel(obj, bevel_amount)
    return obj


def cylinder(
    name: str,
    vertices: int,
    radius: float,
    depth: float,
    location: tuple[float, float, float],
    mat: bpy.types.Material,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
    bevel_amount: float = 0.0,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cylinder_add(
        vertices=vertices,
        radius=radius,
        depth=depth,
        location=location,
        rotation=rotation,
    )
    obj = bpy.context.object
    obj.name = name
    obj.data.materials.append(mat)
    if bevel_amount:
        bevel(obj, bevel_amount)
    return obj


def leaf(
    name: str,
    location: tuple[float, float, float],
    scale: tuple[float, float, float],
    rotation: tuple[float, float, float],
    mat: bpy.types.Material,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=1, radius=1.0, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.scale = scale
    obj.rotation_euler = rotation
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    return obj


def join_meshes(parts: list[bpy.types.Object]) -> bpy.types.Object:
    bpy.ops.object.select_all(action="DESELECT")
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    result = bpy.context.object
    result.name = ASSET_NAME
    result.data.name = f"{ASSET_NAME}_Mesh"
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66.0), island_margin=0.025)
    bpy.ops.object.mode_set(mode="OBJECT")
    triangulate = result.modifiers.new("ExportTriangulation", "TRIANGULATE")
    bpy.context.view_layer.objects.active = result
    bpy.ops.object.modifier_apply(modifier=triangulate.name)
    return result


def add_colliders(preview: bpy.types.Material) -> list[bpy.types.Object]:
    specs = [
        ((128.0, 212.0, 46.0), (-92.0, 0.0, 23.0)),
        ((128.0, 212.0, 46.0), (92.0, 0.0, 23.0)),
        ((24.0, 24.0, 154.0), (-160.0, 82.0, 77.0)),
        ((24.0, 24.0, 154.0), (160.0, 82.0, 77.0)),
    ]
    result = []
    for index, (dimensions, location) in enumerate(specs):
        obj = cube(
            f"{COLLISION_PREFIX}_{index:02d}",
            dimensions,
            location,
            preview,
        )
        obj.display_type = "WIRE"
        obj.hide_render = True
        result.append(obj)
    return result


def build_garden() -> tuple[bpy.types.Object, list[bpy.types.Object]]:
    stone = material("M_Waystone", (0.34, 0.40, 0.37, 1.0), 0.88)
    moss = material("M_WaystoneMoss", (0.25, 0.39, 0.16, 1.0), 0.95)
    timber = material("M_EmbermereTimber", (0.24, 0.105, 0.035, 1.0), 0.9)
    iron = material("M_EmberLampIron", (0.055, 0.07, 0.075, 1.0), 0.62, 0.65)
    ember = material(
        "M_WaystoneEmber",
        (0.58, 0.12, 0.025, 1.0),
        0.5,
        emission=(1.0, 0.18, 0.025, 1.0),
    )
    collision_preview = material("M_CollisionPreview", (0.1, 0.6, 0.2, 0.25), 1.0)
    parts: list[bpy.types.Object] = []

    # Two low raised beds keep a clear central tending aisle.
    for side, x in (("Left", -92.0), ("Right", 92.0)):
        parts.extend(
            [
                cube(f"{side}StoneBase", (128.0, 212.0, 18.0), (x, 0.0, 9.0), stone, 3.0),
                cube(f"{side}TimberBed", (120.0, 204.0, 34.0), (x, 0.0, 29.0), timber, 4.0),
                cube(f"{side}MossSoil", (104.0, 184.0, 12.0), (x, 0.0, 50.0), moss, 3.0),
            ]
        )
        for row, y in enumerate((-66.0, -20.0, 26.0, 72.0)):
            for column, offset in enumerate((-30.0, 0.0, 30.0)):
                stem_x = x + offset
                height = 17.0 + ((row + column) % 3) * 4.0
                parts.append(
                    cylinder(
                        f"{side}HerbStem_{row}_{column}",
                        8,
                        2.5,
                        height,
                        (stem_x, y, 56.0 + height / 2.0),
                        timber,
                    )
                )
                parts.append(
                    leaf(
                        f"{side}HerbLeaf_{row}_{column}",
                        (stem_x + (4.0 if column != 1 else -4.0), y, 66.0 + height),
                        (10.0, 5.0, 4.0),
                        (0.0, math.radians(20.0), math.radians(row * 28.0)),
                        moss,
                    )
                )

    # A tall rear trellis gives the otherwise low garden a route-readable crest.
    for side, x in (("Left", -160.0), ("Right", 160.0)):
        parts.extend(
            [
                cube(f"{side}TrellisPost", (22.0, 22.0, 154.0), (x, 82.0, 77.0), timber, 3.0),
                cube(f"{side}StoneFoot", (36.0, 36.0, 16.0), (x, 82.0, 8.0), stone, 3.0),
                cube(f"{side}MossCap", (34.0, 34.0, 14.0), (x, 82.0, 159.0), moss, 3.0),
            ]
        )
    parts.append(cube("TrellisBeam", (344.0, 20.0, 22.0), (0.0, 82.0, 148.0), timber, 3.0))
    for x in (-112.0, -56.0, 0.0, 56.0, 112.0):
        parts.append(cube(f"TrellisSlat_{x}", (9.0, 9.0, 92.0), (x, 82.0, 97.0), timber, 1.5))
        for z in (70.0, 103.0, 132.0):
            parts.append(
                leaf(
                    f"TrellisLeaf_{x}_{z}",
                    (x + (7.0 if int(z) % 2 else -7.0), 72.0, z),
                    (12.0, 5.0, 6.0),
                    (math.radians(12.0), math.radians(28.0), math.radians(x)),
                    moss,
                )
            )

    # Garden markers and a small watering pail stay decorative.
    for side, x in (("Left", -92.0), ("Right", 92.0)):
        parts.extend(
            [
                cube(f"{side}MarkerPost", (10.0, 10.0, 48.0), (x, -94.0, 70.0), timber, 1.5),
                cube(f"{side}MarkerFace", (48.0, 7.0, 26.0), (x, -98.0, 90.0), stone, 2.0),
                cube(f"{side}MarkerBand", (34.0, 4.0, 5.0), (x, -103.0, 90.0), iron, 1.0),
            ]
        )
    parts.extend(
        [
            cylinder("WaterPail", 12, 20.0, 30.0, (0.0, -84.0, 15.0), iron, bevel_amount=2.0),
            cube("CrestBack", (50.0, 10.0, 42.0), (0.0, 77.0, 174.0), iron, 3.0),
            leaf("CrestEmber", (0.0, 70.0, 174.0), (10.0, 5.0, 14.0), (0.0, 0.0, 0.0), ember),
        ]
    )

    return join_meshes(parts), add_colliders(collision_preview)


def metrics(obj: bpy.types.Object) -> dict:
    mesh = obj.data
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    return {
        "dimensions_cm": [round(value, 3) for value in obj.dimensions],
        "triangles": sum(max(1, len(poly.vertices) - 2) for poly in mesh.polygons),
        "materials": [slot.material.name for slot in obj.material_slots if slot.material],
        "uv_channels": len(mesh.uv_layers),
        "non_manifold_edges": non_manifold,
        "scale": [round(value, 6) for value in obj.scale],
        "origin_z_cm": round(obj.location.z, 6),
    }


def validate(render_mesh: bpy.types.Object, colliders: list[bpy.types.Object]) -> dict:
    result = metrics(render_mesh)
    width, depth, height = result["dimensions_cm"]
    errors = []
    if not (340.0 <= width <= 380.0 and 210.0 <= depth <= 245.0 and 175.0 <= height <= 200.0):
        errors.append(f"unexpected dimensions {result['dimensions_cm']}")
    if result["triangles"] > 8000:
        errors.append(f"triangle budget exceeded {result['triangles']}")
    if result["uv_channels"] != 1:
        errors.append(f"expected one UV channel, found {result['uv_channels']}")
    if result["non_manifold_edges"] != 0:
        errors.append(f"non-manifold edges found {result['non_manifold_edges']}")
    if result["scale"] != [1.0, 1.0, 1.0] or abs(result["origin_z_cm"]) > 0.001:
        errors.append("asset must retain applied scale and ground-level origin")
    expected = {
        "M_Waystone",
        "M_WaystoneMoss",
        "M_EmberLampIron",
        "M_EmbermereTimber",
        "M_WaystoneEmber",
    }
    if not expected.issubset(set(result["materials"])):
        errors.append(f"missing shared material slots {result['materials']}")
    if len(colliders) != 4 or any(not item.name.startswith(COLLISION_PREFIX) for item in colliders):
        errors.append("expected four correctly named Unreal collision boxes")
    if errors:
        raise RuntimeError("Fenwatch herb-garden validation failed: " + "; ".join(errors))
    return result


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    obj.rotation_euler = (Vector(point) - obj.location).to_track_quat("-Z", "Y").to_euler()


def preview(render_mesh: bpy.types.Object, path: Path) -> None:
    ground_mat = material("M_PreviewGround", (0.075, 0.11, 0.075, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=750.0, location=(0.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_mat)
    bpy.ops.object.light_add(type="SUN", rotation=(math.radians(35.0), math.radians(-25.0), math.radians(-35.0)))
    bpy.context.object.data.energy = 2.8
    bpy.context.object.data.angle = math.radians(18.0)
    bpy.ops.object.light_add(type="AREA", location=(-320.0, -420.0, 360.0))
    key = bpy.context.object
    key.data.energy = 2100000.0
    key.data.size = 280.0
    look_at(key, (0.0, 0.0, 80.0))
    bpy.ops.object.light_add(type="AREA", location=(420.0, 300.0, 260.0))
    fill = bpy.context.object
    fill.data.energy = 900000.0
    fill.data.color = (0.35, 0.5, 0.78)
    fill.data.size = 240.0
    look_at(fill, (0.0, 0.0, 85.0))
    bpy.ops.object.camera_add(location=(460.0, -500.0, 290.0))
    camera = bpy.context.object
    camera.data.lens = 58.0
    camera.data.clip_end = 3000.0
    look_at(camera, (0.0, 0.0, 78.0))
    bpy.context.scene.camera = camera
    world = bpy.context.scene.world or bpy.data.worlds.new("EmbermerePreviewWorld")
    bpy.context.scene.world = world
    world.use_nodes = True
    background = world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.025, 0.055, 0.08, 1.0)
    background.inputs["Strength"].default_value = 1.05
    render_mesh.hide_render = False
    bpy.context.scene.render.filepath = str(path)
    bpy.ops.render.render(write_still=True)


def export_fbx(render_mesh: bpy.types.Object, colliders: list[bpy.types.Object], path: Path) -> None:
    bpy.ops.object.select_all(action="DESELECT")
    for obj in [render_mesh, *colliders]:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = render_mesh
    bpy.ops.export_scene.fbx(
        filepath=str(path),
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
    blend_path, fbx_path, preview_path, metrics_path = output_paths(parameters())
    reset_scene()
    configure_scene()
    render_mesh, colliders = build_garden()
    result = validate(render_mesh, colliders)
    metrics_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    export_fbx(render_mesh, colliders, fbx_path)
    preview(render_mesh, preview_path)
    bpy.ops.wm.save_as_mainfile(filepath=str(blend_path))
    return {
        "asset": ASSET_NAME,
        "blend": str(blend_path),
        "fbx": str(fbx_path),
        "preview": str(preview_path),
        "metrics_file": str(metrics_path),
        "metrics": result,
    }


__result__ = main()
