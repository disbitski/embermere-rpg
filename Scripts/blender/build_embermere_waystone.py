"""Build the first original Embermere static prop in Blender.

Run this reviewed script through Blender MCP with inline code disabled, or from
Blender directly. The script resets the current scene, builds a stylized
waystone shrine, validates the render mesh, saves source, renders a preview,
and exports the render mesh plus Unreal collision as FBX.
"""

from __future__ import annotations

import bmesh
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SM_EmbermereWaystone_01"
COLLISION_PREFIX = f"UBX_{ASSET_NAME}"


def get_parameters() -> dict:
    supplied = globals().get("args", {})
    return supplied if isinstance(supplied, dict) else {}


def resolve_output_paths(parameters: dict) -> tuple[Path, Path, Path]:
    project_root = Path(
        parameters.get("project_root", "/Users/wizard/Documents/Unreal Game")
    ).expanduser()
    output_dir = Path(
        parameters.get(
            "output_dir",
            project_root / "ArtSource/Blender/Environment/WaystoneShrine",
        )
    ).expanduser()
    output_dir.mkdir(parents=True, exist_ok=True)
    return (
        output_dir / f"{ASSET_NAME}.blend",
        output_dir / f"{ASSET_NAME}.fbx",
        output_dir / f"{ASSET_NAME}_preview.png",
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
    scene.render.resolution_x = 768
    scene.render.resolution_y = 768
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


def add_tapered_monolith(material: bpy.types.Material) -> bpy.types.Object:
    vertices = [
        (-43.0, -28.0, 38.0),
        (48.0, -25.0, 38.0),
        (43.0, 27.0, 38.0),
        (-46.0, 25.0, 38.0),
        (-27.0, -19.0, 178.0),
        (31.0, -17.0, 198.0),
        (28.0, 18.0, 194.0),
        (-30.0, 20.0, 181.0),
    ]
    faces = [
        (0, 3, 2, 1),
        (4, 5, 6, 7),
        (0, 1, 5, 4),
        (1, 2, 6, 5),
        (2, 3, 7, 6),
        (3, 0, 4, 7),
    ]
    mesh = bpy.data.meshes.new("WaystoneMonolithMesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new("WaystoneMonolith", mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    apply_bevel(obj, 4.0, 3)
    return obj


def add_rune_parts(material: bpy.types.Material) -> list[bpy.types.Object]:
    front_y = -29.2
    parts = [
        add_cube(
            "RuneCore",
            (13.0, 3.0, 13.0),
            (0.0, front_y, 125.0),
            material,
            1.2,
            (0.0, math.radians(45.0), 0.0),
        ),
        add_cube(
            "RuneStem",
            (6.0, 3.0, 66.0),
            (0.0, front_y, 108.0),
            material,
            1.0,
        ),
        add_cube(
            "RuneLeftBranch",
            (6.0, 3.0, 39.0),
            (-12.0, front_y, 93.0),
            material,
            1.0,
            (0.0, math.radians(-38.0), 0.0),
        ),
        add_cube(
            "RuneRightBranch",
            (6.0, 3.0, 39.0),
            (12.0, front_y, 93.0),
            material,
            1.0,
            (0.0, math.radians(38.0), 0.0),
        ),
    ]
    bpy.ops.mesh.primitive_torus_add(
        major_radius=28.0,
        minor_radius=2.5,
        major_segments=24,
        minor_segments=6,
        location=(0.0, front_y + 0.1, 117.0),
        rotation=(math.radians(90.0), 0.0, 0.0),
    )
    halo = bpy.context.object
    halo.name = "RuneHalo"
    halo.data.materials.append(material)
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    parts.append(halo)
    return parts


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
    collision_material = make_material(
        "M_CollisionPreview", (0.1, 0.6, 0.2, 0.25), roughness=1.0
    )
    base = add_cube(
        f"{COLLISION_PREFIX}_00",
        (126.0, 86.0, 38.0),
        (0.0, 0.0, 19.0),
        collision_material,
    )
    upright = add_cube(
        f"{COLLISION_PREFIX}_01",
        (92.0, 58.0, 152.0),
        (0.0, 0.0, 114.0),
        collision_material,
    )
    for obj in (base, upright):
        obj.display_type = "WIRE"
        obj.hide_render = True
    return [base, upright]


def build_waystone() -> tuple[bpy.types.Object, list[bpy.types.Object]]:
    stone = make_material("M_Waystone", (0.34, 0.40, 0.37, 1.0), 0.88)
    moss = make_material("M_WaystoneMoss", (0.25, 0.39, 0.16, 1.0), 0.95)
    ember = make_material(
        "M_WaystoneEmber",
        (0.58, 0.12, 0.025, 1.0),
        0.5,
        emission_color=(1.0, 0.18, 0.025, 1.0),
        emission_strength=5.0,
    )

    render_parts = [
        add_cube("WaystoneBase", (126.0, 86.0, 24.0), (0.0, 0.0, 12.0), stone, 5.0, (0.0, 0.0, math.radians(-3.0))),
        add_cube("WaystoneStep", (102.0, 67.0, 18.0), (2.0, -1.0, 31.0), moss, 4.0, (0.0, 0.0, math.radians(2.0))),
        add_tapered_monolith(stone),
        add_cube("SideStoneLeft", (36.0, 43.0, 55.0), (-61.0, 7.0, 31.0), stone, 5.0, (math.radians(-5.0), math.radians(9.0), math.radians(-12.0))),
        add_cube("SideStoneRight", (31.0, 37.0, 44.0), (58.0, -5.0, 26.0), moss, 5.0, (math.radians(4.0), math.radians(-8.0), math.radians(14.0))),
    ]
    render_parts.extend(add_rune_parts(ember))
    render_mesh = join_render_mesh(render_parts)
    collision = add_collision()
    return render_mesh, collision


def mesh_metrics(obj: bpy.types.Object) -> dict:
    mesh = obj.data
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold_edges = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()

    dimensions = [round(value, 3) for value in obj.dimensions]
    triangle_count = sum(max(1, len(poly.vertices) - 2) for poly in mesh.polygons)
    return {
        "dimensions_cm": dimensions,
        "triangles": triangle_count,
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
    if not (120.0 <= width <= 170.0 and 70.0 <= depth <= 110.0 and 175.0 <= height <= 215.0):
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
    if len(collision) != 2 or any(not obj.name.startswith(COLLISION_PREFIX) for obj in collision):
        errors.append("Expected two correctly named Unreal collision boxes")
    if errors:
        raise RuntimeError("Waystone validation failed: " + "; ".join(errors))
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(render_mesh: bpy.types.Object, preview_path: Path) -> None:
    ground_material = make_material("M_PreviewGround", (0.075, 0.11, 0.075, 1.0), 1.0)
    bpy.ops.mesh.primitive_plane_add(size=900.0, location=(0.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_material)

    bpy.ops.object.light_add(
        type="SUN",
        rotation=(math.radians(35.0), math.radians(-25.0), math.radians(-35.0)),
    )
    sun = bpy.context.object
    sun.name = "Preview_Sun"
    sun.data.energy = 3.0
    sun.data.angle = math.radians(18.0)

    bpy.ops.object.light_add(type="AREA", location=(-260.0, -300.0, 340.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 3200000.0
    key.data.shape = "DISK"
    key.data.size = 260.0
    look_at(key, (0.0, 0.0, 90.0))

    bpy.ops.object.light_add(type="AREA", location=(240.0, -40.0, 180.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 1700000.0
    fill.data.color = (0.38, 0.52, 0.72)
    fill.data.size = 220.0
    look_at(fill, (0.0, 0.0, 95.0))

    bpy.ops.object.light_add(type="AREA", location=(20.0, 240.0, 220.0))
    rim = bpy.context.object
    rim.name = "Preview_Rim"
    rim.data.energy = 2100000.0
    rim.data.color = (1.0, 0.34, 0.12)
    rim.data.size = 180.0
    look_at(rim, (0.0, 0.0, 105.0))

    bpy.ops.object.camera_add(location=(315.0, -385.0, 245.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 58.0
    look_at(camera, (0.0, 0.0, 88.0))
    bpy.context.scene.camera = camera

    world = bpy.context.scene.world or bpy.data.worlds.new("EmbermerePreviewWorld")
    bpy.context.scene.world = world
    world.use_nodes = True
    background = world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.025, 0.055, 0.08, 1.0)
    background.inputs["Strength"].default_value = 1.1

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
        add_leaf_bones=False,
        bake_anim=False,
    )


def main() -> dict:
    parameters = get_parameters()
    blend_path, fbx_path, preview_path = resolve_output_paths(parameters)
    reset_scene()
    configure_scene_units()
    render_mesh, collision = build_waystone()
    metrics = validate_asset(render_mesh, collision)
    export_fbx(render_mesh, collision, fbx_path)
    add_preview_scene(render_mesh, preview_path)
    bpy.ops.wm.save_as_mainfile(filepath=str(blend_path))
    return {
        "asset": ASSET_NAME,
        "blend": str(blend_path),
        "fbx": str(fbx_path),
        "preview": str(preview_path),
        "metrics": metrics,
    }


__result__ = main()
print("EMBERMERE_ASSET_RESULT", __result__)
