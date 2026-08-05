# NPC Presentation Contract

Embermere keeps an NPC's visual representation separate from the gameplay
system that gives the NPC a purpose. A quest giver, vendor, trainer, or ambient
resident may change meshes, animation technology, or art style without moving
dialogue, inventory, training, quest, or interaction authority into art.

## Native Wrapper

`AEmbermereNpcPresentationActor` is the project-owned presentation wrapper for
standalone NPC art. It owns:

- one `UStaticMeshComponent` for the current static lane;
- one `USkeletalMeshComponent` for a future rigged lane;
- soft references to the static mesh, skeletal mesh, and animation class;
- one shared authored relative transform used by either visual lane;
- an explicit preference flag and resolved visual mode;
- a permanent presentation-only collision contract.

Only the resolved lane is visible. Switching from static to skeletal art does
not change the actor transform or require service logic to know which component
is active. Missing preferred art falls back to the available lane.

The wrapper deliberately does not own:

- interaction prompts or input;
- vendor stock, prices, currency, or transactions;
- trainer offerings or progression rules;
- dialogue, quests, rewards, faction, or reputation;
- navigation blockers or gameplay collision.

Those responsibilities belong to separate gameplay actors or components. A
service actor may reference or accompany a presentation actor, but the art is
never the service authority.

## First Use

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
   shared transform, and absence of interaction authority.
5. Initialized-world route traces where nearby solid props matter.
6. Clean PIE grounding, silhouette, marker/name clearance, service-area
   composition, and normal player traversal.

The current focused tests are:

- `Embermere.NPC.PresentationContract`
- `Embermere.NPC.FenwatchQuartermasterPresentation`
- `Embermere.NPC.FenwatchKeeperPresentation`
- `Embermere.Vendor.ServiceContract`
- `Embermere.Vendor.FenwatchStockData`

## Next Step

Prove the static-to-skeletal idle upgrade through the wrapper without changing
the accepted service actor, stock data, interaction, or saved world transform.
The service now supports earned currency, rollback-safe selling, and buyback
without moving any economy rule into NPC art. The next service expansion should
preserve that boundary while adding save-game persistence or a separate trainer
service.
