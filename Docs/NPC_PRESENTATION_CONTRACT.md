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

## First Uses

`Embermere_FenwatchQuartermaster_Vendor_01` is the first saved wrapper
instance. It uses the static lane and
`SM_EmbermereFenwatchQuartermaster_01` at `(-1530, -1190, 0)`, yaw `100`, unit
scale. The 3,632-triangle mesh measures `120.842 x 93.0 x 217.0` cm, uses six
project-owned materials, has no collision, and reads as a quartermaster beside
the existing supply chest.

The co-located `Embermere_FenwatchQuartermaster_Service_01` now owns the
interaction marker and vendor component. It has no art or collision and reads
`DA_FenwatchQuartermasterStock` for stock and prices. The presentation actor
still has no interaction or vendor component. The full boundary and
transaction model are documented in
[VENDOR_SERVICE_CONTRACT.md](VENDOR_SERVICE_CONTRACT.md).

`Embermere_FenwatchArmsmaster_Trainer_01` is the second saved wrapper
instance. It uses `SM_EmbermereFenwatchArmsmaster_01` at
`(-1320, -920, 0)`, yaw `100`, unit scale. Blender authors `2,824` triangles;
classic FBX import removes 24 degenerate triangles and saves a `2,800`-triangle
Unreal mesh with `154.5 x 87.0 x 228.0` cm bounds, six project-owned
materials, grounded feet, and no collision.

The separate `Embermere_FenwatchArmsmaster_Service_01` owns the interaction
marker and trainer component. It reads `DA_FenwatchArmsmasterOfferings` and
contains no art or collision. The full progression boundary is documented in
[TRAINER_SERVICE_CONTRACT.md](TRAINER_SERVICE_CONTRACT.md).

Mara remains a Blueprint-backed quest actor because her existing component owns
real gameplay. Her Fenwatch keeper visual follows the same ownership rule even
though it is reconciled directly into `BP_QuestGiver` rather than placed in the
standalone wrapper.

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
- `Embermere.NPC.FenwatchArmsmasterPresentation`
- `Embermere.NPC.FenwatchKeeperPresentation`
- `Embermere.Vendor.ServiceContract`
- `Embermere.Vendor.FenwatchStockData`
- `Embermere.Trainer.ServiceContract`
- `Embermere.Trainer.FenwatchOfferingsData`

## Next Step

The static-to-skeletal Idle lane is now proven with the real project-owned
Marsh Prowler rig and animation. Fresh automation validates the exact asset,
loop, rate, transform, collision, and ownership contract; a PIE-only swap
proved playback advancing from `0.0` to `1.4153` seconds at `0.75x` without
altering the saved quartermaster.

Keep the accepted quartermaster and armsmaster static until matching rigged art
exists. The next bounded use of this boundary can give one of them a dedicated
project-owned skeleton and Idle clip, but the service actors must remain
unchanged. Any future trainer class restrictions, skill unlocks, or finite
lesson state belong in the data/service/persistence lanes, never the wrapper.
