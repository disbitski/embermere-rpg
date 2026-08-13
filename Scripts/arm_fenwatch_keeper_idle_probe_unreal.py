"""Arm a two-sample Mara Idle probe that waits for the next live PIE world."""

import time

import unreal


PRESENTATION_LABEL = "Embermere_FenwatchKeeper_Mara_Presentation_01"
EXPECTED_IDLE_PATH = (
    "/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/Animations/"
    "A_EmbermereFenwatchKeeper_Mara_Idle.A_EmbermereFenwatchKeeper_Mara_Idle"
)
STATE = {
    "started": None,
    "first_position": None,
    "first_time": None,
    "handle": None,
}


def actor_label(actor):
    try:
        return actor.get_actor_label()
    except Exception:
        return actor.get_name()


def finish(message, error=False):
    handle = STATE.get("handle")
    if handle is not None:
        unreal.unregister_slate_post_tick_callback(handle)
        STATE["handle"] = None
    if error:
        unreal.log_error("Embermere keeper two-sample Idle probe failed: {}".format(message))
    else:
        unreal.log(message)


def tick(delta_seconds):
    now = time.monotonic()
    if STATE["started"] is None:
        STATE["started"] = now
    world = unreal.EditorLevelLibrary.get_game_world()
    if not world:
        if now - STATE["started"] > 30.0:
            finish("PIE did not start within 30 seconds", True)
        return
    matches = [
        actor
        for actor in unreal.GameplayStatics.get_all_actors_of_class(
            world, unreal.EmbermereNpcPresentationActor
        )
        if actor_label(actor) == PRESENTATION_LABEL
    ]
    if len(matches) != 1:
        finish("expected one live keeper presentation, found {}".format(len(matches)), True)
        return
    presentation = matches[0]
    component = presentation.get_component_by_class(unreal.SkeletalMeshComponent)
    animation = presentation.get_editor_property("idle_animation")
    animation_path = animation.get_path_name() if animation else "None"
    if not component or animation_path != EXPECTED_IDLE_PATH:
        finish("live component or Idle reference drifted: {}".format(animation_path), True)
        return
    if presentation.get_resolved_visual_mode() != unreal.EmbermereNpcVisualMode.SKELETAL_MESH:
        finish("live presentation is not using the skeletal lane", True)
        return
    if presentation.get_resolved_animation_mode() != unreal.EmbermereNpcAnimationMode.SINGLE_NODE_IDLE:
        finish("live presentation is not using single-node Idle", True)
        return
    if not component.is_playing() or not presentation.is_presentation_collision_disabled():
        finish("Idle is not playing or presentation collision is enabled", True)
        return
    position = float(component.get_position())
    if STATE["first_position"] is None:
        STATE["first_position"] = position
        STATE["first_time"] = now
        return
    if now - STATE["first_time"] < 1.0:
        return
    first = STATE["first_position"]
    forward_delta = (position - first) % 3.6
    if forward_delta < 0.5 or forward_delta > 2.0:
        finish(
            "clock did not advance as expected: first={:.6f}, second={:.6f}, delta={:.6f}".format(
                first, position, forward_delta
            ),
            True,
        )
        return
    finish(
        "Embermere keeper two-sample Idle probe passed: first={:.6f}, "
        "second={:.6f}, forward_delta={:.6f}, playing=true, "
        "mode=SingleNodeIdle, collision=NoCollision".format(
            first, position, forward_delta
        )
    )


STATE["handle"] = unreal.register_slate_post_tick_callback(tick)
unreal.log("Embermere keeper two-sample Idle probe armed for the next PIE session")
