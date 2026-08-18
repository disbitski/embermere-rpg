"""Validate the saved art-free Fenwatch practice-target integration."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
TARGET_LABEL = "Embermere_FenwatchPracticeTarget_Gameplay_01"
TARGET_LOCATION = (-1120.0, -1120.0, 0.0)
TARGET_YAW = 45.0
ART_LABEL = "Embermere_FenwatchPracticeDummy_TrainingYard_01"
ART_MESH_PATH = (
    "/Game/Art/Embermere/Environment/PrototypeVillage/"
    "SM_EmbermereFenwatchPracticeDummy_01."
    "SM_EmbermereFenwatchPracticeDummy_01"
)
ARMSMASTER_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
ARMSMASTER_SERVICE_LABEL = "Embermere_FenwatchArmsmaster_Service_01"
WORKSHOP_LABEL = "Embermere_FenwatchTrainingWorkshop_Armsmaster_01"
TARGET_RING_MATERIAL = (
    "/Game/Art/Embermere/Targeting/"
    "M_EmbermereTargetRing.M_EmbermereTargetRing"
)


def fail(message):
    unreal.log_error(
        "Embermere Fenwatch practice-target gameplay validation failed: {}".format(
            message
        )
    )
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def nearly_equal(actual, expected, tolerance=0.01):
    return math.isclose(float(actual), float(expected), abs_tol=float(tolerance))


def collision_box_count(mesh):
    body_setup = mesh.get_editor_property("body_setup") if mesh else None
    aggregate = body_setup.get_editor_property("agg_geom") if body_setup else None
    return len(aggregate.get_editor_property("box_elems")) if aggregate else 0


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = list(actor_subsystem.get_all_level_actors())
    actors_by_label = {actor_label(actor): actor for actor in actors}

    required = {
        TARGET_LABEL,
        ART_LABEL,
        ARMSMASTER_LABEL,
        ARMSMASTER_SERVICE_LABEL,
        WORKSHOP_LABEL,
    }
    missing = sorted(required - set(actors_by_label))
    if missing:
        fail("missing required actors: {}".format(missing))

    targets = [
        actor for actor in actors if isinstance(actor, unreal.EmbermerePracticeTargetActor)
    ]
    if len(targets) != 1 or targets[0] != actors_by_label[TARGET_LABEL]:
        fail("expected exactly one native practice target, found {}".format(len(targets)))
    target = targets[0]

    location = target.get_actor_location()
    rotation = target.get_actor_rotation()
    scale = target.get_actor_scale3d()
    if not all(
        (
            nearly_equal(location.x, TARGET_LOCATION[0], 1.0),
            nearly_equal(location.y, TARGET_LOCATION[1], 1.0),
            nearly_equal(location.z, TARGET_LOCATION[2], 1.0),
            nearly_equal(rotation.pitch, 0.0, 0.1),
            nearly_equal(rotation.yaw, TARGET_YAW, 0.1),
            nearly_equal(rotation.roll, 0.0, 0.1),
            nearly_equal(scale.x, 1.0),
            nearly_equal(scale.y, 1.0),
            nearly_equal(scale.z, 1.0),
        )
    ):
        fail("gameplay transform drifted: location={}, rotation={}, scale={}".format(
            location, rotation, scale
        ))

    tags = set(target.tags)
    if unreal.Name("PracticeTarget") not in tags:
        fail("gameplay actor must retain the PracticeTarget tag")
    if unreal.Name("EmbermereGameplay") not in tags:
        fail("gameplay actor must retain the EmbermereGameplay tag")
    if unreal.Name("EmbermereOriginalArt") in tags:
        fail("gameplay actor must remain outside the original-art count")
    if unreal.Name("Hostile") in tags:
        fail("practice target must not enter tag-driven hostile gameplay lanes")

    expected_false_properties = (
        "loot_enabled",
        "grants_defeat_credit",
        "prototype_ai_enabled",
        "gameplay_collision_enabled",
        "trace_target_ring_surface",
    )
    for property_name in expected_false_properties:
        if bool(target.get_editor_property(property_name)):
            fail("{} must remain false".format(property_name))

    exact_numeric_properties = {
        "respawn_delay_seconds": 3.0,
        "loot_quantity": 0.0,
        "loot_drop_chance": 0.0,
        "aggro_radius": 0.0,
        "attack_range": 0.0,
        "attack_damage": 0.0,
        "move_speed_cm_per_second": 0.0,
        "leash_radius": 0.0,
        "return_home_speed_cm_per_second": 0.0,
        "nameplate_height": 285.0,
        "target_marker_height": 335.0,
        "target_ring_radius": 150.0,
        "target_ring_bounds_padding": 0.0,
        "target_ring_height_offset": 16.0,
        "target_ring_surface_clearance": 16.0,
    }
    for property_name, expected in exact_numeric_properties.items():
        actual = target.get_editor_property(property_name)
        if not nearly_equal(actual, expected):
            fail("{} drifted: expected {}, found {}".format(
                property_name, expected, actual
            ))

    if str(target.get_editor_property("enemy_name")) != "Fenwatch Practice Target":
        fail("practice-target display name drifted")
    if target.get_editor_property("loot_item"):
        fail("practice target must not reference loot")
    if target.get_editor_property("visual_skeletal_mesh"):
        fail("practice target must not own authored character art")

    stats = target.get_component_by_class(unreal.EmbermereStatsComponent)
    if not stats:
        fail("practice target is missing its native stats component")
    if not nearly_equal(stats.get_editor_property("max_health"), 150.0):
        fail("practice target maximum health drifted")
    if not nearly_equal(stats.get_editor_property("current_health"), 150.0):
        fail("practice target saved health must begin full")
    if not nearly_equal(stats.get_editor_property("max_mana"), 0.0):
        fail("practice target must not own mana")

    capsule = target.get_component_by_class(unreal.CapsuleComponent)
    if not capsule or capsule.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("practice target capsule must remain NoCollision")
    if target.get_actor_enable_collision():
        fail("practice target actor collision must remain disabled")

    movement = target.get_component_by_class(unreal.CharacterMovementComponent)
    if not movement:
        fail("practice target is missing its native movement component")
    if not nearly_equal(movement.get_editor_property("gravity_scale"), 0.0):
        fail("practice target gravity must remain disabled")
    if movement.get_editor_property("movement_mode") != unreal.MovementMode.MOVE_NONE:
        fail("practice target movement mode must remain MOVE_None")

    skeletal_components = list(target.get_components_by_class(unreal.SkeletalMeshComponent))
    for component in skeletal_components:
        if component.get_editor_property("skeletal_mesh_asset"):
            fail("practice target gameplay actor must not own a skeletal mesh")
        if component.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
            fail("practice target inherited skeletal lane must remain NoCollision")

    if target.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("practice target must not own interaction authority")
    if target.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("practice target must not own trainer authority")
    if target.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("practice target must not own vendor authority")

    if target.get_target_ring_segment_count() != 48:
        fail("practice target must retain exactly 48 target-ring segments")
    if not target.are_target_ring_segments_non_colliding():
        fail("practice target target-ring segments must remain NoCollision")
    if target.get_target_ring_material_path() != TARGET_RING_MATERIAL:
        fail("practice target target-ring material drifted")
    if not nearly_equal(target.get_resolved_target_ring_radius(), 150.0):
        fail("practice target resolved target-ring radius drifted")
    if not nearly_equal(target.get_effective_target_ring_surface_clearance(), 16.0):
        fail("practice target target-ring clearance drifted")
    ring_color = target.get_target_ring_color()
    if not all(
        (
            nearly_equal(ring_color.r, 0.015, 0.001),
            nearly_equal(ring_color.g, 0.68, 0.001),
            nearly_equal(ring_color.b, 1.0, 0.001),
            nearly_equal(ring_color.a, 1.0, 0.001),
        )
    ):
        fail("practice target cyan target-ring color drifted: {}".format(ring_color))

    ring_components = [
        component
        for component in target.get_components_by_class(unreal.StaticMeshComponent)
        if component.get_name().startswith("SelectedTargetRingSegment_")
    ]
    if len(ring_components) != 48:
        fail("saved native actor exposes {} target-ring components".format(
            len(ring_components)
        ))
    for component in ring_components:
        if component.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
            fail("{} unexpectedly contributes collision".format(component.get_name()))

    art = actors_by_label[ART_LABEL]
    if not isinstance(art, unreal.StaticMeshActor):
        fail("accepted practice-dummy art must remain a separate StaticMeshActor")
    art_component = art.get_component_by_class(unreal.StaticMeshComponent)
    art_mesh = art_component.get_editor_property("static_mesh") if art_component else None
    if not art_mesh or art_mesh.get_path_name() != ART_MESH_PATH:
        fail("accepted practice-dummy art mesh drifted")
    if collision_box_count(art_mesh) != 2:
        fail("accepted practice-dummy art must retain two authored colliders")
    if unreal.Name("EmbermereOriginalArt") not in set(art.tags):
        fail("accepted practice-dummy art must retain its original-art tag")
    if unreal.Name("EmbermereGameplay") in set(art.tags):
        fail("practice-dummy art must remain outside gameplay authority")

    service = actors_by_label[ARMSMASTER_SERVICE_LABEL]
    if not service.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("the separate armsmaster service lost trainer authority")
    if target.get_actor_location().distance(actors_by_label[WORKSHOP_LABEL].get_actor_location()) < 1.0:
        fail("practice gameplay target must not replace workshop presentation")

    unreal.log(
        "Embermere Fenwatch practice-target gameplay validation passed: "
        "native_targets=1, health=150, reset=3s, defeat_credit=false, "
        "stationary=true, ring_segments=48, art_colliders=2, "
        "trainer_service_separate=true"
    )


if __name__ == "__main__":
    main()
