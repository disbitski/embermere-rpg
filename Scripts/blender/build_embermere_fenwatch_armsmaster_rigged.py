"""Build Embermere's rigged Fenwatch armsmaster and looping Idle.

This reviewed deterministic script reuses the accepted static armsmaster's
geometry construction stage, assigns every disconnected low-poly part to one
explicit deform bone, authors a restrained planted-foot Idle, exports the
skeletal mesh and animation through classic FBX, records validation metrics,
renders a preview, and saves the editable Blender source.

Run scene-resetting builds through Blender MCP's factory-clean headless lane.
The resulting art owns no trainer, interaction, navigation, or persistence
authority; the existing static mesh remains the reversible fallback.
"""

from __future__ import annotations

import bmesh
import json
import math
from pathlib import Path
import runpy

import bpy


ASSET_NAME = "SK_EmbermereFenwatchArmsmaster_01"
ARMATURE_NAME = "ARM_EmbermereFenwatchArmsmaster_01"
ACTION_NAME = "A_EmbermereFenwatchArmsmaster_Idle"
EXPECTED_TRIANGLES = 2824
EXPECTED_BONES = {
    "root",
    "pelvis",
    "spine",
    "neck",
    "head",
    "upper_arm_l",
    "forearm_l",
    "upper_arm_r",
    "forearm_r",
}


def get_parameters() -> dict:
    supplied = globals().get("args", {})
    return supplied if isinstance(supplied, dict) else {}


def load_static_builder(parameters: dict) -> dict:
    project_root = Path(
        parameters.get("project_root", "/Users/wizard/Documents/Unreal Game")
    ).expanduser()
    script_path = (
        project_root
        / "Scripts/blender/build_embermere_fenwatch_armsmaster.py"
    )
    return runpy.run_path(
        str(script_path),
        init_globals={"EMBERMERE_IMPORT_ONLY": True},
    )


def resolve_output_paths(
    parameters: dict,
) -> tuple[Path, Path, Path, Path, Path]:
    project_root = Path(
        parameters.get("project_root", "/Users/wizard/Documents/Unreal Game")
    ).expanduser()
    output_dir = Path(
        parameters.get(
            "output_dir",
            project_root
            / "ArtSource/Blender/Characters/NPCs/FenwatchArmsmaster/Rigged",
        )
    ).expanduser()
    animation_dir = output_dir / "Animations"
    animation_dir.mkdir(parents=True, exist_ok=True)
    return (
        output_dir / f"{ASSET_NAME}.blend",
        output_dir / f"{ASSET_NAME}.fbx",
        animation_dir / f"{ACTION_NAME}.fbx",
        output_dir / f"{ASSET_NAME}_preview.png",
        output_dir / f"{ASSET_NAME}_metrics.json",
    )


def assign_rigid_group(obj: bpy.types.Object, bone_name: str) -> None:
    group = obj.vertex_groups.new(name=bone_name)
    group.add(list(range(len(obj.data.vertices))), 1.0, "REPLACE")


def create_armature() -> bpy.types.Object:
    bpy.ops.object.armature_add(enter_editmode=True, location=(0.0, 0.0, 0.0))
    armature = bpy.context.object
    armature.name = ARMATURE_NAME
    armature.data.name = f"{ARMATURE_NAME}_Data"
    armature.data.display_type = "STICK"
    edit_bones = armature.data.edit_bones
    edit_bones.remove(edit_bones[0])

    bone_specs = [
        ("root", (0.0, 0.0, 0.0), (0.0, 0.0, 28.0), None),
        ("pelvis", (0.0, 0.0, 72.0), (0.0, 0.0, 105.0), "root"),
        ("spine", (0.0, 0.0, 105.0), (0.0, 0.0, 160.0), "pelvis"),
        ("neck", (0.0, 0.0, 160.0), (0.0, 0.0, 178.0), "spine"),
        ("head", (0.0, 0.0, 178.0), (0.0, 0.0, 208.0), "neck"),
        (
            "upper_arm_l",
            (-34.0, 0.0, 148.0),
            (-49.0, -7.0, 123.0),
            "spine",
        ),
        (
            "forearm_l",
            (-49.0, -7.0, 123.0),
            (-52.0, -24.0, 103.0),
            "upper_arm_l",
        ),
        (
            "upper_arm_r",
            (34.0, 0.0, 148.0),
            (48.0, -5.0, 124.0),
            "spine",
        ),
        (
            "forearm_r",
            (48.0, -5.0, 124.0),
            (58.0, -11.0, 104.0),
            "upper_arm_r",
        ),
    ]
    created: dict[str, bpy.types.EditBone] = {}
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


def part_bone_map() -> dict[str, str]:
    return {
        "BootL": "root",
        "BootR": "root",
        "ShinL": "root",
        "ShinR": "root",
        "TrainingStaff": "root",
        "StaffIronCap": "root",
        "StaffEmber": "root",
        "StoneTrousers": "pelvis",
        "GuardBelt": "pelvis",
        "TrainingPouch": "pelvis",
        "GuardTunic": "spine",
        "MossMantle": "spine",
        "ChestGuard": "spine",
        "ChestCrestStone": "spine",
        "ChestCrestEmber": "spine",
        "UpperArmL": "upper_arm_l",
        "ForearmL": "forearm_l",
        "HandL": "forearm_l",
        "PracticeShieldStone": "forearm_l",
        "PracticeShieldIron": "forearm_l",
        "PracticeShieldEmber": "forearm_l",
        "UpperArmR": "upper_arm_r",
        "ForearmR": "forearm_r",
        "HandR": "forearm_r",
        "Head": "head",
        "MossHood": "head",
        "OpenFace": "head",
        "IronBrow": "head",
        "EyeL": "head",
        "EyeR": "head",
        "Nose": "head",
    }


def join_rigged_mesh(
    parts: list[bpy.types.Object],
    armature: bpy.types.Object,
) -> bpy.types.Object:
    assignments = part_bone_map()
    part_names = {part.name for part in parts}
    missing = sorted(part_names - set(assignments))
    stale = sorted(set(assignments) - part_names)
    if missing or stale:
        raise RuntimeError(
            "Armsmaster rigid assignment drifted: missing={}, stale={}".format(
                missing,
                stale,
            )
        )
    for part in parts:
        assign_rigid_group(part, assignments[part.name])

    bpy.ops.object.select_all(action="DESELECT")
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    mesh = bpy.context.object
    mesh.name = ASSET_NAME
    mesh.data.name = f"{ASSET_NAME}_Mesh"
    bpy.context.scene.cursor.location = (0.0, 0.0, 0.0)
    bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)

    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66.0), island_margin=0.02)
    bpy.ops.object.mode_set(mode="OBJECT")
    triangulate = mesh.modifiers.new(name="ExportTriangulation", type="TRIANGULATE")
    bpy.context.view_layer.objects.active = mesh
    bpy.ops.object.modifier_apply(modifier=triangulate.name)

    modifier = mesh.modifiers.new(name="Armature", type="ARMATURE")
    modifier.object = armature
    mesh.parent = armature
    return mesh


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
) -> None:
    bone = armature.pose.bones[bone_name]
    bone.rotation_mode = "XYZ"
    bone.rotation_euler = tuple(math.radians(value) for value in rotation_degrees)
    bone.keyframe_insert("rotation_euler", frame=frame, group=bone_name)


def create_idle(armature: bpy.types.Object) -> bpy.types.Action:
    scene = bpy.context.scene
    scene.render.fps = 30
    scene.frame_start = 1
    scene.frame_end = 97
    action = bpy.data.actions.new(name=ACTION_NAME)
    action.use_fake_user = True
    armature.animation_data_create()
    armature.animation_data.action = action
    reset_pose(armature)
    for pose_bone in armature.pose.bones:
        for frame in (1, 97):
            pose_bone.keyframe_insert("rotation_euler", frame=frame, group=pose_bone.name)
            pose_bone.keyframe_insert("location", frame=frame, group=pose_bone.name)
            pose_bone.keyframe_insert("scale", frame=frame, group=pose_bone.name)

    poses = (
        (1, (0.0, 0.0, 0.0), (0.0, 0.0, 0.0), (0.0, 0.0, 0.0)),
        (25, (-0.75, 0.0, 0.55), (0.35, 0.0, 0.8), (-0.7, 0.0, 1.3)),
        (49, (0.0, 0.0, 0.0), (0.0, 0.0, 0.0), (0.4, 0.0, 0.0)),
        (73, (0.55, 0.0, -0.45), (-0.25, 0.0, -0.65), (0.7, 0.0, -1.15)),
        (97, (0.0, 0.0, 0.0), (0.0, 0.0, 0.0), (0.0, 0.0, 0.0)),
    )
    for frame, spine_rotation, neck_rotation, head_rotation in poses:
        key_bone(armature, "spine", frame, spine_rotation)
        key_bone(armature, "neck", frame, neck_rotation)
        key_bone(armature, "head", frame, head_rotation)

    # The shield hand settles with the breathing cycle while the staff and feet
    # remain planted on the root bone.
    for frame, rotation in (
        (1, (0.0, 0.0, 0.0)),
        (25, (0.0, 0.0, 0.65)),
        (49, (0.0, 0.0, 0.0)),
        (73, (0.0, 0.0, -0.55)),
        (97, (0.0, 0.0, 0.0)),
    ):
        key_bone(armature, "forearm_l", frame, rotation)
    return action


def mesh_metrics(
    mesh: bpy.types.Object,
    armature: bpy.types.Object,
    action: bpy.types.Action,
) -> dict:
    bm = bmesh.new()
    bm.from_mesh(mesh.data)
    non_manifold_edges = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    unweighted_vertices = sum(1 for vertex in mesh.data.vertices if not vertex.groups)
    multiply_weighted_vertices = sum(
        1 for vertex in mesh.data.vertices if len(vertex.groups) != 1
    )
    return {
        "dimensions_cm": [round(value, 3) for value in mesh.dimensions],
        "triangles": sum(
            max(1, len(polygon.vertices) - 2) for polygon in mesh.data.polygons
        ),
        "materials": [
            slot.material.name for slot in mesh.material_slots if slot.material
        ],
        "uv_channels": len(mesh.data.uv_layers),
        "non_manifold_edges": non_manifold_edges,
        "scale": [round(value, 6) for value in mesh.scale],
        "origin_z_cm": round(mesh.location.z, 6),
        "armature_scale": [round(value, 6) for value in armature.scale],
        "bones": [bone.name for bone in armature.data.bones],
        "unweighted_vertices": unweighted_vertices,
        "multiply_weighted_vertices": multiply_weighted_vertices,
        "action": action.name,
        "action_frames": [
            int(round(action.frame_range[0])),
            int(round(action.frame_range[1])),
        ],
        "fps": bpy.context.scene.render.fps,
        "duration_seconds": round(
            (action.frame_range[1] - action.frame_range[0])
            / bpy.context.scene.render.fps,
            3,
        ),
    }


def validate_asset(
    mesh: bpy.types.Object,
    armature: bpy.types.Object,
    action: bpy.types.Action,
) -> dict:
    metrics = mesh_metrics(mesh, armature, action)
    errors: list[str] = []
    expected_dimensions = (154.5, 87.0, 228.0)
    if any(
        abs(actual - expected) > 0.01
        for actual, expected in zip(metrics["dimensions_cm"], expected_dimensions)
    ):
        errors.append(f"Unexpected dimensions: {metrics['dimensions_cm']}")
    if metrics["triangles"] != EXPECTED_TRIANGLES:
        errors.append(f"Unexpected triangle count: {metrics['triangles']}")
    if set(metrics["bones"]) != EXPECTED_BONES:
        errors.append(f"Bone contract drifted: {metrics['bones']}")
    if metrics["unweighted_vertices"] != 0:
        errors.append(f"Unweighted vertices: {metrics['unweighted_vertices']}")
    if metrics["multiply_weighted_vertices"] != 0:
        errors.append(
            "Rigid weighting drifted: {} vertices".format(
                metrics["multiply_weighted_vertices"]
            )
        )
    if metrics["uv_channels"] < 1:
        errors.append("Rigged mesh has no UV channel")
    if metrics["non_manifold_edges"] != 0:
        errors.append(f"Non-manifold edges found: {metrics['non_manifold_edges']}")
    if metrics["scale"] != [1.0, 1.0, 1.0]:
        errors.append(f"Mesh transforms are not applied: {metrics['scale']}")
    if metrics["armature_scale"] != [1.0, 1.0, 1.0]:
        errors.append(f"Armature scale drifted: {metrics['armature_scale']}")
    if abs(metrics["origin_z_cm"]) > 0.001:
        errors.append(f"Mesh origin must remain at ground: {metrics['origin_z_cm']}")
    if metrics["action"] != ACTION_NAME or metrics["action_frames"] != [1, 97]:
        errors.append(
            "Idle action contract drifted: {} {}".format(
                metrics["action"],
                metrics["action_frames"],
            )
        )
    if metrics["fps"] != 30:
        errors.append(f"Idle must remain authored at 30 fps: {metrics['fps']}")
    expected_materials = {
        "M_FenwatchArmsmasterSkin",
        "M_Waystone",
        "M_WaystoneMoss",
        "M_EmbermereTimber",
        "M_EmberLampIron",
        "M_WaystoneEmber",
    }
    if set(metrics["materials"]) != expected_materials:
        errors.append(f"Material slots drifted: {metrics['materials']}")
    if errors:
        raise RuntimeError(
            "Rigged Fenwatch armsmaster validation failed: " + "; ".join(errors)
        )
    return metrics


def select_export_objects(
    mesh: bpy.types.Object,
    armature: bpy.types.Object,
) -> None:
    bpy.ops.object.select_all(action="DESELECT")
    mesh.select_set(True)
    armature.select_set(True)
    bpy.context.view_layer.objects.active = armature


def export_skeletal_mesh(
    mesh: bpy.types.Object,
    armature: bpy.types.Object,
    output_path: Path,
) -> None:
    select_export_objects(mesh, armature)
    bpy.ops.export_scene.fbx(
        filepath=str(output_path),
        use_selection=True,
        object_types={"ARMATURE", "MESH"},
        global_scale=1.0,
        apply_unit_scale=True,
        axis_forward="-Y",
        axis_up="Z",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        use_armature_deform_only=True,
        bake_anim=False,
    )


def export_idle(
    mesh: bpy.types.Object,
    armature: bpy.types.Object,
    action: bpy.types.Action,
    output_path: Path,
) -> None:
    armature.animation_data.action = action
    bpy.context.scene.frame_start = 1
    bpy.context.scene.frame_end = 97
    select_export_objects(mesh, armature)
    bpy.ops.export_scene.fbx(
        filepath=str(output_path),
        use_selection=True,
        object_types={"ARMATURE", "MESH"},
        global_scale=1.0,
        apply_unit_scale=True,
        axis_forward="-Y",
        axis_up="Z",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        use_armature_deform_only=True,
        bake_anim=True,
        bake_anim_use_all_bones=True,
        bake_anim_use_all_actions=False,
        bake_anim_use_nla_strips=False,
        bake_anim_force_startend_keying=True,
        bake_anim_simplify_factor=0.0,
    )


def main() -> dict:
    parameters = get_parameters()
    (
        blend_path,
        skeletal_fbx_path,
        idle_fbx_path,
        preview_path,
        metrics_path,
    ) = resolve_output_paths(parameters)
    static_builder = load_static_builder(parameters)
    static_builder["reset_scene"]()
    static_builder["configure_scene"]()
    parts = static_builder["build_armsmaster_parts"]()
    armature = create_armature()
    mesh = join_rigged_mesh(parts, armature)
    idle = create_idle(armature)
    metrics = validate_asset(mesh, armature, idle)
    metrics_path.write_text(json.dumps(metrics, indent=2) + "\n", encoding="utf-8")
    export_skeletal_mesh(mesh, armature, skeletal_fbx_path)
    export_idle(mesh, armature, idle, idle_fbx_path)

    bpy.context.scene.frame_set(25)
    static_builder["add_preview_scene"](mesh, preview_path)
    armature.animation_data.action = idle
    bpy.context.scene.frame_set(1)
    bpy.ops.wm.save_as_mainfile(filepath=str(blend_path))
    return {
        "asset": ASSET_NAME,
        "armature": ARMATURE_NAME,
        "idle": ACTION_NAME,
        "blend": str(blend_path),
        "skeletal_fbx": str(skeletal_fbx_path),
        "idle_fbx": str(idle_fbx_path),
        "preview": str(preview_path),
        "metrics_file": str(metrics_path),
        "metrics": metrics,
    }


__result__ = main()
