"""Validate the saved Fenwatch north-commons herb garden."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ASSET_PATH = (
    "/Game/Art/Embermere/Environment/PrototypeVillage/"
    "SM_EmbermereFenwatchHerbGarden_01"
)
PLACEMENT_LABEL = "Embermere_FenwatchHerbGarden_NorthCommons_01"
EXPECTED_LOCATION = unreal.Vector(-1500.0, 1400.0, 0.0)
EXPECTED_YAW = -15.0
CONTEXT_LABELS = {
    "Embermere_FenwatchCottage_North_01": 1200.0,
    "Quest_Giver_Mara_Fenwatch": 2250.0,
    "PlayerStart_Embermere_Village": 2700.0,
    "Embermere_FenwatchNoticeBoard_Road_01": 1600.0,
    "Embermere_FenwatchGatewatchPost_EastRoad_01": 2600.0,
}
EXPECTED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}


def fail(message):
    unreal.log_error("Embermere Fenwatch herb-garden validation failed: {}".format(message))
    sys.exit(1)


def label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def collision_counts(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0, 0
    aggregate = body_setup.get_editor_property("agg_geom")
    total = sum(
        len(aggregate.get_editor_property(name))
        for name in ("box_elems", "sphere_elems", "sphyl_elems", "convex_elems")
    )
    return total, len(aggregate.get_editor_property("box_elems"))


def main():
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("saved static mesh is missing")
    if mesh.get_num_triangles(0) != 4312:
        fail("triangle count drifted: {}".format(mesh.get_num_triangles(0)))
    total_collision, box_count = collision_counts(mesh)
    if total_collision != 4 or box_count != 4:
        fail("expected four box colliders, found {} total / {} boxes".format(total_collision, box_count))
    bounds = mesh.get_bounds()
    dimensions = bounds.box_extent * 2.0
    expected_dimensions = (356.0, 212.0, 195.0)
    actual_dimensions = (float(dimensions.x), float(dimensions.y), float(dimensions.z))
    if any(abs(actual_dimensions[index] - expected_dimensions[index]) > 1.0 for index in range(3)):
        fail("mesh dimensions drifted: {}".format(actual_dimensions))
    if abs(float(bounds.origin.z) - 97.5) > 1.0:
        fail("mesh is no longer grounded at local Z zero")
    import_data = mesh.get_editor_property("asset_import_data")
    import_class = import_data.get_class().get_name() if import_data else "None"
    if import_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_class))
    assignments = {}
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        assigned = mesh.get_material(index)
        assignments[slot_name] = assigned.get_path_name() if assigned else "None"
    if assignments != EXPECTED_MATERIALS:
        fail("shared material assignments drifted: {}".format(assignments))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    placements = [actor for actor in actors if label(actor) == PLACEMENT_LABEL]
    if len(placements) != 1:
        fail("expected one saved herb garden, found {}".format(len(placements)))
    by_label = {label(actor): actor for actor in actors}
    for required in CONTEXT_LABELS:
        if required not in by_label:
            fail("missing context actor {}".format(required))

    actor = placements[0]
    if actor.get_class().get_name() != "StaticMeshActor":
        fail("placement must remain presentation-only StaticMeshActor art")
    if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("placement location drifted: {}".format(actor.get_actor_location()))
    yaw_delta = abs((float(actor.get_actor_rotation().yaw) - EXPECTED_YAW + 180.0) % 360.0 - 180.0)
    if yaw_delta > 0.1:
        fail("placement yaw drifted: {}".format(actor.get_actor_rotation().yaw))
    if (actor.get_actor_scale3d() - unreal.Vector(1.0, 1.0, 1.0)).length() > 0.001:
        fail("placement scale drifted: {}".format(actor.get_actor_scale3d()))
    if unreal.Name("EmbermereOriginalArt") not in list(actor.get_editor_property("tags")):
        fail("placement lost the EmbermereOriginalArt tag")
    component = actor.get_editor_property("static_mesh_component")
    if component.get_editor_property("static_mesh") != mesh:
        fail("placement references the wrong static mesh")
    if str(component.get_collision_profile_name()) != "BlockAll":
        fail("placement collision profile drifted: {}".format(component.get_collision_profile_name()))

    location = actor.get_actor_location()
    distances = {}
    for context_label, minimum in CONTEXT_LABELS.items():
        other = by_label[context_label].get_actor_location()
        distance = math.hypot(location.x - other.x, location.y - other.y)
        distances[context_label] = distance
        if distance < minimum:
            fail("herb garden moved too close to {}: {:.1f} cm".format(context_label, distance))

    unreal.log(
        "EMBERMERE_FENWATCH_HERB_GARDEN_VALIDATION_SUCCESS: "
        "356 x 212 x 195 cm, 4312 triangles, five shared materials, four "
        "authored bed/post boxes, one grounded presentation-only placement, "
        "and protected north-commons spacing {}".format(distances)
    )


if __name__ == "__main__":
    main()
