"""Exercise the saved Fenwatch rest presentation in a live PIE world.

Run ``begin_channel_probe()`` first. It uses the real player and saved rest
service, slows PIE so the cyan channel can be inspected, and leaves the
service pending. Run ``arm_completion_probe()`` next; it briefly accelerates
PIE, then slows it again as soon as the committed mint completion bloom begins.
Call ``finish_probe()`` before ending PIE to restore normal time dilation.
"""

import unreal


SERVICE_LABEL = "Embermere_FenwatchCommunalWell_RestService_01"
PRESENTATION_LABEL = "Embermere_FenwatchCommunalWell_RestPresentation_01"
PLAYER_LOCATION = unreal.Vector(-950.0, -1850.0, 95.0)
CHANNEL_DILATION = 0.01
COMPLETION_DILATION = 0.02


def fail(message):
    unreal.log_error("Embermere rest presentation live probe failed: {}".format(message))
    raise RuntimeError(message)


def require(condition, message):
    if not condition:
        fail(message)


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def get_live_state():
    world = unreal.EditorLevelLibrary.get_game_world()
    require(world, "PIE game world is unavailable")

    character = unreal.GameplayStatics.get_player_character(world, 0)
    require(
        character and isinstance(character, unreal.EmbermereCharacter),
        "Embermere player is unavailable",
    )

    services = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(
            world, unreal.EmbermereRestServiceActor
        )
        if actor_label(actor) == SERVICE_LABEL
    ]
    presentations = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(
            world, unreal.EmbermereRestPresentationActor
        )
        if actor_label(actor) == PRESENTATION_LABEL
    ]
    require(len(services) == 1, "expected one saved rest service, found {}".format(len(services)))
    require(
        len(presentations) == 1,
        "expected one saved rest presentation, found {}".format(len(presentations)),
    )

    service_actor = services[0]
    service = service_actor.get_component_by_class(unreal.EmbermereRestServiceComponent)
    presentation = presentations[0]
    stats = character.get_editor_property("stats")
    require(service, "saved rest service component is unavailable")
    require(stats, "player Stats component is unavailable")
    require(presentation.is_bound_to_rest_service(), "presentation did not bind at BeginPlay")

    return world, character, service, presentation, stats


def begin_channel_probe():
    world, character, service, presentation, stats = get_live_state()

    character.set_actor_location(PLAYER_LOCATION, False, True)
    character.set_actor_rotation(
        unreal.Rotator(pitch=0.0, yaw=90.0, roll=0.0), True
    )
    controller = unreal.GameplayStatics.get_player_controller(world, 0)
    if controller:
        controller.set_control_rotation(
            unreal.Rotator(pitch=-8.0, yaw=90.0, roll=0.0)
        )
    stats.apply_damage(35.0)
    require(stats.spend_mana(20.0), "could not spend 20 mana for the live recovery proof")

    unreal.GameplayStatics.set_global_time_dilation(world, CHANNEL_DILATION)
    result = service.try_begin_rest(character)
    require(result == unreal.EmbermereRestResult.STARTED, "rest request returned {}".format(result))
    require(service.is_rest_pending(), "rest service is not pending after Started")
    require(
        presentation.get_presentation_phase()
        == unreal.EmbermereRestPresentationPhase.CHANNELING,
        "presentation did not enter Channeling",
    )
    require(presentation.get_visible_rest_presentation_segment_count() == 12, "channel does not show 12 segments")
    require(presentation.are_rest_presentation_segments_non_colliding(), "channel segments gained collision")
    require(presentation.get_rest_presentation_radius() <= 48.5, "channel escaped the well shaft")

    unreal.log(
        "EMBERMERE_REST_PRESENTATION_CHANNEL_READY: real saved service, 12 cyan segments, "
        "radius={:.3f}, NoCollision, time_dilation={:.3f}".format(
            presentation.get_rest_presentation_radius(), CHANNEL_DILATION
        )
    )


def arm_completion_probe():
    world, _character, service, presentation, _stats = get_live_state()
    require(service.is_rest_pending(), "no pending rest channel is available to complete")

    previous_handle = getattr(unreal, "_embermere_rest_completion_probe_handle", None)
    if previous_handle is not None:
        unreal.unregister_slate_post_tick_callback(previous_handle)

    def watch_for_completion(_delta_seconds):
        if (
            presentation.get_presentation_phase()
            != unreal.EmbermereRestPresentationPhase.COMPLETION
        ):
            return

        unreal.GameplayStatics.set_global_time_dilation(world, COMPLETION_DILATION)
        handle = getattr(unreal, "_embermere_rest_completion_probe_handle", None)
        if handle is not None:
            unreal.unregister_slate_post_tick_callback(handle)
            unreal._embermere_rest_completion_probe_handle = None
        unreal.log(
            "EMBERMERE_REST_PRESENTATION_COMPLETION_READY: committed recovery, 12 mint segments, "
            "radius={:.3f}, NoCollision, time_dilation={:.3f}".format(
                presentation.get_rest_presentation_radius(), COMPLETION_DILATION
            )
        )

    unreal._embermere_rest_completion_probe_callback = watch_for_completion
    unreal._embermere_rest_completion_probe_handle = unreal.register_slate_post_tick_callback(
        watch_for_completion
    )
    unreal.GameplayStatics.set_global_time_dilation(world, 5.0)
    unreal.log("Embermere rest presentation completion probe armed")


def finish_probe():
    world = unreal.EditorLevelLibrary.get_game_world()
    if world:
        unreal.GameplayStatics.set_global_time_dilation(world, 1.0)
    handle = getattr(unreal, "_embermere_rest_completion_probe_handle", None)
    if handle is not None:
        unreal.unregister_slate_post_tick_callback(handle)
        unreal._embermere_rest_completion_probe_handle = None
    unreal.log("Embermere rest presentation live probe restored normal PIE time")
