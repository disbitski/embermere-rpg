"""Validate the saved decorative relief and its wall-mounted map placement."""

import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ASSET_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereAncientRuinRelief_01"
PLACEMENT_LABEL = "Embermere_AncientRuinRelief_WallA_01"
WALL_LABEL = "FabPass_Ruin_Wall_A"
EXPECTED_LOCATION = unreal.Vector(2302.0, 350.0, 0.0)
EXPECTED_YAW = 90.0
SHARED = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}


def fail(message):
    unreal.log_error("Embermere ancient ruin relief validation failed: {}".format(message))
    sys.exit(1)


def label(actor):
    return actor.get_actor_label()


def collision_count(mesh):
    body = mesh.get_editor_property("body_setup")
    if not body:
        return 0
    aggregate = body.get_editor_property("agg_geom")
    return sum(len(aggregate.get_editor_property(name)) for name in (
        "box_elems", "sphere_elems", "sphyl_elems", "convex_elems"
    ))


def main():
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("saved static mesh is missing")
    if mesh.get_num_triangles(0) != 2900 or collision_count(mesh) != 0:
        fail("triangle or no-collision contract drifted")
    bounds = mesh.get_bounds()
    size = bounds.box_extent * 2.0
    actual = (float(size.x), float(size.y), float(size.z))
    if any(abs(actual[i] - expected) > 1.0 for i, expected in enumerate((206.0, 55.805, 216.0))):
        fail("mesh dimensions drifted: {}".format(actual))
    if abs(float(bounds.origin.z) - 108.0) > 1.0:
        fail("render mesh lost its ground-level pivot")
    import_data = mesh.get_editor_property("asset_import_data")
    if not import_data or import_data.get_class().get_name() != "FbxStaticMeshImportData":
        fail("mesh is not classic-FBX imported")
    assignments = {}
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot = str(static_material.get_editor_property("material_slot_name"))
        material = mesh.get_material(index)
        assignments[slot] = material.get_path_name() if material else "None"
    if assignments != SHARED:
        fail("shared materials drifted: {}".format(assignments))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    by_label = {}
    for actor in actors:
        by_label.setdefault(label(actor), []).append(actor)
    if len(by_label.get(PLACEMENT_LABEL, [])) != 1:
        fail("expected exactly one relief actor")
    if len(by_label.get(WALL_LABEL, [])) != 1:
        fail("backing ruin wall missing or duplicated")
    actor = by_label[PLACEMENT_LABEL][0]
    wall = by_label[WALL_LABEL][0]
    if actor.get_class().get_name() != "StaticMeshActor":
        fail("relief must remain a presentation-only StaticMeshActor")
    if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("relief location drifted: {}".format(actor.get_actor_location()))
    yaw = float(actor.get_actor_rotation().yaw)
    if abs((yaw - EXPECTED_YAW + 180.0) % 360.0 - 180.0) > 0.1:
        fail("relief yaw drifted: {}".format(yaw))
    if (actor.get_actor_scale3d() - unreal.Vector(1, 1, 1)).length() > 0.001:
        fail("relief scale drifted")
    if unreal.Name("EmbermereOriginalArt") not in list(actor.get_editor_property("tags")):
        fail("relief lost project-owned art tag")
    if str(actor.get_folder_path()) != "04_Ruins/EmbermereOriginalArt":
        fail("relief moved out of the original-art ruin folder")
    component = actor.get_editor_property("static_mesh_component")
    if component.get_editor_property("static_mesh") != mesh:
        fail("relief references wrong mesh")
    if str(component.get_collision_profile_name()) != "NoCollision":
        fail("relief gained collision")
    if (wall.get_actor_location() - unreal.Vector(2400, 360, 0)).length() > 0.1:
        fail("backing ruin wall moved")
    wall_bounds = wall.get_actor_bounds(False)
    if not (2320 <= wall_bounds[0].x - wall_bounds[1].x <= 2335):
        fail("backing wall west face drifted")
    distance = math.hypot(actor.get_actor_location().x - 1900, actor.get_actor_location().y - 300)
    if distance < 350:
        fail("relief encroaches on first Prowler")
    unreal.log("EMBERMERE_ANCIENT_RUIN_RELIEF_VALIDATION_SUCCESS: 2900 triangles, four shared materials, no collision, one saved decorative wall placement")


if __name__ == "__main__":
    main()
