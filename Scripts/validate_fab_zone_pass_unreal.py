import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
EXPECTED_FABPASS_COUNT = 65
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
}

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

    unreal.log("Embermere Fab zone validation passed: {} upright FabPass actors, gameplay anchors, moss ground, and daylight baseline intact".format(len(fabpass_labels)))


if __name__ == "__main__":
    main()
