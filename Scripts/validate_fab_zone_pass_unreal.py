import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
EXPECTED_FABPASS_COUNT = 62
EXPECTED_ORIGINAL_ART_COUNT = 9
ORIGINAL_WAYSTONE_LABEL = "Embermere_Waystone_Road_01"
ORIGINAL_WAYSTONE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereWaystone_01.SM_EmbermereWaystone_01"
ORIGINAL_EMBER_LAMP_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereEmberLamp_01.SM_EmbermereEmberLamp_01"
ORIGINAL_EMBER_LAMPS = {
    "Embermere_EmberLamp_Mara_01": ((-1970.0, -775.0, 20.0), 5.0),
    "Embermere_EmberLamp_Road_01": ((-1150.0, -520.0, 20.0), 8.0),
}
ORIGINAL_SIGNPOST_LABEL = "Embermere_RoadSignpost_01"
ORIGINAL_SIGNPOST_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereRoadSignpost_01.SM_EmbermereRoadSignpost_01"
ORIGINAL_SIGNPOST_LOCATION = (20.0, -170.0, 20.0)
ORIGINAL_SIGNPOST_YAW = 22.0
ORIGINAL_GATE_LABEL = "Embermere_RoadGate_01"
ORIGINAL_GATE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereRoadGate_01.SM_EmbermereRoadGate_01"
ORIGINAL_GATE_LOCATION = (1080.0, 540.0, 20.0)
ORIGINAL_GATE_YAW = 20.0
ORIGINAL_FENCE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereBoundaryFence_01.SM_EmbermereBoundaryFence_01"
ORIGINAL_FENCES = {
    "Embermere_BoundaryFence_GateSouth_01": ((1206.55, 192.35, 20.0), 20.0),
    "Embermere_BoundaryFence_GateNorth_01": ((953.45, 887.65, 20.0), 20.0),
}
ORIGINAL_BOUNDARY_STONE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereBoundaryStone_01.SM_EmbermereBoundaryStone_01"
ORIGINAL_BOUNDARY_STONES = {
    "Embermere_BoundaryStone_GateSouth_01": ((1274.951, 4.375, 20.0), 20.0),
    "Embermere_BoundaryStone_GateNorth_01": ((885.049, 1075.625, 20.0), 20.0),
}
COMPOSITION_FOLIAGE = {
    "FabPass_Road_Pine_05": ((600.0, -500.0, 20.0), -35.0, 0.48),
    "FabPass_Wild_Tree_South_01": ((1600.0, -400.0, 20.0), 15.0, 0.5),
}
ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS = {
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}
GROUND_MATERIAL_PATH = "/Game/Art/Embermere/Environment/M_EmbermereGround.M_EmbermereGround"
GROUND_ACTOR_LABELS = {
    "Zone_Ground_Embermere_Glen",
    "Safe_Area_Ring",
    "Combat_Pocket_Ring",
}
VISUAL_ONLY_GROUND_LABELS = {
    "Safe_Area_Ring",
    "Combat_Pocket_Ring",
}
STARTER_ENEMY_LAYOUT = {
    "Starter_Enemy_01": (1900.0, 300.0, 95.0),
    "Starter_Enemy_02": (1700.0, 1100.0, 95.0),
    "Starter_Enemy_03": (2500.0, 1300.0, 95.0),
}
STARTER_MARKER_LAYOUT = {
    "Enemy_Visual_Marker_01": (1900.0, 300.0, 90.0),
    "Enemy_Visual_Marker_02": (1700.0, 1100.0, 90.0),
    "Enemy_Visual_Marker_03": (2500.0, 1300.0, 90.0),
}
STARTER_AGGRO_RADIUS = 525.0
MIN_STARTER_ENEMY_SPACING = 800.0

REQUIRED_LABELS = {
    "PlayerStart_Embermere_Village",
    "Quest_Giver_Mara_Fenwatch",
    "Sun_Key_Light",
    "Sky_Ambient_Light",
    "Sky Atmosphere",
    "Light_Mist_Fog",
    "Starter_Enemy_01",
    "Starter_Enemy_02",
    "Starter_Enemy_03",
    ORIGINAL_WAYSTONE_LABEL,
    ORIGINAL_SIGNPOST_LABEL,
    ORIGINAL_GATE_LABEL,
} | set(ORIGINAL_EMBER_LAMPS) | set(ORIGINAL_FENCES) | set(ORIGINAL_BOUNDARY_STONES) | set(COMPOSITION_FOLIAGE)

REMOVED_GREYBOX_LABELS = {
    "Village_Hall_Blockout",
    "Blacksmith_Blockout",
    "Inn_Blockout",
    "Road_Marker_01",
    "Road_Marker_02",
    "Road_Marker_03",
    "Road_Marker_04",
    "Road_Marker_05",
    "Ruin_Broken_Arch_Left",
    "Ruin_Broken_Arch_Right",
    "Ruin_Broken_Arch_Top",
    "Ruin_Fallen_Stone",
    "FabPass_Village_Lamp_Mara",
    "FabPass_Village_Lamp_Road",
}


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def fail(message):
    unreal.log_error("Embermere Fab zone validation failed: {}".format(message))
    sys.exit(1)


def nearly_equal(actual, expected, tolerance=0.0001):
    return abs(float(actual) - float(expected)) <= tolerance


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = list(unreal.EditorLevelLibrary.get_all_level_actors())
    labels = {actor_label(actor) for actor in actors}
    fabpass_labels = [label for label in labels if label.startswith("FabPass_")]

    if len(fabpass_labels) != EXPECTED_FABPASS_COUNT:
        fail("expected {} FabPass actors, found {}".format(EXPECTED_FABPASS_COUNT, len(fabpass_labels)))

    missing_required = sorted(REQUIRED_LABELS - labels)
    if missing_required:
        fail("missing gameplay anchors {}".format(missing_required))

    lingering_greybox = sorted(REMOVED_GREYBOX_LABELS & labels)
    if lingering_greybox:
        fail("old visual greybox actors still present {}".format(lingering_greybox))

    tilted_fabpass = []
    for actor in actors:
        label = actor_label(actor)
        if not label.startswith("FabPass_"):
            continue

        rotation = actor.get_actor_rotation()
        if abs(rotation.pitch) > 0.1 or abs(rotation.roll) > 0.1:
            tilted_fabpass.append({
                "label": label,
                "pitch": round(rotation.pitch, 2),
                "roll": round(rotation.roll, 2),
            })

    if tilted_fabpass:
        fail("FabPass actors must stay upright; found {}".format(tilted_fabpass))

    actors_by_label = {actor_label(actor): actor for actor in actors}
    original_art_labels = {
        actor_label(actor)
        for actor in actors
        if unreal.Name("EmbermereOriginalArt") in list(actor.tags)
    }
    if len(original_art_labels) != EXPECTED_ORIGINAL_ART_COUNT:
        fail("expected {} EmbermereOriginalArt placements, found {}: {}".format(
            EXPECTED_ORIGINAL_ART_COUNT,
            len(original_art_labels),
            sorted(original_art_labels),
        ))
    waystone = actors_by_label[ORIGINAL_WAYSTONE_LABEL]
    waystone_component = waystone.get_component_by_class(unreal.StaticMeshComponent)
    waystone_mesh = waystone_component.get_editor_property("static_mesh") if waystone_component else None
    waystone_mesh_path = waystone_mesh.get_path_name() if waystone_mesh else "None"
    if waystone_mesh_path != ORIGINAL_WAYSTONE_PATH:
        fail("{} must use {}, found {}".format(ORIGINAL_WAYSTONE_LABEL, ORIGINAL_WAYSTONE_PATH, waystone_mesh_path))

    waystone_location = waystone.get_actor_location()
    waystone_rotation = waystone.get_actor_rotation()
    if not all((
        nearly_equal(waystone_location.x, -690.0, 1.0),
        nearly_equal(waystone_location.y, -25.0, 1.0),
        nearly_equal(waystone_location.z, 20.0, 1.0),
        nearly_equal(waystone_rotation.pitch, 0.0, 0.1),
        nearly_equal(waystone_rotation.yaw, 30.0, 0.1),
        nearly_equal(waystone_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            ORIGINAL_WAYSTONE_LABEL,
            waystone_location,
            waystone_rotation,
        ))

    if unreal.Name("EmbermereOriginalArt") not in list(waystone.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(ORIGINAL_WAYSTONE_LABEL))

    lamp_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_EMBER_LAMP_PATH)
    if not lamp_mesh or not isinstance(lamp_mesh, unreal.StaticMesh):
        fail("missing original ember lamp mesh {}".format(ORIGINAL_EMBER_LAMP_PATH))
    lamp_import_data = lamp_mesh.get_editor_property("asset_import_data")
    import_data_class = lamp_import_data.get_class().get_name() if lamp_import_data else "None"
    if import_data_class != "FbxStaticMeshImportData":
        fail("original ember lamp must retain classic FBX import data, found {}".format(import_data_class))
    lamp_body_setup = lamp_mesh.get_editor_property("body_setup")
    lamp_aggregate = lamp_body_setup.get_editor_property("agg_geom") if lamp_body_setup else None
    lamp_box_count = len(lamp_aggregate.get_editor_property("box_elems")) if lamp_aggregate else 0
    if lamp_box_count != 2:
        fail("original ember lamp must retain 2 authored box colliders, found {}".format(lamp_box_count))
    lamp_bounds = lamp_mesh.get_bounds()
    if not all((
        nearly_equal(lamp_bounds.origin.z, 127.0, 2.0),
        nearly_equal(lamp_bounds.box_extent.z, 127.0, 2.0),
    )):
        fail("original ember lamp bounds drifted: origin={}, extent={}".format(
            lamp_bounds.origin,
            lamp_bounds.box_extent,
        ))

    for label, (expected_location, expected_yaw) in ORIGINAL_EMBER_LAMPS.items():
        lamp = actors_by_label[label]
        lamp_component = lamp.get_component_by_class(unreal.StaticMeshComponent)
        actor_mesh = lamp_component.get_editor_property("static_mesh") if lamp_component else None
        actor_mesh_path = actor_mesh.get_path_name() if actor_mesh else "None"
        if actor_mesh_path != ORIGINAL_EMBER_LAMP_PATH:
            fail("{} must use {}, found {}".format(label, ORIGINAL_EMBER_LAMP_PATH, actor_mesh_path))

        location = lamp.get_actor_location()
        rotation = lamp.get_actor_rotation()
        if not all((
            nearly_equal(location.x, expected_location[0], 1.0),
            nearly_equal(location.y, expected_location[1], 1.0),
            nearly_equal(location.z, expected_location[2], 1.0),
            nearly_equal(rotation.pitch, 0.0, 0.1),
            nearly_equal(rotation.yaw, expected_yaw, 0.1),
            nearly_equal(rotation.roll, 0.0, 0.1),
        )):
            fail("{} transform drifted: location={}, rotation={}".format(label, location, rotation))
        if unreal.Name("EmbermereOriginalArt") not in list(lamp.tags):
            fail("{} must retain the EmbermereOriginalArt tag".format(label))

    signpost_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_SIGNPOST_PATH)
    if not signpost_mesh or not isinstance(signpost_mesh, unreal.StaticMesh):
        fail("missing original road signpost mesh {}".format(ORIGINAL_SIGNPOST_PATH))
    signpost_import_data = signpost_mesh.get_editor_property("asset_import_data")
    signpost_import_class = signpost_import_data.get_class().get_name() if signpost_import_data else "None"
    if signpost_import_class != "FbxStaticMeshImportData":
        fail("original road signpost must retain classic FBX import data, found {}".format(signpost_import_class))
    signpost_body_setup = signpost_mesh.get_editor_property("body_setup")
    signpost_aggregate = signpost_body_setup.get_editor_property("agg_geom") if signpost_body_setup else None
    signpost_box_count = len(signpost_aggregate.get_editor_property("box_elems")) if signpost_aggregate else 0
    if signpost_box_count != 2:
        fail("original road signpost must retain 2 authored box colliders, found {}".format(signpost_box_count))
    signpost_bounds = signpost_mesh.get_bounds()
    if not all((
        nearly_equal(signpost_bounds.origin.z, 132.0, 2.0),
        nearly_equal(signpost_bounds.box_extent.z, 132.0, 2.0),
    )):
        fail("original road signpost bounds drifted: origin={}, extent={}".format(
            signpost_bounds.origin,
            signpost_bounds.box_extent,
        ))
    signpost_material_paths = set()
    for static_material in list(signpost_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            signpost_material_paths.add(material.get_path_name())
    if signpost_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("original road signpost material set drifted: {}".format(sorted(signpost_material_paths)))

    signpost = actors_by_label[ORIGINAL_SIGNPOST_LABEL]
    signpost_component = signpost.get_component_by_class(unreal.StaticMeshComponent)
    actor_signpost_mesh = signpost_component.get_editor_property("static_mesh") if signpost_component else None
    actor_signpost_path = actor_signpost_mesh.get_path_name() if actor_signpost_mesh else "None"
    if actor_signpost_path != ORIGINAL_SIGNPOST_PATH:
        fail("{} must use {}, found {}".format(
            ORIGINAL_SIGNPOST_LABEL,
            ORIGINAL_SIGNPOST_PATH,
            actor_signpost_path,
        ))
    signpost_location = signpost.get_actor_location()
    signpost_rotation = signpost.get_actor_rotation()
    if not all((
        nearly_equal(signpost_location.x, ORIGINAL_SIGNPOST_LOCATION[0], 1.0),
        nearly_equal(signpost_location.y, ORIGINAL_SIGNPOST_LOCATION[1], 1.0),
        nearly_equal(signpost_location.z, ORIGINAL_SIGNPOST_LOCATION[2], 1.0),
        nearly_equal(signpost_rotation.pitch, 0.0, 0.1),
        nearly_equal(signpost_rotation.yaw, ORIGINAL_SIGNPOST_YAW, 0.1),
        nearly_equal(signpost_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            ORIGINAL_SIGNPOST_LABEL,
            signpost_location,
            signpost_rotation,
        ))
    if unreal.Name("EmbermereOriginalArt") not in list(signpost.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(ORIGINAL_SIGNPOST_LABEL))

    gate_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_GATE_PATH)
    if not gate_mesh or not isinstance(gate_mesh, unreal.StaticMesh):
        fail("missing original road gate mesh {}".format(ORIGINAL_GATE_PATH))
    gate_import_data = gate_mesh.get_editor_property("asset_import_data")
    gate_import_class = gate_import_data.get_class().get_name() if gate_import_data else "None"
    if gate_import_class != "FbxStaticMeshImportData":
        fail("original road gate must retain classic FBX import data, found {}".format(gate_import_class))
    gate_body_setup = gate_mesh.get_editor_property("body_setup")
    gate_aggregate = gate_body_setup.get_editor_property("agg_geom") if gate_body_setup else None
    gate_box_count = len(gate_aggregate.get_editor_property("box_elems")) if gate_aggregate else 0
    if gate_box_count != 4:
        fail("original road gate must retain 4 authored box colliders, found {}".format(gate_box_count))
    gate_bounds = gate_mesh.get_bounds()
    if not all((
        nearly_equal(gate_bounds.origin.z, 186.865, 2.0),
        nearly_equal(gate_bounds.box_extent.z, 186.865, 2.0),
    )):
        fail("original road gate bounds drifted: origin={}, extent={}".format(
            gate_bounds.origin,
            gate_bounds.box_extent,
        ))
    gate_material_paths = set()
    for static_material in list(gate_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            gate_material_paths.add(material.get_path_name())
    if gate_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("original road gate material set drifted: {}".format(sorted(gate_material_paths)))

    gate = actors_by_label[ORIGINAL_GATE_LABEL]
    gate_component = gate.get_component_by_class(unreal.StaticMeshComponent)
    actor_gate_mesh = gate_component.get_editor_property("static_mesh") if gate_component else None
    actor_gate_path = actor_gate_mesh.get_path_name() if actor_gate_mesh else "None"
    if actor_gate_path != ORIGINAL_GATE_PATH:
        fail("{} must use {}, found {}".format(ORIGINAL_GATE_LABEL, ORIGINAL_GATE_PATH, actor_gate_path))
    gate_location = gate.get_actor_location()
    gate_rotation = gate.get_actor_rotation()
    if not all((
        nearly_equal(gate_location.x, ORIGINAL_GATE_LOCATION[0], 1.0),
        nearly_equal(gate_location.y, ORIGINAL_GATE_LOCATION[1], 1.0),
        nearly_equal(gate_location.z, ORIGINAL_GATE_LOCATION[2], 1.0),
        nearly_equal(gate_rotation.pitch, 0.0, 0.1),
        nearly_equal(gate_rotation.yaw, ORIGINAL_GATE_YAW, 0.1),
        nearly_equal(gate_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            ORIGINAL_GATE_LABEL,
            gate_location,
            gate_rotation,
        ))
    if unreal.Name("EmbermereOriginalArt") not in list(gate.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(ORIGINAL_GATE_LABEL))

    fence_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_FENCE_PATH)
    if not fence_mesh or not isinstance(fence_mesh, unreal.StaticMesh):
        fail("missing original boundary fence mesh {}".format(ORIGINAL_FENCE_PATH))
    fence_import_data = fence_mesh.get_editor_property("asset_import_data")
    fence_import_class = fence_import_data.get_class().get_name() if fence_import_data else "None"
    if fence_import_class != "FbxStaticMeshImportData":
        fail("original boundary fence must retain classic FBX import data, found {}".format(fence_import_class))
    fence_body_setup = fence_mesh.get_editor_property("body_setup")
    fence_aggregate = fence_body_setup.get_editor_property("agg_geom") if fence_body_setup else None
    fence_box_count = len(fence_aggregate.get_editor_property("box_elems")) if fence_aggregate else 0
    if fence_box_count != 3:
        fail("original boundary fence must retain 3 authored box colliders, found {}".format(fence_box_count))
    fence_bounds = fence_mesh.get_bounds()
    if not all((
        nearly_equal(fence_bounds.origin.z, 90.0, 2.0),
        nearly_equal(fence_bounds.box_extent.z, 90.0, 2.0),
    )):
        fail("original boundary fence bounds drifted: origin={}, extent={}".format(
            fence_bounds.origin,
            fence_bounds.box_extent,
        ))
    fence_material_paths = set()
    for static_material in list(fence_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            fence_material_paths.add(material.get_path_name())
    if fence_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("original boundary fence material set drifted: {}".format(sorted(fence_material_paths)))

    for label, (expected_location, expected_yaw) in ORIGINAL_FENCES.items():
        fence = actors_by_label[label]
        fence_component = fence.get_component_by_class(unreal.StaticMeshComponent)
        actor_mesh = fence_component.get_editor_property("static_mesh") if fence_component else None
        actor_mesh_path = actor_mesh.get_path_name() if actor_mesh else "None"
        if actor_mesh_path != ORIGINAL_FENCE_PATH:
            fail("{} must use {}, found {}".format(label, ORIGINAL_FENCE_PATH, actor_mesh_path))

        location = fence.get_actor_location()
        rotation = fence.get_actor_rotation()
        if not all((
            nearly_equal(location.x, expected_location[0], 1.0),
            nearly_equal(location.y, expected_location[1], 1.0),
            nearly_equal(location.z, expected_location[2], 1.0),
            nearly_equal(rotation.pitch, 0.0, 0.1),
            nearly_equal(rotation.yaw, expected_yaw, 0.1),
            nearly_equal(rotation.roll, 0.0, 0.1),
        )):
            fail("{} transform drifted: location={}, rotation={}".format(label, location, rotation))
        if unreal.Name("EmbermereOriginalArt") not in list(fence.tags):
            fail("{} must retain the EmbermereOriginalArt tag".format(label))

    boundary_stone_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_BOUNDARY_STONE_PATH)
    if not boundary_stone_mesh or not isinstance(boundary_stone_mesh, unreal.StaticMesh):
        fail("missing original boundary stone mesh {}".format(ORIGINAL_BOUNDARY_STONE_PATH))
    boundary_stone_import_data = boundary_stone_mesh.get_editor_property("asset_import_data")
    boundary_stone_import_class = (
        boundary_stone_import_data.get_class().get_name()
        if boundary_stone_import_data
        else "None"
    )
    if boundary_stone_import_class != "FbxStaticMeshImportData":
        fail("original boundary stone must retain classic FBX import data, found {}".format(
            boundary_stone_import_class,
        ))
    boundary_stone_body_setup = boundary_stone_mesh.get_editor_property("body_setup")
    boundary_stone_aggregate = (
        boundary_stone_body_setup.get_editor_property("agg_geom")
        if boundary_stone_body_setup
        else None
    )
    boundary_stone_box_count = (
        len(boundary_stone_aggregate.get_editor_property("box_elems"))
        if boundary_stone_aggregate
        else 0
    )
    if boundary_stone_box_count != 2:
        fail("original boundary stone must retain 2 authored box colliders, found {}".format(
            boundary_stone_box_count,
        ))
    boundary_stone_bounds = boundary_stone_mesh.get_bounds()
    if not all((
        nearly_equal(boundary_stone_bounds.origin.z, 126.8655, 2.0),
        nearly_equal(boundary_stone_bounds.box_extent.z, 126.8655, 2.0),
    )):
        fail("original boundary stone bounds drifted: origin={}, extent={}".format(
            boundary_stone_bounds.origin,
            boundary_stone_bounds.box_extent,
        ))
    boundary_stone_material_paths = set()
    for static_material in list(boundary_stone_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            boundary_stone_material_paths.add(material.get_path_name())
    if boundary_stone_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("original boundary stone material set drifted: {}".format(
            sorted(boundary_stone_material_paths),
        ))

    for label, (expected_location, expected_yaw) in ORIGINAL_BOUNDARY_STONES.items():
        boundary_stone = actors_by_label[label]
        component = boundary_stone.get_component_by_class(unreal.StaticMeshComponent)
        actor_mesh = component.get_editor_property("static_mesh") if component else None
        actor_mesh_path = actor_mesh.get_path_name() if actor_mesh else "None"
        if actor_mesh_path != ORIGINAL_BOUNDARY_STONE_PATH:
            fail("{} must use {}, found {}".format(
                label,
                ORIGINAL_BOUNDARY_STONE_PATH,
                actor_mesh_path,
            ))

        location = boundary_stone.get_actor_location()
        rotation = boundary_stone.get_actor_rotation()
        if not all((
            nearly_equal(location.x, expected_location[0], 1.0),
            nearly_equal(location.y, expected_location[1], 1.0),
            nearly_equal(location.z, expected_location[2], 1.0),
            nearly_equal(rotation.pitch, 0.0, 0.1),
            nearly_equal(rotation.yaw, expected_yaw, 0.1),
            nearly_equal(rotation.roll, 0.0, 0.1),
        )):
            fail("{} transform drifted: location={}, rotation={}".format(
                label,
                location,
                rotation,
            ))
        if unreal.Name("EmbermereOriginalArt") not in list(boundary_stone.tags):
            fail("{} must retain the EmbermereOriginalArt tag".format(label))

    for label, (expected_location, expected_yaw, expected_scale) in COMPOSITION_FOLIAGE.items():
        foliage = actors_by_label[label]
        location = foliage.get_actor_location()
        rotation = foliage.get_actor_rotation()
        scale = foliage.get_actor_scale3d()
        if not all((
            nearly_equal(location.x, expected_location[0], 1.0),
            nearly_equal(location.y, expected_location[1], 1.0),
            nearly_equal(location.z, expected_location[2], 1.0),
            nearly_equal(rotation.pitch, 0.0, 0.1),
            nearly_equal(rotation.yaw, expected_yaw, 0.1),
            nearly_equal(rotation.roll, 0.0, 0.1),
            nearly_equal(scale.x, expected_scale, 0.001),
            nearly_equal(scale.y, expected_scale, 0.001),
            nearly_equal(scale.z, expected_scale, 0.001),
        )):
            fail("{} composition transform drifted: location={}, rotation={}, scale={}".format(
                label,
                location,
                rotation,
                scale,
            ))

    missing_ground_actors = sorted(GROUND_ACTOR_LABELS - set(actors_by_label))
    if missing_ground_actors:
        fail("missing ground foundation actors {}".format(missing_ground_actors))

    wrong_ground_materials = []
    for label in sorted(GROUND_ACTOR_LABELS):
        component = actors_by_label[label].get_component_by_class(unreal.StaticMeshComponent)
        material = component.get_material(0) if component else None
        material_path = material.get_path_name() if material else "None"
        if material_path != GROUND_MATERIAL_PATH:
            wrong_ground_materials.append({"label": label, "material": material_path})

    if wrong_ground_materials:
        fail("ground foundation must use {}: {}".format(GROUND_MATERIAL_PATH, wrong_ground_materials))

    for label in sorted(VISUAL_ONLY_GROUND_LABELS):
        component = actors_by_label[label].get_component_by_class(unreal.StaticMeshComponent)
        if not component:
            fail("{} has no static mesh component".format(label))
        collision_enabled = component.get_collision_enabled()
        if collision_enabled != unreal.CollisionEnabled.NO_COLLISION:
            fail("{} must remain a visual-only ground ring, found {}".format(
                label,
                collision_enabled,
            ))

    combat_pocket = actors_by_label["Combat_Pocket_Ring"]
    combat_location = combat_pocket.get_actor_location()
    combat_scale = combat_pocket.get_actor_scale3d()
    if not all((
        nearly_equal(combat_location.x, 2100.0, 1.0),
        nearly_equal(combat_location.y, 800.0, 1.0),
        nearly_equal(combat_location.z, 8.0, 1.0),
        nearly_equal(combat_scale.x, 15.0, 0.01),
        nearly_equal(combat_scale.y, 15.0, 0.01),
        nearly_equal(combat_scale.z, 0.08, 0.001),
    )):
        fail("combat pocket transform drifted: location={}, scale={}".format(
            combat_location,
            combat_scale,
        ))

    for label, expected_location in {**STARTER_ENEMY_LAYOUT, **STARTER_MARKER_LAYOUT}.items():
        actor = actors_by_label.get(label)
        if not actor:
            fail("missing starter encounter actor {}".format(label))
        location = actor.get_actor_location()
        if not all(nearly_equal(actual, expected, 1.0) for actual, expected in zip(
            (location.x, location.y, location.z),
            expected_location,
        )):
            fail("{} location drifted: expected {}, found {}".format(label, expected_location, location))

    for label in sorted(STARTER_MARKER_LAYOUT):
        marker_component = actors_by_label[label].get_component_by_class(unreal.StaticMeshComponent)
        if not marker_component:
            fail("{} has no static mesh component".format(label))
        collision_enabled = marker_component.get_collision_enabled()
        if collision_enabled != unreal.CollisionEnabled.NO_COLLISION:
            fail("{} must remain visual-only with NoCollision, found {}".format(
                label,
                collision_enabled,
            ))

    enemy_locations = []
    for label in sorted(STARTER_ENEMY_LAYOUT):
        enemy = actors_by_label[label]
        aggro_radius = enemy.get_editor_property("aggro_radius")
        if not nearly_equal(aggro_radius, STARTER_AGGRO_RADIUS):
            fail("{} aggro radius must remain {}, found {}".format(
                label,
                STARTER_AGGRO_RADIUS,
                aggro_radius,
            ))
        enemy_locations.append((label, enemy.get_actor_location()))

    for index, (left_label, left_location) in enumerate(enemy_locations):
        for right_label, right_location in enemy_locations[index + 1:]:
            delta = left_location - right_location
            spacing = math.sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z)
            if spacing < MIN_STARTER_ENEMY_SPACING:
                fail("starter enemies {} and {} are only {:.1f} cm apart".format(
                    left_label,
                    right_label,
                    spacing,
                ))

    sun_component = actors_by_label["Sun_Key_Light"].get_component_by_class(unreal.DirectionalLightComponent)
    sky_component = actors_by_label["Sky_Ambient_Light"].get_component_by_class(unreal.SkyLightComponent)
    fog_component = actors_by_label["Light_Mist_Fog"].get_component_by_class(unreal.ExponentialHeightFogComponent)
    if not sun_component or not sky_component or not fog_component:
        fail("could not resolve saved daylight components")

    if not nearly_equal(sun_component.get_editor_property("intensity"), 10.0):
        fail("sun intensity must remain 10.0")
    if not sun_component.get_editor_property("atmosphere_sun_light"):
        fail("sun must remain the atmosphere sun light")
    if not nearly_equal(sky_component.get_editor_property("intensity"), 1.35):
        fail("skylight intensity must remain 1.35")
    if not sky_component.get_editor_property("real_time_capture"):
        fail("skylight real-time capture must remain enabled")
    if not nearly_equal(fog_component.get_editor_property("fog_density"), 0.008):
        fail("fog density must remain 0.008")
    if not nearly_equal(fog_component.get_editor_property("fog_height_falloff"), 0.25):
        fail("fog height falloff must remain 0.25")
    fog_color = fog_component.get_editor_property("fog_inscattering_luminance")
    if not all((
        nearly_equal(fog_color.r, 0.08),
        nearly_equal(fog_color.g, 0.12),
        nearly_equal(fog_color.b, 0.18),
    )):
        fail("fog inscattering color must remain (0.08, 0.12, 0.18)")
    if fog_component.get_editor_property("enable_volumetric_fog"):
        fail("volumetric fog must stay disabled for the Mac-friendly prototype baseline")

    unreal.log("Embermere zone validation passed: {} upright FabPass actors, {} original-art placements, separated starter pulls, visual-only encounter markers, gameplay anchors, moss ground, and daylight baseline intact".format(
        len(fabpass_labels),
        EXPECTED_ORIGINAL_ART_COUNT,
    ))


if __name__ == "__main__":
    main()
