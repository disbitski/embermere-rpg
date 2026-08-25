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

## Fenwatch Offerings

`DA_FenwatchArmsmasterOfferings` contains two compact repeatable XP lessons:

| ID | Name | Required level | Cost | Reward | Repeatable |
| --- | --- | ---: | ---: | ---: | --- |
| `CombatDrills` | Combat Drills | 1 | 10 copper | 25 XP | Yes |
| `AdvancedCombatDrills` | Advanced Combat Drills | 2 | 20 copper | 50 XP | Yes |

Both current offerings are intentionally repeatable and have no independent
persistent stock. Copper and XP already belong to the versioned save-game
contract, so successful training becomes durable without adding trainer-
specific save records. Offering selection, panel state, and service interaction
remain transient.

Each offering owns an explicit repeatability policy. A future non-repeatable
lesson is enforced once per service runtime and reports a distinct completed
result; making that completion durable would require its own deliberate save-
format contract. No current Fenwatch offering creates that hidden persistent
state.

## Transaction Order

`Train()` performs complete preflight before changing live state:

1. Validate the complete offerings asset, including nonempty unique stable IDs,
   positive costs/rewards, and explicit repeatability.
2. Resolve the authored offering by index.
3. Reject a completed non-repeatable lesson before any mutation.
4. Confirm the authoritative Stats-derived level meets the requirement.
5. Confirm the wallet can spend the exact copper cost.
6. Confirm the stats component can accept the XP without integer overflow.
7. Spend copper.
8. Commit XP.
9. If the progression commit unexpectedly fails after the spend, refund the
   exact copper amount before reporting rejection.
10. Record non-repeatable runtime completion only after both owner commits.

Malformed data, low level, insufficient funds, missing owners, and XP overflow
must leave both wallet and progression unchanged. The panel disables an
ineligible action, but the service still rejects it authoritatively if called
through another path.

## Player Surface

The first native panel is fixed at `500x300` and contains:

- the data-driven trainer name and current purse;
- two selectable offering rows with stable fixed bounds;
- level, cost, XP reward, repeatability, and description details;
- a Train command and a fixed two-line current preflight/status cell, with
  exact transaction outcomes retained in bottom-left chat;
- bracket-key selection, `T` action, and `X` close guidance;
- cursor-aware game/UI input without covering chat or the hotbar.

Unavailable rows remain visible and inspectable by mouse or bracket-key
selection. They use a restrained locked treatment while the Train command is
disabled. At level 1, Advanced Combat Drills must expose the exact service-owned
copy `Advanced Combat Drills requires level 2.` and mutate neither copper nor
XP. The HUD never calculates a level threshold or silently selects another
lesson.

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
- `Embermere.Trainer.LevelGatedProgression`;
- `Embermere.Trainer.LevelGatedPersistence`;
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

The 2026-08-25 level-gated extension passed the no-hot-reload build, all 71
automation tests, the exact trainer package validator, the standalone
progression validator, and the sequential 15-package aggregate. Clean PIE kept
Advanced Combat Drills visible and locked at level 1 with exact rejection and
zero mutation. After Stats derived level 2, the same open panel changed it to
ready with no stale lock sentence. A real HUD request then changed `50` copper /
`100` XP to exactly `30` / `150`, posted exact chat, and remained level 2.

## Training-Yard Presentation

`Embermere_FenwatchPracticeDummy_TrainingYard_01` is a separate world-art
actor at `(-1120, -1120, 0)`, yaw `45`. It reuses five project-owned Fenwatch
materials, grounds a `252.0 x 100.879 x 245.0` cm target silhouette, and owns
only two support/core collision boxes. Its sparring arms remain clear. The
dummy has no trainer component, offering data, interaction marker, wallet/XP
access, panel ownership, or persistence state; removing it cannot change the
trainer loop.

## Practice-Target Gameplay

`Embermere_FenwatchPracticeTarget_Gameplay_01` is a separate native gameplay
actor colocated with the visible dummy. It supplies the `Fenwatch Practice
Target` health pool, normal `Tab` eligibility, ability damage, native
nameplate, and 48-segment cyan target circle without supplying a mesh or
owning either of the dummy's colliders.

The target is frozen at the authored transform with zero gravity, zero
velocity, and `MOVE_None`. It has no navigation, AI, aggro, retaliation,
leash, loot, XP, quest credit, trainer offering, interaction, wallet, or save
authority. Lethal damage clears selection, then a three-second reset restores
`150/150` for another round. Removing this actor leaves the training-yard art
and Combat Drills service unchanged; replacing the dummy mesh leaves combat
rules unchanged.

The complete ownership, reset, validation, and defeat-credit rules live in
[PRACTICE_TARGET_CONTRACT.md](PRACTICE_TARGET_CONTRACT.md).

## Derived-Level Integration

The Trainer remains an XP transaction owner, not a level calculator. On
2026-08-23 clean PIE, one real Combat Drills transaction changed `40` copper /
`0` XP to `30` / `25` while level correctly remained `1`. Mara's separate
quest authority then granted `125` more XP; Stats and rules derived Human
Warrior level `2` at `150` total XP with the exact growth profile.

The service still checks only its authored offering requirements and delegates
the XP mutation to Stats. Thresholds, race/class growth, equipment
reapplication, Chronicle level display, save restore, and level-up feedback
remain outside Trainer authority. The progression boundary is documented in
[LEVEL_PROGRESSION_CONTRACT.md](LEVEL_PROGRESSION_CONTRACT.md).

Advanced Combat Drills extends that same boundary. It is visible but locked at
level 1, becomes available when Stats reports derived level 2, charges exactly
20 copper, and grants exactly 50 XP. Repeated use follows its authored
repeatability flag. Save version 2 needs no new field because Chronicle already
captures the resulting wallet and XP owners; load restores those values and
derives level silently without replaying a lesson.

## Working Rule

Trainer and training-yard art may change, animate, or disappear without
changing progression or practice-target rules.
Offerings may change without rebuilding the NPC. Any future skill unlock,
class requirement, finite lesson, or respec feature must extend the service
and data contract first, then earn its own persistence and rollback rules.
