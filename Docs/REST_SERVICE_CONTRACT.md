# Communal Well Rest Service Contract

## Purpose

The Fenwatch communal well closes a small but useful loop: return from combat,
stand still at a safe village landmark, and recover missing health and mana.
The interaction is deliberately modest. It does not replace consumables,
respawn, class abilities, or future inn and camp systems.

The static well remains removable presentation art. A separate native service
actor owns every gameplay decision, and a third removable observer owns only
transient channel/completion geometry. The observer contract is documented in
[REST_PRESENTATION_CONTRACT.md](REST_PRESENTATION_CONTRACT.md).

## Authority Boundary

`AEmbermereRestServiceActor` is an art-free interaction wrapper containing:

- `UEmbermereInteractableComponent` for the existing nearest-`F` interaction
  lane, world marker, and fixed native dialogue prompt;
- `UEmbermereRestServiceComponent` for request validation, channel state,
  interruption, cooldown, atomic recovery, and exact outcomes;
- `UEmbermereRestServiceData` for stable identity, player-facing copy, range,
  timing, interruption distance, and the full-health/full-mana rule.

`UEmbermereStatsComponent` remains the only health and mana owner. Its combined
full-vitals operation validates the complete candidate first, then commits both
resources together and broadcasts the normal health and mana changes.

The saved `StaticMeshActor` well owns no interaction, rest component, health,
mana, combat, quest, reward, vendor, trainer, or persistence behavior. Replacing
or deleting its mesh cannot change recovery results.

## Request Lifecycle

An eligible request begins a `1.5` second channel. The character must remain
within `300` cm and within `35` cm of the starting point. During the channel,
the service revalidates the complete request rather than trusting the opening
frame.

The service rejects without mutation when:

- its data, actor, character, or vital state is malformed;
- the character is outside the service range;
- the character is dead;
- a live prototype enemy is engaged or a selected live enemy is inside its
  engagement range;
- both configured resources are already full;
- the successful-rest cooldown is active;
- another rest channel is already pending;
- movement interrupts the pending channel.

The stationary practice target is not active combat and does not block rest.
Only a successful completion begins the `12` second session cooldown. The
cooldown, pending request, prompt state, and last result are transient and do
not enter save version `2`.

## Transaction Rule

Preflight validates every owner before mutation. Completion repeats the same
checks, then asks Stats to restore configured resources in one operation.

If health or mana is malformed, the character dies, combat begins, range or
movement changes, or the resources become full before completion, neither
resource changes. A duplicate request never restarts the channel. Teardown
clears pending work and cooldown without committing recovery.

## Presentation

Presentation observes `FEmbermereRestOutcome` only after the service resolves a
request. It may show:

- the existing fixed dialogue panel for the well prompt;
- exact bottom-left chat for starting, success, interruption, cooldown, range,
  death, combat, full-resource, and malformed-data outcomes;
- the standard non-colliding world marker above the well roof;
- the separate twelve-segment cyan shaft channel and `1.1`-second mint
  committed-recovery bloom defined by the rest-presentation contract.

Presentation owns no eligibility, timing, health, mana, combat, or save state.
No dedicated modal is required for this bounded first slice.

## Saved Integration

- Data: `/Game/Data/Services/DA_FenwatchCommunalWellRest`
- Service: `Embermere_FenwatchCommunalWell_RestService_01`
- Observer: `Embermere_FenwatchCommunalWell_RestPresentation_01`
- Transform: `(-950, -1600, 0)`, yaw `-135`
- Art: `Embermere_FenwatchCommunalWell_SouthCommons_01`

The gameplay service is colocated with the well but carries no
`EmbermereOriginalArt` tag, mesh, collision, or navigation contribution. The
map remains at 53 Fab actors plus 24 original-art placements.

## Verification

Focused automation must cover:

- saved data and service/art ownership separation;
- exact health-plus-mana and one-resource recovery;
- full-resource, range, dead, combat, cooldown, duplicate, malformed-data, and
  movement-interruption rejection;
- practice-target exclusion;
- no partial mutation, exact restored amounts, cooldown start only on success,
  and teardown cleanup.

Fresh package validation locks data, transform, tags, components, copy, and art
separation. Clean PIE must prove real `F` interaction, fixed prompt/chat,
stay-still success, movement interruption, cooldown rejection, full-resource
rejection, and preserved well collision, composition, and village routes.

The accepted 2026-08-28 service pass used the real `F` path to restore exactly
`30 Health` and `20 Mana`, then rejected a full-resource repeat. Automation
also starts an eligible channel before engaging a live Prowler and proves the
new combat state cancels before mutation. The 2026-08-29 presentation pass then
visually accepted the real cyan shaft channel and mint committed bloom while
the service alone restored `35 Health` and `20 Mana`. All 76 tests, the fresh
18-package aggregate, focused validators, and initialized-world well traces
passed; save version `2` and the 53 Fab plus 24 original-art baseline remained
unchanged.
