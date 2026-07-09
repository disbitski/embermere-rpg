import importlib.util
import os
import traceback

import unreal


PROJECT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_PATH = os.path.join(PROJECT_DIR, "Scripts", "place_fab_zone_pass.py")
LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"


def load_data():
    spec = importlib.util.spec_from_file_location("embermere_fab_zone_data", DATA_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def vec(data):
    return unreal.Vector(float(data["x"]), float(data["y"]), float(data["z"]))


def rot(yaw):
    rotation = unreal.Rotator()
    rotation.pitch = 0.0
    rotation.yaw = float(yaw)
    rotation.roll = 0.0
    return rotation


def scale_value(value):
    if isinstance(value, dict):
        return unreal.Vector(float(value["x"]), float(value["y"]), float(value["z"]))
    return unreal.Vector(float(value), float(value), float(value))


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def remove_visuals(data):
    removed_prior = 0
    removed_greybox = 0
    greybox_labels = set(data.GREYBOX_LABELS_TO_REMOVE)

    for actor in list(unreal.EditorLevelLibrary.get_all_level_actors()):
        label = actor_label(actor)
        if label.startswith(data.FAB_PREFIX):
            unreal.EditorLevelLibrary.destroy_actor(actor)
            removed_prior += 1
        elif label in greybox_labels:
            unreal.EditorLevelLibrary.destroy_actor(actor)
            removed_greybox += 1

    return removed_prior, removed_greybox


def spawn_entry(data, entry):
    asset_path = entry["asset"]
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset is None:
        return {"name": entry["name"], "asset": asset_path, "status": "missing"}

    location = unreal.Vector(float(entry["x"]), float(entry["y"]), float(entry.get("z", 20.0)))
    actor = unreal.EditorLevelLibrary.spawn_actor_from_object(asset, location, rot(entry.get("yaw", 0.0)))
    if actor is None:
        return {"name": entry["name"], "asset": asset_path, "status": "not_created"}

    actor.set_actor_label(entry["name"])
    actor.set_actor_scale3d(scale_value(entry.get("scale", 1.0)))
    actor.tags = list(actor.tags) + [unreal.Name(data.FAB_TAG)]
    actor.set_folder_path(entry["folder"])
    return {"name": entry["name"], "asset": asset_path, "status": "created"}


def main():
    data = load_data()
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)

    removed_prior, removed_greybox = remove_visuals(data)
    created = []
    skipped = []

    for entry in data.PLACEMENTS:
        try:
            result = spawn_entry(data, entry)
            if result["status"] == "created":
                created.append(result["name"])
            else:
                skipped.append(result)
        except Exception as exc:
            skipped.append({
                "name": entry["name"],
                "asset": entry["asset"],
                "status": "error",
                "error": str(exc),
                "trace": traceback.format_exc(limit=2),
            })

    unreal.EditorLevelLibrary.save_current_level()
    unreal.EditorAssetLibrary.save_asset(LEVEL_PATH, only_if_is_dirty=False)

    unreal.log("Embermere Fab zone pass complete")
    unreal.log("Removed prior FabPass actors: {}".format(removed_prior))
    unreal.log("Removed greybox visuals: {}".format(removed_greybox))
    unreal.log("Created FabPass actors: {}".format(len(created)))
    if skipped:
        unreal.log_warning("Skipped FabPass actors: {}".format(skipped))


if __name__ == "__main__":
    main()
