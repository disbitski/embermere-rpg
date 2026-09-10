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
    if not objects("set_properties", {
        "instance": instance,
        "values": json.dumps(values),
    }):
        raise RuntimeError("Property assignment failed: " + instance["refPath"])


def get_properties(instance):
    result = objects("get_properties", {
        "instance": instance,
        "properties": [
            "skeletalMeshAsset",
            "relativeLocation",
            "relativeRotation",
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
    rotation = values.get("relativeRotation", {})
    scale = values.get("relativeScale3D", {})

    if mesh_ref != MESH_PATH:
        raise RuntimeError(owner_label + " does not resolve the Marsh Prowler mesh")
    if idle_ref != IDLE_PATH:
        raise RuntimeError(owner_label + " does not resolve the Marsh Prowler idle animation")
    if any(abs(float(location.get(axis, 999.0)) - target) > 0.01
           for axis, target in (("x", 0.0), ("y", 0.0), ("z", -95.0))):
        raise RuntimeError(owner_label + " has the wrong mesh-to-capsule offset")
    if any(abs(float(rotation.get(axis, 999.0))) > 0.01 for axis in ("pitch", "yaw", "roll")):
        raise RuntimeError(owner_label + " has the wrong mesh rotation")
    if any(abs(float(scale.get(axis, 0.0)) - 0.65) > 0.001 for axis in ("x", "y", "z")):
        raise RuntimeError(owner_label + " has the wrong Marsh Prowler scale")
    if values.get("animationMode") != "AnimationSingleNode":
        raise RuntimeError(owner_label + " is not using single-node runtime animation")


def native_values():
    values = {
        property_name: {"refPath": asset_path}
        for property_name, asset_path in ANIMATION_PATHS.items()
    }
    values.update({
        "visualSkeletalMesh": {"refPath": MESH_PATH},
        "visualMeshRelativeLocation": {"x": 0.0, "y": 0.0, "z": -95.0},
        "visualMeshRelativeRotation": {"pitch": 0.0, "yaw": 0.0, "roll": 0.0},
        "visualMeshRelativeScale": {"x": 0.65, "y": 0.65, "z": 0.65},
    })
    return values


def verify_native(owner, label, allow_empty_references=False):
    expected = native_values()
    values = json.loads(objects("get_properties", {
        "instance": owner, "properties": list(expected),
    }))
    for prop, target in expected.items():
        actual = values.get(prop)
        if "refPath" in target:
            if allow_empty_references and actual in (None, "None", "", {"refPath": ""}):
                continue
            path = actual.get("refPath") if isinstance(actual, dict) else actual
            if path != target["refPath"]:
                raise RuntimeError(label + " has unexpected " + prop + ": " + repr(actual))
        elif not isinstance(actual, dict) or any(
                abs(float(actual.get(axis, 999)) - value) > 0.001
                for axis, value in target.items()):
            raise RuntimeError(label + " has unexpected " + prop)


def run(repair_instances_only=False):
    blueprint_ref = {"refPath": BLUEPRINT_PATH}
    cdo_ref = blueprints("get_default_object", {"blueprint": blueprint_ref})

    configured = []
    level_actors = scene("find_actors", {
        "name": "Starter_Enemy_",
        "tag": "",
        "collision_channels": [],
    })
    owners = []
    for actor_ref in level_actors:
        label = actor("get_label", {"actor": actor_ref})
        if not actor_ref["refPath"].startswith(MAP_PATH + ".L_Embermere_Prototype:PersistentLevel."):
            raise RuntimeError("Expected an actor in the saved prototype map")
        if objects("get_class", {"instance": actor_ref})["refPath"] != BLUEPRINT_PATH + "_C":
            raise RuntimeError("Unexpected starter-enemy class: " + label)
        component_ref = skeletal_mesh_component(actor_ref)
        owners.append((label, actor_ref, component_ref))
        configured.append(label)

    configured.sort()
    if configured != ["Starter_Enemy_01", "Starter_Enemy_02", "Starter_Enemy_03"]:
        raise RuntimeError("Unexpected saved starter-enemy set: " + repr(configured))

    if repair_instances_only:
        # Reject unrelated authored drift before changing any placed actor.
        verify_native(cdo_ref, "Blueprint defaults")
        verify_component(skeletal_mesh_component(cdo_ref), "Blueprint defaults")
        for label, owner, component in owners:
            verify_native(owner, label, allow_empty_references=True)
            verify_component(component, label)
    else:
        set_properties(cdo_ref, native_values())
        set_properties(skeletal_mesh_component(cdo_ref), presentation_values())
        blueprints("compile_blueprint", {
            "blueprint": blueprint_ref, "warnings_as_errors": True,
        })
        cdo_ref = blueprints("get_default_object", {"blueprint": blueprint_ref})
        verify_native(cdo_ref, "Blueprint defaults")
        verify_component(skeletal_mesh_component(cdo_ref), "Blueprint defaults")

    for label, owner, component in owners:
        if not repair_instances_only:
            component = skeletal_mesh_component(owner)
        values = native_values()
        if repair_instances_only:
            values = {key: value for key, value in values.items() if "refPath" in value}
        set_properties(owner, values)
        if not repair_instances_only:
            set_properties(component, presentation_values())
        verify_native(owner, label)
        verify_component(component, label)

    saved = assets("save_assets", {
        "asset_paths": [MAP_PATH] if repair_instances_only else [
            "/Game/Blueprints/BP_StarterEnemy", MAP_PATH],
    })
    if not saved:
        raise RuntimeError("Prowler package save failed")
    return {
        "configured": configured,
        "mesh": MESH_PATH,
        "animation_count": len(ANIMATION_PATHS),
        "saved": saved,
    }
