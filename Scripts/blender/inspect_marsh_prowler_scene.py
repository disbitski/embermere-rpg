"""Inspect the live Marsh Prowler Blender scene after a guarded build."""

import bpy
from mathutils import Vector


creature = bpy.data.objects.get("SK_EmbermereMarshProwler_01")
armature = bpy.data.objects.get("SKEL_EmbermereMarshProwler_01")

if not creature:
    raise RuntimeError("Marsh Prowler creature is not present in the live scene")

world_corners = [creature.matrix_world @ Vector(corner) for corner in creature.bound_box]
minimum = [min(corner[index] for corner in world_corners) for index in range(3)]
maximum = [max(corner[index] for corner in world_corners) for index in range(3)]

group_names = {group.index: group.name for group in creature.vertex_groups}


def vertex_record(vertex):
    world_position = creature.matrix_world @ vertex.co
    return {
        "index": vertex.index,
        "position": [round(value, 3) for value in world_position],
        "groups": [
            group_names.get(membership.group, str(membership.group))
            for membership in vertex.groups
        ],
    }


extremes = {
    "min_y": vertex_record(
        min(creature.data.vertices, key=lambda vertex: (creature.matrix_world @ vertex.co).y)
    ),
    "max_y": vertex_record(
        max(creature.data.vertices, key=lambda vertex: (creature.matrix_world @ vertex.co).y)
    ),
    "min_z": vertex_record(
        min(creature.data.vertices, key=lambda vertex: (creature.matrix_world @ vertex.co).z)
    ),
    "max_z": vertex_record(
        max(creature.data.vertices, key=lambda vertex: (creature.matrix_world @ vertex.co).z)
    ),
}

__result__ = {
    "dimensions": [round(value, 3) for value in creature.dimensions],
    "minimum": [round(value, 3) for value in minimum],
    "maximum": [round(value, 3) for value in maximum],
    "location": [round(value, 3) for value in creature.location],
    "rotation": [round(value, 6) for value in creature.rotation_euler],
    "scale": [round(value, 6) for value in creature.scale],
    "parent": creature.parent.name if creature.parent else None,
    "vertices": len(creature.data.vertices),
    "polygons": len(creature.data.polygons),
    "armature_bones": len(armature.data.bones) if armature else 0,
    "extremes": extremes,
}
