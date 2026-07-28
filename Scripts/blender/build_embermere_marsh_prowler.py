"""Build Embermere's first original Marsh Prowler skeletal creature.

Run this reviewed script through Blender MCP with inline code disabled. It
creates a stylized marsh dire wolf, a deterministic quadruped skeleton,
in-place prototype animation actions, skeletal FBX exports, a preview render,
editable source, and machine-readable acceptance metrics.
"""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path

import bpy
from mathutils import Vector


ASSET_NAME = "SK_EmbermereMarshProwler_01"
ARMATURE_NAME = "SKEL_EmbermereMarshProwler_01"
ACTION_PREFIX = "A_EmbermereMarshProwler"


def get_parameters() -> dict:
    supplied = globals().get("args", {})
    return supplied if isinstance(supplied, dict) else {}


def resolve_output_paths(parameters: dict) -> dict[str, Path]:
    project_root = Path(
        parameters.get("project_root", "/Users/wizard/Documents/Unreal Game")
    ).expanduser()
    output_dir = Path(
        parameters.get(
            "output_dir",
            project_root / "ArtSource/Blender/Characters/Enemies/MarshProwler",
        )
    ).expanduser()
    output_dir.mkdir(parents=True, exist_ok=True)
    animation_dir = output_dir / "Animations"
    animation_dir.mkdir(parents=True, exist_ok=True)
    return {
        "blend": output_dir / f"{ASSET_NAME}.blend",
        "mesh_fbx": output_dir / f"{ASSET_NAME}.fbx",
        "preview": output_dir / f"{ASSET_NAME}_preview.png",
        "metrics": output_dir / f"{ASSET_NAME}_metrics.json",
        "animation_dir": animation_dir,
    }


def reset_scene() -> None:
    if bpy.context.object and bpy.context.object.mode != "OBJECT":
        bpy.ops.object.mode_set(mode="OBJECT")
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for action in list(bpy.data.actions):
        bpy.data.actions.remove(action)
    for datablocks in (
        bpy.data.meshes,
        bpy.data.curves,
        bpy.data.armatures,
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
    scene.render.fps = 30
    try:
        scene.view_settings.look = "AgX - Medium High Contrast"
    except TypeError:
        pass
    engines = {
        item.identifier
        for item in scene.render.bl_rna.properties["engine"].enum_items
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


def smooth_mesh(obj: bpy.types.Object) -> None:
    if obj.type != "MESH":
        return
    for polygon in obj.data.polygons:
        polygon.use_smooth = True


def assign_rigid_group(obj: bpy.types.Object, bone_name: str) -> None:
    group = obj.vertex_groups.new(name=bone_name)
    group.add(list(range(len(obj.data.vertices))), 1.0, "REPLACE")


def add_ellipsoid(
    name: str,
    location: tuple[float, float, float],
    dimensions: tuple[float, float, float],
    material: bpy.types.Material,
    bone_name: str,
    segments: int = 20,
    rings: int = 12,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_uv_sphere_add(
        segments=segments,
        ring_count=rings,
        location=location,
        rotation=rotation,
    )
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    smooth_mesh(obj)
    assign_rigid_group(obj, bone_name)
    return obj


def add_cone_between(
    name: str,
    start: tuple[float, float, float],
    end: tuple[float, float, float],
    radius_start: float,
    radius_end: float,
    material: bpy.types.Material,
    bone_name: str,
    vertices: int = 14,
) -> bpy.types.Object:
    start_vector = Vector(start)
    end_vector = Vector(end)
    direction = end_vector - start_vector
    midpoint = (start_vector + end_vector) * 0.5
    bpy.ops.mesh.primitive_cone_add(
        vertices=vertices,
        radius1=radius_start,
        radius2=radius_end,
        depth=direction.length,
        location=midpoint,
    )
    obj = bpy.context.object
    obj.name = name
    obj.rotation_euler = direction.to_track_quat("Z", "Y").to_euler()
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    obj.data.materials.append(material)
    smooth_mesh(obj)
    assign_rigid_group(obj, bone_name)
    return obj


def add_box(
    name: str,
    location: tuple[float, float, float],
    dimensions: tuple[float, float, float],
    material: bpy.types.Material,
    bone_name: str,
    rotation: tuple[float, float, float] = (0.0, 0.0, 0.0),
    bevel: float = 0.0,
) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    if bevel > 0.0:
        modifier = obj.modifiers.new(name="StylizedBevel", type="BEVEL")
        modifier.width = bevel
        modifier.segments = 2
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=modifier.name)
    smooth_mesh(obj)
    assign_rigid_group(obj, bone_name)
    return obj


def add_triangular_prism(
    name: str,
    profile_xz: tuple[
        tuple[float, float],
        tuple[float, float],
        tuple[float, float],
    ],
    center_y: float,
    thickness: float,
    material: bpy.types.Material,
    bone_name: str,
) -> bpy.types.Object:
    """Create a closed, flattened triangular form for readable wolf ears."""
    half_thickness = thickness * 0.5
    vertices = [
        (x, center_y - half_thickness, z) for x, z in profile_xz
    ] + [
        (x, center_y + half_thickness, z) for x, z in profile_xz
    ]
    faces = [
        (0, 2, 1),
        (3, 4, 5),
        (0, 1, 4, 3),
        (1, 2, 5, 4),
        (2, 0, 3, 5),
    ]
    mesh = bpy.data.meshes.new(f"{name}_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    assign_rigid_group(obj, bone_name)
    return obj


def add_fur_tuft(
    name: str,
    base: tuple[float, float, float],
    tip: tuple[float, float, float],
    width: float,
    material: bpy.types.Material,
    bone_name: str,
) -> bpy.types.Object:
    return add_cone_between(
        name,
        base,
        tip,
        width,
        0.8,
        material,
        bone_name,
        vertices=7,
    )


def add_eye(
    name: str,
    location: tuple[float, float, float],
    material: bpy.types.Material,
    bone_name: str,
) -> bpy.types.Object:
    return add_ellipsoid(
        name,
        location,
        (6.5, 3.8, 5.0),
        material,
        bone_name,
        segments=12,
        rings=8,
    )


def create_materials() -> dict[str, bpy.types.Material]:
    return {
        "fur": make_material(
            "M_ProwlerFur",
            (0.095, 0.075, 0.055, 1.0),
            0.92,
        ),
        "mane": make_material(
            "M_ProwlerManeMoss",
            (0.12, 0.19, 0.075, 1.0),
            0.96,
        ),
        "mud": make_material(
            "M_ProwlerMudDark",
            (0.035, 0.05, 0.045, 1.0),
            0.88,
        ),
        "bone": make_material(
            "M_ProwlerBone",
            (0.63, 0.55, 0.39, 1.0),
            0.72,
        ),
        "eyes": make_material(
            "M_ProwlerEyes",
            (0.62, 0.22, 0.025, 1.0),
            0.42,
            emission_color=(1.0, 0.22, 0.015, 1.0),
            emission_strength=3.5,
        ),
    }


def add_leg(
    side_name: str,
    side_y: float,
    front: bool,
    materials: dict[str, bpy.types.Material],
) -> list[bpy.types.Object]:
    side_suffix = "l" if side_y > 0.0 else "r"
    if front:
        prefix = f"front_{side_suffix}"
        shoulder = (48.0, side_y, 130.0)
        elbow = (53.0, side_y, 73.0)
        wrist = (60.0, side_y, 18.0)
        paw_center = (79.0, side_y, 10.0)
        paw_dimensions = (42.0, 24.0, 18.0)
        upper_bone = f"{prefix}_upper"
        lower_bone = f"{prefix}_lower"
        paw_bone = f"{prefix}_paw"
        upper_radius = (17.0, 12.0)
        lower_radius = (12.0, 7.0)
    else:
        prefix = f"hind_{side_suffix}"
        shoulder = (-48.0, side_y, 118.0)
        elbow = (-86.0, side_y, 73.0)
        wrist = (-57.0, side_y, 38.0)
        ankle = (-63.0, side_y, 17.0)
        paw_center = (-43.0, side_y, 9.5)
        paw_dimensions = (44.0, 25.0, 18.0)
        upper_bone = f"{prefix}_upper"
        lower_bone = f"{prefix}_lower"
        paw_bone = f"{prefix}_paw"
        upper_radius = (23.0, 14.0)
        lower_radius = (14.0, 9.0)

    parts = [
        add_cone_between(
            f"{side_name}_Upper",
            shoulder,
            elbow,
            upper_radius[0],
            upper_radius[1],
            materials["fur"],
            upper_bone,
            16,
        ),
        add_cone_between(
            f"{side_name}_Lower",
            elbow,
            wrist,
            lower_radius[0],
            lower_radius[1],
            materials["mud"],
            lower_bone,
            14,
        ),
    ]
    if not front:
        parts.append(
            add_cone_between(
                f"{side_name}_Hock",
                wrist,
                ankle,
                9.0,
                6.5,
                materials["mud"],
                paw_bone,
                12,
            )
        )
    joint_location = wrist if front else ankle
    parts.append(
        add_ellipsoid(
            f"{side_name}_FootJoint",
            joint_location,
            (18.0, 18.0, 18.0),
            materials["mud"],
            paw_bone,
            segments=12,
            rings=8,
        )
    )
    parts.append(
        add_ellipsoid(
            f"{side_name}_Paw",
            paw_center,
            paw_dimensions,
            materials["mud"],
            paw_bone,
            segments=14,
            rings=8,
        )
    )
    claw_x = paw_center[0] + paw_dimensions[0] * 0.45
    for index, offset_y in enumerate((-7.0, 0.0, 7.0)):
        parts.append(
            add_cone_between(
                f"{side_name}_Claw_{index}",
                (claw_x - 2.0, side_y + offset_y, 12.0),
                (claw_x + 8.0, side_y + offset_y, 8.0),
                2.6,
                0.3,
                materials["mud"],
                paw_bone,
                8,
            )
        )
    return parts


def build_prowler_parts(
    materials: dict[str, bpy.types.Material],
) -> list[bpy.types.Object]:
    parts = [
        add_ellipsoid(
            "ProwlerPelvis",
            (-51.0, 0.0, 111.0),
            (96.0, 62.0, 78.0),
            materials["fur"],
            "pelvis",
            22,
            14,
        ),
        add_ellipsoid(
            "ProwlerTorso",
            (0.0, 0.0, 121.0),
            (140.0, 66.0, 78.0),
            materials["fur"],
            "spine_01",
            24,
            16,
        ),
        add_ellipsoid(
            "ProwlerShoulders",
            (48.0, 0.0, 135.0),
            (82.0, 74.0, 94.0),
            materials["mane"],
            "spine_02",
            22,
            14,
        ),
        add_ellipsoid(
            "ProwlerNeck",
            (87.0, 0.0, 151.0),
            (74.0, 57.0, 72.0),
            materials["mane"],
            "neck_01",
            20,
            12,
            rotation=(0.0, math.radians(-11.0), 0.0),
        ),
        add_ellipsoid(
            "ProwlerHead",
            (132.0, 0.0, 157.0),
            (70.0, 50.0, 57.0),
            materials["fur"],
            "head",
            22,
            14,
        ),
        add_cone_between(
            "ProwlerMuzzle",
            (147.0, 0.0, 153.0),
            (194.0, 0.0, 144.0),
            19.0,
            10.5,
            materials["fur"],
            "head",
            18,
        ),
        add_ellipsoid(
            "ProwlerNose",
            (198.0, 0.0, 143.0),
            (16.0, 23.0, 18.0),
            materials["mud"],
            "head",
            14,
            9,
        ),
        add_ellipsoid(
            "ProwlerMouth",
            (171.0, 0.0, 132.0),
            (58.0, 30.0, 12.0),
            materials["mud"],
            "head",
            segments=16,
            rings=8,
            rotation=(0.0, math.radians(-5.0), 0.0),
        ),
        add_ellipsoid(
            "ProwlerJaw",
            (170.0, 0.0, 125.0),
            (58.0, 29.0, 13.0),
            materials["fur"],
            "jaw",
            segments=16,
            rings=8,
            rotation=(0.0, math.radians(-5.0), 0.0),
        ),
        add_triangular_prism(
            "ProwlerEar_L",
            ((108.0, 174.0), (133.0, 173.0), (109.0, 210.0)),
            19.0,
            8.0,
            materials["mane"],
            "ear_l",
        ),
        add_triangular_prism(
            "ProwlerEar_R",
            ((108.0, 174.0), (133.0, 173.0), (109.0, 210.0)),
            -19.0,
            8.0,
            materials["mane"],
            "ear_r",
        ),
        add_eye(
            "ProwlerEye_L",
            (155.0, 23.8, 162.0),
            materials["eyes"],
            "head",
        ),
        add_eye(
            "ProwlerEye_R",
            (155.0, -23.8, 162.0),
            materials["eyes"],
            "head",
        ),
        add_ellipsoid(
            "ProwlerBrow_L",
            (150.0, 23.0, 170.0),
            (31.0, 9.0, 8.0),
            materials["mud"],
            "head",
            segments=12,
            rings=8,
            rotation=(math.radians(-8.0), math.radians(8.0), math.radians(-12.0)),
        ),
        add_ellipsoid(
            "ProwlerBrow_R",
            (150.0, -23.0, 170.0),
            (31.0, 9.0, 8.0),
            materials["mud"],
            "head",
            segments=12,
            rings=8,
            rotation=(math.radians(8.0), math.radians(8.0), math.radians(12.0)),
        ),
    ]

    for side_name, side_y, front in (
        ("ProwlerFrontLeg_L", 31.0, True),
        ("ProwlerFrontLeg_R", -31.0, True),
        ("ProwlerHindLeg_L", 29.0, False),
        ("ProwlerHindLeg_R", -29.0, False),
    ):
        parts.extend(add_leg(side_name, side_y, front, materials))

    tail_points = [
        (-82.0, 0.0, 108.0),
        (-120.0, 0.0, 112.0),
        (-154.0, 0.0, 104.0),
        (-183.0, 0.0, 91.0),
        (-207.0, 0.0, 74.0),
    ]
    tail_radii = [(22.0, 17.0), (17.0, 13.0), (13.0, 9.0), (9.0, 3.0)]
    for index, (start, end) in enumerate(zip(tail_points, tail_points[1:]), 1):
        parts.append(
            add_cone_between(
                f"ProwlerTail_{index:02d}",
                start,
                end,
                tail_radii[index - 1][0],
                tail_radii[index - 1][1],
                materials["fur" if index < 3 else "mane"],
                f"tail_{index:02d}",
                14,
            )
        )

    for side_y in (-11.0, 11.0):
        for tooth_index, x_value in enumerate((162.0, 174.0, 186.0)):
            parts.append(
                add_cone_between(
                    f"ProwlerUpperTooth_{side_y}_{tooth_index}",
                    (x_value, side_y, 133.0),
                    (x_value + 1.0, side_y, 122.0),
                    3.2 if tooth_index != 1 else 4.0,
                    0.4,
                    materials["bone"],
                    "head",
                    8,
                )
            )
            parts.append(
                add_cone_between(
                    f"ProwlerLowerTooth_{side_y}_{tooth_index}",
                    (x_value + 2.0, side_y, 119.0),
                    (x_value + 1.0, side_y, 128.0),
                    2.6,
                    0.4,
                    materials["bone"],
                    "jaw",
                    8,
                )
            )

    mane_tufts = [
        ((22.0, 0.0, 168.0), (7.0, 0.0, 181.0), 6.0, "spine_02"),
        ((42.0, 0.0, 177.0), (27.0, 0.0, 191.0), 6.5, "spine_02"),
        ((59.0, 13.0, 176.0), (44.0, 19.0, 190.0), 5.8, "spine_02"),
        ((59.0, -13.0, 176.0), (44.0, -19.0, 190.0), 5.8, "spine_02"),
        ((76.0, 15.0, 177.0), (62.0, 22.0, 190.0), 5.3, "neck_01"),
        ((76.0, -15.0, 177.0), (62.0, -22.0, 190.0), 5.3, "neck_01"),
        ((94.0, 16.0, 179.0), (82.0, 23.0, 192.0), 4.8, "neck_02"),
        ((94.0, -16.0, 179.0), (82.0, -23.0, 192.0), 4.8, "neck_02"),
    ]
    for index, (base, tip, width, bone_name) in enumerate(mane_tufts):
        parts.append(
            add_fur_tuft(
                f"ProwlerManeTuft_{index:02d}",
                base,
                tip,
                width,
                materials["mane"],
                bone_name,
            )
        )

    cheek_tufts = [
        ((137.0, 24.0, 148.0), (124.0, 40.0, 144.0), "head"),
        ((137.0, -24.0, 148.0), (124.0, -40.0, 144.0), "head"),
        ((127.0, 22.0, 136.0), (114.0, 37.0, 127.0), "head"),
        ((127.0, -22.0, 136.0), (114.0, -37.0, 127.0), "head"),
    ]
    for index, (base, tip, bone_name) in enumerate(cheek_tufts):
        parts.append(
            add_fur_tuft(
                f"ProwlerCheekTuft_{index:02d}",
                base,
                tip,
                6.0,
                materials["mane"],
                bone_name,
            )
        )
    return parts


def create_armature() -> bpy.types.Object:
    bpy.ops.object.armature_add(enter_editmode=True, location=(0.0, 0.0, 0.0))
    armature = bpy.context.object
    armature.name = ARMATURE_NAME
    armature.data.name = f"{ARMATURE_NAME}_Data"
    armature.data.display_type = "STICK"
    edit_bones = armature.data.edit_bones
    edit_bones.remove(edit_bones[0])

    bone_specs = [
        ("root", (0.0, 0.0, 0.0), (0.0, 0.0, 24.0), None),
        ("pelvis", (-51.0, 0.0, 110.0), (-20.0, 0.0, 118.0), "root"),
        ("spine_01", (-20.0, 0.0, 118.0), (18.0, 0.0, 127.0), "pelvis"),
        ("spine_02", (18.0, 0.0, 127.0), (54.0, 0.0, 139.0), "spine_01"),
        ("neck_01", (54.0, 0.0, 139.0), (87.0, 0.0, 152.0), "spine_02"),
        ("neck_02", (87.0, 0.0, 152.0), (116.0, 0.0, 157.0), "neck_01"),
        ("head", (116.0, 0.0, 157.0), (164.0, 0.0, 148.0), "neck_02"),
        ("jaw", (143.0, 0.0, 133.0), (187.0, 0.0, 125.0), "head"),
        ("ear_l", (121.0, 19.0, 174.0), (109.0, 19.0, 210.0), "head"),
        ("ear_r", (121.0, -19.0, 174.0), (109.0, -19.0, 210.0), "head"),
        ("tail_01", (-50.0, 0.0, 104.0), (-120.0, 0.0, 112.0), "pelvis"),
        ("tail_02", (-120.0, 0.0, 112.0), (-154.0, 0.0, 104.0), "tail_01"),
        ("tail_03", (-154.0, 0.0, 104.0), (-183.0, 0.0, 91.0), "tail_02"),
        ("tail_04", (-183.0, 0.0, 91.0), (-207.0, 0.0, 74.0), "tail_03"),
    ]
    for side_y, suffix in ((31.0, "l"), (-31.0, "r")):
        bone_specs.extend(
            [
                (
                    f"front_{suffix}_upper",
                    (48.0, side_y, 130.0),
                    (53.0, side_y, 73.0),
                    "spine_02",
                ),
                (
                    f"front_{suffix}_lower",
                    (53.0, side_y, 73.0),
                    (60.0, side_y, 18.0),
                    f"front_{suffix}_upper",
                ),
                (
                    f"front_{suffix}_paw",
                    (60.0, side_y, 18.0),
                    (100.0, side_y, 9.0),
                    f"front_{suffix}_lower",
                ),
            ]
        )
    for side_y, suffix in ((29.0, "l"), (-29.0, "r")):
        bone_specs.extend(
            [
                (
                    f"hind_{suffix}_upper",
                    (-48.0, side_y, 118.0),
                    (-86.0, side_y, 73.0),
                    "pelvis",
                ),
                (
                    f"hind_{suffix}_lower",
                    (-86.0, side_y, 73.0),
                    (-57.0, side_y, 38.0),
                    f"hind_{suffix}_upper",
                ),
                (
                    f"hind_{suffix}_paw",
                    (-57.0, side_y, 38.0),
                    (-21.0, side_y, 9.0),
                    f"hind_{suffix}_lower",
                ),
            ]
        )

    created = {}
    for name, head, tail, parent_name in bone_specs:
        bone = edit_bones.new(name)
        bone.head = head
        bone.tail = tail
        bone.use_deform = True
        if parent_name:
            bone.parent = created[parent_name]
        created[name] = bone
    bpy.ops.object.mode_set(mode="OBJECT")
    armature.show_in_front = True
    return armature


def join_creature(
    parts: list[bpy.types.Object],
    armature: bpy.types.Object,
) -> bpy.types.Object:
    bpy.ops.object.select_all(action="DESELECT")
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    creature = bpy.context.object
    creature.name = ASSET_NAME
    creature.data.name = f"{ASSET_NAME}_Mesh"
    bpy.context.scene.cursor.location = (0.0, 0.0, 0.0)
    bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)

    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(64.0), island_margin=0.02)
    bpy.ops.object.mode_set(mode="OBJECT")
    triangulate = creature.modifiers.new(name="ExportTriangulation", type="TRIANGULATE")
    bpy.context.view_layer.objects.active = creature
    bpy.ops.object.modifier_apply(modifier=triangulate.name)

    modifier = creature.modifiers.new(name="Armature", type="ARMATURE")
    modifier.object = armature
    creature.parent = armature
    return creature


def reset_pose(armature: bpy.types.Object) -> None:
    for pose_bone in armature.pose.bones:
        pose_bone.rotation_mode = "XYZ"
        pose_bone.location = (0.0, 0.0, 0.0)
        pose_bone.rotation_euler = (0.0, 0.0, 0.0)
        pose_bone.scale = (1.0, 1.0, 1.0)


def key_bone(
    armature: bpy.types.Object,
    bone_name: str,
    frame: int,
    rotation_degrees: tuple[float, float, float] = (0.0, 0.0, 0.0),
    location: tuple[float, float, float] = (0.0, 0.0, 0.0),
) -> None:
    bone = armature.pose.bones[bone_name]
    bone.rotation_mode = "XYZ"
    bone.rotation_euler = tuple(math.radians(value) for value in rotation_degrees)
    bone.location = location
    bone.keyframe_insert("rotation_euler", frame=frame, group=bone_name)
    bone.keyframe_insert("location", frame=frame, group=bone_name)


def create_action(
    armature: bpy.types.Object,
    name: str,
    end_frame: int,
) -> bpy.types.Action:
    action = bpy.data.actions.new(name=name)
    action.use_fake_user = True
    armature.animation_data_create()
    armature.animation_data.action = action
    bpy.context.scene.frame_start = 1
    bpy.context.scene.frame_end = end_frame
    reset_pose(armature)
    for pose_bone in armature.pose.bones:
        for frame in (1, end_frame):
            pose_bone.keyframe_insert("rotation_euler", frame=frame, group=pose_bone.name)
            pose_bone.keyframe_insert("location", frame=frame, group=pose_bone.name)
            pose_bone.keyframe_insert("scale", frame=frame, group=pose_bone.name)
    return action


def animate_idle(armature: bpy.types.Object) -> bpy.types.Action:
    action = create_action(armature, f"{ACTION_PREFIX}_Idle", 49)
    for frame, bob, chest, head, tail in (
        (1, 0.0, 0.0, -2.0, -5.0),
        (13, 2.5, 1.8, 1.5, 8.0),
        (25, 0.0, 0.0, 3.0, 5.0),
        (37, -1.5, -1.5, -1.0, -8.0),
        (49, 0.0, 0.0, -2.0, -5.0),
    ):
        key_bone(armature, "pelvis", frame, location=(0.0, 0.0, bob))
        key_bone(armature, "spine_02", frame, (0.0, chest, 0.0))
        key_bone(armature, "head", frame, (0.0, head, 0.0))
        key_bone(armature, "jaw", frame, (0.0, 0.0, 2.0))
        key_bone(armature, "tail_01", frame, (0.0, 0.0, tail))
        key_bone(armature, "tail_02", frame, (0.0, 0.0, tail * 0.8))
        key_bone(armature, "tail_03", frame, (0.0, 0.0, tail * 0.65))
        key_bone(armature, "tail_04", frame, (0.0, 0.0, tail * 0.5))
        key_bone(armature, "ear_l", frame, (0.0, 0.0, 4.0 if frame == 25 else 0.0))
        key_bone(armature, "ear_r", frame, (0.0, 0.0, -5.0 if frame == 37 else 0.0))
    return action


def animate_gait(
    armature: bpy.types.Object,
    name: str,
    end_frame: int,
    stride: float,
    lower_bend: float,
    body_bob: float,
    head_pitch: float,
) -> bpy.types.Action:
    action = create_action(armature, name, end_frame)
    quarter = (end_frame - 1) // 4
    frames = [1, 1 + quarter, 1 + quarter * 2, 1 + quarter * 3, end_frame]
    phases = [1.0, 0.0, -1.0, 0.0, 1.0]
    for frame, phase in zip(frames, phases):
        key_bone(
            armature,
            "pelvis",
            frame,
            (0.0, 0.0, -phase * 1.5),
            (0.0, 0.0, body_bob * (1.0 - abs(phase))),
        )
        key_bone(armature, "spine_01", frame, (0.0, phase * 2.0, 0.0))
        key_bone(armature, "spine_02", frame, (0.0, -phase * 2.0, 0.0))
        key_bone(armature, "head", frame, (0.0, head_pitch + abs(phase) * 2.0, 0.0))
        key_bone(armature, "tail_01", frame, (0.0, 0.0, -phase * 10.0))
        key_bone(armature, "tail_02", frame, (0.0, 0.0, -phase * 8.0))

        leg_phases = {
            "front_l": phase,
            "front_r": -phase,
            "hind_l": -phase,
            "hind_r": phase,
        }
        for prefix, leg_phase in leg_phases.items():
            key_bone(
                armature,
                f"{prefix}_upper",
                frame,
                (0.0, 0.0, leg_phase * stride),
            )
            bend = max(0.0, leg_phase) * lower_bend
            key_bone(
                armature,
                f"{prefix}_lower",
                frame,
                (0.0, 0.0, -bend),
            )
            key_bone(
                armature,
                f"{prefix}_paw",
                frame,
                (0.0, 0.0, bend * 0.55),
            )
    return action


def animate_attack(armature: bpy.types.Object) -> bpy.types.Action:
    action = create_action(armature, f"{ACTION_PREFIX}_Attack", 25)
    for frame, pelvis_x, chest_pitch, neck_pitch, jaw_open, paw_swing in (
        (1, 0.0, 0.0, 0.0, 3.0, 0.0),
        (6, -8.0, -7.0, 10.0, 5.0, -18.0),
        (12, 18.0, 12.0, -24.0, -25.0, 42.0),
        (18, 6.0, 4.0, -8.0, -8.0, 18.0),
        (25, 0.0, 0.0, 0.0, 3.0, 0.0),
    ):
        key_bone(armature, "pelvis", frame, location=(pelvis_x, 0.0, 0.0))
        key_bone(armature, "spine_02", frame, (0.0, chest_pitch, 0.0))
        key_bone(armature, "neck_01", frame, (0.0, neck_pitch * 0.6, 0.0))
        key_bone(armature, "head", frame, (0.0, neck_pitch, 0.0))
        key_bone(armature, "jaw", frame, (0.0, 0.0, jaw_open))
        key_bone(armature, "front_l_upper", frame, (0.0, 0.0, paw_swing))
        key_bone(armature, "front_l_lower", frame, (0.0, 0.0, -abs(paw_swing) * 0.6))
    return action


def animate_hit(armature: bpy.types.Object) -> bpy.types.Action:
    action = create_action(armature, f"{ACTION_PREFIX}_Hit", 17)
    for frame, recoil, twist in (
        (1, 0.0, 0.0),
        (5, -12.0, 9.0),
        (10, 7.0, -5.0),
        (17, 0.0, 0.0),
    ):
        key_bone(armature, "pelvis", frame, location=(recoil, 0.0, 0.0))
        key_bone(armature, "spine_01", frame, (twist, recoil * 0.3, 0.0))
        key_bone(armature, "neck_01", frame, (-twist * 0.7, -recoil * 0.5, 0.0))
        key_bone(armature, "head", frame, (twist * 0.5, -recoil * 0.7, 0.0))
    return action


def animate_death(armature: bpy.types.Object) -> bpy.types.Action:
    action = create_action(armature, f"{ACTION_PREFIX}_Death", 41)
    for frame, drop, roll, neck, legs in (
        (1, 0.0, 0.0, 0.0, 0.0),
        (12, -8.0, 18.0, -10.0, 16.0),
        (24, -42.0, 58.0, -25.0, 38.0),
        (34, -72.0, 82.0, -35.0, 54.0),
        (41, -76.0, 88.0, -38.0, 58.0),
    ):
        key_bone(
            armature,
            "root",
            frame,
            (roll, 0.0, 0.0),
            (0.0, 0.0, drop),
        )
        key_bone(armature, "neck_01", frame, (0.0, neck, 0.0))
        key_bone(armature, "head", frame, (0.0, neck * 0.6, 0.0))
        for prefix in ("front_l", "front_r", "hind_l", "hind_r"):
            key_bone(armature, f"{prefix}_upper", frame, (0.0, 0.0, legs))
            key_bone(armature, f"{prefix}_lower", frame, (0.0, 0.0, -legs * 0.65))
    return action


def create_actions(armature: bpy.types.Object) -> list[bpy.types.Action]:
    actions = [
        animate_idle(armature),
        animate_gait(
            armature,
            f"{ACTION_PREFIX}_Walk",
            33,
            24.0,
            24.0,
            3.0,
            -3.0,
        ),
        animate_gait(
            armature,
            f"{ACTION_PREFIX}_Run",
            25,
            38.0,
            34.0,
            5.0,
            -8.0,
        ),
        animate_attack(armature),
        animate_hit(armature),
        animate_death(armature),
    ]
    armature.animation_data.action = actions[0]
    bpy.context.scene.frame_start = 1
    bpy.context.scene.frame_end = 49
    bpy.context.scene.frame_set(1)
    return actions


def mesh_metrics(
    creature: bpy.types.Object,
    armature: bpy.types.Object,
    actions: list[bpy.types.Action],
) -> dict:
    mesh = creature.data
    world_positions = [creature.matrix_world @ vertex.co for vertex in mesh.vertices]
    minimum = [
        min(position[index] for position in world_positions) for index in range(3)
    ]
    maximum = [
        max(position[index] for position in world_positions) for index in range(3)
    ]
    dimensions = [
        maximum[index] - minimum[index] for index in range(3)
    ]
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold_edges = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    return {
        "dimensions_cm": [round(value, 3) for value in dimensions],
        "bounds_min_cm": [round(value, 3) for value in minimum],
        "bounds_max_cm": [round(value, 3) for value in maximum],
        "triangles": sum(max(1, len(poly.vertices) - 2) for poly in mesh.polygons),
        "vertices": len(mesh.vertices),
        "materials": [
            slot.material.name for slot in creature.material_slots if slot.material
        ],
        "uv_channels": len(mesh.uv_layers),
        "non_manifold_edges": non_manifold_edges,
        "scale": [round(value, 6) for value in creature.scale],
        "origin_cm": [round(value, 6) for value in creature.location],
        "bone_count": len(armature.data.bones),
        "bones": [bone.name for bone in armature.data.bones],
        "actions": {
            action.name: [
                int(round(action.frame_range[0])),
                int(round(action.frame_range[1])),
            ]
            for action in actions
        },
    }


def validate_asset(
    creature: bpy.types.Object,
    armature: bpy.types.Object,
    actions: list[bpy.types.Action],
) -> dict:
    metrics = mesh_metrics(creature, armature, actions)
    errors = []
    length, width, height = metrics["dimensions_cm"]
    if not (360.0 <= length <= 430.0):
        errors.append(f"Unexpected nose-to-tail bounds: {length}")
    if not (85.0 <= width <= 115.0):
        errors.append(f"Unexpected creature width: {width}")
    if not (195.0 <= height <= 225.0):
        errors.append(f"Unexpected creature height: {height}")
    if not (5000 <= metrics["triangles"] <= 20000):
        errors.append(f"Triangle count outside first-pass budget: {metrics['triangles']}")
    if metrics["uv_channels"] < 1:
        errors.append("Creature has no UV channel")
    if metrics["non_manifold_edges"] != 0:
        errors.append(f"Non-manifold edges found: {metrics['non_manifold_edges']}")
    if metrics["scale"] != [1.0, 1.0, 1.0]:
        errors.append(f"Creature transforms are not applied: {metrics['scale']}")
    if metrics["origin_cm"] != [0.0, 0.0, 0.0]:
        errors.append(f"Creature origin must remain at world origin: {metrics['origin_cm']}")
    expected_materials = {
        "M_ProwlerFur",
        "M_ProwlerManeMoss",
        "M_ProwlerMudDark",
        "M_ProwlerBone",
        "M_ProwlerEyes",
    }
    if set(metrics["materials"]) != expected_materials:
        errors.append(f"Material set drifted: {metrics['materials']}")
    required_bones = {
        "root",
        "pelvis",
        "spine_01",
        "spine_02",
        "neck_01",
        "neck_02",
        "head",
        "jaw",
        "tail_04",
        "front_l_paw",
        "front_r_paw",
        "hind_l_paw",
        "hind_r_paw",
    }
    if not required_bones.issubset(set(metrics["bones"])):
        errors.append("Required quadruped bones are missing")
    required_actions = {
        f"{ACTION_PREFIX}_Idle",
        f"{ACTION_PREFIX}_Walk",
        f"{ACTION_PREFIX}_Run",
        f"{ACTION_PREFIX}_Attack",
        f"{ACTION_PREFIX}_Hit",
        f"{ACTION_PREFIX}_Death",
    }
    if set(metrics["actions"]) != required_actions:
        errors.append(f"Animation action set drifted: {sorted(metrics['actions'])}")
    if errors:
        raise RuntimeError("Marsh Prowler validation failed: " + "; ".join(errors))
    return metrics


def look_at(obj: bpy.types.Object, point: tuple[float, float, float]) -> None:
    direction = Vector(point) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_preview_scene(
    creature: bpy.types.Object,
    armature: bpy.types.Object,
    preview_path: Path,
) -> None:
    ground_material = make_material(
        "M_ProwlerPreviewGround",
        (0.055, 0.09, 0.06, 1.0),
        0.98,
    )
    bpy.ops.mesh.primitive_plane_add(size=900.0, location=(0.0, 0.0, -0.5))
    ground = bpy.context.object
    ground.name = "Preview_Ground"
    ground.data.materials.append(ground_material)

    bpy.ops.object.light_add(
        type="SUN",
        rotation=(math.radians(28.0), math.radians(-22.0), math.radians(-38.0)),
    )
    sun = bpy.context.object
    sun.name = "Preview_Sun"
    sun.data.energy = 2.2
    sun.data.angle = math.radians(16.0)

    bpy.ops.object.light_add(type="AREA", location=(330.0, -380.0, 320.0))
    key = bpy.context.object
    key.name = "Preview_Key"
    key.data.energy = 2500000.0
    key.data.shape = "DISK"
    key.data.size = 260.0
    look_at(key, (15.0, 0.0, 105.0))

    bpy.ops.object.light_add(type="AREA", location=(-260.0, 250.0, 240.0))
    fill = bpy.context.object
    fill.name = "Preview_Fill"
    fill.data.energy = 900000.0
    fill.data.color = (0.22, 0.42, 0.35)
    fill.data.size = 300.0
    look_at(fill, (0.0, 0.0, 100.0))

    bpy.ops.object.light_add(type="POINT", location=(185.0, -85.0, 155.0))
    eye_glow = bpy.context.object
    eye_glow.name = "Preview_EyeGlow"
    eye_glow.data.energy = 35000.0
    eye_glow.data.color = (1.0, 0.18, 0.02)
    eye_glow.data.shadow_soft_size = 35.0

    bpy.ops.object.camera_add(location=(260.0, -650.0, 245.0))
    camera = bpy.context.object
    camera.name = "Preview_Camera"
    camera.data.lens = 58.0
    look_at(camera, (5.0, 0.0, 102.0))
    bpy.context.scene.camera = camera

    world = bpy.context.scene.world or bpy.data.worlds.new("ProwlerPreviewWorld")
    bpy.context.scene.world = world
    world.use_nodes = True
    background = world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.018, 0.035, 0.045, 1.0)
    background.inputs["Strength"].default_value = 0.8

    armature.hide_render = True
    creature.hide_render = False
    bpy.context.scene.frame_set(1)
    bpy.context.scene.render.filepath = str(preview_path)
    bpy.ops.render.render(write_still=True)


def select_export_objects(
    creature: bpy.types.Object,
    armature: bpy.types.Object,
) -> None:
    bpy.ops.object.select_all(action="DESELECT")
    creature.select_set(True)
    armature.select_set(True)
    bpy.context.view_layer.objects.active = armature


def export_skeletal_mesh(
    creature: bpy.types.Object,
    armature: bpy.types.Object,
    fbx_path: Path,
) -> None:
    select_export_objects(creature, armature)
    armature.data.pose_position = "REST"
    bpy.ops.export_scene.fbx(
        filepath=str(fbx_path),
        use_selection=True,
        object_types={"ARMATURE", "MESH"},
        global_scale=1.0,
        apply_unit_scale=True,
        axis_forward="-Y",
        axis_up="Z",
        add_leaf_bones=False,
        mesh_smooth_type="FACE",
        use_armature_deform_only=True,
        bake_anim=False,
    )
    armature.data.pose_position = "POSE"


def export_actions(
    creature: bpy.types.Object,
    armature: bpy.types.Object,
    actions: list[bpy.types.Action],
    animation_dir: Path,
) -> dict[str, str]:
    exported = {}
    for action in actions:
        armature.animation_data.action = action
        start_frame = int(round(action.frame_range[0]))
        end_frame = int(round(action.frame_range[1]))
        bpy.context.scene.frame_start = start_frame
        bpy.context.scene.frame_end = end_frame
        bpy.context.scene.frame_set(start_frame)
        select_export_objects(creature, armature)
        short_name = action.name.removeprefix(f"{ACTION_PREFIX}_")
        output_path = animation_dir / f"{ACTION_PREFIX}_{short_name}.fbx"
        bpy.ops.export_scene.fbx(
            filepath=str(output_path),
            use_selection=True,
            object_types={"ARMATURE", "MESH"},
            global_scale=1.0,
            apply_unit_scale=True,
            axis_forward="-Y",
            axis_up="Z",
            add_leaf_bones=False,
            mesh_smooth_type="FACE",
            use_armature_deform_only=True,
            bake_anim=True,
            bake_anim_use_all_actions=False,
            bake_anim_use_nla_strips=False,
            bake_anim_force_startend_keying=True,
            bake_anim_simplify_factor=0.0,
        )
        exported[action.name] = str(output_path)
    return exported


def main() -> dict:
    parameters = get_parameters()
    paths = resolve_output_paths(parameters)
    reset_scene()
    configure_scene()
    materials = create_materials()
    parts = build_prowler_parts(materials)
    armature = create_armature()
    creature = join_creature(parts, armature)
    actions = create_actions(armature)
    metrics = validate_asset(creature, armature, actions)
    paths["metrics"].write_text(
        json.dumps(metrics, indent=2) + "\n",
        encoding="utf-8",
    )
    export_skeletal_mesh(creature, armature, paths["mesh_fbx"])
    animation_paths = export_actions(
        creature,
        armature,
        actions,
        paths["animation_dir"],
    )
    armature.animation_data.action = actions[0]
    add_preview_scene(creature, armature, paths["preview"])
    bpy.ops.wm.save_as_mainfile(filepath=str(paths["blend"]))
    return {
        "asset": ASSET_NAME,
        "blend": str(paths["blend"]),
        "mesh_fbx": str(paths["mesh_fbx"]),
        "preview": str(paths["preview"]),
        "metrics_file": str(paths["metrics"]),
        "animation_fbxs": animation_paths,
        "metrics": metrics,
    }


__result__ = main()
