"""Render deterministic acceptance frames for the Marsh Prowler actions."""

from __future__ import annotations

from pathlib import Path

import bpy


ARMATURE_NAME = "SKEL_EmbermereMarshProwler_01"
ACTION_PREFIX = "A_EmbermereMarshProwler"


def get_parameters() -> dict:
    supplied = globals().get("args", {})
    return supplied if isinstance(supplied, dict) else {}


def main() -> dict:
    parameters = get_parameters()
    project_root = Path(
        parameters.get("project_root", "/Users/wizard/Documents/Unreal Game")
    ).expanduser()
    output_dir = project_root / (
        "ArtSource/Blender/Characters/Enemies/MarshProwler/ActionPreviews"
    )
    output_dir.mkdir(parents=True, exist_ok=True)

    armature = bpy.data.objects.get(ARMATURE_NAME)
    if armature is None or armature.type != "ARMATURE":
        raise RuntimeError(f"Missing armature {ARMATURE_NAME}")
    armature.animation_data_create()

    scene = bpy.context.scene
    scene.render.resolution_x = 720
    scene.render.resolution_y = 480
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"

    samples = {
        "Idle": 13,
        "Walk": 1,
        "Run": 1,
        "Attack": 12,
        "Hit": 5,
        "Death": 41,
    }
    outputs = {}
    for short_name, frame in samples.items():
        action_name = f"{ACTION_PREFIX}_{short_name}"
        action = bpy.data.actions.get(action_name)
        if action is None:
            raise RuntimeError(f"Missing action {action_name}")
        armature.animation_data.action = action
        scene.frame_start = int(action.frame_range[0])
        scene.frame_end = int(action.frame_range[1])
        scene.frame_set(frame)
        output_path = output_dir / f"{action_name}_frame_{frame:03d}.png"
        scene.render.filepath = str(output_path)
        bpy.ops.render.render(write_still=True)
        outputs[action_name] = str(output_path)

    return {
        "armature": armature.name,
        "samples": outputs,
    }


__result__ = main()
