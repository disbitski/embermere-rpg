"""Exercise Still Waters through the saved notice board and rest service in PIE.

Load this file once from Unreal's command box, then call each phase helper around
the real F interactions. The probe changes no package and writes only its own
temporary save slot; it never inspects, overwrites, or loads the player's
Chronicle slot.
"""

import unreal


NOTICE_SERVICE_LABEL = "Embermere_FenwatchNoticeBoard_StillWatersService_01"
REST_SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
MARA_LABEL = "Quest_Giver_Mara_Fenwatch"
STILL_WATERS_ID = "FenwatchStillWaters"
MARA_QUEST_ID = "FirstSignsAtTheRuin"
PROBE_SAVE_SLOT = "EmbermereStillWatersLiveProbe"
PROBE_SAVE_USER = 0


def fail(message):
    unreal.log_error("Embermere Still Waters live probe failed: {}".format(message))
    raise RuntimeError(message)


def require(condition, message):
    if not condition:
        fail(message)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def find_actor(world, label):
    matches = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(
            world, unreal.Actor
        )
        if actor_label(actor) == label
    ]
    require(len(matches) == 1, "expected one {}, found {}".format(label, len(matches)))
    return matches[0]


def get_live_state():
    world = unreal.EditorLevelLibrary.get_game_world()
    require(world, "PIE game world is unavailable")

    character = unreal.GameplayStatics.get_player_character(world, 0)
    require(
        character and isinstance(character, unreal.EmbermereCharacter),
        "Embermere player is unavailable",
    )
    stats = character.get_editor_property("stats")
    wallet = character.get_editor_property("wallet")
    quest_log = character.get_editor_property("quest_log")
    require(all((stats, wallet, quest_log)), "player authority components are unavailable")

    notice_service = find_actor(world, NOTICE_SERVICE_LABEL)
    rest_service_actor = find_actor(world, REST_SERVICE_LABEL)
    mara = find_actor(world, MARA_LABEL)
    rest_service = rest_service_actor.get_component_by_class(
        unreal.EmbermereRestServiceComponent
    )
    require(rest_service, "saved rest service component is unavailable")

    return {
        "world": world,
        "character": character,
        "stats": stats,
        "wallet": wallet,
        "quest_log": quest_log,
        "notice_service": notice_service,
        "rest_service_actor": rest_service_actor,
        "rest_service": rest_service,
        "mara": mara,
    }


def get_quest_state(quest_log, quest_id):
    for state in quest_log.get_editor_property("quest_states"):
        quest = state.get_editor_property("quest")
        if quest and str(quest.get_editor_property("quest_id")) == quest_id:
            return state
    return None


def move_character_to(character, actor):
    location = actor.get_actor_location() + unreal.Vector(220.0, 0.0, 95.0)
    character.set_actor_location(location, False, True)
    character.set_actor_rotation(
        unreal.Rotator(pitch=0.0, yaw=180.0, roll=0.0), True
    )
    movement = character.get_component_by_class(unreal.CharacterMovementComponent)
    if movement:
        movement.stop_movement_immediately()


def prepare_notice_acceptance():
    state = get_live_state()
    require(
        get_quest_state(state["quest_log"], STILL_WATERS_ID) is None,
        "Still Waters is already tracked before notice-board acceptance",
    )
    require(
        int(state["wallet"].get_editor_property("copper")) == 40,
        "fresh wallet is not 40 copper",
    )
    require(
        int(state["stats"].get_editor_property("current_experience")) == 0,
        "fresh XP is not zero",
    )
    move_character_to(state["character"], state["notice_service"])
    unreal.log(
        "EMBERMERE_STILL_WATERS_NOTICE_READY: player is inside the saved art-free notice-board service radius"
    )


def validate_notice_acceptance():
    state = get_live_state()
    quest_state = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    require(quest_state, "physical notice-board F did not accept Still Waters")
    require(
        int(quest_state.get_editor_property("current_objective_count")) == 0,
        "Still Waters did not begin at zero progress",
    )
    require(not bool(quest_state.get_editor_property("completed")), "quest began completed")
    require(int(state["wallet"].get_editor_property("copper")) == 40, "acceptance changed copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 0, "acceptance changed XP")
    unreal.log(
        "EMBERMERE_STILL_WATERS_ACCEPT_SUCCESS: physical F accepted the exact quest at 0/1 without rewards"
    )


def prepare_mara_acceptance():
    state = get_live_state()
    move_character_to(state["character"], state["mara"])
    unreal.log("EMBERMERE_STILL_WATERS_MARA_READY: player is inside Mara's original Blueprint interaction radius")


def validate_parallel_active_quests():
    state = get_live_state()
    still_waters = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    mara = get_quest_state(state["quest_log"], MARA_QUEST_ID)
    require(still_waters and mara, "physical interactions did not leave both quests tracked")
    require(
        int(still_waters.get_editor_property("current_objective_count")) == 0
        and not bool(still_waters.get_editor_property("completed")),
        "Still Waters changed while accepting Mara",
    )
    require(
        int(mara.get_editor_property("current_objective_count")) == 0
        and not bool(mara.get_editor_property("completed")),
        "Mara did not remain independently active at 0/3",
    )
    unreal.log(
        "EMBERMERE_STILL_WATERS_PARALLEL_SUCCESS: Still Waters 0/1 and Mara 0/3 coexist under their original separate owners"
    )


def prepare_interrupted_rest():
    state = get_live_state()
    stats = state["stats"]
    stats.clear_damage_immunity()
    require(stats.apply_damage(25.0) > 0.0, "could not apply real health loss")
    require(stats.spend_mana(15.0), "could not spend real mana")
    move_character_to(state["character"], state["rest_service_actor"])
    unreal.log(
        "EMBERMERE_STILL_WATERS_INTERRUPT_READY: player has missing Health/Mana inside the saved well-service radius"
    )


def force_movement_interruption():
    state = get_live_state()
    require(state["rest_service"].is_rest_pending(), "physical well F did not start a rest channel")
    character = state["character"]
    character.set_actor_location(
        character.get_actor_location() + unreal.Vector(75.0, 0.0, 0.0),
        False,
        True,
    )
    unreal.log("EMBERMERE_STILL_WATERS_INTERRUPT_MOVED: player moved 75 cm during the real pending channel")


def validate_interrupted_rest():
    state = get_live_state()
    quest_state = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    require(not state["rest_service"].is_rest_pending(), "movement did not cancel the rest channel")
    require(
        int(quest_state.get_editor_property("current_objective_count")) == 0,
        "interrupted rest advanced Still Waters",
    )
    require(
        float(state["stats"].get_editor_property("current_health"))
        < float(state["stats"].get_editor_property("max_health")),
        "interrupted rest restored health",
    )
    require(
        float(state["stats"].get_editor_property("current_mana"))
        < float(state["stats"].get_editor_property("max_mana")),
        "interrupted rest restored mana",
    )
    unreal.log(
        "EMBERMERE_STILL_WATERS_INTERRUPT_SUCCESS: movement cleared the channel with 0/1 progress and no partial recovery"
    )


def prepare_committed_rest():
    state = get_live_state()
    move_character_to(state["character"], state["rest_service_actor"])
    require(
        float(state["stats"].get_editor_property("current_health"))
        < float(state["stats"].get_editor_property("max_health")),
        "success proof no longer has missing health",
    )
    require(
        float(state["stats"].get_editor_property("current_mana"))
        < float(state["stats"].get_editor_property("max_mana")),
        "success proof no longer has missing mana",
    )
    unreal.log("EMBERMERE_STILL_WATERS_REST_READY: interrupted vitals remain missing for a second physical F channel")


def validate_committed_rest():
    state = get_live_state()
    quest_state = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    mara = get_quest_state(state["quest_log"], MARA_QUEST_ID)
    require(not state["rest_service"].is_rest_pending(), "committed rest is still pending")
    require(
        int(quest_state.get_editor_property("current_objective_count")) == 1,
        "committed rest did not advance Still Waters to 1/1",
    )
    require(not bool(quest_state.get_editor_property("completed")), "objective completion auto-granted rewards")
    require(
        int(mara.get_editor_property("current_objective_count")) == 0,
        "rest success mutated Mara's hostile objective",
    )
    require(
        abs(float(state["stats"].get_editor_property("current_health")) - float(state["stats"].get_editor_property("max_health"))) < 0.01,
        "committed rest did not restore full health",
    )
    require(
        abs(float(state["stats"].get_editor_property("current_mana")) - float(state["stats"].get_editor_property("max_mana"))) < 0.01,
        "committed rest did not restore full mana",
    )
    move_character_to(state["character"], state["notice_service"])
    unreal.log(
        "EMBERMERE_STILL_WATERS_REST_SUCCESS: one committed service Success produced exact 1/1 ready state, full vitals, and no Mara progress"
    )


def validate_turn_in():
    state = get_live_state()
    quest_state = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    mara = get_quest_state(state["quest_log"], MARA_QUEST_ID)
    require(bool(quest_state.get_editor_property("completed")), "physical notice-board F did not complete Still Waters")
    require(int(state["wallet"].get_editor_property("copper")) == 50, "turn-in did not grant exact 10 copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 50, "turn-in did not grant exact 50 XP")
    require(
        int(mara.get_editor_property("current_objective_count")) == 0
        and not bool(mara.get_editor_property("completed")),
        "Still Waters turn-in mutated Mara",
    )
    unreal.log(
        "EMBERMERE_STILL_WATERS_TURN_IN_SUCCESS: physical F committed 50 XP and 10 copper exactly once while Mara remained active at 0/3"
    )


def validate_no_reward_replay():
    state = get_live_state()
    quest_state = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    require(bool(quest_state.get_editor_property("completed")), "repeat F lost completed state")
    require(int(state["wallet"].get_editor_property("copper")) == 50, "repeat F replayed copper")
    require(int(state["stats"].get_editor_property("current_experience")) == 50, "repeat F replayed XP")
    unreal.log(
        "EMBERMERE_STILL_WATERS_REPLAY_GUARD_SUCCESS: repeat physical F preserved completed state at exact 50 XP and 50 copper"
    )


def save_and_mutate_probe_state():
    state = get_live_state()
    controller = unreal.GameplayStatics.get_player_controller(state["world"], 0)
    require(
        controller and isinstance(controller, unreal.EmbermerePlayerController),
        "Embermere controller is unavailable",
    )
    try:
        controller.set_editor_property("prototype_save_slot_name", PROBE_SAVE_SLOT)
        controller.set_editor_property("prototype_save_user_index", PROBE_SAVE_USER)
        unreal.GameplayStatics.delete_game_in_slot(PROBE_SAVE_SLOT, PROBE_SAVE_USER)
        controller.embermere_save()
        require(
            unreal.GameplayStatics.does_save_game_exist(
                PROBE_SAVE_SLOT, PROBE_SAVE_USER
            ),
            "isolated version-3 probe slot was not created",
        )
        save_game = unreal.GameplayStatics.load_game_from_slot(
            PROBE_SAVE_SLOT, PROBE_SAVE_USER
        )
        require(
            save_game and isinstance(save_game, unreal.EmbermereSaveGame),
            "isolated slot is not an Embermere save",
        )
        require(
            int(save_game.get_editor_property("format_version")) == 3,
            "isolated slot is not save version 3",
        )
        require(
            len(save_game.get_editor_property("quest_states")) == 2,
            "isolated slot did not capture both quest records",
        )

        mara = get_quest_state(state["quest_log"], MARA_QUEST_ID)
        mara_quest = mara.get_editor_property("quest")
        require(
            state["quest_log"].add_objective_progress_for_quest(
                mara_quest.get_editor_property("quest_id"),
                mara_quest.get_editor_property("objective_id"),
                1,
            ),
            "could not create divergent Mara progress before load",
        )
        state["wallet"].set_copper_for_prototype(3)
        require(
            state["stats"].try_add_experience(200),
            "could not create divergent XP before load",
        )
        unreal.log(
            "EMBERMERE_STILL_WATERS_V3_SAVE_SUCCESS: isolated version-3 slot captured completed Still Waters plus active Mara, then live state diverged"
        )
    except Exception:
        unreal.GameplayStatics.delete_game_in_slot(PROBE_SAVE_SLOT, PROBE_SAVE_USER)
        controller.set_editor_property("prototype_save_slot_name", "EmbermerePrototype")
        controller.set_editor_property("prototype_save_user_index", 0)
        raise


def load_and_validate_probe_state(context):
    state = get_live_state()
    controller = unreal.GameplayStatics.get_player_controller(state["world"], 0)
    controller.embermere_load()
    state = get_live_state()
    still_waters = get_quest_state(state["quest_log"], STILL_WATERS_ID)
    mara = get_quest_state(state["quest_log"], MARA_QUEST_ID)
    require(still_waters and mara, "{} did not restore both quest records".format(context))
    require(
        bool(still_waters.get_editor_property("completed"))
        and int(still_waters.get_editor_property("current_objective_count")) == 1,
        "{} did not restore completed Still Waters 1/1".format(context),
    )
    require(
        not bool(mara.get_editor_property("completed"))
        and int(mara.get_editor_property("current_objective_count")) == 0,
        "{} did not restore active Mara 0/3".format(context),
    )
    require(
        int(state["wallet"].get_editor_property("copper")) == 50,
        "{} drifted or replayed copper".format(context),
    )
    require(
        int(state["stats"].get_editor_property("current_experience")) == 50,
        "{} drifted or replayed XP".format(context),
    )
    unreal.log(
        "EMBERMERE_STILL_WATERS_V3_LOAD_SUCCESS: {} restored exact two-quest ledger, 50 XP, and 50 copper without progress or reward replay".format(
            context
        )
    )


def cleanup_probe_save():
    state = get_live_state()
    controller = unreal.GameplayStatics.get_player_controller(state["world"], 0)
    unreal.GameplayStatics.delete_game_in_slot(PROBE_SAVE_SLOT, PROBE_SAVE_USER)
    controller.set_editor_property("prototype_save_slot_name", "EmbermerePrototype")
    controller.set_editor_property("prototype_save_user_index", 0)
    require(
        not unreal.GameplayStatics.does_save_game_exist(
            PROBE_SAVE_SLOT, PROBE_SAVE_USER
        ),
        "isolated live-probe slot was not deleted",
    )
    unreal.log(
        "EMBERMERE_STILL_WATERS_V3_CLEANUP_SUCCESS: isolated probe slot removed and Chronicle target restored"
    )
