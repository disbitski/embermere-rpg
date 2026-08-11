# Trainer Service Contract

Embermere trainers use the same separation already proven by the Fenwatch
quartermaster: art may communicate identity, but it does not own gameplay.

## Ownership

`AEmbermereNpcPresentationActor` owns only swappable presentation:

- soft static or skeletal art references;
- the shared authored visual transform;
- optional Anim Blueprint or compatible single-node Idle presentation;
- deterministic lane fallback;
- no collision or navigation contribution.

`AEmbermereTrainerServiceActor` owns the trainer interaction boundary:

- `UEmbermereInteractableComponent` for range, marker, name, and `F` use;
- `UEmbermereTrainerComponent` for offering lookup and transaction authority;
- no static mesh, skeletal mesh, collision, or visual identity.

`UEmbermereTrainerOfferingsData` owns authored offerings. The player wallet and
stats components remain the authorities for copper and progression. The HUD
only displays those owners and requests a transaction.

## First Fenwatch Offering

`DA_FenwatchArmsmasterOfferings` currently contains one bounded starter action:

| ID | Name | Required level | Cost | Reward |
| --- | --- | ---: | ---: | ---: |
| `CombatDrills` | Combat Drills | 1 | 10 copper | 25 XP |

The offering is intentionally repeatable and has no independent persistent
stock. Copper and XP already belong to the versioned save-game contract, so a
successful training action becomes durable without adding trainer-specific
save records.

## Transaction Order

`Train()` performs complete preflight before changing live state:

1. Resolve the authored offering by stable index and verify its ID, cost,
   required level, and positive reward.
2. Confirm the character meets the level requirement.
3. Confirm the wallet can spend the exact copper cost.
4. Confirm the stats component can accept the XP without integer overflow.
5. Spend copper.
6. Commit XP.
7. If the progression commit unexpectedly fails after the spend, refund the
   exact copper amount before reporting rejection.

Malformed data, low level, insufficient funds, missing owners, and XP overflow
must leave both wallet and progression unchanged. The panel disables an
ineligible action, but the service still rejects it authoritatively if called
through another path.

## Player Surface

The first native panel is fixed at `500x300` and contains:

- the data-driven trainer name and current purse;
- one selectable offering row;
- level, cost, XP reward, and description details;
- a Train command and a fixed two-line result cell;
- bracket-key selection, `T` action, and `X` close guidance;
- cursor-aware game/UI input without covering chat or the hotbar.

Trainer, Vendor, Inventory, and Chronicle panels are mutually exclusive.
Opening one closes the active peer before showing the new surface. Closing the
last interactive panel restores the classic game-only mouse mode.

## Fenwatch Armsmaster Presentation

`Embermere_FenwatchArmsmaster_Trainer_01` is the art-only partner for
`Embermere_FenwatchArmsmaster_Service_01`. Both are saved at
`(-1320, -920, 0)`, yaw `100`, but they remain separate actors.

The deterministic rigged Blender source produces:

- `154.5 x 87.0 x 228.0` cm grounded bounds;
- `2,824` source triangles;
- six project-owned stone, moss, timber, iron, ember, and skin materials;
- one UV channel, zero non-manifold edges, unit scale, and a ground pivot;
- nine authored bones with rigid one-bone weights and a 3.2-second Idle.

Classic skeletal `FbxFactory` import adds one Armature root, so Unreal's saved
reference skeleton contains ten bones while retaining all nine authored names.
The rigged mesh preserves the exact grounded source bounds and six material
assignments; the exact 3.2-second Idle runs through the wrapper's single-node
lane. The original `2,800`-triangle imported static mesh remains a reversible
fallback. The presentation remains `NoCollision`; the separate service marker
is the only trainer interaction surface.

## Acceptance Gates

The trainer slice is eligible only when all layers pass:

- Blender metrics and preview inspection;
- classic FBX import and explicit package saves;
- exact material, bounds, triangle, transform, tag, and ownership validation;
- `Embermere.Trainer.TransactionRules`;
- `Embermere.Trainer.ServiceContract`;
- `Embermere.Trainer.FenwatchOfferingsData`;
- `Embermere.NPC.FenwatchArmsmasterPresentation`;
- `Embermere.NPC.FenwatchArmsmasterIdlePresentation`;
- `Embermere.UI.TrainerPanel`;
- full Embermere regression automation;
- fresh saved-map and initialized-world route validation;
- clean PIE interaction, transaction, rejection, handoff, and visual review.

The 2026-08-09 acceptance passed all 48 tests. Live PIE opened Fenwatch
Training at 40 copper and 0 XP, completed Combat Drills at 30 copper and 25 XP,
exhausted the purse without an illegal mutation, displayed the insufficient-
funds result, handed off cleanly to Inventory and Chronicle, and kept the
armsmaster readable without blocking the village route.

The 2026-08-10 persistence acceptance then saved that trainer-produced `30`
copper/`25` XP state through Chronicle, stopped PIE, proved a second world began
at `40`/`0`, and loaded the slot twice. Both loads restored exactly `30`/`25`
without duplicate XP, currency drift, reward replay, item/equipment/quest/
vendor-stock mutation, buyback persistence, or a save-version change.

The 2026-08-11 art upgrade then moved only the presentation wrapper onto its
project-owned skeletal mesh and 3.2-second Idle. All 49 tests passed, fresh
rig/trainer/zone validators accepted the saved packages, and live PIE measured
the Idle advancing from `0.193888` to `1.670905` seconds while remaining
`playing=true` and `NoCollision`. The trainer service, offering, marker, wallet,
XP transaction, panel, and save schema did not change.

## Training-Yard Presentation

`Embermere_FenwatchPracticeDummy_TrainingYard_01` is a separate world-art
actor at `(-1120, -1120, 0)`, yaw `45`. It reuses five project-owned Fenwatch
materials, grounds a `252.0 x 100.879 x 245.0` cm target silhouette, and owns
only two support/core collision boxes. Its sparring arms remain clear. The
dummy has no trainer component, offering data, interaction marker, wallet/XP
access, panel ownership, or persistence state; removing it cannot change the
trainer loop.

## Working Rule

Trainer art may change, animate, or disappear without changing progression.
Offerings may change without rebuilding the NPC. Any future skill unlock,
class requirement, finite lesson, or respec feature must extend the service
and data contract first, then earn its own persistence and rollback rules.
