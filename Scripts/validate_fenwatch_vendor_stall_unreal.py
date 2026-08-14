"""Validate the saved Fenwatch vendor-stall asset and map integration."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
ASSET_PATH = (
    "/Game/Art/Embermere/Environment/PrototypeVillage/"
    "SM_EmbermereFenwatchVendorStall_01"
)
PLACEMENT_LABEL = "Embermere_FenwatchVendorStall_Quartermaster_01"
REPLACED_FAB_LABEL = "FabPass_Village_Fence_01"
QUARTERMASTER_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
VENDOR_SERVICE_LABEL = "Embermere_FenwatchQuartermaster_Service_01"
SUPPLY_CHEST_LABEL = "Embermere_SupplyChest_Vendor_01"
EXPECTED_LOCATION = unreal.Vector(-1530.0, -1430.0, 0.0)
EXPECTED_YAW = 180.0
EXPECTED_MATERIALS = {
    "M_Waystone": "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "M_WaystoneMoss": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "M_EmberLampIron": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "M_EmbermereTimber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "M_WaystoneEmber": "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}


def fail(message):
    unreal.log_error("Embermere Fenwatch vendor stall validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def collision_box_count(mesh):
    body_setup = mesh.get_editor_property("body_setup")
    if not body_setup:
        return 0
    aggregate = body_setup.get_editor_property("agg_geom")
    return len(aggregate.get_editor_property("box_elems"))


def main():
    mesh = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        fail("saved static mesh is missing")
    if mesh.get_num_triangles(0) != 5476:
        fail("triangle count drifted: {}".format(mesh.get_num_triangles(0)))
    if collision_box_count(mesh) != 5:
        fail("expected five authored collision boxes")

    bounds = mesh.get_bounds()
    full_dimensions = bounds.box_extent * 2.0
    expected_dimensions = (394.0, 242.0, 306.0)
    actual_dimensions = (
        float(full_dimensions.x),
        float(full_dimensions.y),
        float(full_dimensions.z),
    )
    if any(
        abs(actual_dimensions[index] - expected_dimensions[index]) > 1.0
        for index in range(3)
    ):
        fail("mesh dimensions drifted: {}".format(actual_dimensions))
    if abs(float(bounds.origin.z) - 153.0) > 1.0:
        fail("mesh is no longer grounded at local Z zero")

    import_data = mesh.get_editor_property("asset_import_data")
    import_data_class = import_data.get_class().get_name() if import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("expected classic FBX import data, found {}".format(import_data_class))

    assignments = {}
    for index, static_material in enumerate(list(mesh.get_editor_property("static_materials"))):
        slot_name = str(static_material.get_editor_property("material_slot_name"))
        material = mesh.get_material(index)
        assignments[slot_name] = material.get_path_name() if material else "None"
    if assignments != EXPECTED_MATERIALS:
        fail("shared material assignments drifted: {}".format(assignments))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    placements = []
    replaced = []
    context = {}
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == PLACEMENT_LABEL:
            placements.append(actor)
        elif label == REPLACED_FAB_LABEL:
            replaced.append(actor)
        elif label in {QUARTERMASTER_LABEL, VENDOR_SERVICE_LABEL, SUPPLY_CHEST_LABEL}:
            context[label] = actor
    if len(placements) != 1:
        fail("expected one saved vendor stall, found {}".format(len(placements)))
    if replaced:
        fail("replaced generic Fab fence remains in the map")
    for required in {QUARTERMASTER_LABEL, VENDOR_SERVICE_LABEL, SUPPLY_CHEST_LABEL}:
        if required not in context:
            fail("missing vendor context actor {}".format(required))

    actor = placements[0]
    if not isinstance(actor, unreal.StaticMeshActor):
        fail("saved placement is not a StaticMeshActor")
    if (actor.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("placement location drifted: {}".format(actor.get_actor_location()))
    yaw_delta = abs((float(actor.get_actor_rotation().yaw) - EXPECTED_YAW + 180.0) % 360.0 - 180.0)
    if yaw_delta > 0.1:
        fail("placement yaw drifted: {}".format(actor.get_actor_rotation().yaw))
    scale = actor.get_actor_scale3d()
    if (scale - unreal.Vector(1.0, 1.0, 1.0)).length() > 0.001:
        fail("placement scale drifted: {}".format(scale))
    if unreal.Name("EmbermereOriginalArt") not in list(actor.get_editor_property("tags")):
        fail("placement lost the EmbermereOriginalArt tag")

    component = actor.get_editor_property("static_mesh_component")
    if component.get_editor_property("static_mesh") != mesh:
        fail("placement references the wrong static mesh")
    if str(component.get_collision_profile_name()) != "BlockAll":
        fail("placement collision profile drifted: {}".format(component.get_collision_profile_name()))

    quartermaster_location = context[QUARTERMASTER_LABEL].get_actor_location()
    service_location = context[VENDOR_SERVICE_LABEL].get_actor_location()
    if (quartermaster_location - service_location).length() > 0.1:
        fail("quartermaster art and service are no longer co-located")
    if abs(float(quartermaster_location.x) + 1530.0) > 0.1 or abs(float(quartermaster_location.y) + 1190.0) > 0.1:
        fail("quartermaster moved away from the stall approach: {}".format(quartermaster_location))

    # The art sits 240 cm behind the service. The 180-degree placement yaw
    # compensates for FBX's local-Y mirror so the counter faces that approach.
    if abs(float(actor.get_actor_location().y - quartermaster_location.y) + 240.0) > 0.1:
        fail("stall-to-quartermaster spacing drifted")

    unreal.log(
        "Embermere Fenwatch vendor stall validation passed: 394 x 242 x 306 cm, 5476 triangles, five shared materials, five authored support/counter collision boxes, one grounded project-owned placement behind the unchanged art-only quartermaster and vendor service, and the generic Fab fence removed"
    )


if __name__ == "__main__":
    main()
