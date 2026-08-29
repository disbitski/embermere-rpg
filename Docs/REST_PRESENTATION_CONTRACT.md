# Embermere Rest Presentation Contract

## Purpose

Fenwatch's communal well already has two accepted owners:

- the static well mesh owns removable world art;
- the art-free rest service owns interaction, validation, channel state,
  cooldown, and atomic Health/Mana recovery.

The rest presentation is a third, equally removable observer. It makes a valid
channel and committed recovery readable from the normal camera without moving
any gameplay authority into art or effects.

## Authority Boundary

`AEmbermereRestPresentationActor` may:

- subscribe to the service's native C++ mirror of its immutable
  `OnRestOutcome` publication;
- retain a weak reference to the character named by the observed outcome;
- render a fixed twelve-segment project-material effect;
- animate a restrained cyan channel inside the open well shaft;
- transition to a short mint completion bloom only after `Success` is
  published;
- clear on interruption, death, observer unbinding, expiry, or world teardown.

It must not:

- own interaction, range, channel duration, movement tolerance, combat checks,
  cooldown, Health/Mana policy, or mutation;
- infer success from elapsed presentation time;
- publish gameplay messages or rest outcomes;
- contain vendor, trainer, quest, reward, inventory, or persistence authority;
- serialize channel, completion, cooldown, or character state;
- replay presentation when a save is loaded.

## Visual Contract

- The observer owns exactly twelve transient plane segments while active.
- Channeling remains inside the 64 cm open shaft with a bounded cyan rotation.
- Completion expands across the curb to a maximum 120 cm radius for exactly
  1.1 seconds, then clears.
- All segments use the project target-ring material, are `NoCollision`, cast no
  shadow, generate no overlap, and cannot affect navigation.
- Geometry remains at a fixed 55 cm local height, just above the accepted curb,
  so it does not resize the HUD or obstruct the well roof, marker, or approach.
- Rejected requests never create a success effect. A duplicate request during
  a valid channel does not restart or cancel the existing channel.

## Lifecycle

1. `Started` begins the channel observer and records the outcome character.
2. The observer remains channeling only while the authoritative service reports
   a pending rest for that character.
3. `Success` begins the fixed completion lifetime using the exact committed
   outcome; it does not perform recovery itself.
4. Any resolved interruption clears channel presentation immediately.
5. Character death, invalid references, unbinding, and `EndPlay` clear all
   visible segments and transient references.
6. A fresh world or loaded save starts hidden because no live outcome has been
   published.

## Acceptance Gates

- focused automation proves event subscription, deferred geometry, fixed
  segment count and bounds, non-collision, duplicate safety, success-only
  completion, deterministic expiry, interruption/death clearing, and teardown;
- saved-map validation proves one colocated presentation observer references the
  separate service while the well and service remain free of each other's
  authority;
- clean PIE proves the cyan shaft channel and mint completion bloom are readable
  without obscuring the marker, roof, route, chat, or HUD;
- all existing rest, combat, progression, persistence, and world validators
  remain green.

## Accepted Evidence

The 2026-08-29 clean-PIE pass exercised the saved service and observer in the
real starter zone. The channel showed twelve cyan segments at a `46` cm radius
inside the shaft. The committed result showed twelve mint segments beginning
at `52` cm while the service alone restored exactly `35 Health` and `20 Mana`.
Both states remained clear of the well roof, marker, protected routes, chat,
and HUD.

`Embermere.UI.RestWorldPresentation` brought the authoritative suite to
`76/76`. The no-hot-reload build, focused validator, fresh 18-package aggregate,
full-zone validator, and initialized-world well, notice-board, workshop,
cottage, stall, and road traces all passed. Save version `2` and the 53 Fab plus
24 original-art baseline remained unchanged.
