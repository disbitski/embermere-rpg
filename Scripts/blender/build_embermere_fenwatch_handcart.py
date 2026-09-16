"""Build Embermere's project-owned Fenwatch provision handcart.

Run this reviewed scene-resetting script through Blender MCP in a factory-clean
headless process. The cart reuses the shared Fenwatch stone, moss, timber,
iron, and ember language, exports four purposeful UBX colliders, renders a
preview, records deterministic metrics, and saves editable Blender source.
"""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereFenwatchHandcart_01"
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
            project_root / "ArtSource/Blender/Environment/FenwatchHandcart",
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
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
    bevel: float = 0.0,
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
    obj.data.materials.append(material)
    if bevel > 0.0:
        apply_bevel(obj, bevel)
    return obj


def add_torus(
    name: str,
    major_radius: float,
    minor_radius: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    rotation: tuple[float, float, float],
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_torus_add(
        major_segments=16,
        minor_segments=6,
        location=location,
        rotation=rotation,
        major_radius=major_radius,
        minor_radius=minor_radius,
    )
    obj = bpy.context.object
    obj.name = name
    obj.data.materials.append(material)
    return obj


def add_ember_gem(
    name: str,
    location: tuple[float, float, float],
    scale: tuple[float, float, float],
    material: bpy.types.Material,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=1, radius=1.0, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.scale = scale
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
    collision_specs = [
        ((222.0, 120.0, 64.0), (0.0, 0.0, 110.0)),
        ((44.0, 172.0, 98.0), (-46.0, 0.0, 49.0)),
        ((24.0, 24.0, 82.0), (84.0, -38.0, 41.0)),
        ((24.0, 24.0, 82.0), (84.0, 38.0, 41.0)),
    ]
    collision = []
    for index, (dimensions, location) in enumerate(collision_specs):
        collision.append(
            add_cube(
                f"{COLLISION_PREFIX}_{index:02d}",
                dimensions,
                location,
                preview,
            )
        )
    for obj in collision:
        obj.display_type = "WIRE"
        obj.hide_render = True
    return collision


def add_wheel(
    side: str,
    y: float,
    timber: bpy.types.Material,
    iron: bpy.types.Material,
) -> list[bpy.types.Object]:
    rotation = (math.radians(90.0), 0.0, 0.0)
    parts = [
        add_torus(
            f"{side}IronRim",
            43.5,
            5.0,
            (-46.0, y, 53.0),
            iron,
            rotation,
        ),
        add_torus(
            f"{side}TimberFelloe",
            34.0,
            6.5,
            (-46.0, y, 53.0),
            timber,
            rotation,
        ),
        add_cylinder(
            f"{side}Hub",
            12,
            10.0,
            22.0,
            (-46.0, y, 53.0),
            iron,
            rotation,
            1.0,
        ),
    ]
    for index, angle in enumerate(range(0, 180, 30)):
        parts.append(
            add_cube(
                f"{side}Spoke{index + 1}",
                (72.0, 8.0, 7.0),
                (-46.0, y, 53.0),
                timber,
                0.8,
                (0.0, math.radians(float(angle)), 0.0),
            )
        )
    return parts


def build_handcart() -> tuple[bpy.types.Object, list[bpy.types.Object]]:
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

    parts: list[bpy.types.Object] = []

    # The load bed and its frame carry the purposeful broad collision volume.
    parts.extend(
        [
            add_cube("BedFloor", (210.0, 100.0, 16.0), (0.0, 0.0, 91.0), timber, 3.0),
            add_cube("LeftSide", (210.0, 14.0, 52.0), (0.0, -56.0, 122.0), timber, 3.0),
            add_cube("RightSide", (210.0, 14.0, 52.0), (0.0, 56.0, 122.0), timber, 3.0),
            add_cube("RearGate", (14.0, 112.0, 52.0), (-105.0, 0.0, 122.0), timber, 3.0),
            add_cube("FrontLip", (14.0, 112.0, 28.0), (105.0, 0.0, 109.0), timber, 3.0),
            add_cube("RearCrossmember", (22.0, 124.0, 16.0), (-66.0, 0.0, 77.0), timber, 2.0),
            add_cube("MidCrossmember", (22.0, 124.0, 16.0), (8.0, 0.0, 77.0), timber, 2.0),
            add_cube("FrontCrossmember", (22.0, 124.0, 16.0), (82.0, 0.0, 77.0), timber, 2.0),
        ]
    )

    # Two wheels share one iron axle. The authored axle collision approximates
    # the grounded wheel zone while individual rims and spokes remain decorative.
    parts.append(
        add_cylinder(
            "Axle",
            12,
            8.0,
            168.0,
            (-46.0, 0.0, 53.0),
            iron,
            (math.radians(90.0), 0.0, 0.0),
            1.0,
        )
    )
    parts.extend(add_wheel("Left", -78.0, timber, iron))
    parts.extend(add_wheel("Right", 78.0, timber, iron))

    # Front rests keep the parked cart grounded. Handles and grips are
    # deliberately non-colliding so they cannot snag the nearby cottage path.
    for side, y in (("Left", -38.0), ("Right", 38.0)):
        parts.extend(
            [
                add_cube(f"{side}Rest", (16.0, 16.0, 82.0), (84.0, y, 41.0), timber, 2.0),
                add_cube(f"{side}RestFoot", (28.0, 28.0, 12.0), (84.0, y, 6.0), stone, 2.0),
                add_cube(
                    f"{side}Handle",
                    (184.0, 14.0, 14.0),
                    (190.0, y, 86.0),
                    timber,
                    2.0,
                    (0.0, math.radians(-4.0), 0.0),
                ),
                add_cube(f"{side}Grip", (38.0, 22.0, 20.0), (280.0, y, 79.5), iron, 3.0),
            ]
        )

    # Supplies break up the silhouette and make the prop read as a working
    # provision cart rather than an empty vehicle.
    parts.extend(
        [
            add_cube("RearCrate", (66.0, 58.0, 58.0), (-51.0, -18.0, 158.0), timber, 3.0, (0.0, 0.0, math.radians(-4.0))),
            add_cube("FrontCrate", (72.0, 60.0, 46.0), (33.0, 24.0, 152.0), timber, 3.0, (0.0, 0.0, math.radians(3.0))),
            add_cube("RearCrateBand", (70.0, 7.0, 62.0), (-51.0, -18.0, 158.0), iron, 1.0, (0.0, 0.0, math.radians(-4.0))),
            add_cube("FrontCrateBand", (76.0, 7.0, 50.0), (33.0, 24.0, 152.0), iron, 1.0, (0.0, 0.0, math.radians(3.0))),
        ]
    )

    bpy.ops.mesh.primitive_uv_sphere_add(
        segments=16,
        ring_count=8,
        location=(58.0, -28.0, 158.0),
        scale=(34.0, 27.0, 31.0),
    )
    sack = bpy.context.object
    sack.name = "MossWrappedSupply"
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    sack.data.materials.append(moss)
    parts.append(sack)
    parts.append(
        add_cylinder(
            "SupplyTie",
            12,
            5.0,
            9.0,
            (58.0, -28.0, 190.0),
            iron,
            bevel=0.8,
        )
    )

    # A small rear crest carries the village's ember signature without making
    # the cart look ceremonial or introducing gameplay promises.
    parts.extend(
        [
            add_cylinder("RearCrestIron", 12, 20.0, 8.0, (-116.0, 0.0, 127.0), iron, (0.0, math.radians(90.0), 0.0), 1.2),
            add_cylinder("RearCrestStone", 12, 13.0, 9.0, (-121.0, 0.0, 127.0), stone, (0.0, math.radians(90.0), 0.0), 1.0),
            add_ember_gem("RearCrestEmber", (-128.0, 0.0, 127.0), (4.0, 8.0, 8.0), ember),
        ]
    )

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
    if not (405.0 <= width <= 435.0 and 165.0 <= depth <= 180.0 and 190.0 <= height <= 215.0):
        errors.append(f"Unexpected dimensions: {metrics['dimensions_cm']}")
    if metrics["triangles"] > 8000:
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
    if len(collision) != 4 or any(
        not obj.name.startswith(COLLISION_PREFIX) for obj in collision
    ):
        errors.append("Expected four correctly named Unreal collision boxes")
    if errors:
        raise RuntimeError(
            "Embermere Fenwatch handcart validation failed: " + "; ".join(errors)
        )
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(render_mesh: bpy.types.Object, preview_path: Path) -> None:
    ground_material = make_material("M_PreviewGround", (0.075, 0.11, 0.075, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=760.0, location=(70.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_material)

    bpy.ops.object.light_add(type="SUN", rotation=(math.radians(35.0), math.radians(-25.0), math.radians(-35.0)))
    sun = bpy.context.object
    sun.name = "Preview_Sun"
    sun.data.energy = 2.7
    sun.data.angle = math.radians(18.0)

    bpy.ops.object.light_add(type="AREA", location=(-320.0, 470.0, 360.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 2400000.0
    key.data.shape = "DISK"
    key.data.size = 270.0
    look_at(key, (45.0, 0.0, 100.0))

    bpy.ops.object.light_add(type="AREA", location=(430.0, -300.0, 260.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 1050000.0
    fill.data.color = (0.35, 0.5, 0.78)
    fill.data.size = 240.0
    look_at(fill, (45.0, 0.0, 100.0))

    bpy.ops.object.light_add(type="POINT", location=(-120.0, 30.0, 135.0))
    glow = bpy.context.object
    glow.name = "Preview_EmberGlow"
    glow.data.energy = 55000.0
    glow.data.color = (1.0, 0.16, 0.025)
    glow.data.shadow_soft_size = 45.0

    bpy.ops.object.camera_add(location=(510.0, 560.0, 285.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 58.0
    camera.data.clip_end = 3000.0
    look_at(camera, (45.0, 0.0, 100.0))
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


def export_fbx(
    render_mesh: bpy.types.Object,
    collision: list[bpy.types.Object],
    fbx_path: Path,
) -> None:
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
    render_mesh, collision = build_handcart()
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
