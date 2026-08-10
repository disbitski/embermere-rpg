"""Build Embermere's original Fenwatch practice dummy for Unreal Engine.

Run this reviewed script through Blender MCP with inline code disabled. It
builds a compact Stylized Classic training-yard prop using Embermere's shared
stone, moss, timber, iron, and ember material language; exports explicit UBX
collision; renders a preview; records deterministic metrics; and saves the
editable Blender source.
"""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereFenwatchPracticeDummy_01"
COLLISION_PREFIX = f"UBX_{ASSET_NAME}"


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
            project_root / "ArtSource/Blender/Environment/FenwatchPracticeDummy",
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
        bpy.data.curves,
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
    metallic: float = 0.0,
    emission_color: tuple[float, float, float, float] | None = None,
    emission_strength: float = 0.0,
) -> bpy.types.Material:
    material = bpy.data.materials.new(name=name)
    material.use_nodes = True
    node = material.node_tree.nodes.get("Principled BSDF")
    node.inputs["Base Color"].default_value = base_color
    node.inputs["Roughness"].default_value = roughness
    node.inputs["Metallic"].default_value = metallic
    if emission_color is not None:
        emission_input = node.inputs.get("Emission Color") or node.inputs.get("Emission")
        if emission_input:
            emission_input.default_value = emission_color
        strength_input = node.inputs.get("Emission Strength")
        if strength_input:
            strength_input.default_value = emission_strength
    return material


def apply_bevel(obj: bpy.types.Object, width_cm: float, segments: int = 2) -> None:
    modifier = obj.modifiers.new(name="StylizedBevel", type="BEVEL")
    modifier.width = width_cm
    modifier.segments = segments
    modifier.limit_method = "ANGLE"
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)


def add_cube(
    name: str,
    dimensions: tuple[float, float, float],
    location: tuple[float, float, float],
    material: bpy.types.Material,
    bevel: float = 0.0,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    if bevel > 0.0:
        apply_bevel(obj, bevel)
    return obj


def add_cylinder(
    name: str,
    vertices: int,
    radius: float,
    depth: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    bevel: float = 0.0,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
    scale: tuple[float, float, float] = (1.0, 1.0, 1.0),
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
    obj.scale = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    if bevel > 0.0:
        apply_bevel(obj, bevel)
    return obj


def add_ember_gem(
    name: str,
    location: tuple[float, float, float],
    scale: tuple[float, float, float],
    material: bpy.types.Material,
) -> bpy.types.Object:
    vertices = [
        (0.0, 0.0, 1.0),
        (0.0, 0.0, -1.0),
        (1.0, 0.0, 0.0),
        (0.0, 1.0, 0.0),
        (-1.0, 0.0, 0.0),
        (0.0, -1.0, 0.0),
    ]
    faces = [
        (0, 2, 3),
        (0, 3, 4),
        (0, 4, 5),
        (0, 5, 2),
        (1, 3, 2),
        (1, 4, 3),
        (1, 5, 4),
        (1, 2, 5),
    ]
    mesh = bpy.data.meshes.new(f"{name}_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.location = location
    obj.scale = scale
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
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


def add_collision() -> list[bpy.types.Object]:
    preview = make_material("M_CollisionPreview", (0.1, 0.6, 0.2, 0.25), 1.0)
    collision = [
        add_cube(
            f"{COLLISION_PREFIX}_00",
            (102.0, 102.0, 22.0),
            (0.0, 0.0, 11.0),
            preview,
        ),
        add_cube(
            f"{COLLISION_PREFIX}_01",
            (78.0, 54.0, 190.0),
            (0.0, 0.0, 116.0),
            preview,
        ),
    ]
    for obj in collision:
        obj.display_type = "WIRE"
        obj.hide_render = True
    return collision


def build_practice_dummy() -> tuple[bpy.types.Object, list[bpy.types.Object]]:
    stone = make_material("M_Waystone", (0.34, 0.40, 0.37, 1.0), 0.88)
    moss = make_material("M_WaystoneMoss", (0.25, 0.39, 0.16, 1.0), 0.95)
    iron = make_material("M_EmberLampIron", (0.055, 0.07, 0.075, 1.0), 0.62, 0.65)
    timber = make_material("M_EmbermereTimber", (0.24, 0.105, 0.035, 1.0), 0.9)
    ember = make_material(
        "M_WaystoneEmber",
        (0.58, 0.12, 0.025, 1.0),
        0.5,
        emission_color=(1.0, 0.18, 0.025, 1.0),
        emission_strength=7.0,
    )

    parts = [
        add_cylinder("DummyStonePlinth", 8, 51.0, 18.0, (0.0, 0.0, 9.0), stone, 2.5),
        add_cylinder(
            "DummyMossPlinthCap",
            8,
            47.0,
            4.0,
            (0.0, 0.0, 19.5),
            moss,
            1.0,
            rotation=(0.0, 0.0, math.radians(10.0)),
        ),
        add_cube("DummyCentralPost", (28.0, 32.0, 178.0), (0.0, 0.0, 109.0), timber, 3.0),
        add_cube("DummyTorso", (82.0, 40.0, 70.0), (0.0, 0.0, 151.0), timber, 6.0),
        add_cube("DummyShoulderBeam", (142.0, 30.0, 24.0), (0.0, 0.0, 178.0), timber, 3.0),
        add_cube(
            "DummyArmLeft",
            (72.0, 22.0, 22.0),
            (-80.0, 0.0, 158.0),
            timber,
            3.0,
            rotation=(0.0, math.radians(-20.0), 0.0),
        ),
        add_cube(
            "DummyArmRight",
            (72.0, 22.0, 22.0),
            (80.0, 0.0, 158.0),
            timber,
            3.0,
            rotation=(0.0, math.radians(20.0), 0.0),
        ),
        add_cube("DummyForearmLeft", (22.0, 22.0, 58.0), (-115.0, 0.0, 125.0), timber, 3.0),
        add_cube("DummyForearmRight", (22.0, 22.0, 58.0), (115.0, 0.0, 125.0), timber, 3.0),
        add_cube("DummyHead", (50.0, 38.0, 45.0), (0.0, 0.0, 222.5), timber, 7.0),
        add_cube("DummyPostBandLow", (36.0, 38.0, 13.0), (0.0, 0.0, 54.0), iron, 2.0),
        add_cube("DummyPostBandHigh", (36.0, 38.0, 13.0), (0.0, 0.0, 188.0), iron, 2.0),
        add_cube("DummyShoulderBandLeft", (14.0, 36.0, 31.0), (-52.0, 0.0, 178.0), iron, 2.0),
        add_cube("DummyShoulderBandRight", (14.0, 36.0, 31.0), (52.0, 0.0, 178.0), iron, 2.0),
        add_cube("DummyBrowBand", (56.0, 43.0, 10.0), (0.0, 0.0, 230.0), iron, 2.0),
        add_cylinder(
            "DummyTargetDisc",
            12,
            31.0,
            7.0,
            (0.0, -23.5, 151.0),
            iron,
            1.4,
            rotation=(math.radians(90.0), 0.0, 0.0),
        ),
        add_cylinder(
            "DummyTargetMossRing",
            12,
            21.0,
            8.0,
            (0.0, -27.5, 151.0),
            moss,
            1.0,
            rotation=(math.radians(90.0), 0.0, 0.0),
        ),
        add_ember_gem("DummyEmberBullseye", (0.0, -34.0, 151.0), (10.5, 4.0, 10.5), ember),
        add_cylinder(
            "DummyMossShoulderLeft",
            8,
            16.0,
            3.0,
            (-42.0, 1.0, 192.0),
            moss,
            0.8,
            rotation=(0.0, 0.0, math.radians(12.0)),
            scale=(1.45, 0.65, 1.0),
        ),
        add_cylinder(
            "DummyMossShoulderRight",
            8,
            12.0,
            3.0,
            (24.0, -1.0, 192.0),
            moss,
            0.8,
            rotation=(0.0, 0.0, math.radians(-8.0)),
            scale=(1.4, 0.7, 1.0),
        ),
    ]

    render_mesh = join_render_mesh(parts)
    return render_mesh, add_collision()


def mesh_metrics(obj: bpy.types.Object) -> dict:
    mesh = obj.data
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold_edges = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    return {
        "dimensions_cm": [round(value, 3) for value in obj.dimensions],
        "triangles": sum(max(1, len(poly.vertices) - 2) for poly in mesh.polygons),
        "materials": [slot.material.name for slot in obj.material_slots if slot.material],
        "uv_channels": len(mesh.uv_layers),
        "non_manifold_edges": non_manifold_edges,
        "scale": [round(value, 6) for value in obj.scale],
        "origin_z_cm": round(obj.location.z, 6),
    }


def validate_asset(render_mesh: bpy.types.Object, collision: list[bpy.types.Object]) -> dict:
    metrics = mesh_metrics(render_mesh)
    errors = []
    width, depth, height = metrics["dimensions_cm"]
    if not (245.0 <= width <= 260.0 and 95.0 <= depth <= 115.0 and 240.0 <= height <= 250.0):
        errors.append(f"Unexpected dimensions: {metrics['dimensions_cm']}")
    if metrics["triangles"] > 6000:
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
        "M_Waystone",
        "M_WaystoneMoss",
        "M_EmberLampIron",
        "M_EmbermereTimber",
        "M_WaystoneEmber",
    }
    if not expected_materials.issubset(set(metrics["materials"])):
        errors.append(f"Missing material slots: {metrics['materials']}")
    if len(collision) != 2 or any(not obj.name.startswith(COLLISION_PREFIX) for obj in collision):
        errors.append("Expected two correctly named Unreal collision boxes")
    if errors:
        raise RuntimeError("Embermere practice dummy validation failed: " + "; ".join(errors))
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(render_mesh: bpy.types.Object, preview_path: Path) -> None:
    ground_material = make_material("M_PreviewGround", (0.075, 0.11, 0.075, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=750.0, location=(0.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_material)

    bpy.ops.object.light_add(
        type="SUN",
        rotation=(math.radians(35.0), math.radians(-25.0), math.radians(-35.0)),
    )
    sun = bpy.context.object
    sun.name = "Preview_Sun"
    sun.data.energy = 2.7
    sun.data.angle = math.radians(18.0)

    bpy.ops.object.light_add(type="AREA", location=(-330.0, -420.0, 330.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 2700000.0
    key.data.shape = "DISK"
    key.data.size = 260.0
    look_at(key, (0.0, 0.0, 120.0))

    bpy.ops.object.light_add(type="AREA", location=(310.0, 180.0, 260.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 1150000.0
    fill.data.color = (0.35, 0.5, 0.78)
    fill.data.size = 240.0
    look_at(fill, (0.0, 0.0, 125.0))

    bpy.ops.object.light_add(type="POINT", location=(0.0, -72.0, 151.0))
    glow = bpy.context.object
    glow.name = "Preview_EmberGlow"
    glow.data.energy = 70000.0
    glow.data.color = (1.0, 0.16, 0.025)
    glow.data.shadow_soft_size = 60.0

    bpy.ops.object.camera_add(location=(340.0, -490.0, 285.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 58.0
    look_at(camera, (0.0, 0.0, 122.0))
    bpy.context.scene.camera = camera

    world = bpy.context.scene.world or bpy.data.worlds.new("EmbermerePreviewWorld")
    bpy.context.scene.world = world
    world.use_nodes = True
    background = world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.025, 0.055, 0.08, 1.0)
    background.inputs["Strength"].default_value = 1.05

    render_mesh.hide_render = False
    bpy.context.scene.render.filepath = str(preview_path)
    bpy.ops.render.render(write_still=True)


def export_fbx(render_mesh: bpy.types.Object, collision: list[bpy.types.Object], fbx_path: Path) -> None:
    bpy.ops.object.select_all(action="DESELECT")
    for obj in [render_mesh, *collision]:
        obj.select_set(True)
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
    render_mesh, collision = build_practice_dummy()
    metrics = validate_asset(render_mesh, collision)
    metrics_path.write_text(json.dumps(metrics, indent=2) + "\n", encoding="utf-8")
    export_fbx(render_mesh, collision, fbx_path)
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
