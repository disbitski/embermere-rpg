import math
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
EXPECTED_FABPASS_COUNT = 56
EXPECTED_ORIGINAL_ART_COUNT = 19
ORIGINAL_WAYSTONE_LABEL = "Embermere_Waystone_Road_01"
ORIGINAL_WAYSTONE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereWaystone_01.SM_EmbermereWaystone_01"
ORIGINAL_EMBER_LAMP_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereEmberLamp_01.SM_EmbermereEmberLamp_01"
ORIGINAL_EMBER_LAMPS = {
    "Embermere_EmberLamp_Mara_01": ((-1970.0, -775.0, 0.0), 5.0),
    "Embermere_EmberLamp_Road_01": ((-1150.0, -520.0, 0.0), 8.0),
}
ORIGINAL_FENWATCH_SHELTER_LABEL = "Embermere_FenwatchShelter_Mara_01"
ORIGINAL_FENWATCH_SHELTER_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereFenwatchShelter_01.SM_EmbermereFenwatchShelter_01"
ORIGINAL_FENWATCH_SHELTER_LOCATION = (-1740.0, -700.0, 0.0)
ORIGINAL_FENWATCH_SHELTER_YAW = -64.0
FENWATCH_KEEPER_LABEL = "Quest_Giver_Mara_Fenwatch"
FENWATCH_KEEPER_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/"
    "SM_EmbermereFenwatchKeeper_Mara_01.SM_EmbermereFenwatchKeeper_Mara_01"
)
FENWATCH_KEEPER_SKIN_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/"
    "M_FenwatchKeeperSkin.M_FenwatchKeeperSkin"
)
FENWATCH_KEEPER_LOCATION = (-2050.0, -850.0, 140.0)
FENWATCH_KEEPER_YAW = 35.0
FENWATCH_KEEPER_VISUAL_LOCATION = (0.0, 0.0, -140.0)
FENWATCH_KEEPER_VISUAL_YAW = 100.0
FENWATCH_QUARTERMASTER_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
FENWATCH_QUARTERMASTER_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/"
    "SM_EmbermereFenwatchQuartermaster_01.SM_EmbermereFenwatchQuartermaster_01"
)
FENWATCH_QUARTERMASTER_SKIN_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/"
    "M_FenwatchQuartermasterSkin.M_FenwatchQuartermasterSkin"
)
FENWATCH_QUARTERMASTER_LOCATION = (-1530.0, -1190.0, 0.0)
FENWATCH_QUARTERMASTER_YAW = 100.0
FENWATCH_QUARTERMASTER_ROUTE_CLEARANCE = 275.0
FENWATCH_ARMSMASTER_LABEL = "Embermere_FenwatchArmsmaster_Trainer_01"
FENWATCH_ARMSMASTER_SERVICE_LABEL = "Embermere_FenwatchArmsmaster_Service_01"
FENWATCH_ARMSMASTER_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/"
    "SM_EmbermereFenwatchArmsmaster_01.SM_EmbermereFenwatchArmsmaster_01"
)
FENWATCH_ARMSMASTER_SKELETAL_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/"
    "SK_EmbermereFenwatchArmsmaster_01.SK_EmbermereFenwatchArmsmaster_01"
)
FENWATCH_ARMSMASTER_IDLE_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/Animations/"
    "A_EmbermereFenwatchArmsmaster_Idle.A_EmbermereFenwatchArmsmaster_Idle"
)
FENWATCH_ARMSMASTER_SKIN_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/"
    "M_FenwatchArmsmasterSkin.M_FenwatchArmsmasterSkin"
)
FENWATCH_ARMSMASTER_OFFERINGS_PATH = (
    "/Game/Data/Trainers/"
    "DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"
)
FENWATCH_ARMSMASTER_LOCATION = (-1320.0, -920.0, 0.0)
FENWATCH_ARMSMASTER_YAW = 100.0
ORIGINAL_SIGNPOST_LABEL = "Embermere_RoadSignpost_01"
ORIGINAL_SIGNPOST_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereRoadSignpost_01.SM_EmbermereRoadSignpost_01"
ORIGINAL_SIGNPOST_LOCATION = (20.0, -170.0, 0.0)
ORIGINAL_SIGNPOST_YAW = 22.0
ORIGINAL_GATE_LABEL = "Embermere_RoadGate_01"
ORIGINAL_GATE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereRoadGate_01.SM_EmbermereRoadGate_01"
ORIGINAL_GATE_LOCATION = (1080.0, 540.0, 0.0)
ORIGINAL_GATE_YAW = 20.0
ORIGINAL_FENCE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereBoundaryFence_01.SM_EmbermereBoundaryFence_01"
ORIGINAL_FENCES = {
    "Embermere_BoundaryFence_GateSouth_01": ((1206.55, 192.35, 0.0), 20.0),
    "Embermere_BoundaryFence_GateNorth_01": ((953.45, 887.65, 0.0), 20.0),
}
ORIGINAL_BOUNDARY_STONE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereBoundaryStone_01.SM_EmbermereBoundaryStone_01"
ORIGINAL_BOUNDARY_STONES = {
    "Embermere_BoundaryStone_GateSouth_01": ((1274.951, 4.375, 0.0), 20.0),
    "Embermere_BoundaryStone_GateNorth_01": ((885.049, 1075.625, 0.0), 20.0),
}
ORIGINAL_SUPPLY_CHEST_LABEL = "Embermere_SupplyChest_Vendor_01"
ORIGINAL_SUPPLY_CHEST_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereSupplyChest_01.SM_EmbermereSupplyChest_01"
ORIGINAL_SUPPLY_CHEST_LOCATION = (-1740.0, -1180.0, 0.0)
ORIGINAL_SUPPLY_CHEST_YAW = 108.0
ORIGINAL_PRACTICE_DUMMY_LABEL = "Embermere_FenwatchPracticeDummy_TrainingYard_01"
ORIGINAL_PRACTICE_DUMMY_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereFenwatchPracticeDummy_01.SM_EmbermereFenwatchPracticeDummy_01"
ORIGINAL_PRACTICE_DUMMY_LOCATION = (-1120.0, -1120.0, 0.0)
ORIGINAL_PRACTICE_DUMMY_YAW = 45.0
SPAWN_AUTORUN_ROUTE_START = (-2400.0, -1200.0)
SPAWN_AUTORUN_ROUTE_END = (-1350.0, -750.0)
SUPPLY_CHEST_ROUTE_CLEARANCE = 225.0
ORIGINAL_MARSH_REED_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereMarshReedCluster_01.SM_EmbermereMarshReedCluster_01"
ORIGINAL_MARSH_REEDS = {
    "Embermere_MarshReeds_RoadSouth_01": ((1300.0, -180.0, 0.0), 15.0, 0.78),
    "Embermere_MarshReeds_RoadNorth_01": ((1020.0, 1650.0, 0.0), 75.0, 0.72),
    "Embermere_MarshReeds_WildSouth_01": ((2860.0, 80.0, 0.0), -30.0, 0.85),
    "Embermere_MarshReeds_WildNorth_01": ((3400.0, 1700.0, 0.0), 45.0, 0.80),
}
COMPOSITION_FOLIAGE = {
    "FabPass_Road_Pine_05": ((600.0, -500.0, 0.0), -35.0, 0.48),
    "FabPass_Wild_Tree_South_01": ((1600.0, -400.0, 0.0), 15.0, 0.5),
}
MOSS_MATERIAL_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss"
STONE_MATERIAL_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone"
TIMBER_MATERIAL_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber"
TREE_BILLBOARD_MATERIAL_PATH = (
    "/Game/KiteDemo/Environments/Trees/HillTree_02/"
    "M_CustomDepthBillboard_Inst.M_CustomDepthBillboard_Inst"
)
FOLIAGE_OVERRIDE_LABELS = {
    "FabPass_Road_Fern_01",
    "FabPass_Road_Fern_02",
    "FabPass_Road_Flowers_01",
    "FabPass_Road_Flowers_02",
    "FabPass_Road_Grass_01",
    "FabPass_Road_Grass_02",
    "FabPass_Wild_Ferns_01",
    "FabPass_Wild_Ferns_02",
}
TREE_OVERRIDE_LABELS = {
    "FabPass_Road_Pine_01",
    "FabPass_Road_Pine_02",
    "FabPass_Road_Pine_03",
    "FabPass_Road_Pine_04",
    "FabPass_Road_Pine_05",
    "FabPass_Road_Pine_06",
    "FabPass_Wild_Tree_North_01",
    "FabPass_Wild_Tree_North_02",
    "FabPass_Wild_Tree_North_03",
    "FabPass_Wild_Tree_South_01",
    "FabPass_Wild_Tree_South_02",
    "FabPass_Wild_Tree_East_01",
}
SPECIAL_FABPASS_Z = {
    "FabPass_Ruin_Statue": 68.0,
    "FabPass_Ruin_Water_Glint": 25.0,
    "FabPass_Ruin_Fog_Sheet_A": 60.0,
}
ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS = {
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmberLampIron.M_EmberLampIron",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber",
    "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneEmber.M_WaystoneEmber",
}
GROUND_MATERIAL_PATH = "/Game/Art/Embermere/Environment/M_EmbermereGround.M_EmbermereGround"
ORIGINAL_MARSH_REED_MATERIAL_PATHS = {
    GROUND_MATERIAL_PATH,
    MOSS_MATERIAL_PATH,
    STONE_MATERIAL_PATH,
    TIMBER_MATERIAL_PATH,
}
GROUND_MATERIAL_EXPRESSION_COUNT = 38
GROUND_SCALAR_PARAMETERS = {
    "MossRoughness",
    "PathHalfWidthCm",
    "PathRoughness",
}
GROUND_VECTOR_PARAMETERS = {
    "MossDark",
    "MossLight",
    "PathDirt",
}
GROUND_PATH_HALF_WIDTH_CM = 300.0
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
MARSH_PROWLER_MESH_PATH = (
    "/Game/Art/Embermere/Characters/Enemies/MarshProwler/"
    "SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01"
)
MARSH_PROWLER_IDLE_PATH = (
    "/Game/Art/Embermere/Characters/Enemies/MarshProwler/Animations/"
    "A_EmbermereMarshProwler_Idle.A_EmbermereMarshProwler_Idle"
)
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
    ORIGINAL_SUPPLY_CHEST_LABEL,
    ORIGINAL_PRACTICE_DUMMY_LABEL,
    ORIGINAL_FENWATCH_SHELTER_LABEL,
    FENWATCH_QUARTERMASTER_LABEL,
    FENWATCH_ARMSMASTER_LABEL,
    FENWATCH_ARMSMASTER_SERVICE_LABEL,
} | set(ORIGINAL_EMBER_LAMPS) | set(ORIGINAL_FENCES) | set(ORIGINAL_BOUNDARY_STONES) | set(ORIGINAL_MARSH_REEDS) | set(COMPOSITION_FOLIAGE)

REMOVED_GREYBOX_LABELS = {
    "Vendor_Placeholder",
    "Trainer_Placeholder",
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
    "FabPass_Village_Crates_A",
    "FabPass_Mara_Stone_Backdrop",
    "FabPass_Village_Market_Cover",
    "Enemy_Visual_Marker_01",
    "Enemy_Visual_Marker_02",
    "Enemy_Visual_Marker_03",
    "FabPass_Ruin_Soul_Arch_Accent",
    "FabPass_Ruin_Soul_Pillar",
    "FabPass_Village_Crate_C",
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


def distance_to_segment_2d(point, start, end):
    segment_x = end[0] - start[0]
    segment_y = end[1] - start[1]
    length_squared = segment_x * segment_x + segment_y * segment_y
    if length_squared <= 0.0:
        return math.hypot(point[0] - start[0], point[1] - start[1])
    projection = (
        (point[0] - start[0]) * segment_x
        + (point[1] - start[1]) * segment_y
    ) / length_squared
    projection = max(0.0, min(1.0, projection))
    closest = (
        start[0] + projection * segment_x,
        start[1] + projection * segment_y,
    )
    return math.hypot(point[0] - closest[0], point[1] - closest[1])


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
    grounding_drift = []
    for label in fabpass_labels:
        expected_z = SPECIAL_FABPASS_Z.get(label, 0.0)
        actual_z = actors_by_label[label].get_actor_location().z
        if not nearly_equal(actual_z, expected_z, 1.0):
            grounding_drift.append({
                "label": label,
                "expected_z": expected_z,
                "actual_z": round(actual_z, 3),
            })
    if grounding_drift:
        fail("FabPass actor grounding drifted: {}".format(grounding_drift))

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
        nearly_equal(waystone_location.z, 0.0, 1.0),
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

    shelter_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_FENWATCH_SHELTER_PATH)
    if not shelter_mesh or not isinstance(shelter_mesh, unreal.StaticMesh):
        fail("missing original Fenwatch shelter mesh {}".format(ORIGINAL_FENWATCH_SHELTER_PATH))
    shelter_import_data = shelter_mesh.get_editor_property("asset_import_data")
    shelter_import_class = shelter_import_data.get_class().get_name() if shelter_import_data else "None"
    if shelter_import_class != "FbxStaticMeshImportData":
        fail("original Fenwatch shelter must retain classic FBX import data, found {}".format(shelter_import_class))
    shelter_body_setup = shelter_mesh.get_editor_property("body_setup")
    shelter_aggregate = shelter_body_setup.get_editor_property("agg_geom") if shelter_body_setup else None
    shelter_box_count = len(shelter_aggregate.get_editor_property("box_elems")) if shelter_aggregate else 0
    if shelter_box_count != 4:
        fail("original Fenwatch shelter must retain 4 authored support colliders, found {}".format(shelter_box_count))
    shelter_bounds = shelter_mesh.get_bounds()
    shelter_size = shelter_bounds.box_extent * 2.0
    shelter_xy = sorted([float(shelter_size.x), float(shelter_size.y)])
    if not all((
        nearly_equal(shelter_xy[0], 296.782, 2.0),
        nearly_equal(shelter_xy[1], 438.0, 2.0),
        nearly_equal(shelter_size.z, 369.5, 2.0),
        nearly_equal(shelter_bounds.origin.z, shelter_bounds.box_extent.z, 1.0),
    )):
        fail("original Fenwatch shelter bounds drifted: origin={}, extent={}".format(
            shelter_bounds.origin,
            shelter_bounds.box_extent,
        ))
    if shelter_mesh.get_num_triangles(0) != 4348:
        fail("original Fenwatch shelter triangle contract drifted: {}".format(shelter_mesh.get_num_triangles(0)))
    shelter_material_paths = set()
    for static_material in list(shelter_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            shelter_material_paths.add(material.get_path_name())
    if shelter_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("original Fenwatch shelter material set drifted: {}".format(sorted(shelter_material_paths)))

    shelter = actors_by_label[ORIGINAL_FENWATCH_SHELTER_LABEL]
    shelter_component = shelter.get_component_by_class(unreal.StaticMeshComponent)
    shelter_actor_mesh = shelter_component.get_editor_property("static_mesh") if shelter_component else None
    shelter_actor_mesh_path = shelter_actor_mesh.get_path_name() if shelter_actor_mesh else "None"
    if shelter_actor_mesh_path != ORIGINAL_FENWATCH_SHELTER_PATH:
        fail("{} must use {}, found {}".format(
            ORIGINAL_FENWATCH_SHELTER_LABEL,
            ORIGINAL_FENWATCH_SHELTER_PATH,
            shelter_actor_mesh_path,
        ))
    shelter_location = shelter.get_actor_location()
    shelter_rotation = shelter.get_actor_rotation()
    if not all((
        nearly_equal(shelter_location.x, ORIGINAL_FENWATCH_SHELTER_LOCATION[0], 1.0),
        nearly_equal(shelter_location.y, ORIGINAL_FENWATCH_SHELTER_LOCATION[1], 1.0),
        nearly_equal(shelter_location.z, ORIGINAL_FENWATCH_SHELTER_LOCATION[2], 1.0),
        nearly_equal(shelter_rotation.pitch, 0.0, 0.1),
        nearly_equal(shelter_rotation.yaw, ORIGINAL_FENWATCH_SHELTER_YAW, 0.1),
        nearly_equal(shelter_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            ORIGINAL_FENWATCH_SHELTER_LABEL,
            shelter_location,
            shelter_rotation,
        ))
    if unreal.Name("EmbermereOriginalArt") not in list(shelter.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(ORIGINAL_FENWATCH_SHELTER_LABEL))

    keeper_mesh = unreal.EditorAssetLibrary.load_asset(FENWATCH_KEEPER_PATH)
    if not keeper_mesh or not isinstance(keeper_mesh, unreal.StaticMesh):
        fail("missing original Fenwatch keeper mesh {}".format(FENWATCH_KEEPER_PATH))
    keeper_import_data = keeper_mesh.get_editor_property("asset_import_data")
    keeper_import_class = keeper_import_data.get_class().get_name() if keeper_import_data else "None"
    if keeper_import_class != "FbxStaticMeshImportData":
        fail("Fenwatch keeper must retain classic FBX import data, found {}".format(keeper_import_class))
    keeper_body_setup = keeper_mesh.get_editor_property("body_setup")
    keeper_aggregate = keeper_body_setup.get_editor_property("agg_geom") if keeper_body_setup else None
    keeper_collision_count = sum(
        len(keeper_aggregate.get_editor_property(property_name))
        for property_name in ("box_elems", "sphere_elems", "sphyl_elems", "convex_elems")
    ) if keeper_aggregate else 0
    if keeper_collision_count != 0:
        fail("Fenwatch keeper must remain presentation-only, found {} collision shapes".format(
            keeper_collision_count,
        ))
    keeper_bounds = keeper_mesh.get_bounds()
    keeper_size = keeper_bounds.box_extent * 2.0
    if not all((
        nearly_equal(keeper_size.x, 107.45, 1.0),
        nearly_equal(keeper_size.y, 71.0, 1.0),
        nearly_equal(keeper_size.z, 207.5, 1.0),
        nearly_equal(keeper_bounds.origin.z, keeper_bounds.box_extent.z, 1.0),
    )):
        fail("Fenwatch keeper bounds drifted: origin={}, extent={}".format(
            keeper_bounds.origin,
            keeper_bounds.box_extent,
        ))
    if keeper_mesh.get_num_triangles(0) != 3280:
        fail("Fenwatch keeper triangle contract drifted: {}".format(keeper_mesh.get_num_triangles(0)))
    keeper_material_paths = set()
    for static_material in list(keeper_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            keeper_material_paths.add(material.get_path_name())
    expected_keeper_materials = set(ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS)
    expected_keeper_materials.add(FENWATCH_KEEPER_SKIN_PATH)
    if keeper_material_paths != expected_keeper_materials:
        fail("Fenwatch keeper material set drifted: {}".format(sorted(keeper_material_paths)))

    keeper = actors_by_label[FENWATCH_KEEPER_LABEL]
    keeper_component = keeper.get_component_by_class(unreal.StaticMeshComponent)
    keeper_actor_mesh = keeper_component.get_editor_property("static_mesh") if keeper_component else None
    keeper_actor_mesh_path = keeper_actor_mesh.get_path_name() if keeper_actor_mesh else "None"
    if keeper_actor_mesh_path != FENWATCH_KEEPER_PATH:
        fail("{} must use {}, found {}".format(
            FENWATCH_KEEPER_LABEL,
            FENWATCH_KEEPER_PATH,
            keeper_actor_mesh_path,
        ))
    keeper_location = keeper.get_actor_location()
    keeper_rotation = keeper.get_actor_rotation()
    if not all((
        nearly_equal(keeper_location.x, FENWATCH_KEEPER_LOCATION[0], 1.0),
        nearly_equal(keeper_location.y, FENWATCH_KEEPER_LOCATION[1], 1.0),
        nearly_equal(keeper_location.z, FENWATCH_KEEPER_LOCATION[2], 1.0),
        nearly_equal(keeper_rotation.pitch, 0.0, 0.1),
        nearly_equal(keeper_rotation.yaw, FENWATCH_KEEPER_YAW, 0.1),
        nearly_equal(keeper_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            FENWATCH_KEEPER_LABEL,
            keeper_location,
            keeper_rotation,
        ))
    keeper_relative_location = keeper_component.get_editor_property("relative_location")
    keeper_relative_rotation = keeper_component.get_editor_property("relative_rotation")
    keeper_relative_scale = keeper_component.get_editor_property("relative_scale3d")
    if not all((
        nearly_equal(keeper_relative_location.x, FENWATCH_KEEPER_VISUAL_LOCATION[0], 0.1),
        nearly_equal(keeper_relative_location.y, FENWATCH_KEEPER_VISUAL_LOCATION[1], 0.1),
        nearly_equal(keeper_relative_location.z, FENWATCH_KEEPER_VISUAL_LOCATION[2], 0.1),
        nearly_equal(keeper_relative_rotation.pitch, 0.0, 0.1),
        nearly_equal(keeper_relative_rotation.yaw, FENWATCH_KEEPER_VISUAL_YAW, 0.1),
        nearly_equal(keeper_relative_rotation.roll, 0.0, 0.1),
        nearly_equal(keeper_relative_scale.x, 1.0, 0.001),
        nearly_equal(keeper_relative_scale.y, 1.0, 0.001),
        nearly_equal(keeper_relative_scale.z, 1.0, 0.001),
    )):
        fail("Fenwatch keeper local presentation drifted: location={}, rotation={}, scale={}".format(
            keeper_relative_location,
            keeper_relative_rotation,
            keeper_relative_scale,
        ))
    if keeper_component.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("Fenwatch keeper visual must remain non-colliding")
    if unreal.Name("EmbermereOriginalArt") not in list(keeper.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(FENWATCH_KEEPER_LABEL))

    keeper_blueprint = unreal.EditorAssetLibrary.load_asset("/Game/Blueprints/BP_QuestGiver")
    subobject_subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    keeper_template = None
    for handle in subobject_subsystem.k2_gather_subobject_data_for_blueprint(keeper_blueprint):
        data = subobject_subsystem.k2_find_subobject_data_from_handle(handle)
        candidate = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
        if isinstance(candidate, unreal.StaticMeshComponent):
            keeper_template = candidate
            break
    if not keeper_template:
        fail("BP_QuestGiver is missing its Fenwatch keeper visual template")
    if keeper_template.get_editor_property("static_mesh") != keeper_mesh:
        fail("BP_QuestGiver template does not use the Fenwatch keeper mesh")
    if keeper_template.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
        fail("BP_QuestGiver keeper template must remain non-colliding")

    quartermaster_mesh = unreal.EditorAssetLibrary.load_asset(FENWATCH_QUARTERMASTER_PATH)
    if not quartermaster_mesh or not isinstance(quartermaster_mesh, unreal.StaticMesh):
        fail("missing original Fenwatch quartermaster mesh {}".format(
            FENWATCH_QUARTERMASTER_PATH,
        ))
    quartermaster_import_data = quartermaster_mesh.get_editor_property("asset_import_data")
    quartermaster_import_class = (
        quartermaster_import_data.get_class().get_name()
        if quartermaster_import_data
        else "None"
    )
    if quartermaster_import_class != "FbxStaticMeshImportData":
        fail("Fenwatch quartermaster must retain classic FBX import data, found {}".format(
            quartermaster_import_class,
        ))
    quartermaster_body_setup = quartermaster_mesh.get_editor_property("body_setup")
    quartermaster_aggregate = (
        quartermaster_body_setup.get_editor_property("agg_geom")
        if quartermaster_body_setup
        else None
    )
    quartermaster_collision_count = sum(
        len(quartermaster_aggregate.get_editor_property(property_name))
        for property_name in ("box_elems", "sphere_elems", "sphyl_elems", "convex_elems")
    ) if quartermaster_aggregate else 0
    if quartermaster_collision_count != 0:
        fail("Fenwatch quartermaster must remain presentation-only, found {} collision shapes".format(
            quartermaster_collision_count,
        ))
    quartermaster_bounds = quartermaster_mesh.get_bounds()
    quartermaster_size = quartermaster_bounds.box_extent * 2.0
    if not all((
        nearly_equal(quartermaster_size.x, 120.842, 1.0),
        nearly_equal(quartermaster_size.y, 93.0, 1.0),
        nearly_equal(quartermaster_size.z, 217.0, 1.0),
        nearly_equal(quartermaster_bounds.origin.z, quartermaster_bounds.box_extent.z, 1.0),
    )):
        fail("Fenwatch quartermaster bounds drifted: origin={}, extent={}".format(
            quartermaster_bounds.origin,
            quartermaster_bounds.box_extent,
        ))
    if quartermaster_mesh.get_num_triangles(0) != 3632:
        fail("Fenwatch quartermaster triangle contract drifted: {}".format(
            quartermaster_mesh.get_num_triangles(0),
        ))
    quartermaster_material_paths = set()
    for static_material in list(quartermaster_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            quartermaster_material_paths.add(material.get_path_name())
    expected_quartermaster_materials = set(ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS)
    expected_quartermaster_materials.add(FENWATCH_QUARTERMASTER_SKIN_PATH)
    if quartermaster_material_paths != expected_quartermaster_materials:
        fail("Fenwatch quartermaster material set drifted: {}".format(
            sorted(quartermaster_material_paths),
        ))

    quartermaster = actors_by_label[FENWATCH_QUARTERMASTER_LABEL]
    if quartermaster.get_class().get_name() != "EmbermereNpcPresentationActor":
        fail("{} must use the reusable NPC presentation wrapper, found {}".format(
            FENWATCH_QUARTERMASTER_LABEL,
            quartermaster.get_class().get_name(),
        ))
    quartermaster_component = quartermaster.get_component_by_class(unreal.StaticMeshComponent)
    quartermaster_actor_mesh = (
        quartermaster_component.get_editor_property("static_mesh")
        if quartermaster_component
        else None
    )
    quartermaster_actor_mesh_path = (
        quartermaster_actor_mesh.get_path_name()
        if quartermaster_actor_mesh
        else "None"
    )
    if quartermaster_actor_mesh_path != FENWATCH_QUARTERMASTER_PATH:
        fail("{} must use {}, found {}".format(
            FENWATCH_QUARTERMASTER_LABEL,
            FENWATCH_QUARTERMASTER_PATH,
            quartermaster_actor_mesh_path,
        ))
    quartermaster_location = quartermaster.get_actor_location()
    quartermaster_rotation = quartermaster.get_actor_rotation()
    quartermaster_scale = quartermaster.get_actor_scale3d()
    if not all((
        nearly_equal(quartermaster_location.x, FENWATCH_QUARTERMASTER_LOCATION[0], 1.0),
        nearly_equal(quartermaster_location.y, FENWATCH_QUARTERMASTER_LOCATION[1], 1.0),
        nearly_equal(quartermaster_location.z, FENWATCH_QUARTERMASTER_LOCATION[2], 1.0),
        nearly_equal(quartermaster_rotation.pitch, 0.0, 0.1),
        nearly_equal(quartermaster_rotation.yaw, FENWATCH_QUARTERMASTER_YAW, 0.1),
        nearly_equal(quartermaster_rotation.roll, 0.0, 0.1),
        nearly_equal(quartermaster_scale.x, 1.0, 0.001),
        nearly_equal(quartermaster_scale.y, 1.0, 0.001),
        nearly_equal(quartermaster_scale.z, 1.0, 0.001),
    )):
        fail("{} transform drifted: location={}, rotation={}, scale={}".format(
            FENWATCH_QUARTERMASTER_LABEL,
            quartermaster_location,
            quartermaster_rotation,
            quartermaster_scale,
        ))
    if quartermaster.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.STATIC_MESH:
        fail("Fenwatch quartermaster must currently resolve through the static visual lane")
    if not quartermaster.is_presentation_collision_disabled():
        fail("Fenwatch quartermaster visual lanes must remain non-colliding")
    if quartermaster.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("Fenwatch quartermaster presentation must not own interaction or vendor behavior")
    if unreal.Name("EmbermereOriginalArt") not in list(quartermaster.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(
            FENWATCH_QUARTERMASTER_LABEL,
        ))
    quartermaster_route_clearance = distance_to_segment_2d(
        (quartermaster_location.x, quartermaster_location.y),
        SPAWN_AUTORUN_ROUTE_START,
        SPAWN_AUTORUN_ROUTE_END,
    )
    if quartermaster_route_clearance < FENWATCH_QUARTERMASTER_ROUTE_CLEARANCE:
        fail("{} is only {:.1f} cm from the spawn autorun corridor; expected at least {:.1f} cm".format(
            FENWATCH_QUARTERMASTER_LABEL,
            quartermaster_route_clearance,
            FENWATCH_QUARTERMASTER_ROUTE_CLEARANCE,
        ))

    armsmaster_mesh = unreal.EditorAssetLibrary.load_asset(FENWATCH_ARMSMASTER_PATH)
    if not armsmaster_mesh or not isinstance(armsmaster_mesh, unreal.StaticMesh):
        fail("missing original Fenwatch armsmaster mesh {}".format(
            FENWATCH_ARMSMASTER_PATH,
        ))
    armsmaster_import_data = armsmaster_mesh.get_editor_property("asset_import_data")
    armsmaster_import_class = (
        armsmaster_import_data.get_class().get_name()
        if armsmaster_import_data
        else "None"
    )
    if armsmaster_import_class != "FbxStaticMeshImportData":
        fail("Fenwatch armsmaster must retain classic FBX import data, found {}".format(
            armsmaster_import_class,
        ))
    armsmaster_body_setup = armsmaster_mesh.get_editor_property("body_setup")
    armsmaster_aggregate = (
        armsmaster_body_setup.get_editor_property("agg_geom")
        if armsmaster_body_setup
        else None
    )
    armsmaster_collision_count = sum(
        len(armsmaster_aggregate.get_editor_property(property_name))
        for property_name in ("box_elems", "sphere_elems", "sphyl_elems", "convex_elems")
    ) if armsmaster_aggregate else 0
    if armsmaster_collision_count != 0:
        fail("Fenwatch armsmaster must remain presentation-only, found {} collision shapes".format(
            armsmaster_collision_count,
        ))
    armsmaster_bounds = armsmaster_mesh.get_bounds()
    armsmaster_size = armsmaster_bounds.box_extent * 2.0
    if not all((
        nearly_equal(armsmaster_size.x, 154.5, 1.0),
        nearly_equal(armsmaster_size.y, 87.0, 1.0),
        nearly_equal(armsmaster_size.z, 228.0, 1.0),
        nearly_equal(armsmaster_bounds.origin.z, armsmaster_bounds.box_extent.z, 1.0),
    )):
        fail("Fenwatch armsmaster bounds drifted: origin={}, extent={}".format(
            armsmaster_bounds.origin,
            armsmaster_bounds.box_extent,
        ))
    if armsmaster_mesh.get_num_triangles(0) != 2800:
        fail("Fenwatch armsmaster imported triangle contract drifted: {}".format(
            armsmaster_mesh.get_num_triangles(0),
        ))
    armsmaster_material_paths = set()
    for static_material in list(armsmaster_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            armsmaster_material_paths.add(material.get_path_name())
    expected_armsmaster_materials = set(ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS)
    expected_armsmaster_materials.add(FENWATCH_ARMSMASTER_SKIN_PATH)
    if armsmaster_material_paths != expected_armsmaster_materials:
        fail("Fenwatch armsmaster material set drifted: {}".format(
            sorted(armsmaster_material_paths),
        ))

    armsmaster_skeletal_mesh = unreal.EditorAssetLibrary.load_asset(
        FENWATCH_ARMSMASTER_SKELETAL_PATH
    )
    armsmaster_idle = unreal.EditorAssetLibrary.load_asset(
        FENWATCH_ARMSMASTER_IDLE_PATH
    )
    if not armsmaster_skeletal_mesh or not isinstance(
        armsmaster_skeletal_mesh,
        unreal.SkeletalMesh,
    ):
        fail("missing rigged Fenwatch armsmaster mesh {}".format(
            FENWATCH_ARMSMASTER_SKELETAL_PATH,
        ))
    if not armsmaster_idle or not isinstance(armsmaster_idle, unreal.AnimSequence):
        fail("missing Fenwatch armsmaster Idle {}".format(
            FENWATCH_ARMSMASTER_IDLE_PATH,
        ))
    if (
        armsmaster_idle.get_editor_property("skeleton")
        != armsmaster_skeletal_mesh.get_editor_property("skeleton")
    ):
        fail("Fenwatch armsmaster mesh and Idle must share one Skeleton")

    armsmaster = actors_by_label[FENWATCH_ARMSMASTER_LABEL]
    if armsmaster.get_class().get_name() != "EmbermereNpcPresentationActor":
        fail("{} must use the reusable NPC presentation wrapper, found {}".format(
            FENWATCH_ARMSMASTER_LABEL,
            armsmaster.get_class().get_name(),
        ))
    armsmaster_static_component = armsmaster.get_component_by_class(
        unreal.StaticMeshComponent
    )
    armsmaster_actor_static_mesh = (
        armsmaster_static_component.get_editor_property("static_mesh")
        if armsmaster_static_component
        else None
    )
    armsmaster_actor_static_path = (
        armsmaster_actor_static_mesh.get_path_name()
        if armsmaster_actor_static_mesh
        else "None"
    )
    if not armsmaster_static_component:
        fail("{} is missing its reusable static fallback component".format(
            FENWATCH_ARMSMASTER_LABEL,
        ))
    if armsmaster_actor_static_mesh:
        fail("{} inactive static component must be cleared while rigged art is active, found {}".format(
            FENWATCH_ARMSMASTER_LABEL,
            armsmaster_actor_static_path,
        ))
    armsmaster_skeletal_component = armsmaster.get_component_by_class(
        unreal.SkeletalMeshComponent
    )
    armsmaster_actor_skeletal_mesh = (
        armsmaster_skeletal_component.get_editor_property("skeletal_mesh_asset")
        if armsmaster_skeletal_component
        else None
    )
    armsmaster_actor_skeletal_path = (
        armsmaster_actor_skeletal_mesh.get_path_name()
        if armsmaster_actor_skeletal_mesh
        else "None"
    )
    if armsmaster_actor_skeletal_path != FENWATCH_ARMSMASTER_SKELETAL_PATH:
        fail("{} must use rigged mesh {}, found {}".format(
            FENWATCH_ARMSMASTER_LABEL,
            FENWATCH_ARMSMASTER_SKELETAL_PATH,
            armsmaster_actor_skeletal_path,
        ))
    armsmaster_location = armsmaster.get_actor_location()
    armsmaster_rotation = armsmaster.get_actor_rotation()
    armsmaster_scale = armsmaster.get_actor_scale3d()
    if not all((
        nearly_equal(armsmaster_location.x, FENWATCH_ARMSMASTER_LOCATION[0], 1.0),
        nearly_equal(armsmaster_location.y, FENWATCH_ARMSMASTER_LOCATION[1], 1.0),
        nearly_equal(armsmaster_location.z, FENWATCH_ARMSMASTER_LOCATION[2], 1.0),
        nearly_equal(armsmaster_rotation.pitch, 0.0, 0.1),
        nearly_equal(armsmaster_rotation.yaw, FENWATCH_ARMSMASTER_YAW, 0.1),
        nearly_equal(armsmaster_rotation.roll, 0.0, 0.1),
        nearly_equal(armsmaster_scale.x, 1.0, 0.001),
        nearly_equal(armsmaster_scale.y, 1.0, 0.001),
        nearly_equal(armsmaster_scale.z, 1.0, 0.001),
    )):
        fail("{} transform drifted: location={}, rotation={}, scale={}".format(
            FENWATCH_ARMSMASTER_LABEL,
            armsmaster_location,
            armsmaster_rotation,
            armsmaster_scale,
        ))
    if armsmaster.get_editor_property("static_visual_mesh") != armsmaster_mesh:
        fail("Fenwatch armsmaster saved static fallback reference drifted")
    if (
        armsmaster.get_editor_property("skeletal_visual_mesh")
        != armsmaster_skeletal_mesh
    ):
        fail("Fenwatch armsmaster saved rigged mesh reference drifted")
    if armsmaster.get_editor_property("idle_animation") != armsmaster_idle:
        fail("Fenwatch armsmaster saved Idle reference drifted")
    if not bool(armsmaster.get_editor_property("prefer_skeletal_visual")):
        fail("Fenwatch armsmaster must prefer the rigged visual lane")
    if not bool(armsmaster.get_editor_property("loop_idle_animation")):
        fail("Fenwatch armsmaster Idle must remain looping")
    if not nearly_equal(
        float(armsmaster.get_editor_property("idle_animation_play_rate")),
        1.0,
        0.001,
    ):
        fail("Fenwatch armsmaster Idle play rate drifted")
    if (
        armsmaster.get_resolved_visual_mode()
        != unreal.EmbermereNpcVisualMode.SKELETAL_MESH
    ):
        fail("Fenwatch armsmaster must resolve through the rigged visual lane")
    if (
        armsmaster.get_resolved_animation_mode()
        != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE
    ):
        fail("Fenwatch armsmaster must resolve the single-node Idle lane")
    if not armsmaster.is_presentation_collision_disabled():
        fail("Fenwatch armsmaster visual lanes must remain non-colliding")
    if armsmaster.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("Fenwatch armsmaster presentation must not own interaction behavior")
    if armsmaster.get_component_by_class(unreal.EmbermereTrainerComponent):
        fail("Fenwatch armsmaster presentation must not own trainer behavior")
    if unreal.Name("EmbermereOriginalArt") not in list(armsmaster.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(
            FENWATCH_ARMSMASTER_LABEL,
        ))

    armsmaster_service = actors_by_label[FENWATCH_ARMSMASTER_SERVICE_LABEL]
    if armsmaster_service.get_class().get_name() != "EmbermereTrainerServiceActor":
        fail("{} must use the art-free trainer service class, found {}".format(
            FENWATCH_ARMSMASTER_SERVICE_LABEL,
            armsmaster_service.get_class().get_name(),
        ))
    service_location = armsmaster_service.get_actor_location()
    service_rotation = armsmaster_service.get_actor_rotation()
    if not all((
        nearly_equal(service_location.x, FENWATCH_ARMSMASTER_LOCATION[0], 1.0),
        nearly_equal(service_location.y, FENWATCH_ARMSMASTER_LOCATION[1], 1.0),
        nearly_equal(service_location.z, FENWATCH_ARMSMASTER_LOCATION[2], 1.0),
        nearly_equal(service_rotation.pitch, 0.0, 0.1),
        nearly_equal(service_rotation.yaw, FENWATCH_ARMSMASTER_YAW, 0.1),
        nearly_equal(service_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            FENWATCH_ARMSMASTER_SERVICE_LABEL,
            service_location,
            service_rotation,
        ))
    trainer_component = armsmaster_service.get_component_by_class(
        unreal.EmbermereTrainerComponent
    )
    interactable_component = armsmaster_service.get_component_by_class(
        unreal.EmbermereInteractableComponent
    )
    if not trainer_component or not interactable_component:
        fail("Fenwatch armsmaster service is missing native trainer or interaction authority")
    offerings = trainer_component.get_editor_property("offerings_data")
    offerings_path = offerings.get_path_name() if offerings else "None"
    if offerings_path != FENWATCH_ARMSMASTER_OFFERINGS_PATH:
        fail("Fenwatch armsmaster service offerings drifted: {}".format(offerings_path))
    if armsmaster_service.get_component_by_class(unreal.StaticMeshComponent):
        fail("Fenwatch armsmaster service must not own static art")
    if armsmaster_service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("Fenwatch armsmaster service must not own skeletal art")

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

    supply_chest_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_SUPPLY_CHEST_PATH)
    if not supply_chest_mesh or not isinstance(supply_chest_mesh, unreal.StaticMesh):
        fail("missing original supply chest mesh {}".format(ORIGINAL_SUPPLY_CHEST_PATH))
    supply_chest_import_data = supply_chest_mesh.get_editor_property("asset_import_data")
    supply_chest_import_class = (
        supply_chest_import_data.get_class().get_name()
        if supply_chest_import_data
        else "None"
    )
    if supply_chest_import_class != "FbxStaticMeshImportData":
        fail("original supply chest must retain classic FBX import data, found {}".format(
            supply_chest_import_class,
        ))
    supply_chest_body_setup = supply_chest_mesh.get_editor_property("body_setup")
    supply_chest_aggregate = (
        supply_chest_body_setup.get_editor_property("agg_geom")
        if supply_chest_body_setup
        else None
    )
    supply_chest_box_count = (
        len(supply_chest_aggregate.get_editor_property("box_elems"))
        if supply_chest_aggregate
        else 0
    )
    if supply_chest_box_count != 2:
        fail("original supply chest must retain 2 authored box colliders, found {}".format(
            supply_chest_box_count,
        ))
    supply_chest_bounds = supply_chest_mesh.get_bounds()
    if not all((
        nearly_equal(supply_chest_bounds.origin.x, 0.0, 1.0),
        nearly_equal(supply_chest_bounds.origin.y, 5.5, 1.0),
        nearly_equal(supply_chest_bounds.origin.z, 61.55, 2.0),
        nearly_equal(supply_chest_bounds.box_extent.x, 90.0, 2.0),
        nearly_equal(supply_chest_bounds.box_extent.y, 59.5, 2.0),
        nearly_equal(supply_chest_bounds.box_extent.z, 61.55, 2.0),
    )):
        fail("original supply chest bounds drifted: origin={}, extent={}".format(
            supply_chest_bounds.origin,
            supply_chest_bounds.box_extent,
        ))
    if supply_chest_mesh.get_num_triangles(0) != 2364:
        fail("original supply chest triangle count drifted: {}".format(
            supply_chest_mesh.get_num_triangles(0),
        ))
    supply_chest_material_paths = set()
    for static_material in list(supply_chest_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            supply_chest_material_paths.add(material.get_path_name())
    if supply_chest_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("original supply chest material set drifted: {}".format(
            sorted(supply_chest_material_paths),
        ))

    supply_chest = actors_by_label[ORIGINAL_SUPPLY_CHEST_LABEL]
    supply_chest_component = supply_chest.get_component_by_class(unreal.StaticMeshComponent)
    actor_supply_chest_mesh = (
        supply_chest_component.get_editor_property("static_mesh")
        if supply_chest_component
        else None
    )
    actor_supply_chest_path = (
        actor_supply_chest_mesh.get_path_name()
        if actor_supply_chest_mesh
        else "None"
    )
    if actor_supply_chest_path != ORIGINAL_SUPPLY_CHEST_PATH:
        fail("{} must use {}, found {}".format(
            ORIGINAL_SUPPLY_CHEST_LABEL,
            ORIGINAL_SUPPLY_CHEST_PATH,
            actor_supply_chest_path,
        ))
    supply_chest_location = supply_chest.get_actor_location()
    supply_chest_rotation = supply_chest.get_actor_rotation()
    if not all((
        nearly_equal(supply_chest_location.x, ORIGINAL_SUPPLY_CHEST_LOCATION[0], 1.0),
        nearly_equal(supply_chest_location.y, ORIGINAL_SUPPLY_CHEST_LOCATION[1], 1.0),
        nearly_equal(supply_chest_location.z, ORIGINAL_SUPPLY_CHEST_LOCATION[2], 1.0),
        nearly_equal(supply_chest_rotation.pitch, 0.0, 0.1),
        nearly_equal(supply_chest_rotation.yaw, ORIGINAL_SUPPLY_CHEST_YAW, 0.1),
        nearly_equal(supply_chest_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            ORIGINAL_SUPPLY_CHEST_LABEL,
            supply_chest_location,
            supply_chest_rotation,
        ))
    if unreal.Name("EmbermereOriginalArt") not in list(supply_chest.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(
            ORIGINAL_SUPPLY_CHEST_LABEL,
        ))
    route_clearance = distance_to_segment_2d(
        (supply_chest_location.x, supply_chest_location.y),
        SPAWN_AUTORUN_ROUTE_START,
        SPAWN_AUTORUN_ROUTE_END,
    )
    if route_clearance < SUPPLY_CHEST_ROUTE_CLEARANCE:
        fail("{} is only {:.1f} cm from the spawn autorun corridor; expected at least {:.1f} cm".format(
            ORIGINAL_SUPPLY_CHEST_LABEL,
            route_clearance,
            SUPPLY_CHEST_ROUTE_CLEARANCE,
        ))

    practice_dummy_mesh = unreal.EditorAssetLibrary.load_asset(
        ORIGINAL_PRACTICE_DUMMY_PATH
    )
    if not practice_dummy_mesh or not isinstance(practice_dummy_mesh, unreal.StaticMesh):
        fail("missing original Fenwatch practice dummy {}".format(
            ORIGINAL_PRACTICE_DUMMY_PATH,
        ))
    practice_dummy_import_data = practice_dummy_mesh.get_editor_property(
        "asset_import_data"
    )
    practice_dummy_import_class = (
        practice_dummy_import_data.get_class().get_name()
        if practice_dummy_import_data
        else "None"
    )
    if practice_dummy_import_class != "FbxStaticMeshImportData":
        fail("Fenwatch practice dummy must retain classic FBX import data, found {}".format(
            practice_dummy_import_class,
        ))
    practice_dummy_body_setup = practice_dummy_mesh.get_editor_property("body_setup")
    practice_dummy_aggregate = (
        practice_dummy_body_setup.get_editor_property("agg_geom")
        if practice_dummy_body_setup
        else None
    )
    practice_dummy_box_count = (
        len(practice_dummy_aggregate.get_editor_property("box_elems"))
        if practice_dummy_aggregate
        else 0
    )
    if practice_dummy_box_count != 2:
        fail("Fenwatch practice dummy must retain 2 authored box colliders, found {}".format(
            practice_dummy_box_count,
        ))
    practice_dummy_bounds = practice_dummy_mesh.get_bounds()
    if not all((
        nearly_equal(practice_dummy_bounds.box_extent.x, 126.0, 1.0),
        nearly_equal(practice_dummy_bounds.box_extent.y, 50.4395, 1.0),
        nearly_equal(practice_dummy_bounds.origin.z, 122.5, 1.0),
        nearly_equal(practice_dummy_bounds.box_extent.z, 122.5, 1.0),
    )):
        fail("Fenwatch practice dummy bounds drifted: origin={}, extent={}".format(
            practice_dummy_bounds.origin,
            practice_dummy_bounds.box_extent,
        ))
    if practice_dummy_mesh.get_num_triangles(0) != 2572:
        fail("Fenwatch practice dummy triangle count drifted: {}".format(
            practice_dummy_mesh.get_num_triangles(0),
        ))
    practice_dummy_material_paths = set()
    for static_material in list(practice_dummy_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            practice_dummy_material_paths.add(material.get_path_name())
    if practice_dummy_material_paths != ORIGINAL_ROAD_FAMILY_MATERIAL_PATHS:
        fail("Fenwatch practice dummy material set drifted: {}".format(
            sorted(practice_dummy_material_paths),
        ))

    practice_dummy = actors_by_label[ORIGINAL_PRACTICE_DUMMY_LABEL]
    practice_dummy_component = practice_dummy.get_component_by_class(
        unreal.StaticMeshComponent
    )
    actor_practice_dummy_mesh = (
        practice_dummy_component.get_editor_property("static_mesh")
        if practice_dummy_component
        else None
    )
    actor_practice_dummy_path = (
        actor_practice_dummy_mesh.get_path_name()
        if actor_practice_dummy_mesh
        else "None"
    )
    if actor_practice_dummy_path != ORIGINAL_PRACTICE_DUMMY_PATH:
        fail("{} must use {}, found {}".format(
            ORIGINAL_PRACTICE_DUMMY_LABEL,
            ORIGINAL_PRACTICE_DUMMY_PATH,
            actor_practice_dummy_path,
        ))
    practice_dummy_location = practice_dummy.get_actor_location()
    practice_dummy_rotation = practice_dummy.get_actor_rotation()
    if not all((
        nearly_equal(practice_dummy_location.x, ORIGINAL_PRACTICE_DUMMY_LOCATION[0], 1.0),
        nearly_equal(practice_dummy_location.y, ORIGINAL_PRACTICE_DUMMY_LOCATION[1], 1.0),
        nearly_equal(practice_dummy_location.z, ORIGINAL_PRACTICE_DUMMY_LOCATION[2], 1.0),
        nearly_equal(practice_dummy_rotation.pitch, 0.0, 0.1),
        nearly_equal(practice_dummy_rotation.yaw, ORIGINAL_PRACTICE_DUMMY_YAW, 0.1),
        nearly_equal(practice_dummy_rotation.roll, 0.0, 0.1),
    )):
        fail("{} transform drifted: location={}, rotation={}".format(
            ORIGINAL_PRACTICE_DUMMY_LABEL,
            practice_dummy_location,
            practice_dummy_rotation,
        ))
    if unreal.Name("EmbermereOriginalArt") not in list(practice_dummy.tags):
        fail("{} must retain the EmbermereOriginalArt tag".format(
            ORIGINAL_PRACTICE_DUMMY_LABEL,
        ))
    if str(practice_dummy_component.get_collision_profile_name()) != "BlockAll":
        fail("{} collision profile drifted: {}".format(
            ORIGINAL_PRACTICE_DUMMY_LABEL,
            practice_dummy_component.get_collision_profile_name(),
        ))
    armsmaster_distance = math.hypot(
        practice_dummy_location.x - FENWATCH_ARMSMASTER_LOCATION[0],
        practice_dummy_location.y - FENWATCH_ARMSMASTER_LOCATION[1],
    )
    if not nearly_equal(armsmaster_distance, 282.843, 1.0):
        fail("{} training-yard spacing from the armsmaster drifted: {:.1f} cm".format(
            ORIGINAL_PRACTICE_DUMMY_LABEL,
            armsmaster_distance,
        ))

    marsh_reed_mesh = unreal.EditorAssetLibrary.load_asset(ORIGINAL_MARSH_REED_PATH)
    if not marsh_reed_mesh or not isinstance(marsh_reed_mesh, unreal.StaticMesh):
        fail("missing original marsh reed mesh {}".format(ORIGINAL_MARSH_REED_PATH))
    marsh_reed_import_data = marsh_reed_mesh.get_editor_property("asset_import_data")
    marsh_reed_import_class = (
        marsh_reed_import_data.get_class().get_name()
        if marsh_reed_import_data
        else "None"
    )
    if marsh_reed_import_class != "FbxStaticMeshImportData":
        fail("original marsh reeds must retain classic FBX import data, found {}".format(
            marsh_reed_import_class,
        ))
    marsh_reed_body_setup = marsh_reed_mesh.get_editor_property("body_setup")
    marsh_reed_aggregate = (
        marsh_reed_body_setup.get_editor_property("agg_geom")
        if marsh_reed_body_setup
        else None
    )
    marsh_reed_collision_count = 0
    if marsh_reed_aggregate:
        marsh_reed_collision_count = sum(
            len(marsh_reed_aggregate.get_editor_property(property_name))
            for property_name in (
                "box_elems",
                "sphere_elems",
                "sphyl_elems",
                "convex_elems",
                "tapered_capsule_elems",
            )
        )
    if marsh_reed_collision_count != 0:
        fail("visual-only marsh reeds must retain zero collision shapes, found {}".format(
            marsh_reed_collision_count,
        ))
    marsh_reed_bounds = marsh_reed_mesh.get_bounds()
    if not all((
        nearly_equal(marsh_reed_bounds.box_extent.x, 84.0, 2.0),
        nearly_equal(marsh_reed_bounds.box_extent.y, 65.52, 2.0),
        nearly_equal(marsh_reed_bounds.origin.z, 93.9, 2.0),
        nearly_equal(marsh_reed_bounds.box_extent.z, 93.9, 2.0),
    )):
        fail("original marsh reed bounds drifted: origin={}, extent={}".format(
            marsh_reed_bounds.origin,
            marsh_reed_bounds.box_extent,
        ))
    if marsh_reed_mesh.get_num_triangles(0) != 1012:
        fail("original marsh reed triangle count drifted: {}".format(
            marsh_reed_mesh.get_num_triangles(0),
        ))
    marsh_reed_material_paths = set()
    for static_material in list(marsh_reed_mesh.get_editor_property("static_materials")):
        material = static_material.get_editor_property("material_interface")
        if material:
            marsh_reed_material_paths.add(material.get_path_name())
    if marsh_reed_material_paths != ORIGINAL_MARSH_REED_MATERIAL_PATHS:
        fail("original marsh reed material set drifted: {}".format(
            sorted(marsh_reed_material_paths),
        ))

    for label, (expected_location, expected_yaw, expected_scale) in ORIGINAL_MARSH_REEDS.items():
        reeds = actors_by_label[label]
        component = reeds.get_component_by_class(unreal.StaticMeshComponent)
        actor_mesh = component.get_editor_property("static_mesh") if component else None
        actor_mesh_path = actor_mesh.get_path_name() if actor_mesh else "None"
        if actor_mesh_path != ORIGINAL_MARSH_REED_PATH:
            fail("{} must use {}, found {}".format(
                label,
                ORIGINAL_MARSH_REED_PATH,
                actor_mesh_path,
            ))
        location = reeds.get_actor_location()
        rotation = reeds.get_actor_rotation()
        scale = reeds.get_actor_scale3d()
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
            fail("{} transform drifted: location={}, rotation={}, scale={}".format(
                label,
                location,
                rotation,
                scale,
            ))
        if component.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION:
            fail("{} must remain visual-only NoCollision".format(label))
        if unreal.Name("EmbermereOriginalArt") not in list(reeds.tags):
            fail("{} must retain the EmbermereOriginalArt tag".format(label))
        if unreal.Name("EmbermereGroundDressing") not in list(reeds.tags):
            fail("{} must retain the EmbermereGroundDressing tag".format(label))

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

    expected_tree_materials = [
        TIMBER_MATERIAL_PATH,
        TIMBER_MATERIAL_PATH,
        MOSS_MATERIAL_PATH,
        MOSS_MATERIAL_PATH,
        TREE_BILLBOARD_MATERIAL_PATH,
    ]
    for label in sorted(TREE_OVERRIDE_LABELS):
        component = actors_by_label[label].get_component_by_class(unreal.StaticMeshComponent)
        material_paths = [
            component.get_material(index).get_path_name() if component.get_material(index) else "None"
            for index in range(component.get_num_materials())
        ]
        if material_paths != expected_tree_materials:
            fail("{} project-owned tree material override drifted: {}".format(label, material_paths))

    for label in sorted(FOLIAGE_OVERRIDE_LABELS):
        component = actors_by_label[label].get_component_by_class(unreal.StaticMeshComponent)
        material = component.get_material(0) if component else None
        material_path = material.get_path_name() if material else "None"
        if material_path != MOSS_MATERIAL_PATH:
            fail("{} project-owned foliage override drifted: {}".format(label, material_path))

    boulder_component = actors_by_label["FabPass_Road_Boulder_01"].get_component_by_class(
        unreal.StaticMeshComponent
    )
    boulder_material = boulder_component.get_material(0) if boulder_component else None
    boulder_material_path = boulder_material.get_path_name() if boulder_material else "None"
    if boulder_material_path != STONE_MATERIAL_PATH:
        fail("FabPass_Road_Boulder_01 stone override drifted: {}".format(
            boulder_material_path,
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

    ground_material = unreal.EditorAssetLibrary.load_asset(GROUND_MATERIAL_PATH)
    if not ground_material or not isinstance(ground_material, unreal.Material):
        fail("missing project-owned ground material {}".format(GROUND_MATERIAL_PATH))

    expression_count = unreal.MaterialEditingLibrary.get_num_material_expressions(
        ground_material
    )
    if expression_count != GROUND_MATERIAL_EXPRESSION_COUNT:
        fail("ground material must retain {} expressions, found {}".format(
            GROUND_MATERIAL_EXPRESSION_COUNT,
            expression_count,
        ))

    scalar_parameter_names = {
        str(name)
        for name in unreal.MaterialEditingLibrary.get_scalar_parameter_names(
            ground_material
        )
    }
    vector_parameter_names = {
        str(name)
        for name in unreal.MaterialEditingLibrary.get_vector_parameter_names(
            ground_material
        )
    }
    if scalar_parameter_names != GROUND_SCALAR_PARAMETERS:
        fail("ground scalar parameters drifted: {}".format(
            sorted(scalar_parameter_names),
        ))
    if vector_parameter_names != GROUND_VECTOR_PARAMETERS:
        fail("ground vector parameters drifted: {}".format(
            sorted(vector_parameter_names),
        ))

    path_half_width = None
    for expression in unreal.MaterialEditingLibrary.get_material_expressions(
        ground_material
    ):
        if not isinstance(expression, unreal.MaterialExpressionScalarParameter):
            continue
        if str(expression.get_editor_property("parameter_name")) == "PathHalfWidthCm":
            path_half_width = expression.get_editor_property("default_value")
            break
    if path_half_width is None or not nearly_equal(
        path_half_width,
        GROUND_PATH_HALF_WIDTH_CM,
    ):
        fail("ground path half-width must remain {} cm, found {}".format(
            GROUND_PATH_HALF_WIDTH_CM,
            path_half_width,
        ))

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

    for label, expected_location in STARTER_ENEMY_LAYOUT.items():
        actor = actors_by_label.get(label)
        if not actor:
            fail("missing starter encounter actor {}".format(label))
        location = actor.get_actor_location()
        if not all(nearly_equal(actual, expected, 1.0) for actual, expected in zip(
            (location.x, location.y, location.z),
            expected_location,
        )):
            fail("{} location drifted: expected {}, found {}".format(label, expected_location, location))

        mesh_component = actor.get_component_by_class(unreal.SkeletalMeshComponent)
        if not mesh_component:
            fail("{} has no skeletal mesh component".format(label))
        skeletal_mesh = mesh_component.get_editor_property("skeletal_mesh_asset")
        skeletal_mesh_path = skeletal_mesh.get_path_name() if skeletal_mesh else "None"
        if skeletal_mesh_path != MARSH_PROWLER_MESH_PATH:
            fail("{} must use {}, found {}".format(
                label,
                MARSH_PROWLER_MESH_PATH,
                skeletal_mesh_path,
            ))

        relative_location = mesh_component.get_editor_property("relative_location")
        relative_scale = mesh_component.get_editor_property("relative_scale3d")
        if not all((
            nearly_equal(relative_location.x, 0.0, 0.01),
            nearly_equal(relative_location.y, 0.0, 0.01),
            nearly_equal(relative_location.z, -95.0, 0.01),
            nearly_equal(relative_scale.x, 0.65, 0.001),
            nearly_equal(relative_scale.y, 0.65, 0.001),
            nearly_equal(relative_scale.z, 0.65, 0.001),
        )):
            fail("{} Marsh Prowler component transform drifted: location={}, scale={}".format(
                label,
                relative_location,
                relative_scale,
            ))

        animation_data = mesh_component.get_editor_property("animation_data")
        idle_animation = animation_data.get_editor_property("anim_to_play")
        idle_animation_path = idle_animation.get_path_name() if idle_animation else "None"
        if idle_animation_path != MARSH_PROWLER_IDLE_PATH:
            fail("{} must default to {}, found {}".format(
                label,
                MARSH_PROWLER_IDLE_PATH,
                idle_animation_path,
            ))
        animation_mode = mesh_component.get_editor_property("animation_mode")
        if animation_mode != unreal.AnimationMode.ANIMATION_SINGLE_NODE:
            fail("{} must use single-node animation mode, found {}".format(
                label,
                animation_mode,
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

    unreal.log("Embermere zone validation passed: {} grounded upright FabPass actors, {} grounded original-art placements including Mara's Fenwatch keeper, the presentation-only Fenwatch quartermaster and armsmaster, the solid-core Fenwatch practice dummy, and four visual-only marsh reed clusters, three saved Marsh Prowler presentations, separated starter pulls, restored foliage materials, gameplay anchors, 38-node moss-and-earth ground, and daylight baseline intact".format(
        len(fabpass_labels),
        EXPECTED_ORIGINAL_ART_COUNT,
    ))


if __name__ == "__main__":
    main()
