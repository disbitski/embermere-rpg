import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
EXPECTED_FABPASS_COUNT = 62
ORIGINAL_WAYSTONE_LABEL = "Embermere_Waystone_Road_01"
ORIGINAL_WAYSTONE_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereWaystone_01.SM_EmbermereWaystone_01"
ORIGINAL_EMBER_LAMP_PATH = "/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereEmberLamp_01.SM_EmbermereEmberLamp_01"
ORIGINAL_EMBER_LAMPS = {
    "Embermere_EmberLamp_Mara_01": ((-1970.0, -775.0, 20.0), 5.0),
    "Embermere_EmberLamp_Road_01": ((-1150.0, -520.0, 20.0), 8.0),
}
GROUND_MATERIAL_PATH = "/Game/Art/Embermere/Environment/M_EmbermereGround.M_EmbermereGround"
GROUND_ACTOR_LABELS = {
    "Zone_Ground_Embermere_Glen",
    "Safe_Area_Ring",
    "Combat_Pocket_Ring",
}

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
} | set(ORIGINAL_EMBER_LAMPS)

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

    unreal.log("Embermere zone validation passed: {} upright FabPass actors, 3 original-art placements, gameplay anchors, moss ground, and daylight baseline intact".format(len(fabpass_labels)))


if __name__ == "__main__":
    main()
