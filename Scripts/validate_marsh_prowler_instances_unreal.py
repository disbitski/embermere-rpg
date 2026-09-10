"""Validate native animation routing on the actual saved Prowler instances."""

import os
import sys

import unreal

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from apply_marsh_prowler_presentation_mcp import ANIMATION_PATHS, MESH_PATH


MAP_PATH = "/Game/Maps/L_Embermere_Prototype"
BLUEPRINT_CLASS = "/Game/Blueprints/BP_StarterEnemy.BP_StarterEnemy_C"
LABELS = {"Starter_Enemy_01", "Starter_Enemy_02", "Starter_Enemy_03"}
ROLES = ("idle", "walk", "run", "attack", "hit", "death")


def main():
    errors = []
    mesh = unreal.load_asset(MESH_PATH)
    if not isinstance(mesh, unreal.SkeletalMesh):
        raise RuntimeError("Missing reviewed Prowler skeletal mesh")
    skeleton = mesh.get_editor_property("skeleton")
    expected = {"visual_skeletal_mesh": mesh}
    for role in ROLES:
        animation = unreal.load_asset(ANIMATION_PATHS[role + "Animation"])
        if not isinstance(animation, unreal.AnimSequence):
            raise RuntimeError("Missing {} animation".format(role))
        if animation.get_editor_property("skeleton") != skeleton or animation.get_play_length() <= 0:
            raise RuntimeError("Invalid {} animation skeleton/duration".format(role))
        expected[role + "_animation"] = animation

    if not unreal.EditorLevelLibrary.load_level(MAP_PATH):
        raise RuntimeError("Could not load saved prototype map")
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    enemies = [a for a in actors if a.get_class().get_path_name() == BLUEPRINT_CLASS]
    if len(enemies) != 3 or {a.get_actor_label() for a in enemies} != LABELS:
        raise RuntimeError("Unexpected saved Prowler actor set")

    owners = [("Blueprint defaults", unreal.get_default_object(enemies[0].get_class()))]
    owners += [(a.get_actor_label(), a) for a in enemies]
    for label, owner in owners:
        for prop, asset in expected.items():
            actual = owner.get_editor_property(prop)
            if actual != asset:
                errors.append("{} {}: expected {}, found {}".format(
                    label, prop, asset.get_path_name(), actual.get_path_name() if actual else "None"))
        if not owner.has_complete_visual_animation_set():
            errors.append(label + " lacks a complete native six-role set")
        component = owner.get_component_by_class(unreal.SkeletalMeshComponent)
        if not component or component.get_editor_property("skeletal_mesh_asset") != mesh:
            errors.append(label + " has the wrong component mesh")
            continue
        for native_prop, component_prop, target in (
            ("visual_mesh_relative_location", "relative_location", unreal.Vector(0, 0, -95)),
            ("visual_mesh_relative_scale", "relative_scale3d", unreal.Vector(0.65, 0.65, 0.65)),
        ):
            for obj, prop in ((owner, native_prop), (component, component_prop)):
                if (obj.get_editor_property(prop) - target).length() > 0.001:
                    errors.append("{} {} drifted".format(label, prop))
        for obj, prop in ((owner, "visual_mesh_relative_rotation"), (component, "relative_rotation")):
            rotation = obj.get_editor_property(prop)
            if max(abs(rotation.pitch), abs(rotation.yaw), abs(rotation.roll)) > 0.01:
                errors.append("{} {} drifted".format(label, prop))
        if (component.get_collision_enabled() != unreal.CollisionEnabled.QUERY_ONLY
                or str(component.get_collision_profile_name()) != "CharacterMesh"):
            errors.append(label + " existing CharacterMesh collision profile drifted")
        if component.get_editor_property("animation_data").get_editor_property("anim_to_play") != expected["idle_animation"]:
            errors.append(label + " default component Idle drifted")

    if errors:
        unreal.log_error("Embermere placed Prowler validation failed:\n" + "\n".join(errors))
        sys.exit(1)
    unreal.log("EMBERMERE_PLACED_PROWLER_VALIDATION_SUCCESS: actors=3 roles=6 skeleton=shared native_component_transforms=matched")


if __name__ == "__main__":
    main()
