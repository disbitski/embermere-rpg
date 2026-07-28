import json
import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
GROUND_LABEL = "Zone_Ground_Embermere_Glen"
ROUTE_MIN = (-2500.0, -1900.0)
ROUTE_MAX = (3300.0, 2200.0)
FLOAT_TOLERANCE_CM = 8.0


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def fail(message):
    unreal.log_error("Embermere grounding audit failed: {}".format(message))
    sys.exit(1)


def is_in_route(origin, extent):
    return not (
        origin.x + extent.x < ROUTE_MIN[0]
        or origin.x - extent.x > ROUTE_MAX[0]
        or origin.y + extent.y < ROUTE_MIN[1]
        or origin.y - extent.y > ROUTE_MAX[1]
    )


def component_material_paths(component):
    material_paths = []
    for material_index in range(component.get_num_materials()):
        material = component.get_material(material_index)
        material_paths.append(material.get_path_name() if material else "None")
    return material_paths


def mesh_material_slots(mesh):
    if not mesh:
        return []
    return [
        str(static_material.get_editor_property("material_slot_name"))
        for static_material in list(mesh.get_editor_property("static_materials"))
    ]


def main():
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actors = list(unreal.EditorLevelLibrary.get_all_level_actors())
    actors_by_label = {actor_label(actor): actor for actor in actors}

    ground = actors_by_label.get(GROUND_LABEL)
    if not ground:
        fail("missing ground actor {}".format(GROUND_LABEL))
    ground_origin, ground_extent = ground.get_actor_bounds(False)
    ground_top_z = ground_origin.z + ground_extent.z

    route_records = []
    for actor in actors:
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        if not component:
            continue

        origin, extent = actor.get_actor_bounds(False)
        if not is_in_route(origin, extent):
            continue

        mesh = component.get_editor_property("static_mesh")
        minimum_z = origin.z - extent.z
        record = {
            "label": actor_label(actor),
            "mesh": mesh.get_path_name() if mesh else "None",
            "location": [
                round(actor.get_actor_location().x, 2),
                round(actor.get_actor_location().y, 2),
                round(actor.get_actor_location().z, 2),
            ],
            "minimum_z": round(minimum_z, 2),
            "maximum_z": round(origin.z + extent.z, 2),
            "ground_gap_cm": round(minimum_z - ground_top_z, 2),
            "material_slots": mesh_material_slots(mesh),
            "materials": component_material_paths(component),
            "collision": str(component.get_collision_enabled()),
            "tags": sorted(str(tag) for tag in actor.tags),
        }
        route_records.append(record)

    route_records.sort(key=lambda item: (item["ground_gap_cm"], item["label"]), reverse=True)
    floating = [
        record
        for record in route_records
        if record["ground_gap_cm"] > FLOAT_TOLERANCE_CM
        and record["label"] != GROUND_LABEL
    ]
    missing_materials = [
        record
        for record in route_records
        if "None" in record["materials"]
    ]

    report = {
        "level": LEVEL_PATH,
        "ground_label": GROUND_LABEL,
        "ground_top_z": round(ground_top_z, 2),
        "route_actor_count": len(route_records),
        "floating_candidate_count": len(floating),
        "missing_material_candidate_count": len(missing_materials),
        "floating_candidates": floating,
        "missing_material_candidates": missing_materials,
    }
    unreal.log("EMBERMERE_GROUNDING_AUDIT {}".format(json.dumps(report, sort_keys=True)))
    unreal.log("Embermere grounding audit completed successfully.")


if __name__ == "__main__":
    main()
