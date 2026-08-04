"""Create Fenwatch vendor stock and place its art-free gameplay service actor."""

import sys

import unreal


LEVEL_PATH = "/Game/Maps/L_Embermere_Prototype"
STOCK_PATH = "/Game/Data/Vendors/DA_FenwatchQuartermasterStock"
TONIC_PATH = "/Game/Data/Items/DI_MarshTonic"
RECRUIT_PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack"
SERVICE_LABEL = "Embermere_FenwatchQuartermaster_Service_01"
PRESENTATION_LABEL = "Embermere_FenwatchQuartermaster_Vendor_01"
SERVICE_LOCATION = (-1530.0, -1190.0, 0.0)
SERVICE_YAW = 100.0
SERVICE_FOLDER = "01_Village/NPCs/Services"
SERVICE_TAGS = [unreal.Name("EmbermereGameplayService"), unreal.Name("EmbermereVendor")]


def fail(message):
    unreal.log_error("Embermere Fenwatch vendor integration failed: {}".format(message))
    sys.exit(1)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def make_rotation(yaw):
    rotation = unreal.Rotator()
    rotation.pitch = 0.0
    rotation.yaw = float(yaw)
    rotation.roll = 0.0
    return rotation


def load_required(path, expected_type):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset or not isinstance(asset, expected_type):
        fail("missing {} at {}".format(expected_type.__name__, path))
    return asset


def load_or_create_stock():
    stock = (
        unreal.EditorAssetLibrary.load_asset(STOCK_PATH)
        if unreal.EditorAssetLibrary.does_asset_exist(STOCK_PATH)
        else None
    )
    if stock and not isinstance(stock, unreal.EmbermereVendorStockData):
        fail("{} exists with the wrong class".format(STOCK_PATH))
    if not stock:
        package_path, asset_name = STOCK_PATH.rsplit("/", 1)
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.EmbermereVendorStockData)
        stock = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            asset_name,
            package_path,
            unreal.EmbermereVendorStockData,
            factory,
        )
    if not stock:
        fail("could not create {}".format(STOCK_PATH))

    tonic = load_required(TONIC_PATH, unreal.EmbermereItemData)
    recruit_pack = load_required(RECRUIT_PACK_PATH, unreal.EmbermereItemData)

    tonic_entry = unreal.EmbermereVendorStockEntry()
    tonic_entry.set_editor_property("item", tonic)
    tonic_entry.set_editor_property("unit_price_copper", 8)
    tonic_entry.set_editor_property("initial_quantity", -1)

    pack_entry = unreal.EmbermereVendorStockEntry()
    pack_entry.set_editor_property("item", recruit_pack)
    pack_entry.set_editor_property("unit_price_copper", 30)
    pack_entry.set_editor_property("initial_quantity", 1)

    stock.set_editor_property("vendor_name", "Fenwatch Supplies")
    stock.set_editor_property("entries", [tonic_entry, pack_entry])
    if not unreal.EditorAssetLibrary.save_loaded_asset(stock, only_if_is_dirty=False):
        fail("could not save {}".format(STOCK_PATH))
    return stock


def place_service(stock):
    unreal.EditorLevelLibrary.load_level(LEVEL_PATH)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    presentation = None
    removed = []
    for actor in list(actor_subsystem.get_all_level_actors()):
        label = actor_label(actor)
        if label == SERVICE_LABEL:
            actor_subsystem.destroy_actor(actor)
            removed.append(label)
        elif label == PRESENTATION_LABEL:
            presentation = actor

    if not presentation or not isinstance(presentation, unreal.EmbermereNpcPresentationActor):
        fail("accepted quartermaster presentation is missing")
    if presentation.get_component_by_class(unreal.EmbermereInteractableComponent):
        fail("quartermaster presentation unexpectedly owns interaction")
    if presentation.get_component_by_class(unreal.EmbermereVendorComponent):
        fail("quartermaster presentation unexpectedly owns vendor behavior")

    service = actor_subsystem.spawn_actor_from_class(
        unreal.EmbermereVendorServiceActor,
        unreal.Vector(*SERVICE_LOCATION),
        make_rotation(SERVICE_YAW),
        False,
    )
    if not service:
        fail("could not spawn {}".format(SERVICE_LABEL))

    service.set_actor_label(SERVICE_LABEL)
    service.set_folder_path(unreal.Name(SERVICE_FOLDER))
    service.set_editor_property("tags", SERVICE_TAGS)
    vendor = service.get_component_by_class(unreal.EmbermereVendorComponent)
    interactable = service.get_component_by_class(unreal.EmbermereInteractableComponent)
    if not vendor or not interactable:
        fail("service actor is missing its native components")
    vendor.set_editor_property("stock_data", stock)
    interactable.set_editor_property("display_name", "Fenwatch Quartermaster")
    interactable.set_editor_property("dialogue_text", "")
    interactable.set_editor_property("show_world_marker", True)
    interactable.set_editor_property("marker_height", 238.0)

    if service.get_component_by_class(unreal.StaticMeshComponent):
        fail("service actor must not own static art")
    if service.get_component_by_class(unreal.SkeletalMeshComponent):
        fail("service actor must not own skeletal art")
    if not unreal.EditorLevelLibrary.save_current_level():
        fail("could not save {}".format(LEVEL_PATH))
    return removed


def main():
    stock = load_or_create_stock()
    removed = place_service(stock)
    unreal.log(
        "Embermere Fenwatch vendor integration passed: stock={}, entries=2, service={}, removed={}".format(
            STOCK_PATH,
            SERVICE_LABEL,
            removed,
        )
    )


if __name__ == "__main__":
    main()
