import json


BLUEPRINT_PATH = "/Game/Blueprints/BP_StarterEnemy.BP_StarterEnemy"
MAP_PATH = "/Game/Maps/L_Embermere_Prototype"
MESH_PATH = (
    "/Game/Art/Embermere/Characters/Enemies/MarshProwler/"
    "SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01"
)
ANIMATION_ROOT = (
    "/Game/Art/Embermere/Characters/Enemies/MarshProwler/Animations/"
)
ANIMATION_PATHS = {
    "idleAnimation": ANIMATION_ROOT
    + "A_EmbermereMarshProwler_Idle.A_EmbermereMarshProwler_Idle",
    "walkAnimation": ANIMATION_ROOT
    + "A_EmbermereMarshProwler_Walk.A_EmbermereMarshProwler_Walk",
    "runAnimation": ANIMATION_ROOT
    + "A_EmbermereMarshProwler_Run.A_EmbermereMarshProwler_Run",
    "attackAnimation": ANIMATION_ROOT
    + "A_EmbermereMarshProwler_Attack.A_EmbermereMarshProwler_Attack",
    "hitAnimation": ANIMATION_ROOT
    + "A_EmbermereMarshProwler_Hit.A_EmbermereMarshProwler_Hit",
    "deathAnimation": ANIMATION_ROOT
    + "A_EmbermereMarshProwler_Death.A_EmbermereMarshProwler_Death",
}
IDLE_PATH = ANIMATION_PATHS["idleAnimation"]


def call(tool_name, payload):
    return execute_tool(tool_name, json.dumps(payload))["returnValue"]


def scene(tool_name, payload):
    return call("editor_toolset.toolsets.scene.SceneTools." + tool_name, payload)


def actor(tool_name, payload):
    return call("editor_toolset.toolsets.actor.ActorTools." + tool_name, payload)


def objects(tool_name, payload):
    return call("editor_toolset.toolsets.object.ObjectTools." + tool_name, payload)


def blueprints(tool_name, payload):
    return call("editor_toolset.toolsets.blueprint.BlueprintTools." + tool_name, payload)


def assets(tool_name, payload):
    return call("editor_toolset.toolsets.asset.AssetTools." + tool_name, payload)


def skeletal_mesh_component(actor_ref):
    components = actor("get_components", {
        "actor": actor_ref,
        "component_type": {"refPath": "/Script/Engine.SkeletalMeshComponent"},
    })
    if not components:
        raise RuntimeError("Actor has no skeletal mesh component: " + actor_ref["refPath"])
    return components[0]


def presentation_values():
    return {
        "skeletalMeshAsset": {"refPath": MESH_PATH},
        "relativeLocation": {"x": 0.0, "y": 0.0, "z": -95.0},
        "relativeRotation": {"pitch": 0.0, "yaw": 0.0, "roll": 0.0},
        "relativeScale3D": {"x": 0.65, "y": 0.65, "z": 0.65},
        "animationMode": "AnimationSingleNode",
        "animationData": {
            "animToPlay": {"refPath": IDLE_PATH},
            "bSavedLooping": True,
            "bSavedPlaying": True,
            "savedPosition": 0.0,
            "savedPlayRate": 1.0,
        },
    }


def set_properties(instance, values):
    return objects("set_properties", {
        "instance": instance,
        "values": json.dumps(values),
    })


def get_properties(instance):
    result = objects("get_properties", {
        "instance": instance,
        "properties": [
            "skeletalMeshAsset",
            "relativeLocation",
            "relativeScale3D",
            "animationMode",
            "animationData",
        ],
    })
    return json.loads(result)


def verify_component(component_ref, owner_label):
    values = get_properties(component_ref)
    mesh_ref = values.get("skeletalMeshAsset", {}).get("refPath", "")
    idle_ref = values.get("animationData", {}).get("animToPlay", {}).get("refPath", "")
    location = values.get("relativeLocation", {})
    scale = values.get("relativeScale3D", {})

    if mesh_ref != MESH_PATH:
        raise RuntimeError(owner_label + " does not resolve the Marsh Prowler mesh")
    if idle_ref != IDLE_PATH:
        raise RuntimeError(owner_label + " does not resolve the Marsh Prowler idle animation")
    if abs(float(location.get("z", 0.0)) + 95.0) > 0.01:
        raise RuntimeError(owner_label + " has the wrong mesh-to-capsule offset")
    if any(abs(float(scale.get(axis, 0.0)) - 0.65) > 0.001 for axis in ("x", "y", "z")):
        raise RuntimeError(owner_label + " has the wrong Marsh Prowler scale")
    if values.get("animationMode") != "AnimationSingleNode":
        raise RuntimeError(owner_label + " is not using single-node runtime animation")


def run():
    blueprint_ref = {"refPath": BLUEPRINT_PATH}
    cdo_ref = blueprints("get_default_object", {"blueprint": blueprint_ref})
    cdo_animation_values = {
        property_name: {"refPath": asset_path}
        for property_name, asset_path in ANIMATION_PATHS.items()
    }
    cdo_animation_values.update({
        "visualSkeletalMesh": {"refPath": MESH_PATH},
        "visualMeshRelativeLocation": {"x": 0.0, "y": 0.0, "z": -95.0},
        "visualMeshRelativeRotation": {"pitch": 0.0, "yaw": 0.0, "roll": 0.0},
        "visualMeshRelativeScale": {"x": 0.65, "y": 0.65, "z": 0.65},
    })
    set_properties(cdo_ref, cdo_animation_values)
    set_properties(skeletal_mesh_component(cdo_ref), presentation_values())
    blueprints("compile_blueprint", {
        "blueprint": blueprint_ref,
        "warnings_as_errors": True,
    })

    configured = []
    level_actors = scene("find_actors", {
        "name": "Starter_Enemy_",
        "tag": "",
        "collision_channels": [],
    })
    for actor_ref in level_actors:
        label = actor("get_label", {"actor": actor_ref})
        component_ref = skeletal_mesh_component(actor_ref)
        set_properties(component_ref, presentation_values())
        verify_component(component_ref, label)
        configured.append(label)

    configured.sort()
    if configured != ["Starter_Enemy_01", "Starter_Enemy_02", "Starter_Enemy_03"]:
        raise RuntimeError("Unexpected saved starter-enemy set: " + repr(configured))

    saved = assets("save_assets", {
        "asset_paths": [
            "/Game/Blueprints/BP_StarterEnemy",
            MAP_PATH,
        ],
    })
    return {
        "configured": configured,
        "mesh": MESH_PATH,
        "animation_count": len(ANIMATION_PATHS),
        "saved": saved,
    }
