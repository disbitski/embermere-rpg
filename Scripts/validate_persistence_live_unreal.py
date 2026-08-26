"""Exercise Embermere's disk-backed save/load contract in a live PIE world.

Run ``prepare_and_save()`` in a fresh PIE session, stop PIE, start a second
session, then run ``load_and_validate()``. The two-session split proves the
save survives beyond transient world objects. The trainer helpers prepare and
inspect the smaller Chronicle-driven progression proof without bypassing the
player-facing Save/Load surface.
"""

import unreal


TONIC_PATH = "/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"
PACK_PATH = "/Game/Data/Items/DI_EmbermereRecruitPack.DI_EmbermereRecruitPack"
QUEST_PATH = "/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"
SAVE_SLOT = "EmbermerePrototype"
SAVE_USER = 0


def fail(message):
    unreal.log_error("Embermere live persistence validation failed: {}".format(message))
    raise RuntimeError(message)


def require(condition, message):
    if not condition:
        fail(message)


def load_asset(path, expected_class):
    asset = unreal.load_asset(path)
    require(asset and isinstance(asset, expected_class), "missing or invalid asset {}".format(path))
    return asset


def get_live_state():
    world = unreal.EditorLevelLibrary.get_game_world()
    require(world, "PIE game world is unavailable")

    character = unreal.GameplayStatics.get_player_character(world, 0)
    controller = unreal.GameplayStatics.get_player_controller(world, 0)
    require(character and isinstance(character, unreal.EmbermereCharacter), "Embermere player is unavailable")
    require(controller and isinstance(controller, unreal.EmbermerePlayerController), "Embermere controller is unavailable")

    services = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.EmbermereVendorServiceActor)
    require(len(services) == 1, "expected one live vendor service, found {}".format(len(services)))
    vendor = services[0].get_component_by_class(unreal.EmbermereVendorComponent)
    require(vendor, "live vendor component is unavailable")

    trainer_services = unreal.GameplayStatics.get_all_actors_of_class(
        world, unreal.EmbermereTrainerServiceActor
    )
    require(
        len(trainer_services) == 1,
        "expected one live trainer service, found {}".format(len(trainer_services)),
    )
    trainer = trainer_services[0].get_component_by_class(unreal.EmbermereTrainerComponent)
    require(trainer, "live trainer component is unavailable")

    inventory = character.get_editor_property("inventory")
    equipment = character.get_editor_property("equipment")
    quest_log = character.get_editor_property("quest_log")
    stats = character.get_editor_property("stats")
    wallet = character.get_editor_property("wallet")
    hotbar = character.get_editor_property("hotbar")
    require(all((inventory, equipment, quest_log, stats, wallet, hotbar)), "one or more player components are unavailable")

    return {
        "world": world,
        "character": character,
        "controller": controller,
        "vendor": vendor,
        "trainer": trainer,
        "inventory": inventory,
        "equipment": equipment,
        "quest_log": quest_log,
        "stats": stats,
        "wallet": wallet,
        "hotbar": hotbar,
    }


def require_success(result, description):
    require(int(result.value) == 0, "{} returned {}".format(description, result))


def validate_trainer_progression_state(state, context):
    tonic = load_asset(TONIC_PATH, unreal.EmbermereItemData)
    pack = load_asset(PACK_PATH, unreal.EmbermereItemData)

    require(int(state["wallet"].get_editor_property("copper")) == 30, "{} did not contain 30 copper".format(context))
    require(
        int(state["stats"].get_editor_property("current_experience")) == 25,
        "{} did not contain 25 XP".format(context),
    )
    require(int(state["stats"].get_editor_property("level")) == 1, "{} changed the player level".format(context))
    require(state["inventory"].get_item_quantity(tonic) == 0, "{} added a Marsh Tonic".format(context))
    require(state["inventory"].get_item_quantity(pack) == 0, "{} added a Recruit Pack".format(context))
    require(
        state["equipment"].get_equipped_item(unreal.EmbermereEquipmentSlot.BACK) is None,
        "{} restored unexpected equipment".format(context),
    )
    require(abs(float(state["stats"].get_editor_property("max_health")) - 100.0) < 0.01, "{} changed max health".format(context))
    require(abs(float(state["stats"].get_editor_property("armor"))) < 0.01, "{} changed armor".format(context))
    require(state["vendor"].get_remaining_quantity(1) == 1, "{} changed finite vendor stock".format(context))
    require(state["vendor"].get_buyback_entry_count() == 0, "{} restored session-only buyback".format(context))

    quest_state = state["quest_log"].get_editor_property("active_quest")
    require(quest_state.get_editor_property("quest") is None, "{} restored an unexpected quest".format(context))


def prepare_trainer_progression_for_chronicle():
    state = get_live_state()
    unreal.GameplayStatics.delete_game_in_slot(SAVE_SLOT, SAVE_USER)
    require(not unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "old Chronicle slot was not deleted")
    require(int(state["wallet"].get_editor_property("copper")) == 40, "fresh trainer wallet is not 40 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 0, "fresh trainer XP is not zero")
    require(state["trainer"].get_offering_count() == 2, "live trainer does not expose exactly two offerings")

    require_success(
        state["trainer"].try_train(0, state["stats"], state["wallet"]),
        "Combat Drills training",
    )
    validate_trainer_progression_state(state, "prepared trainer progression")
    require(not unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "trainer preparation bypassed Chronicle and created a save")
    unreal.log(
        "Embermere trainer persistence prepare passed: Combat Drills produced exactly 30 copper and 25 XP without creating a save"
    )


def validate_fresh_trainer_session_before_chronicle_load():
    state = get_live_state()
    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "Chronicle trainer save is missing")
    require(int(state["wallet"].get_editor_property("copper")) == 40, "fresh load session did not start with 40 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 0, "fresh load session did not start with zero XP")
    require(state["trainer"].get_offering_count() == 2, "fresh load session lost the trainer offerings")
    unreal.log("Embermere trainer persistence fresh-session preflight passed: 40 copper, 0 XP, saved Chronicle present")


def validate_trainer_chronicle_load(context="Chronicle load"):
    state = get_live_state()
    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "Chronicle trainer save is missing")
    validate_trainer_progression_state(state, context)
    require(state["trainer"].get_offering_count() == 2, "{} changed transient trainer offerings".format(context))
    unreal.log(
        "Embermere trainer persistence {} passed: exact 30 copper and 25 XP with no schema expansion, duplication, or transient service state".format(context)
    )


def enum_index(value):
    return int(value.value)


def require_float(actual, expected, description):
    require(abs(float(actual) - expected) < 0.01, "{}: expected {}, got {}".format(description, expected, actual))


def validate_advanced_trainer_chronicle_state(state, context):
    pack = load_asset(PACK_PATH, unreal.EmbermereItemData)
    quest = load_asset(QUEST_PATH, unreal.EmbermereQuestData)

    character = state["character"]
    stats = state["stats"]
    require(enum_index(character.get_editor_property("race")) == 1, "{} did not restore Elf race".format(context))
    require(enum_index(character.get_editor_property("class")) == 3, "{} did not restore Wizard class".format(context))
    require(bool(character.get_editor_property("has_deliberate_character_choice")), "{} lost deliberate identity".format(context))
    require(int(state["wallet"].get_editor_property("copper")) == 40, "{} did not contain 40 copper".format(context))
    require(int(stats.get_editor_property("current_experience")) == 175, "{} did not contain 175 XP".format(context))
    require(int(stats.get_editor_property("level")) == 2, "{} did not derive level 2".format(context))

    require_float(stats.get_editor_property("max_health"), 89.0, "{} max health".format(context))
    require_float(stats.get_editor_property("current_health"), 89.0, "{} current health".format(context))
    require_float(stats.get_editor_property("max_mana"), 122.0, "{} max mana".format(context))
    require_float(stats.get_editor_property("current_mana"), 122.0, "{} current mana".format(context))
    require_float(stats.get_editor_property("attack_power"), 6.5, "{} attack power".format(context))
    require_float(stats.get_editor_property("strength"), 6.5, "{} strength".format(context))
    require_float(stats.get_editor_property("spirit"), 13.5, "{} spirit".format(context))
    require_float(stats.get_editor_property("agility"), 9.25, "{} agility".format(context))
    require_float(stats.get_editor_property("intellect"), 18.75, "{} intellect".format(context))
    require_float(stats.get_editor_property("armor"), 1.0, "{} armor".format(context))

    require(state["inventory"].get_item_quantity(pack) == 0, "{} duplicated the equipped Recruit Pack in the bag".format(context))
    require(
        state["equipment"].get_equipped_item(unreal.EmbermereEquipmentSlot.BACK) == pack,
        "{} did not restore the equipped Recruit Pack".format(context),
    )
    quest_state = state["quest_log"].get_editor_property("active_quest")
    require(quest_state.get_editor_property("quest") == quest, "{} restored the wrong quest".format(context))
    require(bool(quest_state.get_editor_property("completed")), "{} did not restore the completed quest".format(context))
    require(
        int(quest_state.get_editor_property("current_objective_count"))
        == int(quest.get_editor_property("required_objective_count")),
        "{} restored the wrong quest progress".format(context),
    )
    require(state["vendor"].get_remaining_quantity(1) == 1, "{} changed untouched finite vendor stock".format(context))
    require(state["vendor"].get_buyback_entry_count() == 0, "{} restored session-only buyback".format(context))
    require(state["trainer"].get_offering_count() == 2, "{} changed transient trainer offerings".format(context))

    expected_abilities = ("SparkBolt", "FrostRoot", "ArcaneBurst", "Meditate")
    slots = state["hotbar"].get_editor_property("slots")
    for index, expected_ability in enumerate(expected_abilities):
        actual_ability = str(slots[index].get_editor_property("ability_id"))
        require(
            actual_ability == expected_ability,
            "{} hotbar slot {} expected {}, got {}".format(context, index + 1, expected_ability, actual_ability),
        )


def prepare_advanced_trainer_progression_for_chronicle():
    state = get_live_state()
    pack = load_asset(PACK_PATH, unreal.EmbermereItemData)
    quest = load_asset(QUEST_PATH, unreal.EmbermereQuestData)

    unreal.GameplayStatics.delete_game_in_slot(SAVE_SLOT, SAVE_USER)
    require(not unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "old Chronicle slot was not deleted")
    require(enum_index(state["character"].get_editor_property("race")) == 1, "prepared character is not Elf")
    require(enum_index(state["character"].get_editor_property("class")) == 3, "prepared character is not Wizard")
    require(int(state["wallet"].get_editor_property("copper")) == 40, "fresh wallet is not 40 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 0, "fresh XP is not zero")
    require(state["trainer"].get_offering_count() == 2, "live trainer does not expose two offerings")

    require(state["quest_log"].accept_quest(quest), "starter quest was not accepted")
    require(
        state["quest_log"].add_objective_progress(
            quest.get_editor_property("objective_id"),
            int(quest.get_editor_property("required_objective_count")),
        ),
        "starter quest progress was not accepted",
    )
    require(state["quest_log"].try_complete_active_quest(), "starter quest did not complete")
    require(int(state["wallet"].get_editor_property("copper")) == 60, "quest reward did not produce 60 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 125, "quest reward did not produce 125 XP")
    require(int(state["stats"].get_editor_property("level")) == 2, "quest reward did not derive level 2")

    require_success(
        state["trainer"].try_train(1, state["stats"], state["wallet"]),
        "Advanced Combat Drills training",
    )
    require(
        state["equipment"].equip_from_inventory(
            pack,
            int(state["stats"].get_editor_property("level")),
            state["inventory"],
        ),
        "quest Recruit Pack did not equip from the bag",
    )
    validate_advanced_trainer_chronicle_state(state, "prepared Advanced Chronicle state")
    require(not unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "preparation bypassed Chronicle and created a save")
    unreal.log(
        "EMBERMERE_ADVANCED_CHRONICLE_PREPARE_SUCCESS: Elf Wizard level 2, 175 XP, 40 copper, completed quest, equipped Recruit Pack, one real Advanced Combat Drills transaction"
    )


def validate_advanced_chronicle_slot_created():
    state = get_live_state()
    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "Chronicle did not create the Advanced trainer save")
    save = unreal.GameplayStatics.load_game_from_slot(SAVE_SLOT, SAVE_USER)
    require(save and isinstance(save, unreal.EmbermereSaveGame), "Chronicle slot is not an Embermere save")
    require(int(save.get_editor_property("format_version")) == 2, "Chronicle trainer save changed format version")
    validate_advanced_trainer_chronicle_state(state, "post-save live state")
    unreal.log("EMBERMERE_ADVANCED_CHRONICLE_SAVE_SUCCESS: version 2 slot created through Chronicle with exact live state")


def validate_fresh_advanced_session_before_chronicle_load():
    state = get_live_state()
    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "Advanced Chronicle save is missing")
    require(enum_index(state["character"].get_editor_property("race")) == 2, "fresh comparison character is not Dwarf")
    require(enum_index(state["character"].get_editor_property("class")) == 0, "fresh comparison character is not Warrior")
    require(int(state["wallet"].get_editor_property("copper")) == 40, "fresh comparison wallet is not 40 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 0, "fresh comparison XP is not zero")
    require(int(state["stats"].get_editor_property("level")) == 1, "fresh comparison level is not 1")
    require(state["equipment"].get_equipped_item(unreal.EmbermereEquipmentSlot.BACK) is None, "fresh comparison character has equipment")
    require(state["vendor"].get_remaining_quantity(1) == 1, "fresh comparison vendor stock is not one")
    unreal.log("EMBERMERE_ADVANCED_CHRONICLE_FRESH_WORLD_SUCCESS: Dwarf Warrior level 1, 0 XP, 40 copper before load")


def validate_advanced_chronicle_load(context="Advanced Chronicle load"):
    state = get_live_state()
    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "Advanced Chronicle save is missing")
    validate_advanced_trainer_chronicle_state(state, context)
    require(not state["quest_log"].try_complete_active_quest(), "{} replayed the completed quest reward".format(context))
    require(int(state["wallet"].get_editor_property("copper")) == 40, "{} drifted copper after replay check".format(context))
    require(int(state["stats"].get_editor_property("current_experience")) == 175, "{} drifted XP after replay check".format(context))
    unreal.log(
        "EMBERMERE_ADVANCED_CHRONICLE_LOAD_SUCCESS: {} restored exact identity, progression, equipment, quest, stock, and hotbar without replay or drift".format(context)
    )


def prepare_level_up_world_vfx_live_probe():
    state = get_live_state()
    character = state["character"]
    stats = state["stats"]
    wallet = state["wallet"]

    require(enum_index(character.get_editor_property("race")) == 0, "world VFX probe character is not Human")
    require(enum_index(character.get_editor_property("class")) == 0, "world VFX probe character is not Warrior")
    require(int(wallet.get_editor_property("copper")) == 40, "world VFX probe wallet is not 40 copper")
    require(int(stats.get_editor_property("current_experience")) == 0, "world VFX probe XP is not zero")

    for training_index in range(4):
        require_success(
            state["trainer"].try_train(0, stats, wallet),
            "Combat Drills world VFX transaction {}".format(training_index + 1),
        )

    require(int(wallet.get_editor_property("copper")) == 0, "four Combat Drills did not consume exactly 40 copper")
    require(int(stats.get_editor_property("current_experience")) == 100, "four Combat Drills did not produce exactly 100 XP")
    require(int(stats.get_editor_property("level")) == 2, "four Combat Drills did not derive level 2")
    require(character.is_level_up_world_vfx_visible(), "live level transition did not start world VFX")
    require(character.get_level_up_world_vfx_segment_count() == 12, "live world VFX does not own twelve segments")
    require(character.get_visible_level_up_world_vfx_segment_count() == 12, "live world VFX does not show twelve segments")
    require(character.are_level_up_world_vfx_segments_non_colliding(), "live world VFX contributes collision or navigation")
    require(character.get_level_up_world_vfx_levels_gained() == 1, "live world VFX reports the wrong level span")
    require(
        "M_EmbermereTargetRing" in character.get_level_up_world_vfx_material_path(),
        "live world VFX does not use the project-owned emissive material",
    )
    require(unreal.GameplayStatics.set_game_paused(state["world"], True), "could not pause the live world VFX frame")
    unreal.log(
        "EMBERMERE_LEVEL_UP_WORLD_VFX_LIVE_SUCCESS: real Combat Drills reached level 2 and exposed twelve paused non-colliding Warrior segments"
    )


def prepare_and_save():
    state = get_live_state()
    tonic = load_asset(TONIC_PATH, unreal.EmbermereItemData)
    pack = load_asset(PACK_PATH, unreal.EmbermereItemData)
    quest = load_asset(QUEST_PATH, unreal.EmbermereQuestData)

    unreal.GameplayStatics.delete_game_in_slot(SAVE_SLOT, SAVE_USER)
    require(int(state["wallet"].get_editor_property("copper")) == 40, "fresh wallet is not 40 copper")
    require(state["inventory"].get_item_quantity(tonic) == 0, "fresh bag already contains Marsh Tonic")
    require(state["inventory"].get_item_quantity(pack) == 0, "fresh bag already contains Recruit Pack")
    require(state["vendor"].get_remaining_quantity(1) == 1, "fresh finite Recruit Pack stock is not one")

    require_success(
        state["vendor"].try_purchase(0, 1, state["inventory"], state["wallet"]),
        "Marsh Tonic purchase",
    )
    require(int(state["wallet"].get_editor_property("copper")) == 32, "tonic purchase did not leave 32 copper")
    require_success(
        state["vendor"].try_sell(tonic, 1, state["inventory"], state["wallet"]),
        "Marsh Tonic sale",
    )
    require(int(state["wallet"].get_editor_property("copper")) == 35, "tonic sale did not leave 35 copper")
    require_success(
        state["vendor"].try_buyback(0, 1, state["inventory"], state["wallet"]),
        "Marsh Tonic buyback",
    )
    require(int(state["wallet"].get_editor_property("copper")) == 32, "tonic buyback did not return to 32 copper")
    require_success(
        state["vendor"].try_purchase(1, 1, state["inventory"], state["wallet"]),
        "Recruit Pack purchase",
    )
    require(int(state["wallet"].get_editor_property("copper")) == 2, "Recruit Pack purchase did not leave two copper")
    require(state["vendor"].get_remaining_quantity(1) == 0, "finite Recruit Pack stock was not exhausted")

    require(state["quest_log"].accept_quest(quest), "starter quest was not accepted")
    objective_id = quest.get_editor_property("objective_id")
    objective_count = int(quest.get_editor_property("required_objective_count"))
    require(
        state["quest_log"].add_objective_progress(objective_id, objective_count),
        "starter quest progress was not accepted",
    )
    require(state["quest_log"].try_complete_active_quest(), "starter quest did not complete")
    require(int(state["wallet"].get_editor_property("copper")) == 22, "quest reward did not leave exactly 22 copper")

    level = int(state["stats"].get_editor_property("level"))
    require(
        state["equipment"].equip_from_inventory(pack, level, state["inventory"]),
        "Recruit Pack did not equip from the real bag",
    )
    require(state["inventory"].get_item_quantity(tonic) == 1, "prepared save does not contain one tonic")
    require(state["inventory"].get_item_quantity(pack) == 1, "prepared save does not contain one bagged pack")
    require(state["equipment"].get_equipped_item(unreal.EmbermereEquipmentSlot.BACK) == pack, "Back slot is not equipped")

    state["controller"].embermere_save()
    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "disk save was not created")
    unreal.log(
        "Embermere live persistence prepare passed: 22 copper, completed quest, one tonic, one bagged Recruit Pack, one equipped Recruit Pack, exhausted finite stock"
    )


def validate_loaded_state(state, tonic, pack, quest, expected_experience):
    require(int(state["wallet"].get_editor_property("copper")) == 22, "load did not restore 22 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == expected_experience, "load restored the wrong XP")
    require(state["inventory"].get_item_quantity(tonic) == 1, "load did not restore one tonic")
    require(state["inventory"].get_item_quantity(pack) == 1, "load did not restore one bagged Recruit Pack")
    require(state["equipment"].get_equipped_item(unreal.EmbermereEquipmentSlot.BACK) == pack, "load did not restore the Back slot")
    require(abs(float(state["stats"].get_editor_property("max_health")) - 105.0) < 0.01, "equipment health bonus was not applied exactly once")
    require(abs(float(state["stats"].get_editor_property("armor")) - 1.0) < 0.01, "equipment armor bonus was not applied exactly once")
    require(abs(float(state["stats"].get_editor_property("current_health")) - 105.0) < 0.01, "load did not reset health to equipped maximum")

    quest_state = state["quest_log"].get_editor_property("active_quest")
    require(quest_state.get_editor_property("quest") == quest, "load restored the wrong quest identity")
    require(bool(quest_state.get_editor_property("completed")), "load did not restore completed quest state")
    require(
        int(quest_state.get_editor_property("current_objective_count"))
        == int(quest.get_editor_property("required_objective_count")),
        "load restored the wrong quest progress",
    )
    require(state["vendor"].get_remaining_quantity(1) == 0, "load did not restore exhausted finite stock")
    require(state["vendor"].get_buyback_entry_count() == 0, "load did not clear session-only buyback")


def load_and_validate():
    state = get_live_state()
    tonic = load_asset(TONIC_PATH, unreal.EmbermereItemData)
    pack = load_asset(PACK_PATH, unreal.EmbermereItemData)
    quest = load_asset(QUEST_PATH, unreal.EmbermereQuestData)
    expected_experience = int(quest.get_editor_property("reward_experience"))

    require(unreal.GameplayStatics.does_save_game_exist(SAVE_SLOT, SAVE_USER), "prepared disk save is missing")
    require(int(state["wallet"].get_editor_property("copper")) == 40, "second PIE session did not start from fresh state")
    require(state["inventory"].get_item_quantity(tonic) == 0, "second PIE bag is not fresh")
    require(state["vendor"].get_remaining_quantity(1) == 1, "second PIE vendor stock is not fresh")

    state["controller"].embermere_load()
    validate_loaded_state(state, tonic, pack, quest, expected_experience)
    require(not state["quest_log"].try_complete_active_quest(), "loaded completed quest paid out again")
    require(int(state["wallet"].get_editor_property("copper")) == 22, "repeat quest completion changed copper")

    state["controller"].embermere_load()
    validate_loaded_state(state, tonic, pack, quest, expected_experience)
    unreal.log(
        "Embermere live persistence load passed: fresh PIE restored exact identity, quest, stock, XP, copper, and equipment stats twice without duplication"
    )
