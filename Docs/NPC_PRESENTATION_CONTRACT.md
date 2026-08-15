# NPC Presentation Contract

Embermere keeps an NPC's visual representation separate from the gameplay
system that gives the NPC a purpose. A quest giver, vendor, trainer, or ambient
resident may change meshes, animation technology, or art style without moving
dialogue, inventory, training, quest, or interaction authority into art.

## Native Wrapper

`AEmbermereNpcPresentationActor` is the project-owned presentation wrapper for
standalone NPC art. It owns:

- one `UStaticMeshComponent` for the current static lane;
- one `USkeletalMeshComponent` for the rigged lane;
- soft references to the static mesh, skeletal mesh, animation class, and
  lightweight Idle animation;
- explicit Idle looping and play-rate settings;
- one shared authored relative transform used by either visual lane;
- an explicit preference flag and resolved visual mode;
- a permanent presentation-only collision contract.

Only the resolved lane is visible. Switching from static to skeletal art does
not change the actor transform or require service logic to know which component
is active. Missing preferred art falls back to the available lane.

Animation resolution is also explicit. A compatible Anim Blueprint class wins
when assigned. Otherwise, a skeleton-compatible `IdleAnimation` uses Unreal's
single-node lane with the authored loop and play rate. An incompatible or
missing animation leaves the skeletal presentation unanimated instead of
quietly binding the wrong skeleton.

`OverrideAnimationData` stores the construction-safe single-node contract. A
registered component is then reinitialized with `InitAnim(true)` so the same
data creates a live `UAnimSingleNodeInstance` after a runtime art swap. This
extra step matters because clearing an Anim Blueprint class destroys the
transient instance without changing `AnimationSingleNode` mode; another mode
assignment alone therefore does not recreate playback.

The wrapper deliberately does not own:

- interaction prompts or input;
- vendor stock, prices, currency, or transactions;
- trainer offerings or progression rules;
- dialogue, quests, rewards, faction, or reputation;
- navigation blockers or gameplay collision.

Those responsibilities belong to separate gameplay actors or components. A
service actor may reference or accompany a presentation actor, but the art is
never the service authority.

## Optional Context Observer

The wrapper may opt into a read-only contextual greeting when a nearby NPC
benefits from a little more world presence. This observer requires an explicit
authority actor reference, resolves state from that actor's existing data and
the player's authoritative quest log, and subscribes only to quest-state
changes. It does not infer ownership from proximity or duplicate quest rules.

The fixed, hit-test-invisible greeting widget is presentation-only,
`NoCollision`, hidden outside its configured radius, and removable without
changing interaction results. The observer never invokes interaction, accepts
or completes a quest, advances an objective, grants rewards, or owns marker,
dialogue, or input behavior.

## First Uses

`Embermere_FenwatchQuartermaster_Vendor_01` is the first saved wrapper
instance. Its preferred lane now uses
`SK_EmbermereFenwatchQuartermaster_01` and
`A_EmbermereFenwatchQuartermaster_Idle` at `(-1530, -1190, 0)`, yaw `100`, unit
scale. Blender retains the reviewed 3,632-triangle merchant silhouette,
`120.842 x 93.0 x 217.0` cm grounded bounds, and six project-owned materials,
then adds nine named bones with complete rigid one-bone weights and an exact
4.0-second Idle. Classic FBX adds one imported Armature root, giving Unreal an
accepted ten-bone reference skeleton. The wrapper remains `NoCollision` and
retains `SM_EmbermereFenwatchQuartermaster_01` as its reversible static
fallback.

The co-located `Embermere_FenwatchQuartermaster_Service_01` now owns the
interaction marker and vendor component. It has no art or collision and reads
`DA_FenwatchQuartermasterStock` for stock and prices. The presentation actor
still has no interaction or vendor component. The full boundary and
transaction model are documented in
[VENDOR_SERVICE_CONTRACT.md](VENDOR_SERVICE_CONTRACT.md).

`Embermere_FenwatchArmsmaster_Trainer_01` is the second saved wrapper
instance. Its preferred lane now uses
`SK_EmbermereFenwatchArmsmaster_01` and
`A_EmbermereFenwatchArmsmaster_Idle` at `(-1320, -920, 0)`, yaw `100`, unit
scale. Blender authors `2,824` triangles, six project-owned materials, nine
named bones with rigid one-bone weights, and a 3.2-second Idle. Classic FBX
adds one imported Armature root, so Unreal's accepted reference skeleton has
ten bones while retaining all nine authored names and the authored `root`
beneath that imported root. The mesh retains `154.5 x 87.0 x 228.0` cm bounds,
grounded feet, and no collision.

`SM_EmbermereFenwatchArmsmaster_01` remains the exact reversible static
fallback. Activating the skeletal lane clears the inactive static component's
render mesh but does not erase the wrapper's soft fallback reference. This
distinction lets the current art switch lanes without changing its world
transform or requiring the service actor to understand presentation state.

The separate `Embermere_FenwatchArmsmaster_Service_01` owns the interaction
marker and trainer component. It reads `DA_FenwatchArmsmasterOfferings` and
contains no art or collision. The full progression boundary is documented in
[TRAINER_SERVICE_CONTRACT.md](TRAINER_SERVICE_CONTRACT.md).

Mara is the third production wrapper use, but her migration has a different
ownership shape. The original `BP_QuestGiver` remains at
`(-2050, -850, 140)`, yaw `35`, because its interactable component owns her
display name, dialogue, quest data, marker, and reward flow. Its old SCS static
component is retained as a dormant reversible template with the accepted local
`(0, 0, -140)` offset, yaw `100`, unit scale, and `NoCollision`, but its render
mesh is deliberately cleared.

The colocated `Embermere_FenwatchKeeper_Mara_Presentation_01` wrapper uses
`SK_EmbermereFenwatchKeeper_Mara_01` and
`A_EmbermereFenwatchKeeper_Mara_Idle` at `(-2050, -850, 0)`, yaw `135`. The
deterministic source retains the reviewed 3,280-triangle,
`107.45 x 71.0 x 207.5` cm, six-material keeper silhouette, adds nine authored
bones with complete rigid one-bone weights, and supplies an exact 3.6-second
Idle. Classic FBX adds one imported Armature root, giving Unreal ten reference
bones. The wrapper remains `NoCollision`, owns no interactable or quest
component, and retains `SM_EmbermereFenwatchKeeper_Mara_01` as its reversible
static fallback. Its optional contextual greeting observes the original
`BP_QuestGiver` and the player's quest log within 420 cm. The greeting reacts
to available, active, ready-to-turn-in, and completed states without taking
over Mara's original `F` interaction or any quest authority.

## Acceptance Gates

Every wrapper-based NPC presentation should prove:

1. Deterministic source metrics, ground pivot, applied scale, materials, and
   topology in Blender.
2. Explicit classic-FBX import and saved Unreal mesh/material packages.
3. Exact saved actor class, art reference, transform, preference, tag, and
   `NoCollision` state in a fresh process.
4. Native automation for static resolution, skeletal resolution, fallback,
   shared transform, animation precedence, exact Idle asset/loop/play rate,
   and absence of interaction authority.
5. Initialized-world route traces where nearby solid props matter.
6. Clean PIE grounding, silhouette, marker/name clearance, service-area
   composition, and normal player traversal. A single-node lane must also
   report `playing`, retain its authored rate, and advance between two measured
   positions; serialized animation data alone is insufficient acceptance.

The current focused tests are:

- `Embermere.NPC.PresentationContract`
- `Embermere.NPC.SkeletalIdlePresentation`
- `Embermere.NPC.FenwatchQuartermasterPresentation`
- `Embermere.NPC.FenwatchQuartermasterIdlePresentation`
- `Embermere.NPC.FenwatchArmsmasterPresentation`
- `Embermere.NPC.FenwatchArmsmasterIdlePresentation`
- `Embermere.NPC.FenwatchKeeperPresentation`
- `Embermere.NPC.FenwatchKeeperIdlePresentation`
- `Embermere.NPC.ContextGreetingPresentation`
- `Embermere.Vendor.ServiceContract`
- `Embermere.Vendor.FenwatchStockData`
- `Embermere.Trainer.ServiceContract`
- `Embermere.Trainer.FenwatchOfferingsData`

## Production Skeletal Lane

The 2026-08-11 armsmaster pass established the first persisted production NPC
on the skeletal lane; the 2026-08-12 quartermaster pass proved that the same
contract scales to a second service character. The 2026-08-13 keeper pass then
proved that a Blueprint-backed quest NPC can adopt the wrapper without moving
or duplicating her existing gameplay authority. Fresh-process validation locks
each exact skeletal mesh, Skeleton, Idle, material order, bounds, wrapper
transform, loop/rate, static fallback, and separate gameplay owner. Native
automation also rejects collision or gameplay authority on every presentation
actor.

Clean PIE supplied the runtime gate that package inspection cannot. The
armsmaster advanced from `0.193888` to `1.670905` seconds; the quartermaster
advanced from `0.853735` to `2.195707` seconds; and the fresh-module keeper
advanced from `0.333814` to `1.525603` seconds. All remained `playing=true`,
grounded, readable, and `NoCollision`, while initialized-world traces retained
the training-yard and village-service routes. The keeper additionally retained
Mara's unobstructed marker and name. On 2026-08-14 a physical `F` press accepted
the original Blueprint-owned quest, real Prowler combat advanced it to `3/3`,
and the same original interactable completed the return while the rigged
wrapper remained presentation-only.

On 2026-08-15, clean normal-route PIE accepted Mara's optional contextual
greeting in all four quest states. It stayed hidden outside 420 cm, showed the
authored available copy before interaction, changed to active and ready copy
as the authoritative quest log changed, and settled on completed copy after
the original physical `F` interaction granted exactly 125 XP, 20 copper, and
one Recruit Pack. A second `F` press granted nothing, proving the observer did
not replay rewards or create a parallel dialogue path.

Future vendor, trainer, quest, or ambient-rig upgrades should reuse this exact
contract. Any class restriction, skill unlock, finite lesson, stock rule, quest
state, or persistence field still belongs in its data/service owner, never in
the wrapper.
