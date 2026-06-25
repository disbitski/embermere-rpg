"""Build the Embermere starter-zone blockout in the currently loaded Unreal map.

Run from the Unreal console with:
py exec(open('/Users/wizard/Documents/Unreal Game/Scripts/setup_prototype_level.py').read())
"""

import unreal


MAP_PATH = "/Game/Maps/L_Embermere_Prototype"
ACTOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
EDITOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)


def vec(x, y, z):
    return unreal.Vector(float(x), float(y), float(z))


def rot(pitch=0, yaw=0, roll=0):
    return unreal.Rotator(float(pitch), float(yaw), float(roll))


def load_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"Could not load asset: {path}")
    return asset


def set_common(actor, label, folder, tags=None, scale=None):
    actor.set_actor_label(label)
    actor.set_folder_path(folder)
    if tags:
        actor.tags = [unreal.Name(tag) for tag in tags]
    if scale:
        actor.set_actor_scale3d(vec(*scale))
    return actor


def spawn_asset(asset_path, label, location, folder, scale=(1, 1, 1), yaw=0, tags=None):
    asset = load_asset(asset_path)
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_object(asset, vec(*location), rot(0, yaw, 0))
    if not actor:
        raise RuntimeError(f"Could not spawn asset: {asset_path}")
    return set_common(actor, label, folder, tags, scale)


def spawn_class(class_path, label, location, folder, scale=(1, 1, 1), yaw=0, tags=None):
    actor_class = unreal.load_class(None, class_path)
    if not actor_class:
        raise RuntimeError(f"Could not load class: {class_path}")
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_class(actor_class, vec(*location), rot(0, yaw, 0))
    if not actor:
        raise RuntimeError(f"Could not spawn class: {class_path}")
    return set_common(actor, label, folder, tags, scale)


def clear_existing_blockout():
    keep_labels = set()
    for actor in ACTOR_SUBSYSTEM.get_all_level_actors():
        label = actor.get_actor_label()
        tags = {str(tag) for tag in actor.tags}
        if "EmbermereBlockout" in tags and label not in keep_labels:
            ACTOR_SUBSYSTEM.destroy_actor(actor)


def build_level():
    unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
    clear_existing_blockout()

    blockout_tag = ["EmbermereBlockout"]

    # World foundation and navigation read.
    spawn_asset("/Engine/BasicShapes/Plane", "Zone_Ground_Embermere_Glen", (0, 0, 0), "00_World", (90, 90, 1), tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cylinder", "Safe_Area_Ring", (-1800, -850, 8), "01_Village", (8, 8, 0.08), tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cylinder", "Combat_Pocket_Ring", (1900, 850, 8), "03_Wilderness", (9, 9, 0.08), tags=blockout_tag)

    # Lighting and atmosphere.
    spawn_class("/Script/Engine.DirectionalLight", "Sun_Key_Light", (-900, -1200, 1200), "00_World/Lighting", yaw=-35, tags=blockout_tag)
    spawn_class("/Script/Engine.SkyLight", "Sky_Ambient_Light", (0, 0, 500), "00_World/Lighting", tags=blockout_tag)
    spawn_class("/Script/Engine.ExponentialHeightFog", "Light_Mist_Fog", (0, 0, 0), "00_World/Lighting", tags=blockout_tag)

    # Spawn point and village placeholders.
    spawn_class("/Script/Engine.PlayerStart", "PlayerStart_Embermere_Village", (-2400, -1200, 95), "01_Village", yaw=35, tags=blockout_tag)
    spawn_asset("/Game/Blueprints/BP_QuestGiver", "Quest_Giver_Mara_Fenwatch", (-2050, -850, 0), "01_Village/NPCs", yaw=35, tags=blockout_tag + ["QuestNPC"])
    spawn_asset("/Engine/BasicShapes/Cube", "Vendor_Placeholder", (-1550, -1120, 95), "01_Village/NPCs", (0.75, 0.75, 1.9), tags=blockout_tag + ["Vendor"])
    spawn_asset("/Engine/BasicShapes/Cube", "Trainer_Placeholder", (-1450, -610, 95), "01_Village/NPCs", (0.75, 0.75, 1.9), tags=blockout_tag + ["Trainer"])
    spawn_asset("/Engine/BasicShapes/Cube", "Village_Hall_Blockout", (-1850, -1350, 150), "01_Village/Buildings", (5, 3, 2.2), yaw=12, tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cube", "Blacksmith_Blockout", (-1200, -950, 120), "01_Village/Buildings", (3, 2.5, 1.8), yaw=-18, tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cube", "Inn_Blockout", (-2250, -450, 140), "01_Village/Buildings", (3.5, 3, 2.1), yaw=28, tags=blockout_tag)

    # Path, wilderness pocket, and ruin landmark.
    for index, position in enumerate([(-1100, -450, 20), (-500, -150, 20), (150, 120, 20), (780, 430, 20), (1350, 650, 20)], start=1):
        spawn_asset("/Engine/BasicShapes/Cube", f"Road_Marker_{index:02d}", position, "02_Path", (1.4, 0.7, 0.12), yaw=20, tags=blockout_tag)

    spawn_asset("/Engine/BasicShapes/Cube", "Ruin_Broken_Arch_Left", (2450, 450, 190), "04_Ruins", (0.8, 0.8, 3.8), yaw=-10, tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cube", "Ruin_Broken_Arch_Right", (2850, 450, 190), "04_Ruins", (0.8, 0.8, 3.8), yaw=10, tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cube", "Ruin_Broken_Arch_Top", (2650, 450, 380), "04_Ruins", (2.8, 0.75, 0.55), tags=blockout_tag)
    spawn_asset("/Engine/BasicShapes/Cube", "Ruin_Fallen_Stone", (3050, 730, 55), "04_Ruins", (2.1, 0.8, 0.45), yaw=32, tags=blockout_tag)

    # Starter enemies use our project Blueprint so gameplay hooks survive art swaps.
    for index, position in enumerate([(1750, 630, 95), (2100, 980, 95), (2380, 760, 95)], start=1):
        spawn_asset("/Game/Blueprints/BP_StarterEnemy", f"Starter_Enemy_{index:02d}", position, "03_Wilderness/Enemies", tags=blockout_tag + ["Hostile"])

    # Extra hostile markers remain visible even before enemy art is assigned.
    for index, position in enumerate([(1750, 630, 90), (2100, 980, 90), (2380, 760, 90)], start=1):
        spawn_asset("/Engine/BasicShapes/Cone", f"Enemy_Visual_Marker_{index:02d}", position, "03_Wilderness/Enemies", (0.9, 0.9, 1.7), tags=blockout_tag + ["HostileMarker"])

    # Let the editor open on the useful overview.
    EDITOR_SUBSYSTEM.set_level_viewport_camera_info(vec(-1250, -3600, 1850), rot(-28, 36, 0))
    unreal.EditorLoadingAndSavingUtils.save_current_level()
    print("EMBERMERE_BLOCKOUT_READY", MAP_PATH)


build_level()
