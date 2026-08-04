"""Validate Fenwatch vendor data, service separation, and saved placement."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
STOCK_PATH = "/Game/Data/Vendors/DA_FenwatchQuartermasterStock"
SERVICE_LABEL = "Embermere_FenwatchQuartermaster_Service_01"
PRESENTATION_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
EXPECTED_LOCATION = unreal.Vector(-1530.0, -1190.0, 0.0)
EXPECTED_YAW = 100.0


def fail(message):
    unreal.log_error("Embermere Fenwatch vendor validation failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def main():
    stock = unreal.EditorAssetLibrary.load_asset(STOCK_PATH)
    if not stock or not isinstance(stock, unreal.EmbermereVendorStockData):
        fail("stock asset is missing or has the wrong class")
    entries = list(stock.get_editor_property("entries"))
    if str(stock.get_editor_property("vendor_name")) != "Fenwatch Supplies":
        fail("vendor name drifted")
    if len(entries) != 2:
        fail("expected two stock entries, found {}".format(len(entries)))

    expected = [
        ("MarshTonic", 8, -1),
        ("RecruitPack", 30, 1),
    ]
    for index, (item_id, price, quantity) in enumerate(expected):
        entry = entries[index]
        item = entry.get_editor_property("item")
        if not item or str(item.get_editor_property("item_id")) != item_id:
            fail("stock row {} item drifted".format(index))
        if int(entry.get_editor_property("unit_price_copper")) != price:
            fail("stock row {} price drifted".format(index))
        if int(entry.get_editor_property("initial_quantity")) != quantity:
            fail("stock row {} quantity drifted".format(index))

    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    services = []
    presentations = []
    for actor in actor_subsystem.get_all_level_actors():
        label = actor_label(actor)
        if label == SERVICE_LABEL:
            services.append(actor)
        elif label == PRESENTATION_LABEL:
            presentations.append(actor)
    if len(services) != 1:
        fail("expected one saved vendor service, found {}".format(len(services)))
    if len(presentations) != 1:
        fail("expected one saved quartermaster presentation, found {}".format(len(presentations)))

    service = services[0]
    presentation = presentations[0]
    if not isinstance(service, unreal.EmbermereVendorServiceActor):
        fail("saved service uses the wrong native class")
    if not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("saved presentation uses the wrong native class")
    if (service.get_actor_location() - EXPECTED_LOCATION).length() > 0.1:
        fail("service location drifted: {}".format(service.get_actor_location()))
    if abs(float(service.get_actor_rotation().yaw) - EXPECTED_YAW) > 0.1:
        fail("service yaw drifted: {}".format(service.get_actor_rotation().yaw))

    tags = {str(tag) for tag in service.get_editor_property("tags")}
    if not {"EmbermereGameplayService", "EmbermereVendor"}.issubset(tags):
        fail("service tags drifted: {}".format(sorted(tags)))
    vendor = service.get_component_by_class(unreal.EmbermereVendorComponent)
    interactable = service.get_component_by_class(unreal.EmbermereInteractableComponent)
    if not vendor or vendor.get_editor_property("stock_data") != stock:
        fail("service does not reference the reviewed stock data")
    if not interactable:
        fail("service is not interactable")
    if str(interactable.get_editor_property("display_name")) != "Fenwatch Quartermaster":
        fail("interaction display name drifted")
    if not bool(interactable.get_editor_property("show_world_marker")):
        fail("interaction marker is disabled")
    if abs(float(interactable.get_editor_property("marker_height")) - 238.0) > 0.1:
        fail("interaction marker height drifted")
    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("service unexpectedly owns static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("service unexpectedly owns skeletal art")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("presentation unexpectedly owns interaction")
    if presentation.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("presentation unexpectedly owns vendor behavior")

    unreal.log(
        "Embermere Fenwatch vendor validation passed: two data-driven wares, atomic service component, one saved art-free service actor, and presentation/service ownership separation intact"
    )


if __name__ == "__main__":
    main()
