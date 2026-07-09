import json


FAB_TAG = "EmbermereFabPass"
FAB_PREFIX = "FabPass_"


def call(tool_name, payload):
    return execute_tool(tool_name, json.dumps(payload))


def scene(tool_name, payload):
    return call("editor_toolset.toolsets.scene.SceneTools." + tool_name, payload)


def actor(tool_name, payload):
    return call("editor_toolset.toolsets.actor.ActorTools." + tool_name, payload)


def assets(tool_name, payload):
    return call("editor_toolset.toolsets.asset.AssetTools." + tool_name, payload)


def xform(x, y, z=0, yaw=0, scale=1):
    if isinstance(scale, (int, float)):
        scale = {"x": scale, "y": scale, "z": scale}
    return {
        "location": {"x": x, "y": y, "z": z},
        "rotation": {"pitch": 0, "yaw": yaw, "roll": 0},
        "scale": scale,
    }


def find(name):
    return scene("find_actors", {
        "name": name,
        "tag": "",
        "collision_channels": [],
    })["returnValue"]


def label_of(actor_ref):
    return actor("get_label", {"actor": actor_ref})["returnValue"]


def remove_by_label(label):
    removed = 0
    for actor_ref in find(label):
        if label_of(actor_ref) == label:
            if scene("remove_from_scene", {"actor": actor_ref})["returnValue"]:
                removed += 1
    return removed


def spawn(entry):
    asset_path = entry["asset"]
    if not assets("exists", {"path": asset_path})["returnValue"]:
        return {"name": entry["name"], "asset": asset_path, "status": "missing"}

    result = scene("add_to_scene_from_asset", {
        "asset_path": asset_path,
        "name": entry["name"],
        "xform": xform(entry["x"], entry["y"], entry.get("z", 0), entry.get("yaw", 0), entry.get("scale", 1)),
        "snap_to_ground": entry.get("snap", True),
    })
    actor_ref = result.get("returnValue")
    if not actor_ref:
        return {"name": entry["name"], "asset": asset_path, "status": "not_created"}

    actor("set_label", {"actor": actor_ref, "label": entry["name"]})
    actor("add_tag", {"actor": actor_ref, "tag": FAB_TAG})
    scene("set_actor_folder", {"actor": actor_ref, "folder_path": entry["folder"]})
    return {"name": entry["name"], "asset": asset_path, "status": "created", "actor": actor_ref}


# These tree package files exist on disk, but the current UE 5.8 asset registry
# resolves HillTree reliably in headless placement while some Kite pine assets do
# not. Use the working tree for the first local art pass and revisit variants in
# editor after the pack finishes any one-time upgrade/resave work.
KITE_TREE = "/Game/KiteDemo/Environments/Trees/HillTree_02/HillTree_02"
KITE_TALL_TREE = "/Game/KiteDemo/Environments/Trees/HillTree_02/HillTree_02"
KITE_HILL_TREE = "/Game/KiteDemo/Environments/Trees/HillTree_02/HillTree_02"
KITE_STUMP = "/Game/KiteDemo/Environments/Trees/Tree_Stump_01/Tree_Stump_01"
KITE_BUSH = "/Game/SoulCave/Environment/Meshes/Nature/SM_S_Soul_bush"
KITE_BUSH_2 = "/Game/SoulCave/Environment/Meshes/Nature/SM_S_Soul_Plants_Fern"
KITE_FERN = "/Game/KiteDemo/Environments/Foliage/Ferns/SM_Fern_01"
KITE_FLOWERS = "/Game/KiteDemo/Environments/Foliage/Ferns/SM_Fern_03"
KITE_GRASS = "/Game/KiteDemo/Environments/Foliage/Grass/FieldGrass/SM_FieldGrass_01"
KITE_ROCK = "/Game/KiteDemo/Environments/Rocks/Medium_Boulder_001/Medium_Boulder_001"
KITE_RIVER_ROCK = "/Game/KiteDemo/Environments/Rocks/River_Rock_01/SM_River_Rock_01"
KITE_BOULDER = "/Game/ParagonProps/Monolith/Rocks/Meshes/SM_RockNordic_05"

PARAGON_ARCH = "/Game/ParagonProps/Monolith/Ruins/Meshes/JungleArch_01A"
PARAGON_ARCH_B = "/Game/ParagonProps/Monolith/Ruins/Meshes/JungleArch_01B"
PARAGON_WALL = "/Game/ParagonProps/Monolith/Ruins/Meshes/JungleWall_02A"
PARAGON_WALL_CURVE = "/Game/ParagonProps/Monolith/Ruins/Meshes/JungleWall_02A_Curved01"
PARAGON_PILLAR = "/Game/ParagonProps/Monolith/Ruins/Meshes/JunglePillarBlock_01A"
PARAGON_PILLAR_B = "/Game/ParagonProps/Monolith/Ruins/Meshes/JunglePillarBlock_02A"
PARAGON_RUBBLE = "/Game/ParagonProps/Monolith/Ruins/Meshes/JungleRubblePile_A"
PARAGON_STAIRS = "/Game/ParagonProps/Monolith/Ruins/Meshes/JungleStairs_Broken1"
PARAGON_STATUE = "/Game/ParagonProps/Monolith/Ruins/Meshes/MonoStatue_Damaged"
PARAGON_STATUE_BASE = "/Game/ParagonProps/Monolith/Ruins/Meshes/MonoStatueBase_Damaged"
PARAGON_NORDIC_ROCK = "/Game/ParagonProps/Monolith/Rocks/Meshes/SM_RockNordic_03"
PARAGON_AGORA_PORTAL = "/Game/ParagonProps/Agora/Props/Meshes/Agora_Deco_Portal_B"
PARAGON_AGORA_REMAINS = "/Game/ParagonProps/Agora/Props/Meshes/RuinRemains3"
PARAGON_AGORA_PILLAR = "/Game/ParagonProps/Agora/Props/Meshes/SM_Angelsjon_TriPillar"

SOUL_ARCH = "/Game/SoulCave/Environment/Meshes/Building/SM_Cave_Arches"
SOUL_PILLAR = "/Game/SoulCave/Environment/Meshes/Building/SM_Cave_Pillar_NoFlag"
SOUL_BROKEN_PILLAR = "/Game/SoulCave/Environment/Meshes/Building/SM_Cave_Rock_PillarBroken01b"
SOUL_ROCK = "/Game/SoulCave/Environment/Meshes/Rocks/SM_Cave_Rock_Large02"
SOUL_SMALL_ROCK = "/Game/SoulCave/Environment/Meshes/Rocks/SM_Cave_Rock_Small01"
SOUL_ROOTS = "/Game/SoulCave/Environment/Meshes/Nature/SM_Cave_Roots01"
SOUL_FOG = "/Game/SoulCave/EffectsMobile/SimpleFogSheet/BP/BP_simpleFogSheet"
SOUL_WATER = "/Game/SoulCave/EffectsMobile/Water/Meshes/SM_Flow_01"

SCI_HOUSE = "/Game/Scifi_desert_city/Meshes/Houses/SM_house_01"
SCI_HOUSE_2 = "/Game/Scifi_desert_city/Meshes/Houses/Update_01/SM_house_02_Update"
SCI_ROUND_BUILDING = "/Game/Scifi_desert_city/Meshes/Round_buildings/SM_round_building_01"
SCI_CRATE = "/Game/Scifi_desert_city/Meshes/Crates/SM_crate_01"
SCI_CRATE_2 = "/Game/Scifi_desert_city/Meshes/Crates/SM_crate_03"
SCI_CRATE_GROUP = "/Game/Scifi_desert_city/Meshes/Crates/SM_crates_group_"
SCI_FABRIC = "/Game/Scifi_desert_city/Meshes/Fabric/SM_fabric_cover_01"
SCI_FENCE = "/Game/Scifi_desert_city/Meshes/Fence/SM_fence"
SCI_LAMP = "/Game/Scifi_desert_city/Meshes/Lamps/SM_lamp_01"
SCI_TABLE = "/Game/Scifi_desert_city/Meshes/Table/SM_table"
SCI_STOOL = "/Game/Scifi_desert_city/Meshes/Stool/SM_stool"


PLACEMENTS = [
    # Village safe-area dressing. Full sci-fi building shells were removed after
    # PIE showed that their scale and collision obscured PlayerStart and Mara.
    {"name": FAB_PREFIX + "Mara_Stone_Backdrop", "asset": PARAGON_AGORA_PORTAL, "x": -2150, "y": -1025, "z": 20, "yaw": 35, "scale": 0.34, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Market_Cover", "asset": SCI_FABRIC, "x": -1740, "y": -820, "z": 20, "yaw": 8, "scale": 1.25, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Table", "asset": SCI_TABLE, "x": -1705, "y": -760, "z": 20, "yaw": -18, "scale": 1.0, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Stool_A", "asset": SCI_STOOL, "x": -1645, "y": -770, "z": 20, "yaw": 22, "scale": 1.0, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Stool_B", "asset": SCI_STOOL, "x": -1745, "y": -710, "z": 20, "yaw": -44, "scale": 1.0, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Crates_A", "asset": SCI_CRATE_GROUP, "x": -1545, "y": -920, "z": 20, "yaw": 12, "scale": 0.9, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Crate_B", "asset": SCI_CRATE, "x": -2060, "y": -650, "z": 20, "yaw": 74, "scale": 0.8, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Crate_C", "asset": SCI_CRATE_2, "x": -1320, "y": -1120, "z": 20, "yaw": -20, "scale": 0.85, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Lamp_Mara", "asset": SCI_LAMP, "x": -1970, "y": -775, "z": 20, "yaw": 5, "scale": 0.9, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Lamp_Road", "asset": SCI_LAMP, "x": -1150, "y": -520, "z": 20, "yaw": 8, "scale": 0.9, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Fence_01", "asset": SCI_FENCE, "x": -1580, "y": -1375, "z": 20, "yaw": 74, "scale": 1.2, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Fence_02", "asset": SCI_FENCE, "x": -2380, "y": -700, "z": 20, "yaw": -35, "scale": 1.2, "folder": "04_Fab_Zone_Pass/01_Village"},
    {"name": FAB_PREFIX + "Village_Fence_03", "asset": SCI_FENCE, "x": -980, "y": -800, "z": 20, "yaw": 28, "scale": 1.2, "folder": "04_Fab_Zone_Pass/01_Village"},

    # Road and wilderness boundary.
    {"name": FAB_PREFIX + "Road_Pine_01", "asset": KITE_TREE, "x": -1260, "y": -725, "z": 20, "yaw": 12, "scale": 0.52, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Pine_02", "asset": KITE_TALL_TREE, "x": -820, "y": -650, "z": 20, "yaw": -25, "scale": 0.5, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Pine_03", "asset": KITE_HILL_TREE, "x": -420, "y": -390, "z": 20, "yaw": 61, "scale": 0.44, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Pine_04", "asset": KITE_TREE, "x": 250, "y": -155, "z": 20, "yaw": 30, "scale": 0.5, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Pine_05", "asset": KITE_TALL_TREE, "x": 845, "y": 150, "z": 20, "yaw": -35, "scale": 0.48, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Pine_06", "asset": KITE_HILL_TREE, "x": 1380, "y": 290, "z": 20, "yaw": 14, "scale": 0.43, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Stump_01", "asset": KITE_STUMP, "x": -690, "y": -25, "z": 20, "yaw": 30, "scale": 0.8, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Boulder_01", "asset": KITE_ROCK, "x": -230, "y": 285, "z": 20, "yaw": -22, "scale": 0.75, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Boulder_02", "asset": KITE_RIVER_ROCK, "x": 525, "y": 690, "z": 20, "yaw": 88, "scale": 0.9, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Bush_01", "asset": KITE_BUSH, "x": -1060, "y": -125, "z": 20, "yaw": 0, "scale": 0.85, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Bush_02", "asset": KITE_BUSH_2, "x": 80, "y": 410, "z": 20, "yaw": 0, "scale": 0.85, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Fern_01", "asset": KITE_FERN, "x": -980, "y": -310, "z": 20, "yaw": 14, "scale": 1.2, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Fern_02", "asset": KITE_FERN, "x": 620, "y": 250, "z": 20, "yaw": -54, "scale": 1.2, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Flowers_01", "asset": KITE_FLOWERS, "x": -1450, "y": -540, "z": 20, "yaw": 10, "scale": 1.2, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Flowers_02", "asset": KITE_FLOWERS, "x": 1010, "y": 570, "z": 20, "yaw": -20, "scale": 1.2, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Grass_01", "asset": KITE_GRASS, "x": -375, "y": 80, "z": 20, "yaw": 0, "scale": 2.0, "folder": "04_Fab_Zone_Pass/02_Road"},
    {"name": FAB_PREFIX + "Road_Grass_02", "asset": KITE_GRASS, "x": 1190, "y": 865, "z": 20, "yaw": 0, "scale": 2.0, "folder": "04_Fab_Zone_Pass/02_Road"},

    # Wilderness combat pocket framing; leave the enemy centers open.
    {"name": FAB_PREFIX + "Wild_Tree_North_01", "asset": KITE_TALL_TREE, "x": 1450, "y": 1460, "z": 20, "yaw": 12, "scale": 0.55, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Tree_North_02", "asset": KITE_TREE, "x": 2030, "y": 1575, "z": 20, "yaw": -30, "scale": 0.55, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Tree_North_03", "asset": KITE_HILL_TREE, "x": 2680, "y": 1430, "z": 20, "yaw": 38, "scale": 0.48, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Tree_South_01", "asset": KITE_TREE, "x": 1340, "y": 95, "z": 20, "yaw": 15, "scale": 0.5, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Tree_South_02", "asset": KITE_TALL_TREE, "x": 1980, "y": -120, "z": 20, "yaw": -14, "scale": 0.5, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Tree_East_01", "asset": KITE_HILL_TREE, "x": 3200, "y": 1080, "z": 20, "yaw": 42, "scale": 0.5, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Rock_01", "asset": PARAGON_NORDIC_ROCK, "x": 1550, "y": 310, "z": 20, "yaw": 65, "scale": 0.72, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Rock_02", "asset": KITE_BOULDER, "x": 2840, "y": 1180, "z": 20, "yaw": 22, "scale": 0.68, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Rock_03", "asset": KITE_RIVER_ROCK, "x": 2460, "y": 210, "z": 20, "yaw": -18, "scale": 0.9, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Soul_Rock_01", "asset": SOUL_ROCK, "x": 3060, "y": 560, "z": 20, "yaw": 104, "scale": 1.05, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Bush_01", "asset": KITE_BUSH, "x": 1680, "y": 1260, "z": 20, "yaw": 0, "scale": 1.0, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Bush_02", "asset": KITE_BUSH_2, "x": 2250, "y": 1320, "z": 20, "yaw": 0, "scale": 1.0, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Ferns_01", "asset": KITE_FERN, "x": 1415, "y": 780, "z": 20, "yaw": -35, "scale": 1.25, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Ferns_02", "asset": KITE_FERN, "x": 2605, "y": 980, "z": 20, "yaw": 28, "scale": 1.25, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Roots_01", "asset": SOUL_ROOTS, "x": 1160, "y": 1030, "z": 20, "yaw": 46, "scale": 0.8, "folder": "04_Fab_Zone_Pass/03_Wilderness"},
    {"name": FAB_PREFIX + "Wild_Roots_02", "asset": SOUL_ROOTS, "x": 2940, "y": 1320, "z": 20, "yaw": -12, "scale": 0.8, "folder": "04_Fab_Zone_Pass/03_Wilderness"},

    # Ruin landmark.
    {"name": FAB_PREFIX + "Ruin_Main_Arch", "asset": PARAGON_ARCH, "x": 2645, "y": 410, "z": 20, "yaw": 90, "scale": 0.82, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Secondary_Arch", "asset": PARAGON_ARCH_B, "x": 2845, "y": 650, "z": 20, "yaw": 48, "scale": 0.58, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Wall_A", "asset": PARAGON_WALL, "x": 2400, "y": 360, "z": 20, "yaw": 82, "scale": 0.7, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Wall_B", "asset": PARAGON_WALL_CURVE, "x": 3060, "y": 820, "z": 20, "yaw": -35, "scale": 0.65, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Pillar_A", "asset": PARAGON_PILLAR, "x": 2480, "y": 680, "z": 20, "yaw": 15, "scale": 0.75, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Pillar_B", "asset": PARAGON_PILLAR_B, "x": 2915, "y": 330, "z": 20, "yaw": -24, "scale": 0.75, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Soul_Pillar", "asset": SOUL_PILLAR, "x": 2710, "y": 790, "z": 20, "yaw": 12, "scale": 0.85, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Broken_Soul_Pillar", "asset": SOUL_BROKEN_PILLAR, "x": 3130, "y": 560, "z": 20, "yaw": 35, "scale": 0.8, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Rubble_A", "asset": PARAGON_RUBBLE, "x": 2575, "y": 875, "z": 20, "yaw": 0, "scale": 0.85, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Rubble_B", "asset": PARAGON_AGORA_REMAINS, "x": 3020, "y": 1015, "z": 20, "yaw": -18, "scale": 0.7, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Stairs", "asset": PARAGON_STAIRS, "x": 2260, "y": 730, "z": 20, "yaw": 44, "scale": 0.72, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Statue_Base", "asset": PARAGON_STATUE_BASE, "x": 3180, "y": 900, "z": 20, "yaw": -125, "scale": 0.52, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Statue", "asset": PARAGON_STATUE, "x": 3200, "y": 900, "z": 120, "yaw": -125, "scale": 0.52, "folder": "04_Fab_Zone_Pass/04_Ruins", "snap": False},
    {"name": FAB_PREFIX + "Ruin_Agora_Pillar", "asset": PARAGON_AGORA_PILLAR, "x": 2300, "y": 515, "z": 20, "yaw": 28, "scale": 0.52, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Soul_Arch_Accent", "asset": SOUL_ARCH, "x": 2900, "y": 1040, "z": 20, "yaw": 18, "scale": 0.8, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Small_Rock_A", "asset": SOUL_SMALL_ROCK, "x": 2385, "y": 895, "z": 20, "yaw": 42, "scale": 1.15, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Small_Rock_B", "asset": SOUL_SMALL_ROCK, "x": 3300, "y": 715, "z": 20, "yaw": -10, "scale": 1.0, "folder": "04_Fab_Zone_Pass/04_Ruins"},
    {"name": FAB_PREFIX + "Ruin_Water_Glint", "asset": SOUL_WATER, "x": 2860, "y": 1120, "z": 25, "yaw": 30, "scale": 0.55, "folder": "04_Fab_Zone_Pass/04_Ruins", "snap": False},
    {"name": FAB_PREFIX + "Ruin_Fog_Sheet_A", "asset": SOUL_FOG, "x": 2700, "y": 740, "z": 60, "yaw": 72, "scale": 0.75, "folder": "04_Fab_Zone_Pass/04_Ruins", "snap": False},
]


GREYBOX_LABELS_TO_REMOVE = [
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
]


def run():
    current_level = scene("get_current_level", {})["returnValue"]
    if current_level != "/Game/Maps/L_Embermere_Prototype":
        scene("load_level", {"level_path": "/Game/Maps/L_Embermere_Prototype"})

    removed_prior = 0
    for actor_ref in find(FAB_PREFIX):
        if scene("remove_from_scene", {"actor": actor_ref})["returnValue"]:
            removed_prior += 1

    removed_greybox = 0
    for label in GREYBOX_LABELS_TO_REMOVE:
        removed_greybox += remove_by_label(label)

    created = []
    skipped = []
    for entry in PLACEMENTS:
        try:
            result = spawn(entry)
            if result["status"] == "created":
                created.append(result["name"])
            else:
                skipped.append(result)
        except Exception as exc:
            skipped.append({"name": entry["name"], "asset": entry["asset"], "status": "error", "error": str(exc)})

    saved = assets("save_assets", {"asset_paths": ["/Game/Maps/L_Embermere_Prototype"]})["returnValue"]
    return {
        "level": scene("get_current_level", {})["returnValue"],
        "removed_prior_fabpass": removed_prior,
        "removed_greybox_visuals": removed_greybox,
        "created_count": len(created),
        "created": created,
        "skipped": skipped,
        "saved": saved,
    }
