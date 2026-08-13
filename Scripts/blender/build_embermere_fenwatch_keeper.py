"""Build Mara's original static Fenwatch keeper presentation.

Run this reviewed script through Blender MCP with inline code disabled. It
creates a bounded Stylized Classic quest-giver proxy, renders a preview,
exports classic-FBX-compatible source, records deterministic metrics, and
saves the editable Blender file. Gameplay interaction, quest data, and world
collision remain owned by the Unreal Blueprint rather than this art asset.
"""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereFenwatchKeeper_Mara_01"


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
            project_root / "ArtSource/Blender/Characters/NPCs/FenwatchKeeper",
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


def configure_scene() -> None:
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 0.01
    scene.unit_settings.length_unit = "CENTIMETERS"
    scene.render.resolution_x = 1100
    scene.render.resolution_y = 800
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    scene.render.film_transparent = False
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


def smooth_mesh(obj: bpy.types.Object) -> None:
    if obj.type != "MESH":
        return
    for polygon in obj.data.polygons:
        polygon.use_smooth = True


def apply_bevel(obj: bpy.types.Object, width_cm: float, segments: int = 2) -> None:
    modifier = obj.modifiers.new(name="StylizedBevel", type="BEVEL")
    modifier.width = width_cm
    modifier.segments = segments
    modifier.limit_method = "ANGLE"
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)


def add_box(
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


def add_ellipsoid(
    name: str,
    dimensions: tuple[float, float, float],
    location: tuple[float, float, float],
    material: bpy.types.Material,
    segments: int = 16,
    rings: int = 10,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_uv_sphere_add(
        segments=segments,
        ring_count=rings,
        location=location,
    )
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    smooth_mesh(obj)
    return obj


def add_tapered_body(
    name: str,
    radius_bottom: float,
    radius_top: float,
    depth: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    vertices: int = 12,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cone_add(
        vertices=vertices,
        radius1=radius_bottom,
        radius2=radius_top,
        depth=depth,
        location=location,
    )
    obj = bpy.context.object
    obj.name = name
    obj.data.materials.append(material)
    smooth_mesh(obj)
    return obj


def add_cylinder_between(
    name: str,
    start: tuple[float, float, float],
    end: tuple[float, float, float],
    radius: float,
    material: bpy.types.Material,
    vertices: int = 12,
) -> bpy.types.Object:
    start_vector = Vector(start)
    end_vector = Vector(end)
    direction = end_vector - start_vector
    midpoint = (start_vector + end_vector) * 0.5
    bpy.ops.mesh.primitive_cylinder_add(
        vertices=vertices,
        radius=radius,
        depth=direction.length,
        location=midpoint,
    )
    obj = bpy.context.object
    obj.name = name
    obj.rotation_euler = direction.to_track_quat("Z", "Y").to_euler()
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    obj.data.materials.append(material)
    smooth_mesh(obj)
    return obj


def add_face_disc(
    name: str,
    radius: float,
    depth: float,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    vertices: int = 12,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cylinder_add(
        vertices=vertices,
        radius=radius,
        depth=depth,
        location=location,
        rotation=(math.radians(90.0), 0.0, 0.0),
    )
    obj = bpy.context.object
    obj.name = name
    obj.data.materials.append(material)
    return obj


def add_trapezoid_prism(
    name: str,
    top_width: float,
    bottom_width: float,
    height: float,
    depth: float,
    center_y: float,
    bottom_z: float,
    material: bpy.types.Material,
) -> bpy.types.Object:
    half_top = top_width * 0.5
    half_bottom = bottom_width * 0.5
    half_depth = depth * 0.5
    top_z = bottom_z + height
    vertices = [
        (-half_bottom, center_y - half_depth, bottom_z),
        (half_bottom, center_y - half_depth, bottom_z),
        (-half_top, center_y - half_depth, top_z),
        (half_top, center_y - half_depth, top_z),
        (-half_bottom, center_y + half_depth, bottom_z),
        (half_bottom, center_y + half_depth, bottom_z),
        (-half_top, center_y + half_depth, top_z),
        (half_top, center_y + half_depth, top_z),
    ]
    faces = [
        (0, 1, 3, 2),
        (4, 6, 7, 5),
        (0, 4, 5, 1),
        (2, 3, 7, 6),
        (0, 2, 6, 4),
        (1, 5, 7, 3),
    ]
    mesh = bpy.data.meshes.new(f"{name}_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    apply_bevel(obj, 2.0, 2)
    return obj


def add_belt(material: bpy.types.Material) -> bpy.types.Object:
    bpy.ops.mesh.primitive_torus_add(
        align="WORLD",
        major_segments=12,
        minor_segments=4,
        location=(0.0, 0.0, 104.0),
        major_radius=28.0,
        minor_radius=3.8,
    )
    belt = bpy.context.object
    belt.name = "LeatherBelt"
    belt.data.materials.append(material)
    return belt


def join_render_mesh(parts: list[bpy.types.Object]) -> bpy.types.Object:
    bpy.ops.object.select_all(action="DESELECT")
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    result = bpy.context.object
    result.name = ASSET_NAME
    result.data.name = f"{ASSET_NAME}_Mesh"
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66.0), island_margin=0.02)
    bpy.ops.object.mode_set(mode="OBJECT")

    triangulate = result.modifiers.new(name="ExportTriangulation", type="TRIANGULATE")
    bpy.context.view_layer.objects.active = result
    bpy.ops.object.modifier_apply(modifier=triangulate.name)
    return result


def build_keeper_parts() -> list[bpy.types.Object]:
    skin = make_material("M_FenwatchKeeperSkin", (0.67, 0.37, 0.22, 1.0), 0.82)
    stone = make_material("M_Waystone", (0.34, 0.40, 0.37, 1.0), 0.88)
    moss = make_material("M_WaystoneMoss", (0.25, 0.39, 0.16, 1.0), 0.95)
    timber = make_material("M_EmbermereTimber", (0.24, 0.105, 0.035, 1.0), 0.9)
    iron = make_material("M_EmberLampIron", (0.055, 0.07, 0.075, 1.0), 0.62, 0.65)
    ember = make_material(
        "M_WaystoneEmber",
        (0.58, 0.12, 0.025, 1.0),
        0.5,
        emission_color=(1.0, 0.18, 0.025, 1.0),
        emission_strength=7.0,
    )

    parts: list[bpy.types.Object] = []

    # Grounded boots and a layered field-keeper silhouette.
    parts.extend(
        [
            add_box("BootL", (22.0, 35.0, 14.0), (-15.0, -5.0, 7.0), iron, 4.0),
            add_box("BootR", (22.0, 35.0, 14.0), (15.0, -5.0, 7.0), iron, 4.0),
            add_cylinder_between("ShinL", (-15.0, 0.0, 15.0), (-15.0, 0.0, 52.0), 9.0, timber),
            add_cylinder_between("ShinR", (15.0, 0.0, 15.0), (15.0, 0.0, 52.0), 9.0, timber),
            add_tapered_body("StoneUnderskirt", 35.0, 25.0, 67.0, (0.0, 0.0, 68.0), stone),
            add_tapered_body("LeatherBodice", 27.0, 33.0, 62.0, (0.0, 0.0, 128.0), timber),
            add_belt(timber),
            add_trapezoid_prism("FenwatchCape", 70.0, 48.0, 116.0, 8.0, 18.0, 47.0, moss),
            add_ellipsoid("MossCowl", (78.0, 43.0, 25.0), (0.0, 0.0, 157.0), moss),
        ]
    )

    # Relaxed arms keep the static proxy readable without implying combat.
    parts.extend(
        [
            add_cylinder_between("UpperArmL", (-31.0, 0.0, 148.0), (-39.0, -3.0, 118.0), 10.0, moss),
            add_cylinder_between("ForearmL", (-39.0, -3.0, 118.0), (-43.0, -9.0, 94.0), 8.0, timber),
            add_ellipsoid("HandL", (15.0, 13.0, 18.0), (-43.0, -9.0, 88.0), skin, 12, 8),
            add_cylinder_between("UpperArmR", (31.0, 0.0, 148.0), (38.0, -2.0, 119.0), 10.0, moss),
            add_cylinder_between("ForearmR", (38.0, -2.0, 119.0), (34.0, -10.0, 92.0), 8.0, timber),
            add_ellipsoid("HandR", (15.0, 13.0, 18.0), (34.0, -10.0, 86.0), skin, 12, 8),
        ]
    )

    # Warm face, auburn hair, and restrained features read at gameplay distance.
    parts.extend(
        [
            add_ellipsoid("HairBack", (43.0, 31.0, 53.0), (0.0, 7.0, 181.0), timber),
            add_ellipsoid("Face", (36.0, 32.0, 43.0), (0.0, -3.0, 184.0), skin),
            add_ellipsoid("HairLockL", (10.0, 12.0, 42.0), (-17.0, 2.0, 166.0), timber, 12, 8),
            add_ellipsoid("HairLockR", (10.0, 12.0, 42.0), (17.0, 2.0, 166.0), timber, 12, 8),
            add_ellipsoid("EyeL", (4.0, 3.0, 4.0), (-7.0, -18.5, 190.0), iron, 8, 6),
            add_ellipsoid("EyeR", (4.0, 3.0, 4.0), (7.0, -18.5, 190.0), iron, 8, 6),
            add_cylinder_between("Nose", (0.0, -18.0, 184.0), (0.0, -23.0, 181.0), 2.7, skin, 8),
        ]
    )

    # The Embermere crest and field satchel establish role and setting identity.
    parts.extend(
        [
            add_face_disc("CrestStone", 14.0, 5.0, (0.0, -25.0, 153.0), stone),
            add_face_disc("CrestIron", 9.0, 7.0, (0.0, -29.0, 153.0), iron),
            add_ellipsoid("CrestEmber", (7.0, 4.0, 12.0), (0.0, -34.0, 153.0), ember, 8, 6),
            add_box("FieldSatchel", (29.0, 15.0, 34.0), (29.0, 8.0, 73.0), timber, 3.0),
            add_box("SatchelClasp", (9.0, 5.0, 9.0), (29.0, -1.0, 76.0), ember, 1.5),
        ]
    )

    # A slim staff gives Mara a quest-giver silhouette without adding collision.
    parts.extend(
        [
            add_cylinder_between("FenwatchStaff", (-48.0, -8.0, 4.0), (-48.0, -8.0, 179.0), 4.0, timber, 12),
            add_ellipsoid("StaffStone", (24.0, 18.0, 27.0), (-48.0, -8.0, 188.0), stone, 12, 8),
            add_ellipsoid("StaffEmber", (8.0, 7.0, 13.0), (-48.0, -17.0, 188.0), ember, 8, 6),
            add_box("StaffIronBand", (13.0, 13.0, 9.0), (-48.0, -8.0, 164.0), iron, 2.0),
        ]
    )

    return parts


def build_keeper() -> bpy.types.Object:
    return join_render_mesh(build_keeper_parts())


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


def validate_asset(render_mesh: bpy.types.Object) -> dict:
    metrics = mesh_metrics(render_mesh)
    errors = []
    width, depth, height = metrics["dimensions_cm"]
    if not (90.0 <= width <= 125.0 and 45.0 <= depth <= 90.0 and 190.0 <= height <= 215.0):
        errors.append(f"Unexpected dimensions: {metrics['dimensions_cm']}")
    if not (1000 <= metrics["triangles"] <= 8000):
        errors.append(f"Unexpected triangle count: {metrics['triangles']}")
    if metrics["uv_channels"] < 1:
        errors.append("Render mesh has no UV channel")
    if metrics["non_manifold_edges"] != 0:
        errors.append(f"Non-manifold edges found: {metrics['non_manifold_edges']}")
    if metrics["scale"] != [1.0, 1.0, 1.0]:
        errors.append(f"Transforms are not applied: {metrics['scale']}")
    if abs(metrics["origin_z_cm"]) > 0.001:
        errors.append(f"Origin must remain at ground level: {metrics['origin_z_cm']}")
    expected_materials = {
        "M_FenwatchKeeperSkin",
        "M_Waystone",
        "M_WaystoneMoss",
        "M_EmbermereTimber",
        "M_EmberLampIron",
        "M_WaystoneEmber",
    }
    if set(metrics["materials"]) != expected_materials:
        errors.append(f"Material slots drifted: {metrics['materials']}")
    if errors:
        raise RuntimeError("Embermere Fenwatch keeper validation failed: " + "; ".join(errors))
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(render_mesh: bpy.types.Object, preview_path: Path) -> None:
    ground_material = make_material("M_PreviewGround", (0.075, 0.11, 0.075, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=620.0, location=(0.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_material)

    bpy.ops.object.light_add(
        type="SUN",
        rotation=(math.radians(35.0), math.radians(-25.0), math.radians(-35.0)),
    )
    sun = bpy.context.object
    sun.name = "Preview_Sun"
    sun.data.energy = 2.6
    sun.data.angle = math.radians(18.0)

    bpy.ops.object.light_add(type="AREA", location=(-250.0, -360.0, 300.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 1700000.0
    key.data.shape = "DISK"
    key.data.size = 220.0
    look_at(key, (0.0, 0.0, 105.0))

    bpy.ops.object.light_add(type="AREA", location=(240.0, 160.0, 240.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 850000.0
    fill.data.color = (0.32, 0.48, 0.75)
    fill.data.size = 200.0
    look_at(fill, (0.0, 0.0, 105.0))

    bpy.ops.object.light_add(type="POINT", location=(-22.0, -70.0, 155.0))
    glow = bpy.context.object
    glow.name = "Preview_EmberGlow"
    glow.data.energy = 35000.0
    glow.data.color = (1.0, 0.16, 0.025)
    glow.data.shadow_soft_size = 45.0

    bpy.ops.object.camera_add(location=(255.0, -410.0, 235.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 62.0
    look_at(camera, (0.0, 0.0, 105.0))
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
    configure_scene()
    render_mesh = build_keeper()
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


if not globals().get("EMBERMERE_IMPORT_ONLY", False):
    __result__ = main()
