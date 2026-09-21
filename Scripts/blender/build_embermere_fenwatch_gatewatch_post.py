"""Build Embermere's original Fenwatch eastern gatewatch post.

Run this reviewed scene-resetting script through Blender MCP in a factory-clean
headless process. The asset is a compact open-front roadside watch shelter that
uses the shared Fenwatch stone, moss, timber, iron, and ember language. It
exports seven purposeful UBX colliders, renders a preview, records deterministic
metrics, and saves the editable Blender source alongside the classic FBX.
"""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereFenwatchGatewatchPost_01"
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
            project_root
            / "ArtSource/Blender/Environment/FenwatchGatewatchPost",
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
    scene.render.resolution_x = 1100
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
        emission_input = node.inputs.get("Emission Color") or node.inputs.get(
            "Emission"
        )
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


def add_cone(
    name: str,
    vertices: int,
    radius1: float,
    radius2: float,
    depth: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cone_add(
        vertices=vertices,
        radius1=radius1,
        radius2=radius2,
        depth=depth,
        location=location,
        rotation=rotation,
    )
    obj = bpy.context.object
    obj.name = name
    obj.data.materials.append(material)
    return obj


def add_torus(
    name: str,
    major_radius: float,
    minor_radius: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_torus_add(
        major_radius=major_radius,
        minor_radius=minor_radius,
        major_segments=16,
        minor_segments=6,
        location=location,
        rotation=rotation,
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
        ((400.0, 250.0, 46.0), (0.0, 0.0, 23.0)),
        ((180.0, 62.0, 16.0), (0.0, -153.0, 8.0)),
        ((370.0, 24.0, 166.0), (0.0, 104.0, 132.0)),
        ((32.0, 32.0, 264.0), (-174.0, -96.0, 178.0)),
        ((32.0, 32.0, 264.0), (174.0, -96.0, 178.0)),
        ((32.0, 32.0, 264.0), (-174.0, 96.0, 178.0)),
        ((32.0, 32.0, 264.0), (174.0, 96.0, 178.0)),
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


def build_gatewatch_post() -> tuple[bpy.types.Object, list[bpy.types.Object]]:
    stone = make_material("M_Waystone", (0.34, 0.40, 0.37, 1.0), 0.88)
    moss = make_material("M_WaystoneMoss", (0.25, 0.39, 0.16, 1.0), 0.95)
    iron = make_material(
        "M_EmberLampIron", (0.055, 0.07, 0.075, 1.0), 0.62, 0.65
    )
    timber = make_material("M_EmbermereTimber", (0.24, 0.105, 0.035, 1.0), 0.9)
    ember = make_material(
        "M_WaystoneEmber",
        (0.58, 0.12, 0.025, 1.0),
        0.5,
        emission_color=(1.0, 0.18, 0.025, 1.0),
        emission_strength=7.0,
    )

    parts: list[bpy.types.Object] = []

    # A low stepped foundation and road-facing entry make the post usable as a
    # visual shelter without turning it into a service or interaction owner.
    parts.extend(
        [
            add_cube("FoundationLower", (430.0, 280.0, 24.0), (0.0, 0.0, 12.0), stone, 4.0),
            add_cube("FoundationMossCourse", (410.0, 260.0, 8.0), (0.0, 0.0, 28.0), moss, 2.0),
            add_cube("TimberDeck", (400.0, 250.0, 18.0), (0.0, 0.0, 39.0), timber, 3.0),
            add_cube("EntryStep", (180.0, 62.0, 16.0), (0.0, -153.0, 8.0), stone, 3.0),
            add_cube("EntryStepMoss", (164.0, 48.0, 6.0), (0.0, -153.0, 19.0), moss, 1.5),
        ]
    )

    # Four structural posts and a solid rear half-wall leave the road-facing
    # side open. Only these major forms receive authored collision.
    for side, x in (("Left", -174.0), ("Right", 174.0)):
        for lane, y in (("Front", -96.0), ("Rear", 96.0)):
            parts.extend(
                [
                    add_cube(f"{side}{lane}Post", (32.0, 32.0, 264.0), (x, y, 178.0), timber, 3.0),
                    add_cube(f"{side}{lane}IronBoot", (40.0, 40.0, 18.0), (x, y, 55.0), iron, 2.0),
                    add_cube(f"{side}{lane}MossFoot", (44.0, 44.0, 8.0), (x, y, 43.0), moss, 1.5),
                ]
            )
    parts.extend(
        [
            add_cube("RearStoneWall", (370.0, 24.0, 118.0), (0.0, 104.0, 108.0), stone, 4.0),
            add_cube("RearTimberPanel", (370.0, 18.0, 48.0), (0.0, 103.0, 190.0), timber, 3.0),
            add_cube("RearMossCap", (388.0, 32.0, 16.0), (0.0, 104.0, 222.0), moss, 3.0),
            add_cube("FrontHeader", (402.0, 34.0, 32.0), (0.0, -96.0, 310.0), timber, 4.0),
            add_cube("RearHeader", (402.0, 34.0, 32.0), (0.0, 96.0, 310.0), timber, 4.0),
            add_cube("LeftHeader", (34.0, 226.0, 30.0), (-174.0, 0.0, 310.0), timber, 4.0),
            add_cube("RightHeader", (34.0, 226.0, 30.0), (174.0, 0.0, 310.0), timber, 4.0),
            add_cube("FrontBraceLeft", (92.0, 20.0, 20.0), (-137.0, -105.0, 270.0), timber, 2.0, (0.0, math.radians(-43.0), 0.0)),
            add_cube("FrontBraceRight", (92.0, 20.0, 20.0), (137.0, -105.0, 270.0), timber, 2.0, (0.0, math.radians(43.0), 0.0)),
        ]
    )

    # Decorative side railings define the platform edge but remain non-solid
    # so the compact roadside landmark cannot snag normal traversal.
    for side, x in (("Left", -174.0), ("Right", 174.0)):
        parts.extend(
            [
                add_cube(f"{side}RailingTop", (18.0, 150.0, 18.0), (x, 10.0, 138.0), timber, 2.0),
                add_cube(f"{side}RailingLow", (16.0, 150.0, 16.0), (x, 10.0, 92.0), timber, 2.0),
                add_cube(f"{side}RailingFront", (18.0, 18.0, 82.0), (x, -58.0, 104.0), timber, 2.0),
                add_cube(f"{side}RailingRear", (18.0, 18.0, 82.0), (x, 78.0, 104.0), timber, 2.0),
            ]
        )

    # A bell, map shelf, signal horn, and two spears communicate watch duty.
    # They are presentation detail only and never become interaction targets.
    parts.extend(
        [
            add_cylinder("BellCord", 8, 3.0, 54.0, (0.0, -101.0, 270.0), timber),
            add_cone("WatchBell", 16, 25.0, 11.0, 34.0, (0.0, -101.0, 235.0), iron),
            add_cylinder("BellClapper", 12, 5.0, 16.0, (0.0, -101.0, 214.0), iron),
            add_cube("MapShelf", (252.0, 58.0, 16.0), (0.0, 69.0, 245.0), timber, 3.0),
            add_cube("MapBoard", (210.0, 8.0, 86.0), (0.0, 99.0, 257.0), stone, 2.0),
            add_cube("MapMarkLeft", (42.0, 5.0, 8.0), (-48.0, 94.0, 264.0), ember, 1.0, (0.0, math.radians(-18.0), 0.0)),
            add_cube("MapMarkRight", (54.0, 5.0, 8.0), (42.0, 94.0, 248.0), ember, 1.0, (0.0, math.radians(22.0), 0.0)),
            add_cylinder("SignalHornBody", 12, 10.0, 72.0, (-104.0, 60.0, 252.0), iron, (0.0, math.radians(64.0), 0.0)),
            add_cone("SignalHornMouth", 12, 20.0, 10.0, 28.0, (-133.0, 60.0, 266.0), iron, (0.0, math.radians(64.0), 0.0)),
            add_cube("SpearLeftShaft", (10.0, 10.0, 184.0), (-132.0, 91.0, 188.0), timber, 1.5, (0.0, math.radians(-8.0), 0.0)),
            add_cone("SpearLeftHead", 8, 11.0, 2.0, 34.0, (-119.0, 91.0, 293.0), iron, (0.0, math.radians(-8.0), 0.0)),
            add_cube("SpearRightShaft", (10.0, 10.0, 184.0), (132.0, 91.0, 188.0), timber, 1.5, (0.0, math.radians(8.0), 0.0)),
            add_cone("SpearRightHead", 8, 11.0, 2.0, 34.0, (119.0, 91.0, 293.0), iron, (0.0, math.radians(8.0), 0.0)),
        ]
    )

    # A steep moss gable and suspended ember crest give the small post enough
    # silhouette to mark the transition from Fenwatch to the dangerous road.
    roof_angle = math.radians(29.0)
    parts.extend(
        [
            add_cube("RoofFront", (470.0, 174.0, 18.0), (0.0, -58.0, 352.0), moss, 3.0, (roof_angle, 0.0, 0.0)),
            add_cube("RoofRear", (470.0, 174.0, 18.0), (0.0, 58.0, 352.0), moss, 3.0, (-roof_angle, 0.0, 0.0)),
            add_cube("RoofRidge", (482.0, 22.0, 24.0), (0.0, 0.0, 402.0), iron, 3.0),
            add_cube("FrontFascia", (474.0, 18.0, 24.0), (0.0, -134.0, 310.0), timber, 3.0),
            add_cube("RearFascia", (474.0, 18.0, 24.0), (0.0, 134.0, 310.0), timber, 3.0),
            add_torus("FrontCrestIron", 28.0, 5.0, (0.0, -145.0, 326.0), iron, (math.radians(90.0), 0.0, 0.0)),
            add_cylinder("FrontCrestStone", 12, 18.0, 10.0, (0.0, -151.0, 326.0), stone, (math.radians(90.0), 0.0, 0.0), 1.0),
            add_ember_gem("FrontCrestEmber", (0.0, -159.0, 326.0), (7.0, 4.0, 12.0), ember),
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
    if not (465.0 <= width <= 490.0 and 290.0 <= depth <= 330.0 and 390.0 <= height <= 425.0):
        errors.append(f"Unexpected dimensions: {metrics['dimensions_cm']}")
    if metrics["triangles"] > 12000:
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
    if len(collision) != 7 or any(
        not obj.name.startswith(COLLISION_PREFIX) for obj in collision
    ):
        errors.append("Expected seven correctly named Unreal collision boxes")
    if errors:
        raise RuntimeError(
            "Embermere Fenwatch gatewatch post validation failed: "
            + "; ".join(errors)
        )
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(render_mesh: bpy.types.Object, preview_path: Path) -> None:
    ground_material = make_material("M_PreviewGround", (0.075, 0.11, 0.075, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=1050.0, location=(0.0, 0.0, -0.5))
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

    bpy.ops.object.light_add(type="AREA", location=(-430.0, -620.0, 520.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 3200000.0
    key.data.shape = "DISK"
    key.data.size = 320.0
    look_at(key, (0.0, 0.0, 190.0))

    bpy.ops.object.light_add(type="AREA", location=(430.0, 340.0, 410.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 1400000.0
    fill.data.color = (0.35, 0.5, 0.78)
    fill.data.size = 280.0
    look_at(fill, (0.0, 0.0, 190.0))

    bpy.ops.object.light_add(type="POINT", location=(0.0, -170.0, 300.0))
    glow = bpy.context.object
    glow.name = "Preview_EmberGlow"
    glow.data.energy = 95000.0
    glow.data.color = (1.0, 0.16, 0.025)
    glow.data.shadow_soft_size = 75.0

    bpy.ops.object.camera_add(location=(760.0, -930.0, 560.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 56.0
    camera.data.clip_end = 5000.0
    look_at(camera, (0.0, 0.0, 190.0))
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
    render_mesh, collision = build_gatewatch_post()
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
