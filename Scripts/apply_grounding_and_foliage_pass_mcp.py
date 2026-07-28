import json


FAB_PREFIX = "FabPass_"
ORIGINAL_LABELS = {
    "Embermere_Waystone_Road_01",
    "Embermere_EmberLamp_Mara_01",
    "Embermere_EmberLamp_Road_01",
    "Embermere_RoadSignpost_01",
    "Embermere_RoadGate_01",
    "Embermere_BoundaryFence_GateSouth_01",
    "Embermere_BoundaryFence_GateNorth_01",
    "Embermere_BoundaryStone_GateSouth_01",
    "Embermere_BoundaryStone_GateNorth_01",
    "Embermere_SupplyChest_Vendor_01",
}
REMOVED_LABELS = {
    "FabPass_Ruin_Soul_Arch_Accent",
    "FabPass_Ruin_Soul_Pillar",
    "Enemy_Visual_Marker_01",
    "Enemy_Visual_Marker_02",
    "Enemy_Visual_Marker_03",
}
TREE_PREFIXES = (
    "FabPass_Road_Pine_",
    "FabPass_Wild_Tree_",
)
GREEN_FOLIAGE_TOKENS = (
    "_Fern",
    "_Ferns",
    "_Flowers",
    "_Grass",
)
TIMBER_MATERIAL = "/Game/Art/Embermere/Environment/PrototypeVillage/M_EmbermereTimber.M_EmbermereTimber"
MOSS_MATERIAL = "/Game/Art/Embermere/Environment/PrototypeVillage/M_WaystoneMoss.M_WaystoneMoss"
STONE_MATERIAL = "/Game/Art/Embermere/Environment/PrototypeVillage/M_Waystone.M_Waystone"
TREE_BILLBOARD_MATERIAL = (
    "/Game/KiteDemo/Environments/Trees/HillTree_02/"
    "M_CustomDepthBillboard_Inst.M_CustomDepthBillboard_Inst"
)


def call(tool_name, payload):
    return execute_tool(tool_name, json.dumps(payload))["returnValue"]


def scene(tool_name, payload):
    return call("editor_toolset.toolsets.scene.SceneTools." + tool_name, payload)


def actor(tool_name, payload):
    return call("editor_toolset.toolsets.actor.ActorTools." + tool_name, payload)


def objects(tool_name, payload):
    return call("editor_toolset.toolsets.object.ObjectTools." + tool_name, payload)


def assets(tool_name, payload):
    return call("editor_toolset.toolsets.asset.AssetTools." + tool_name, payload)


def find_exact(label):
    matches = scene("find_actors", {
        "name": label,
        "tag": "",
        "collision_channels": [],
    })
    return [
        actor_ref
        for actor_ref in matches
        if actor("get_label", {"actor": actor_ref}) == label
    ]


def static_mesh_component(actor_ref):
    components = actor("get_components", {
        "actor": actor_ref,
        "component_type": {"refPath": "/Script/Engine.StaticMeshComponent"},
    })
    return components[0] if components else None


def set_override_materials(component_ref, material_paths):
    values = {
        "OverrideMaterials": [{"refPath": material_path} for material_path in material_paths],
    }
    return objects("set_properties", {
        "instance": component_ref,
        "values": json.dumps(values),
    })


def transform_with_z(transform, z):
    updated = dict(transform)
    updated["location"] = dict(updated["location"])
    updated["location"]["z"] = float(z)
    return updated


def run():
    removed = []
    for label in sorted(REMOVED_LABELS):
        for actor_ref in find_exact(label):
            if scene("remove_from_scene", {"actor": actor_ref}):
                removed.append(label)

    grounded = []
    material_overrides = []
    candidates = scene("find_actors", {
        "name": FAB_PREFIX,
        "tag": "",
        "collision_channels": [],
    })
    for label in sorted(ORIGINAL_LABELS):
        candidates.extend(find_exact(label))

    seen = set()
    for actor_ref in candidates:
        ref_path = actor_ref["refPath"]
        if ref_path in seen:
            continue
        seen.add(ref_path)

        label = actor("get_label", {"actor": actor_ref})
        if label in REMOVED_LABELS:
            continue

        transform = actor("get_actor_transform", {"actor": actor_ref})
        current_z = float(transform["location"]["z"])
        desired_z = 68.0 if label == "FabPass_Ruin_Statue" else current_z
        if abs(current_z - 20.0) <= 0.01:
            desired_z = 0.0
        if abs(desired_z - current_z) > 0.01:
            if actor("set_actor_transform", {
                "actor": actor_ref,
                "xform": transform_with_z(transform, desired_z),
                "worldspace": True,
            }):
                grounded.append({"label": label, "from_z": current_z, "to_z": desired_z})

        component_ref = static_mesh_component(actor_ref)
        if not component_ref:
            continue

        if label.startswith(TREE_PREFIXES):
            if set_override_materials(component_ref, [
                TIMBER_MATERIAL,
                TIMBER_MATERIAL,
                MOSS_MATERIAL,
                MOSS_MATERIAL,
                TREE_BILLBOARD_MATERIAL,
            ]):
                material_overrides.append(label)
        elif any(token in label for token in GREEN_FOLIAGE_TOKENS):
            if set_override_materials(component_ref, [MOSS_MATERIAL]):
                material_overrides.append(label)
        elif label == "FabPass_Road_Boulder_01":
            if set_override_materials(component_ref, [STONE_MATERIAL]):
                material_overrides.append(label)

    saved = assets("save_assets", {
        "asset_paths": ["/Game/Maps/L_Embermere_Prototype"],
    })
    return {
        "removed": removed,
        "grounded_count": len(grounded),
        "grounded": grounded,
        "material_override_count": len(material_overrides),
        "material_overrides": material_overrides,
        "saved": saved,
    }
