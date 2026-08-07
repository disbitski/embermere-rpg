# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

For a fresh Codex task or context reset, read [Docs/THREAD_HANDOFF.md](Docs/THREAD_HANDOFF.md) before this daily file.

## Start Here

- Confirm Unreal is running the 2026-08-07 no-hot-reload Chronicle module plus
  the saved Fenwatch vendor stock/service, item, quest, and current map
  packages. Restart if the editor predates that build or test discovery exposes
  fewer than 42 Embermere tests.
  Start MCP with `-ModelContextProtocolStartServer
  -ModelContextProtocolPort=8123`; on macOS pass the full `.uproject` after
  `open ... --args`. Confirm Blender only when original-art work is selected.
- Discover and run all 42 tests, especially
  `Embermere.Persistence.RoundTrip`,
  `Embermere.Persistence.ValidationRollback`,
  `Embermere.Persistence.SlotInspection`,
  `Embermere.UI.SaveLoadPanel`,
  `Embermere.Economy.FenwatchRewardsAndValues`,
  `Embermere.Vendor.TransactionRules`,
  `Embermere.Vendor.SellBuybackTransactions`,
  `Embermere.Vendor.ServiceContract`,
  `Embermere.Vendor.FenwatchStockData`,
  `Embermere.UI.VendorPanel`, the three NPC presentation tests, Prowler/world
  presentation, player recovery, and inventory transaction suites. The
  authoritative 2026-08-07 commandlet passed 42/42; the no-hot-reload Mac
  build, saved-map, UI-art, Fenwatch-vendor, and initialized-world route
  validators also passed.
- Recheck the accepted Fenwatch vendor loop through normal `F` interaction:
  - presentation actor `Embermere_FenwatchQuartermaster_Vendor_01` remains
    art-only at `(-1530, -1190, 0)`, yaw `100`, unit scale and `NoCollision`;
  - co-located `Embermere_FenwatchQuartermaster_Service_01` owns interaction
    and vendor behavior but no mesh, collision, navigation, or art;
  - `DA_FenwatchQuartermasterStock` offers unlimited Marsh Tonic at `8` copper
    and one Recruit Pack at `30` copper;
  - a new player starts with `40` copper; buying a tonic produces `32`, selling
    that exact selected stack produces `35`, and buying it back for the same
    `3` copper produces `32` with the original item identity restored;
  - buying the one Recruit Pack then produces `2`, grants the item, leaves its
    stock at zero, and a second tonic request is rejected for insufficient
    funds without mutation;
  - Marsh Tonic and Recruit Pack sell values are data-driven at `3` and `12`
    copper; quest-category or zero-value items are unsellable;
  - completing Mara's quest grants `20` copper exactly once alongside `125` XP
    and the Recruit Pack reward, making currency part of the playable loop;
  - the fixed `500x325` native panel keeps title, purse, stock, icon/detail,
    two-line result, Buy, Sell selected, latest Buyback, and footer inside
    bounds, hides Inventory while open, posts chat feedback, and restores
    game-only input when closed;
  - full-bag, insufficient-funds, sold-out, not-owned, unsellable, and wallet-
    overflow rejections preserve copper, stock, buyback, and inventory without
    partial mutation.
- Treat [Docs/VENDOR_SERVICE_CONTRACT.md](Docs/VENDOR_SERVICE_CONTRACT.md) as
  the economy boundary. The NPC wrapper owns only art; the service owns
  interaction and stock; the wallet and inventory own player state; the HUD
  only presents and requests transactions.
- Treat [Docs/SAVE_GAME_CONTRACT.md](Docs/SAVE_GAME_CONTRACT.md) as the durable
  state boundary. Version 1 stores copper, XP, inventory stack identity and
  quantity, equipped-item identity and slot, quest state/progress, and finite
  vendor stock through stable IDs plus validated soft paths. Capture and load
  are preflighted before mutation; unknown assets, bad versions, invalid
  quantities, capacity conflicts, mismatched IDs/slots, and malformed vendor
  records reject the entire restore. Equipment stats are rebuilt once instead
  of double-applying bonuses, and quest rewards are not replayed.
- Retain the accepted live persistence proof. In one normal PIE session the
  vendor and quest loop saved exactly `22` copper, `125` XP, one Marsh Tonic,
  one bagged Recruit Pack, one equipped Back-slot Recruit Pack, completed Mara
  state, and exhausted finite Recruit Pack stock. A fresh PIE session restored
  that exact state through `EmbermereLoad`, and a second load remained
  idempotent with no duplication or stat inflation. `EmbermereSave` and
  `EmbermereLoad` use prototype slot `EmbermerePrototype`; buyback history,
  transient combat, cooldowns, temporary effects, and world position are
  intentionally session-only/reset state.
- Retain the accepted player-facing Chronicle. `M` opens a centered `460x260`
  one-slot panel while `Ctrl+M` still toggles mouse inversion. The panel
  inspects `EmbermerePrototype` without mutating it and displays the accepted
  `22 copper | 125 XP`, `2 bag stacks | 1 equipped | Quest complete` summary.
  Save requires explicit overwrite confirmation when the slot exists; Load
  always confirms replacement of the current session. Empty, unreadable,
  unsupported-version, missing-asset, and other rejected states surface the
  persistence result and keep Load disabled or leave live state unchanged.
  Inventory, Vendor, and Chronicle remain mutually exclusive, and closing the
  panel restores game-only input. `EmbermereSave` and `EmbermereLoad` remain
  console fallbacks; there is still no autosave, deletion, multiple profiles,
  or implicit migration.
- Retain the accepted EverQuest-inspired target circle. Clean normal-route PIE
  proved all three Marsh Prowlers own exactly 48 stationary, non-colliding
  cyan-blue segments at 16 cm effective surface clearance. The visible circle
  leaves the selected Prowler's footprint readable, switches immediately with
  `Tab`, and clears from the old target. Do not retune it without a concrete
  normal-camera readability issue.
- Retain finite-world recovery. A sustained transform-proven `Q` autorun probe
  exposed that the player could leave the prototype foundation and fall
  forever. Crossing `Z=-1000` now cancels autorun, forces death even during
  recovery immunity, freezes movement, and uses the existing five-second
  village respawn. Fresh PIE proved full health at `(-2400, -1200, 90.15)`,
  `MOVE_Walking`, zero velocity, and the three-second protection message.
- In clean PIE, accept the first original Marsh Prowler as a complete gameplay
  presentation:
  - peat, charcoal, moss, mud, bone, and amber materials read at normal camera
    distance;
  - paws contact terrain and the capsule, target ring, and nameplate remain
    readable;
  - Idle, Walk, Run, Attack, Hit, and Death route from generic enemy state;
  - Tab targeting, Strike, real retaliation through player death/recovery,
    target clear, Marsh Tonic loot, death hold, hide, and respawn complete
    without changing combat or quest rules;
  - all three saved map instances retain the skeletal mesh and animation set,
    not only the Blueprint CDO.
- Recheck the completed grounding pass:
  - 57 grounded upright `FabPass_` actors and 17 original-art placements;
  - all ordinary art that previously sat at `Z=20` now rests at `Z=0`;
  - the unsupported SoulCave arch/pillar accents and three enemy marker meshes
    remain removed;
  - the project-owned 38-expression moss/earth material keeps its broad
    variation and `300` cm worn-road half width;
  - four 1,012-triangle visual-only marsh-reed clusters use the terrain material
    on their low footprint and remain `NoCollision`;
  - foliage, reeds, rocks, enemies, target rings, and nameplates leave the road
    and combat pocket readable.
- Walk from PlayerStart through Mara, the road gate, and the three Prowler
  homes. The reproduced straight-line `Q` contact was
  `Embermere_SupplyChest_Vendor_01`; its accepted transform is now
  `(-1740, -1180, 0)`, yaw `108`. Clean PIE moved beyond the old stop point,
  while the saved-map validator enforces a `225` cm spawn-corridor clearance
  and the live trace validator proves the player-height corridor clear.
  Preserve independent `W` and `S` autorun cancellation, physical `Ctrl+M`
  feedback, three clear gate lanes, and solo `525` cm pulls.
- Inspect `Embermere_FenwatchShelter_Mara_01` at `(-1740, -700, 0)`, yaw
  `-64`. Its 4,348-triangle open-sided silhouette must sit behind Mara, reuse
  all five road-family materials, keep four grounded supports solid, and leave
  its center and overhead roof clear. The old stone backdrop, market cover,
  vendor cube, and trainer cube must remain absent. A clean PIE route already
  moved from `(-2400, -1200, 90.15)` to `(-1793.188, -831.382, 90.15)` under
  `Q`, then froze at that exact transform after `W` cancellation.
- Inspect Mara's project-owned `SM_EmbermereFenwatchKeeper_Mara_01`
  presentation. The static 3,280-triangle keeper is `107.45 x 71.0 x 207.5`
  cm, faces PlayerStart, plants its feet at ground level, and keeps the gold
  quest marker and name readable under the shelter. Its visual component is
  `NoCollision`; the existing quest actor and interactable component remain
  gameplay authorities. The saved Blueprint SCS template and placed map actor
  must both retain the mesh, local `(0, 0, -140)` offset, yaw `100`, unit scale,
  and no-collision state.
- Inspect `Embermere_FenwatchQuartermaster_Vendor_01` beside the accepted
  supply chest. Its 3,632-triangle, `120.842 x 93.0 x 217.0` cm static mesh
  uses six project-owned materials and should read as a stocky moss-capped
  merchant with apron, ledger, pouches, and keys. Retain `(-1530, -1190, 0)`,
  yaw `100`, unit scale, grounded feet, `NoCollision`, and clear Mara/chest/
  PlayerStart sightlines. The native `AEmbermereNpcPresentationActor` owns only
  interchangeable static/skeletal visuals and one shared transform; it must
  remain free of interaction, vendor, trainer, quest, and dialogue authority.
- Retain the accepted HUD, effects, inventory, and equipment baseline:
  fixed timed-status cells, four-class ability art and semantics, the
  `700x330` paper-doll inventory, Recruit Pack bag/Back transactions, Marsh
  Tonic use, tooltips/comparison, Sort, the `236x62` drag token, cooldowns,
  clipped chat, and atomic full-bag rollback.
- Retain the asset-agnostic world-status presentation: eight small non-colliding
  segments subscribe to the same successful-effect snapshots as the HUD.
  Harmful auras now size from the transformed creature footprint and trace the
  supporting surface instead of orbiting the actor root. On a Marsh Prowler the
  accepted inner aura resolves to a `94.352` cm radius at `21` cm above the
  support hit. Snare is marsh green and Frost Root is a lighter ice-white cyan
  (`0.46, 0.92, 1.0`), leaving both distinct from the calm outer 48-segment
  cyan target circle. Harmful effects take visual priority and all segments
  hide when no timed effect is active or the character is dead.
- The imported KiteDemo meshes still reference some absent internal vendor
  materials/textures in fresh commandlet logs even though project-owned
  component overrides keep the visible zone readable. Do not edit or commit raw
  vendor packages. Prefer replacing affected meshes with project-owned Blender
  art or a complete signed-in Stylized Classic pack.
- Highest-value next work:
  1. prove the NPC wrapper's skeletal/idle upgrade lane without moving quest,
     vendor, trainer, dialogue, or interaction authority into presentation;
  2. if that bounded contract is accepted, build a matching trainer
     presentation and separate trainer service;
  3. retain the accepted Chronicle slot inspection, confirmation, rejection,
     panel handoff, and two-session idempotence contracts; keep console commands
     as debug fallbacks and defer autosave, deletion, profiles, and migrations;
  4. retain the accepted keeper/quartermaster/service scale, marker clearance,
     chest composition, panel layout, and PlayerStart route;
  5. tune Prowler timing or add subtle `NoCollision` marsh dressing only when
     normal-route PIE exposes a concrete issue;
  6. consider authored Niagara, class-specific VFX, or audio only after the
     current asset-agnostic presentation has carried the playable slice farther.

## Full Manual Regression Checklist

- Restart Unreal before manual PIE when the editor predates the 2026-08-07
  Chronicle module and saved item/quest/stock packages. Current code passes
  all 42 tests.
- Verify the original Blender assets in clean-restart PIE:
  - find `Embermere_Waystone_Road_01` where the temporary road stump used to be;
  - approach it from the rune side and confirm scale, terrain contact, camera
    readability, and both-box collision without blocking the village route;
  - confirm its pale stone, moss accent, and ember rune remain readable in the
    current daylight alongside both matching ember lamps, the road signpost,
    and mixed Fab foliage;
  - inspect both ember lamps for scale, ground contact, warm crystal readability,
    two-box collision, and clear village/road traversal.
  - inspect the road signpost for readable directional silhouette, saved timber
    material, two-box base/post collision, and non-colliding overhead arms.
  - inspect the road gate for terrain contact, route framing, all five shared
    fantasy materials, a clear center opening, four support colliders, and a
    non-colliding overhead span.
  - inspect both boundary fences for terrain contact, all five shared materials,
    three colliders each, a solid crossed-rail body, and no intrusion into the
    road-gate opening;
  - inspect both rune-topped boundary stones for terrain contact, all five
    shared materials, two colliders each, readable ember crests, and clean
    termination of the fence silhouette after the south-side foliage cleanup.
  - inspect `Embermere_SupplyChest_Vendor_01` for its route-facing lock and
    plank silhouette, five shared materials, terrain contact, two solid authored
    body/lid boxes, accepted `(-1740, -1180, 0)` transform, and clear
    PlayerStart-to-Mara traversal.
  - inspect `Embermere_FenwatchShelter_Mara_01` behind Mara for all five shared
    materials, terrain contact, four solid support boxes, a clear center and
    roof span, readable quest marker/name, and an unobstructed spawn route.
  - inspect Mara's Fenwatch keeper for grounded feet, PlayerStart-facing pose,
    readable staff/satchel/moss-cowl silhouette, no collision, and an
    unobstructed gold quest marker and name.
  - inspect the Fenwatch quartermaster beside the supply chest for grounded
    feet, readable moss-cap/apron/ledger/pouch silhouette, no collision, a
    readable service marker, a clear route, and separation from Mara and her
    marker;
  - press `F` in range and run the vendor acceptance sequence from Start Here,
    including both successful buys, sold-out/insufficient-funds states, chat,
    item inventory changes, fixed panel copy, and close/input restoration.
- Confirm both first-class MCP servers after their host apps restart. Blender's
  bridge remains Safe Mode on, inline code off, localhost-only, and limited to
  approved project script roots.
- Confirm the local Fab/Epic folders are present but ignored by Git:
  - `Content/KiteDemo/`
  - `Content/SoulCave/`
  - `Content/ParagonProps/`
  - `Content/Scifi_desert_city/`
- Run these automation tests:
  - `Embermere.Combat.TargetSelectionPresentation`
  - `Embermere.Combat.DeadCasterRejected`
  - `Embermere.Combat.StarterAbilityEffects`
  - `Embermere.Enemy.LeashRules`
  - `Embermere.Enemy.LootRules`
  - `Embermere.Enemy.MarshProwlerPresentation`
  - `Embermere.Economy.FenwatchRewardsAndValues`
  - `Embermere.Equipment.SlotRules`
  - `Embermere.Equipment.InventoryTransactions`
  - `Embermere.Equipment.StatApplication`
  - `Embermere.Inventory.CapacityTransactions`
  - `Embermere.Inventory.ConsumableUse`
  - `Embermere.Inventory.IdentityActions`
  - `Embermere.Inventory.StableSorting`
  - `Embermere.Input.AutorunCancellation`
  - `Embermere.NPC.FenwatchKeeperPresentation`
  - `Embermere.NPC.FenwatchQuartermasterPresentation`
  - `Embermere.NPC.PresentationContract`
  - `Embermere.Persistence.RoundTrip`
  - `Embermere.Persistence.SlotInspection`
  - `Embermere.Persistence.ValidationRollback`
  - `Embermere.Player.OutOfBoundsRecovery`
  - `Embermere.Quests.CompletionRewards`
  - `Embermere.Rules.RaceClassMatrix`
  - `Embermere.Stats.DamageImmunity`
  - `Embermere.UI.AbilityIconPresentation`
  - `Embermere.UI.ChatLog`
  - `Embermere.UI.EnemyNameplateWidget`
  - `Embermere.UI.HotbarCooldownDisplay`
  - `Embermere.UI.IconPresentation`
  - `Embermere.UI.InventoryDragDrop`
  - `Embermere.UI.InventoryToggle`
  - `Embermere.UI.ItemComparison`
  - `Embermere.UI.PaperDollPresentation`
  - `Embermere.UI.SaveLoadPanel`
  - `Embermere.UI.TimedStatusPresentation`
  - `Embermere.UI.WorldStatusVfxPresentation`
  - `Embermere.UI.VendorPanel`
  - `Embermere.Vendor.FenwatchStockData`
  - `Embermere.Vendor.ServiceContract`
  - `Embermere.Vendor.SellBuybackTransactions`
  - `Embermere.Vendor.TransactionRules`
- Manually verify the styled first-pass HUD in PIE:
  - player HP, mana, XP, health bar, and mana bar are visible;
  - target panel appears after `Tab` and shows target HP plus range state;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - all hotbar slots show stable labels for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`;
  - Mara interaction shows the dialogue panel;
  - the structured inventory window starts empty and shows its title, `Slots 0 / 24`, clickable item list, selected-item detail pane, quest-reward hint, and keyboard/mouse footer;
  - the inventory starts in cursor-aware game/UI mode; pressing `I` hides it, hides the cursor, and restores classic game-only mouse input;
  - clicking a populated row or pressing `[` and `]` changes the inspected inventory stack after multiple stacks exist;
  - hovering a populated row shows quantity, category, slot/level, effects, comparison, and description without expanding the fixed window;
  - clicking Sort groups weapon, armor, consumable, quest, and misc stacks in that priority, sorts names inside each category, preserves the inspected item occurrence, posts chat feedback, and is unavailable during drag;
  - dragging an item shows the fixed project-owned fantasy token with a category sigil, warm item name, and slot/level or consumable-effect context without overlapping the inventory footer or hotbar;
  - equippable item details show net HP, mana, armor, and power changes against the currently equipped item, or against an empty destination slot;
  - Mara's Recruit Pack identifies as level-1 Back armor with `+5 HP, +1 Armor`; equipping it removes it from the bag, updates the gold Back-slot control, bonus totals, HP to `105/105`, and chat; clicking the occupied Back slot returns it to the bag and restores base stats;
  - a full bag refuses unequip/replacement without losing, duplicating, or partially moving either item;
  - the 700x330 inventory/equipment window keeps all three columns and its footer inside the shaded panel without overlapping the hotbar;
  - defeating a Marsh Prowler drops one stackable Marsh Tonic, posts a loot message, and adds it to inventory;
  - after taking damage, selecting Marsh Tonic and clicking `Use` restores up to 25 health and 10 mana and consumes one tonic, while full resources disable use and preserve inventory;
  - quest completion/reward shows the loot popup and the inventory panel lists the reward item;
  - combat, target, quest, XP, inventory, mouse, cooldown, and death/recovery messages appear clipped as single-line rows inside the bottom-left chat/combat log instead of overlapping the top-left player status panel or spilling beyond the chat panel border.
- Manually verify prototype persistence in two clean PIE sessions:
  - complete the exact vendor and Mara sequence from Start Here, equip one
    Recruit Pack in Back, open Chronicle with `M`, inspect the exact slot
    summary, cancel one overwrite request, then confirm Save Journey;
  - stop PIE, start a fresh PIE session, open Chronicle, confirm Load Journey,
    and confirm `22`
    copper, `125` XP, the exact bag/equipment identities and quantities,
    completed quest state, zero finite Recruit Pack stock, and correct
    equipment-derived stats;
  - load again and confirm no duplicate items, repeated quest
    payment, or doubled equipment bonuses;
  - confirm Chronicle, Inventory, and Vendor hand off visibility and cursor
    mode cleanly, and that `M`/`X` closes Chronicle back to game-only input;
  - confirm vendor buyback history is empty after load because version 1 keeps
    it session-only. Retain `EmbermereSave` and `EmbermereLoad` as debug
    fallbacks.
- Manually verify selected-target world readability in PIE:
  - `Tab` shows the selected enemy's UMG nameplate, selected marker, HP text,
    HP bar, and complete cyan-blue emissive circle around its footprint rather
    than buried inside the supporting surface or touching its paws;
  - the 48 overlapping segments remain restrained and stationary, follow a
    target switch immediately, and coexist with Snare/Frost Root world VFX;
  - the nameplate accent/HP bar changes toward red/orange as enemy HP falls;
  - switching targets clears the old enemy marker;
  - killing the selected enemy clears the target and marker.
- Manually verify the original Marsh Prowler presentation:
  - all three saved enemies use the project-owned skeletal mesh and retain the
    complete Idle, Walk, Run, Attack, Hit, and Death set;
  - peat/charcoal/moss/mud, bone, and amber materials read at gameplay distance;
  - paws contact terrain while the capsule, target ring, and nameplate remain
    clear;
  - targeting, Strike, retaliation, death, target clear, Marsh Tonic loot,
    hide, and respawn preserve the existing gameplay rules.
- Manually verify the live control fix in PIE:
  - `W`/`S` should cancel autorun.
  - `Ctrl+M` should toggle mouse Y inversion and show a temporary message.
  - Crossing below the prototype world at `Z=-1000` should cancel autorun,
    show `You have fallen`, freeze movement, and return the player to the
    village after five seconds at full health with zero residual velocity,
    walking restored, and three seconds of damage protection.
  - The fix now runs through the pawn/controller path known to receive live movement input, and the project builds successfully.
- Manually verify hotbar cooldown feedback in PIE:
  - Press `1` to use `Strike`.
  - Confirm the slot dims and displays its live cooldown countdown.
  - Press `1` again before cooldown expires and confirm the bottom-left chat/combat log shows a ready-time message.
  - Confirm non-empty failed hotbar activations show an `Unable to use ...` message instead of silently failing.
- Manually verify world-status VFX in PIE:
  - Battle Shout and Nature's Focus show eight restrained orange-gold segments
    around the player while their HUD status cell remains authoritative;
  - Ward uses the same fixed footprint with a blue-white palette;
  - Snare shows a grounded marsh-green inner aura on the affected Prowler and
    Frost Root uses a visibly lighter ice-white cyan;
  - both harmful auras resolve to the Prowler footprint (`94.352` cm in the
    accepted mesh/scale) and `21` cm above the support hit, staying inside the
    stationary outer cyan target circle without touching the paws;
  - harmful effects take presentation priority when both effect types overlap;
  - expiration, death, respawn, and status clearing hide the segments, while
    instantaneous Meditate creates no world aura;
  - the non-colliding presentation does not move the HUD, obscure the character,
    or become a gameplay authority.
- Manually verify the new Mara marker in PIE:
  - Mara should show a temporary gold `!` and name marker above her.
- Manually verify the first `FabPass_` environment layer in PIE:
  - the map has a blue atmospheric sky, readable ambient fill, and the
    38-expression moss/earth road material instead of the previous black
    sky/white or flat-green ground presentation;
  - all 57 remaining Fab art-pass actors are grounded and upright; all 15
    original placements remain present, including four terrain-blended
    `NoCollision` marsh-reed clusters;
  - the three oversized sci-fi building shells, old Mara backdrop/market cover,
    and vendor/trainer cubes are gone; PlayerStart has an unobstructed view
    toward Mara in front of the Fenwatch shelter;
  - the unsupported SoulCave accents and redundant enemy marker meshes remain removed;
  - village props leave Mara, PlayerStart, and the quest interaction route readable;
  - the road dressing points naturally from the village toward the wilderness pocket;
  - foliage and rocks frame the enemy pocket without hiding enemies, target rings, or nameplates;
  - the upgraded ruin reads as a landmark and does not trap the player;
  - any sci-fi-kit props that read too futuristic are noted for replacement.
- Manually verify starter enemy leash behavior in PIE:
  - pull a Marsh Prowler away from the wilderness pocket;
  - confirm it drops aggro and returns home instead of chasing indefinitely into the village;
  - tune `LeashRadius`, `ReturnHomeRadius`, and `ReturnHomeSpeedCmPerSecond` if it feels too abrupt or too permissive.
- Manually verify player respawn protection in PIE:
  - let the player die near the enemy pocket;
  - confirm the recovery message mentions the short protection window;
  - confirm immediate follow-up enemy damage is blocked during the protection window.
- Run the editor smoke test in [Docs/PLAYTESTING.md](Docs/PLAYTESTING.md).
- If Codex needs editor access, start MCP inside Unreal:

```text
ModelContextProtocol.StartServer 8123
```

## Current Prototype Status

Embermere has a working first-pass starter slice:

- player spawn in `L_Embermere_Prototype`;
- classic mouse camera behavior;
- `WASD`, `Q` autorun, tab target, and hotbar ability input;
- first quest giver, quest data, and reward item data;
- a separated Fenwatch vendor vertical slice with art-only quartermaster,
  art-free interactable service actor, data-driven stock/prices, player copper,
  atomic buy/rollback rules, finite/unlimited stock, native fixed vendor UI,
  inventory/chat feedback, and saved-package validation;
- a versioned, atomic prototype save/load contract for copper, XP, inventory
  and equipment identity, quest state, and finite vendor stock, with stable
  asset/service identifiers, malformed-record rollback, session-only buyback,
  explicit `EmbermereSave`/`EmbermereLoad` commands, and fresh-session PIE
  round-trip proof;
- hostile starter enemies that aggro, chase, attack, die, and respawn;
- starter enemy leash and return-home behavior for safer village/wilderness boundaries;
- player respawn protection for safer recovery during prototype combat;
- bottom-left chat/combat log feedback for targeting, combat, death, respawn, quest progress, XP, inventory, and rewards;
- temporary in-world interactable markers, including a gold quest marker for Mara;
- styled native HUD panels for player status, target, range state, quest progress, dialogue, loot, and hotbar labels;
- structured first-pass inventory window with capacity header, item rows, selected-item details, empty/reward states, `[`/`]` inspection cycling, and `I` show/hide toggle;
- a data-driven project-owned icon set with explicit Recruit Pack and Marsh
  Tonic art, ten equipment-slot symbols, sixteen starter-ability illustrations,
  category/slot/ability missing-art fallbacks, fixed UI dimensions,
  deterministic source generation, and durable Unreal texture packages;
- mouse-clickable inventory rows and cursor-aware game/UI input while the inventory is open;
- data-driven item categories, paper-doll equipment slots, level requirements, stat-bonus fields, and transactional Equip/Unequip actions;
- ten clickable equipment-slot controls, atomic bag/equipment transfers, rollback-safe replacement, idempotent health/mana/armor/power application, armor mitigation, and safe consumable depletion;
- item-row and occupied-slot tooltips plus net stat comparison against the current item or empty equipment slot;
- item-identity action routing plus typed bag/equipment drag payloads, valid and invalid drop feedback, bag-to-slot equip, and equipment-to-bag return on top of atomic transactions;
- explicit stable category/name inventory sorting that preserves selected item identity and duplicate-stack occurrence and refuses to reorder during a pending or active drag;
- data-driven Marsh Tonic consumables dropped by starter enemies so recovery is reachable through the normal combat loop;
- selected-target UMG nameplate widget plus a bounds-aware, surface-traced,
  non-colliding cyan-blue 48-segment emissive target circle with restrained
  pulse and deterministic clear/switch behavior;
- first-pass hotbar cooldown enforcement and ready-time feedback;
- live hotbar slot countdown and unavailable-state dimming;
- data-driven starter utility effects: timed Attack Power and Armor buffs,
  Snare and Frost Root movement control, Meditate mana recovery, effective-stat
  combat consumption, natural expiration, and respawn-safe effect clearing;
- asset-agnostic world-status VFX made from eight fixed non-colliding segments
  that read the same successful-effect snapshots as the HUD, with distinct
  Attack Power, Armor, Snare, and Frost Root palettes and no authority over
  gameplay rules;
- fixed hotbar ability illustrations with class-specific palettes, data-driven
  soft references, accessible stat/description tooltips, and missing-art
  fallback;
- first local Fab/Epic environment pass over the village, road, wilderness pocket, and ruin landmark;
- a project-owned Blender waystone, ember-lamp, road-signpost, road-gate,
  boundary-fence, boundary-stone, and supply-chest family with reviewed scripts,
  editable source, FBX, previews, authored collision, and ten validated saved
  placements, plus four visual-only marsh-reed placements;
- the first project-owned rigged enemy, `SK_EmbermereMarshProwler_01`, with
  deterministic Blender source, 7,464 triangles, 26 authored bones, five
  materials, six imported animations, asset-agnostic runtime routing, three
  saved enemy instances, and placeholder fallback;
- a project-owned fantasy inventory drag token with category sigils, contextual item text, and fixed bounds;
- first Mac-friendly sky, ambient fill, fog-readability correction, and a
  38-expression project-owned moss/earth/path material;
- a measured grounding pass that lowered ordinary art to the real ground
  surface, removed unsupported suspended accents and enemy marker geometry,
  restored readable foliage overrides, and added four terrain-blended,
  `NoCollision` marsh-reed clusters;
- automation coverage for the race/class matrix, quest completion rewards,
  selected-target presentation, dead-caster rejection, starter ability effects,
  timed-status and world-status VFX presentation, Prowler presentation, enemy
  leash and loot rules, equipment slot/stat/transaction rules, atomic inventory
  capacity, consumable use, item comparison/tooltips, identity-based inventory
  and drag/drop actions, autorun cancellation, damage immunity, enemy nameplate
  widget, chat log, hotbar cooldown display, item/slot/ability/paper-doll
  presentation, inventory toggle, buy/sell/buyback transactions, saved
  economy values, service ownership, saved stock, native vendor-panel behavior,
  and persistence round-trip/rollback rules, for 40 authoritative tests.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The environment is
upright, spawn-safe, and readable, while inventory/equipment now has clickable
and draggable gear, project-owned drag/icon/paper-doll presentation, stable
sorting, hover inspection, item comparison, and lossless transactional RPG
rules rather than display-only state. Starter combat feeds inventory through
Marsh Tonic drops, closing the first damage-loot-recovery loop. Clean PIE has
verified the full quest, a real bag-to-Back drag, the icon-bearing reward
popup, all four hotbar palettes, timed root/snare/buff/mana-recovery behavior,
identity-preserving Sort, solo Prowler pulls, collision-cleared encounter
markers, targeting, the original-art road family, transform-proven `W`/`S`
autorun cancellation, and the empty-state paper-doll composition. Data-driven
player and target status rows now make timed effects inspectable and passed
clean-PIE review alongside occupied paper-doll contrast. Embermere now has its
first original rigged creature and a grounded moss/earth road treatment, so the
art lane has moved beyond props without taking ownership of combat rules. The
first world-status aura now consumes those same data-driven snapshots: clean
PIE accepted the restrained orange-gold beneficial presentation plus grounded
marsh-green Snare and ice-white cyan Frost Root segments around the Prowler.
Selected targets now use a surface-traced cyan-blue circle sized from their
visual footprint instead of a fixed rotating gold marker; clean PIE accepted the
Prowler read under daylight.
The reproduced Mara-route contact is also resolved by moving the supply chest away from the straight
PlayerStart corridor and proving both geometric clearance and a live native
trace. The populated drag token in motion and physical `Ctrl+M` feedback remain
honest physical-eye checks. The world remains stylistically mixed without real
fantasy village buildings, player/race art, authored Niagara/class-specific
effects, or audio.

The first durable-state lane is now complete as a bounded prototype contract:
wallet, XP, inventory/equipment identity, quest state, and finite vendor stock
survive a fresh PIE session through validated atomic restore. Buyback and
combat-temporary state remain deliberately transient. The next persistence
work is player-facing lifecycle and corruption/version feedback, not adding
more serialized fields by accident.

## Next Work

- Extend the accepted persistence/economy loop without weakening its boundaries:
  - build a minimal native Save/Load panel or pause/settings surface over the
    proven `EmbermereSave`/`EmbermereLoad` lifecycle;
  - expose readable empty-slot, success, rejected-version, and corrupt/missing-
    asset feedback without partially mutating runtime state;
  - keep buyback history session-only and document any future schema migration
    before changing `EmbermereSaveGameVersion::Current`;
  - retain one-at-a-time vendor quantities, the fixed vendor panel,
    visual/service split, stable IDs, and atomic transaction/load invariants.
- Replace temporary selected-target text with better world readability:
  - retain the UMG nameplate widget, selected marker, HP bar, and HP-aware
    accent color;
  - retain the accepted complete cyan-blue target circle on all three Prowlers,
    then add rune/soft-edge texture art only if it improves rather than obscures
    the classic target read;
  - tune screen-space widget size/height against camera distance.
- Improve inventory presentation:
  - after restarting Unreal, visually verify all ten empty equipment-slot icons,
    Recruit Pack and Marsh Tonic icons in rows/detail/occupied slots, fixed icon
    dimensions, text fallback/tooltips, category fallback, and no layout shift;
  - verify the 700x330 bag/detail/equipment layout, all ten slot controls, Recruit Pack bag-to-Back transfer, gold occupied state, slot-click unequip, stat changes, and full-bag failure message in PIE;
  - manually verify mouse row/action clicks, cursor capture/release, and bracket-key item cycling once inventory has multiple stacks;
  - verify Marsh Prowler deaths grant Marsh Tonic, repeated drops stack, and `Use` heals damaged players without wasting a full-resource tonic;
  - visually verify item comparison lines and row/occupied-slot hover tooltips after a clean restart;
  - visually verify bag-to-equipment and equipment-to-bag drag/drop, valid/invalid feedback, stale identity rejection, and click/keyboard fallbacks;
  - visually verify stable identity-preserving sorting and the new fixed-size fantasy drag token across armor and consumable payloads;
  - retain item-art reuse in the fantasy drag token and reward popup, then
    visually approve the `128x160` illustrated paper-doll backdrop beneath
    empty and occupied slots without contrast or layout regressions.
- Finish starter-class combat identity:
  - retain all sixteen accepted ability icons, class palettes, data-driven
    tooltips, and cooldown dimming/countdown without layout shift;
  - play each timed effect through normal input and judge duration, feedback,
    damage cadence, and enemy recovery from the player's perspective;
  - visually accept the new fixed player/target status rows, live countdowns,
    hover descriptions, duplicate prevention, and clearing rules;
  - retain the asset-agnostic eight-segment world aura, including grounded
    footprint-aware harmful presentation, before deciding whether authored
    Niagara or class-specific effects are warranted.
- Clean up WIP HUD layout issues:
  - manually verify the 2026-07-04 chat clipping fix in PIE after a clean editor restart;
  - continue tuning chat panel height/line count against the hotbar and common desktop viewport sizes.
- Manually review the first Fab/Epic environment pass in PIE:
  - verify the atmospheric sky, ambient fill, and fog balance on the Mac viewport;
  - confirm the removal of the oversized sci-fi shells leaves Mara and PlayerStart readable;
  - tune the validated sky/ambient/moss-ground balance only if clean-restart PIE exposes a readability problem;
  - confirm the dressed road still leads clearly from spawn to Mara and the enemy pocket;
  - confirm the wilderness foliage/rocks do not hide starter enemies or target nameplates;
  - confirm the upgraded ruin reads better than the old greybox and does not trap the player;
  - identify any sci-fi props that look too off-theme for the starter village.
- Add proper stylized fantasy village buildings now that the oversized sci-fi shells have been removed.
- Expand the proven original Blender-to-Unreal lane from the waystone, ember
  lamps, road signpost, road gate, boundary fences, end stones, and supply chest
  into compact village pieces while the accepted creature lane remains
  independently reversible.
- Polish the accepted Marsh Prowler only from concrete PIE feedback: attack
  timing, action transitions, material balance, silhouette, physics bodies,
  target-ring/nameplate clearance, and respawn presentation.
- Retain the completed measured grounding baseline from
  `Docs/GROUNDING_AND_TERRAIN_PASS.md`; replace incomplete vendor meshes with
  project-owned or complete signed-in art rather than masking real placement or
  dependency errors with more foliage.
- Preserve the collision-cleared 525 cm solo-pull baseline while tuning leash,
  return-home speed, damage, and respawn timing only from normal-route PIE.
- Tune player respawn delay, protection duration, and recovery rules after in-editor playtesting.
- Keep automation coverage growing around cooldowns, death/respawn, targeting, and hotbar behavior.
- Keep world-status VFX subscribed to gameplay-owned status metadata; polish
  scale, color, or motion only from normal-camera feedback.

## Last Completed

- 2026-07-02: built successfully after target/inventory polish.
- Replaced the loud overhead `TARGET` text with a smaller selected marker and routed target nameplate text/color through reusable C++ helpers.
- Swapped the target ring off the engine debug material and onto a gold-tinted basic material path.
- Added automation assertions for selected-target presentation text and low-health color behavior.
- Expanded inventory reward inspection text to show the inspected item name and stack count.
- Ran the four-test suite after build through MCP: 4 passed, 0 failed. The already-open editor still emitted hot-reload no-world warnings from the transient target test; restart Unreal before treating warning absence as authoritative.
- Started and stopped PIE through MCP after the build. PIE started successfully, but full visual verification should be done after a clean editor restart because this run followed a live C++ build.
- 2026-07-03: added a native `UEmbermereEnemyNameplateWidget` and mounted it on enemies through a screen-space `UWidgetComponent`.
- The legacy text-render nameplate/marker remains as fallback, but selected enemies now use UMG for name, selected marker, HP text, and HP bar.
- Added `Embermere.UI.EnemyNameplateWidget`; MCP automation suite now runs 5 tests, all passing with 0 warnings.
- Built successfully after the nameplate widget work and booted/stopped PIE through MCP with no current Embermere gameplay or Blueprint errors.
- Direct unauthenticated Fab search/API access is Cloudflare-gated from terminal automation; first asset import should happen through the signed-in Unreal Fab window or Epic Launcher.
- 2026-07-03: moved gameplay feedback out of Unreal's top-left debug overlay and into a native bottom-left chat/combat log.
- Routed player, target, combat, quest, XP, inventory reward, dialogue, enemy attack, death, and respawn messages through the HUD log.
- Added `Embermere.UI.ChatLog`; headless automation now runs 6 tests, all passing with 0 warnings.
- 2026-07-04: tightened the bottom-left chat/combat log so its text is clipped inside the shaded panel.
- Reduced visible chat history to the configured message limit, set a fixed wrap width, and added explicit widget clipping to the panel, stack, and text rows.
- Built successfully and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- No local `/Game/ThirdParty/Fab` import was present yet, so the first asset pass still needs the signed-in Unreal Fab/Epic Launcher import step before Codex can wire assets into the level.
- 2026-07-05: added bracket-key inventory inspection cycling for multi-stack inventories.
- The top-right inventory panel now marks the selected stack, shows `Inspecting X/Y`, and uses `[`/`]` to wrap between stacks.
- Expanded `Embermere.UI.InventoryToggle` coverage to verify inventory selection advances and wraps in both directions.
- Built successfully and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- 2026-07-05: added the first local Fab/Epic environment pass for the starter zone.
- Ignored raw local marketplace asset folders in Git so the public repo does not redistribute vendor content.
- Added reusable placement and validation scripts for the tagged `FabPass_` art layer.
- Saved `L_Embermere_Prototype` with 68 created `FabPass_` actors after removing the old visual-only greybox village, road, and ruin markers.
- Built successfully, passed the headless FabPass map validator, and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- Manual PIE verification is still needed after restarting Unreal because MCP dropped during the first heavy asset compile pass.
- 2026-07-06: added hotbar cooldown enforcement and ready-time feedback.
- Combat now rejects abilities from dead characters, and `Embermere.Combat.DeadCasterRejected` covers that death/recovery contract.
- The bottom-left chat/combat log now uses clipped single-line rows to avoid wrapping below the shaded panel border.
- Built successfully with `-NoHotReloadFromIDE` and ran clean headless automation: 7 passed, 0 failed, 0 warnings.
- Captured the no-hot-reload build lesson in `Docs/UNREAL_LESSONS.md`; restart Unreal before manual PIE so the editor loads the relinked base module.
- 2026-07-07: added first-pass starter enemy leash and return-home behavior.
- Added `LeashRadius`, `ReturnHomeRadius`, and `ReturnHomeSpeedCmPerSecond` so enemies can drop aggro and return home if pulled too far from their wilderness pocket.
- Added `Embermere.Enemy.LeashRules`; headless automation now runs 8 tests.
- Built successfully with `-NoHotReloadFromIDE` and ran clean headless automation: 8 passed, 0 failed, 0 warnings.
- FabPass validator exited successfully when run sequentially. Do not run Unreal commandlets in parallel because they can race on shared UBT trace/log files.
- 2026-07-08: added short player respawn damage protection.
- `UEmbermereStatsComponent` now supports temporary damage immunity, and player respawn grants a configurable protection window.
- Added `Embermere.Stats.DamageImmunity`; headless automation now runs 9 tests.
- Build/test/validator remain clean when run sequentially. Manual PIE should verify the recovery message and damage-protection feel after restarting Unreal.
- 2026-07-09: tightened the inventory HUD empty/reward-state presentation.
- Added `GetInventoryDisplayText()` as a reusable/testable HUD API, so the live panel and automation use the same inventory text path.
- Empty inventory now explicitly says quest rewards appear there, and multi-stack inventory shows the bracket-key inspection hint.
- Expanded `Embermere.UI.InventoryToggle` coverage to assert empty text, selected stack text, stack quantities, stack limits, item descriptions, and bracket-key hint text.
- Built successfully with `-NoHotReloadFromIDE`, ran clean headless automation: 9 passed, 0 failed, 0 warnings, and reran the FabPass validator successfully.
- Direct MCP initialized, but raw `tools/call` streaming did not return usable output in this session. Manual PIE still needs a clean editor restart before validating the live UI and FabPass feel.
- 2026-07-09: the refreshed ChatGPT/Codex app exposed Unreal MCP as first-class tools; the live nine-test baseline passed in one MCP call and Slate screenshots captured the running HUD.
- Found that the Unreal Python placement helper mapped positional `Rotator` arguments into pitch instead of yaw, leaving almost the entire Fab art pass tilted.
- Corrected all 68 actor rotations, removed three oversized sci-fi building shells, restored PlayerStart, and saved a 65-actor upright art layer through Unreal's asset API.
- Strengthened the Fab validator to reject tilted actors; a fresh headless validation passed with 65 upright actors and all gameplay anchors intact.
- Replaced the compact inventory text block with a 510x292 structured inventory window containing capacity, item rows, selected-item metadata, descriptions, empty/reward state, and keyboard hints.
- Replaced chunky cube target-ring segments with 24 flat plane segments plus subtle rotation and pulse.
- Added live hotbar cooldown countdown text and unavailable-state dimming plus `Embermere.UI.HotbarCooldownDisplay`.
- Built successfully with `-NoHotReloadFromIDE`; the expanded suite passed 10/10 with zero warnings.
- Published the related Real World AI Lab field note, `When Unreal MCP Started Feeling Native`, plus flashcards and a private LinkedIn draft.
- 2026-07-10: verified the fresh live MCP baseline: 10 tests passed with zero warnings, and the structured HUD plus corrected Fab layer were present in PIE.
- Added `SkyAtmosphere`, movable real-time skylight fill, and restrained fog color/density; PIE now shows a blue sky and readable village route instead of the prior black-sky presentation.
- Added the atmosphere actor to `setup_prototype_level.py`, corrected that foundational helper's positional `unreal.Rotator` bug, and made the fresh-process Fab validator require the atmosphere.
- Replaced passive inventory rows with non-focusable clickable buttons, direct stack selection, selected-row highlighting, and controller input-mode/cursor switching while the inventory is open.
- Preserved `I` and `[`/`]` keyboard behavior and expanded `Embermere.UI.InventoryToggle` with direct-selection and invalid-index assertions.
- Created and saved `/Game/Art/Embermere/Targeting/M_EmbermereTargetRing`, an unlit additive parameterized gold material, and wired the 24 ring segments to it with an engine-material fallback.
- Added a target-presentation assertion proving the dedicated material resolves.
- Final no-hot-reload build succeeded; headless automation passed 10/10 with zero warnings; the saved-map validator passed with 65 upright Fab actors, required gameplay anchors, and the atmosphere present.
- Restart the interactive editor before manually validating row clicks/cursor behavior and the emissive ring, because the current editor predates the final C++ relink.
- 2026-07-11: added `/Game/Art/Embermere/Environment/M_EmbermereGround` and applied its muted moss treatment to the zone plane plus both raised prototype platforms.
- Switched the target-ring material from additive to opaque unlit/emissive and raised the constructor offset so its 24 segments clear the combat-pocket platform.
- Added data-driven item categories, equipment slots, level requirements, stat bonuses, and `UEmbermereEquipmentComponent` equip/replace/unequip rules.
- Added a real inventory Equip/Unequip button, dynamic action label, level gating, richer item metadata, and chat feedback.
- Migrated the tracked Recruit Pack quest reward to level-1 Back armor through an idempotent Unreal asset script so the action is testable in the normal loop.
- Added `Embermere.Equipment.SlotRules` and expanded `Embermere.UI.InventoryToggle`; final headless automation passed 11/11 with zero warnings.
- Made the level rebuild script reproduce the moss ground and exact successful daylight settings.
- Strengthened fresh-process map validation for the ground overrides, sun, real-time skylight, fog baseline, 65 upright Fab actors, and gameplay anchors; validation passed.
- Clean-restart PIE still needs to verify the newly linked Equip/Unequip button and target-ring clearance because safely closing the live editor requires explicit user confirmation.
- 2026-07-12: clean-restart MCP discovered and passed all 11 existing tests, and PIE verified daylight/moss terrain, Mara quest acceptance, the inventory shell, target/nameplate, chat clipping, and target-ring platform clearance.
- Lowered target-ring emissive strength from `2.5` to `0.75` and changed the runtime default to a more saturated orange-gold so outdoor exposure preserves its color.
- Expanded inventory to a 700x330 three-column bag/detail/equipment layout with visible slots, aggregate bonuses, item effect summaries, and `[E]` markers.
- Added idempotent equipment stat application for maximum health/mana, power, and armor plus missing-resource preservation and armor mitigation.
- Added data-driven consumable recovery and transactional `Use`: inventory decrements only after health or mana actually increases.
- Updated the Recruit Pack asset to grant `+5 HP` and `+1 Armor`.
- Added the stackable Marsh Tonic data asset with `+25 Health` and `+10 Mana` recovery and wired Marsh Prowlers to grant one on death.
- Added editable enemy loot item, quantity, and drop-chance rules plus player inventory/chat delivery.
- Added `Embermere.Equipment.StatApplication`, `Embermere.Inventory.ConsumableUse`, and `Embermere.Enemy.LootRules`; final headless automation passed 14/14 with zero warnings.
- Re-ran the saved-map validator: all 65 upright FabPass actors, gameplay anchors, moss ground, and daylight baseline remain intact.
- Restart Unreal before visually verifying today's newly linked equipment pane, stat changes, Marsh Tonic drop/use flow, and ring color.
- 2026-07-13: clean live PIE verified the daylight/moss/Fab baseline, Mara quest acceptance, targeting/nameplate, combat progression, and a real Marsh Tonic drop before source work.
- Replaced the equipment text list with ten stable clickable paper-doll slot controls; occupied slots use a restrained gold state and click-to-unequip behavior.
- Equipped items now leave the bag, replaced items return to it, and unequipped items return only when capacity exists.
- Added rollback-safe same-slot replacement and full-bag unequip rejection so gear cannot be lost or duplicated.
- Made inventory add/remove primitives atomic; failed capacity or quantity operations leave all stacks unchanged.
- Added `Embermere.Equipment.InventoryTransactions` and `Embermere.Inventory.CapacityTransactions`; final headless automation passed 16/16 with zero warnings.
- Built successfully with `-NoHotReloadFromIDE` and reran saved-map validation: 65 upright FabPass actors, gameplay anchors, moss ground, and daylight baseline remain intact.
- Restart Unreal before visually verifying the newly linked paper-doll slot grid and bag/equipment transfer flow.
- 2026-07-13: installed Blender 5.1.2 and the pinned structured Blender MCP
  bridge with Safe Mode on, inline code off, localhost binding, and project-only
  approved script roots.
- Generated `SM_EmbermereWaystone_01` from the tracked Blender script, revised
  its first dark preview, and validated 1,340 triangles, three materials, one UV
  channel, ground pivot, and two Unreal collision boxes.
- Imported and saved the waystone plus its materials under
  `/Game/Art/Embermere/Environment/PrototypeVillage`, replaced the temporary
  road stump, and tagged the map actor `EmbermereOriginalArt`.
- Caught and fixed a live snap-order issue that initially placed the waystone on
  top of the outgoing stump; deterministic placement now disables snap and the
  validator asserts Z `20`.
- Final Mac editor build succeeded; saved-map validation passed with 64 upright
  FabPass actors plus the original waystone; headless automation passed 16/16
  with zero failures and zero warnings.
- 2026-07-14: added net item comparison against equipped gear or an empty slot,
  with concise detail-pane output and hover tooltips for bag rows and occupied
  paper-doll slots.
- Added item-identity action routing and explicit target-slot validation so
  future drag/drop can reject wrong-slot and stale payloads before mutation
  while reusing the atomic equipment transaction path.
- Added [Docs/INVENTORY_INTERACTION_PLAN.md](Docs/INVENTORY_INTERACTION_PLAN.md)
  covering drag payload identity, bag/equipment rules, sorting invariants,
  implementation order, and loss/duplication edge cases.
- Added `Embermere.UI.ItemComparison` and
  `Embermere.Inventory.IdentityActions`; the authoritative Mac build succeeded
  and headless automation passed 18/18.
- First-class MCP proved the still-open editor was stale by discovering only 14
  tests. Restart Unreal before visual comparison/tooltip or paper-doll testing.
- 2026-07-15: added typed, item-identity drag payloads and bounded bag-to-slot
  plus equipment-to-bag drag/drop without replacing click, keyboard, or action
  button paths.
- Added gold valid-target, restrained red invalid-target, bag-return, footer,
  and compact drag-label feedback inside the fixed inventory window.
- Added `Embermere.UI.InventoryDragDrop` covering matching/wrong slot,
  under-level, stale bag/equipment identity, successful return, and full-bag
  preservation. Final headless automation passed 19/19.
- Built `SM_EmbermereEmberLamp_01` through the reviewed Blender MCP script lane:
  editable `.blend`, FBX, preview, metrics, one UV channel, no non-manifold
  edges, dark iron/stone/moss/ember materials, and two authored `UBX_` boxes.
- Replaced the two temporary sci-fi lamps with original Mara-side and road-side
  ember lamps, leaving 62 upright `FabPass_` actors plus three tagged original
  placements. The fresh-process map validator passed exact transform, mesh,
  bounds, classic FBX provenance, and collision assertions.
- Captured the UE 5.8 import lesson: the generic Interchange path discarded
  authored collision when `bCollision` stayed false; pinning `FbxFactory` and
  recreating the partial mesh package preserved both UBX colliders and clean
  `FbxStaticMeshImportData`.
- The no-hot-reload editor build succeeded, the final 19-test suite passed, and
  saved-map validation passed. Restart Unreal before authoritative drag/drop
  PIE because the open editor predates the final relink.
- 2026-07-16: clean live PIE verified the full Mara quest/reward loop, three
  Marsh Prowler kills and tonic stacking, target clear on death, bracket
  cycling, Recruit Pack comparison, valid/invalid drag targets,
  equipment-to-bag return, W autorun cancel, and both ember lamps in the saved
  route. Slate could not hold `Ctrl` across a later game tick, so `Ctrl+M`
  remains a manual input check rather than a detected regression.
- Added an explicit compact Sort control that orders weapon, armor, consumable,
  quest, and misc stacks by category then name, preserves selected item identity
  and duplicate-stack occurrence, and disables during pending or active drags.
- Added `Embermere.Inventory.StableSorting`; the authoritative Mac build
  succeeded and headless automation passed 20/20.
- Fresh-process map validation passed with 62 upright FabPass actors, the
  waystone and two ember lamps, all gameplay anchors, moss ground, and the exact
  daylight baseline intact. Restart Unreal before visually checking the newly
  linked Sort control.
- 2026-07-17: ran all 20 tests in the live editor and through authoritative
  headless automation; both suites passed with no test failures after a
  successful `-NoHotReloadFromIDE` Mac editor build.
- Completed the full Mara quest in clean PIE and verified tonic stacking,
  identity-preserving Sort, bracket cycling, Recruit Pack bag-to-Back and
  Back-to-bag drags, wrong-slot rejection, target clear, and W autorun cancel.
- Tuned the starter encounter around the proven combat cadence: default aggro
  is now `525` cm, while damage remains 6 and attack cooldown remains 2 seconds;
  automation asserts all three defaults.
- Used native WorldStatic overlap probes to move the Prowler homes to the
  collision-clear triangle `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`.
  A focused PIE pull moved and activated only the nearest enemy while both
  neighbors remained at home.
- Made enemy marker cones plus safe/combat area cylinders explicitly
  `NoCollision` in both the saved map and foundational setup script. The map
  validator now rejects visual guides that become invisible blockers.
- Built `SM_EmbermereRoadSignpost_01` through Blender MCP with reviewed source,
  editable `.blend`, FBX, preview, deterministic metrics, 1,828 triangles, one
  UV, no non-manifold edges, shared fantasy materials, and two authored boxes.
- Imported the signpost through explicit `FbxFactory`, explicitly saved its new
  `M_EmbermereTimber` package, and placed `Embermere_RoadSignpost_01` beside the
  village road with base/post collision and non-colliding overhead arms.
- The tracked validator reloaded the saved level and passed 62 upright Fab
  actors, four exact original-art placements, the collision-cleared encounter,
  visual-only marker collision, gameplay anchors, moss ground, and daylight.
  A separate final commandlet launch stalled in macOS app-launch services
  before Unreal produced a log, so this result is recorded accurately as a
  live-editor saved-map reload rather than a fresh-process pass.
- 2026-07-18: separated automatic forward movement from the manual movement
  handler so `W` and `S` can cancel autorun without the autorun tick cancelling
  itself. Added `Embermere.Input.AutorunCancellation`; the authoritative
  no-hot-reload suite passed 21/21.
- Replaced the compact inventory drag label with a fixed 236x62 fantasy token
  using category sigils, warm item text, contextual slot/effect copy, and
  project-owned moss/gold presentation. Extended drag/drop automation for its
  armor sigil, context text, and accent color.
- Built `SM_EmbermereRoadGate_01` through Blender MCP with reviewed source,
  editable `.blend`, FBX, preview, deterministic metrics, 3,296 triangles, one
  UV, no non-manifold edges, five shared fantasy materials, and four authored
  support colliders.
- Imported the gate through explicit `FbxFactory`, placed
  `Embermere_RoadGate_01` at `(1080, 540, 20)`, yaw `20`, and saved the map.
  Native traces proved the 250 cm center opening is clear while a support is
  solid; the validator passed 62 upright Fab actors and five exact original-art
  placements.
- The open editor predates the final C++ relink. Restart Unreal before claiming
  authoritative live `W`/`S` cancellation or visual approval of the new drag
  token; those two checks are tomorrow's first manual boundary.
- 2026-07-19: rebuilt `EmbermereEditor` with `-NoHotReloadFromIDE` and ran the
  complete headless suite; all 21 tests passed with zero failures and zero
  warning tests. The clean live editor also discovered and passed all 21 after
  the final boundary-fence map save.
- Restarted Unreal cleanly, started MCP, and used timed player-transform reads
  in PIE to prove `Q` movement plus independent `W` and `S` cancellation. Both
  cancellation checks held the exact same transform for a full second.
- Built `SM_EmbermereBoundaryFence_01` through the reviewed Blender MCP script
  lane: editable `.blend`, FBX, preview, deterministic metrics, 2,632 triangles,
  one UV, no non-manifold edges, five shared fantasy materials, and three
  authored `UBX_` colliders.
- Imported through explicit `FbxFactory` and placed matching south/north fence
  instances beside `Embermere_RoadGate_01`. The map now contains 62 upright
  Fab actors plus seven exact original-art placements.
- Expanded the saved-map validator for both fence transforms, materials,
  bounds, tags, and three-box collision. Added a native trace validator proving
  three gate lanes clear, one gate support solid, and both fence centers solid.
- Inspected the Blender preview and live Unreal road approach. The north fence
  and center opening read clearly; an existing tree partly masks the south
  fence and is recorded for foliage-only composition review.
- Captured two durable UE 5.8 lessons: Python line traces return `HitResult` or
  `None`, and commandlet Python errors can coexist with a zero process exit, so
  validators must require a success marker and reject `LogPython: Error`.
- Slate's atomic `Ctrl+M` synthesis still cannot span the later controller tick
  that polls Ctrl state. Keep physical inversion feedback and populated-item
  drag-token appearance as the next manual visual checks.
- 2026-07-20: built `SM_EmbermereBoundaryStone_01` through the reviewed Blender
  MCP script lane with editable source, FBX, preview, deterministic metrics,
  1,872 triangles, five shared materials, one UV, no non-manifold edges, a
  ground pivot, and two authored `UBX_` colliders.
- Imported it through explicit `FbxFactory` and placed matching south/north
  rune-topped end markers outside the existing boundary fences. The saved map
  now contains 62 upright Fab actors plus nine exact original-art placements.
- Moved only `FabPass_Road_Pine_05` and `FabPass_Wild_Tree_South_01` within the
  south foliage band. A fresh road capture now shows both fences, both end
  markers, and the full gate opening without changing validated boundary
  geometry or the Fab actor count.
- Expanded fresh-process validation for the boundary-stone classic-FBX import,
  bounds, five materials, two colliders, tags, exact transforms, and both
  foliage transforms. Native traces pass three clear gate lanes, one gate
  support, two fence centers, and two boundary-stone cores.
- The authoritative Mac build succeeded with `-NoHotReloadFromIDE`; the live
  editor discovered and passed all 21 tests with no failures or warnings.
  Fresh PIE again proved `Q` movement and independent `W`/`S` cancellation by
  holding identical transforms for one second after each stop input.
- Recorded the Slate focus lesson: refresh the accessibility snapshot and click
  the game viewport before synthetic gameplay keys, or editor-tool calls can
  make a correct input path look broken. Physical `Ctrl+M` and populated-item
  drag-token appearance remain honest manual checks.
- 2026-07-21: generated a deterministic project-owned family of 14 square UI
  icons: Recruit Pack, Marsh Tonic, ten equipment slots, and explicit missing
  item/slot fallbacks. The tracked standard-library generator keeps the source
  art reproducible and the palette aligned with Embermere's moss, stone, iron,
  gold, leather, and ember roadside family.
- Added `UEmbermereUiIconSet`, which resolves explicit soft item references,
  category fallbacks, equipment-slot art, and missing-art fallbacks without
  hardwiring textures into inventory rules. Recruit Pack and Marsh Tonic now
  own explicit icon references in their saved data assets.
- Integrated fixed `18x18` row/slot icons and a fixed `42x42` detail icon into
  the native inventory window while preserving item names, hover tooltips,
  click/keyboard interaction, and collapsed text-only fallback behavior.
- Imported and explicitly saved all 14 `128x128` textures plus the icon-set data
  asset under `/Game/UI/Icons`. A fresh-process Python validator reloaded every
  package and passed exact dimensions, mappings, texture settings, and starter
  item assignments.
- Added `Embermere.UI.IconPresentation`; after adapting the source-dimension
  assertion for `-NullRHI`, the authoritative no-hot-reload build succeeded and
  the complete suite passed 22/22. Fresh saved-map and road-boundary validators
  also passed with the accepted 62 Fab actors, nine original placements, and
  three clear gate lanes intact.
- The already-open interactive editor predates the final icon C++ link. Restart
  it before visual approval of the row, detail, slot, fallback, and tooltip
  presentation in PIE.
- 2026-07-22: restarted PIE on the July 21 icon module, passed all 22 tests in
  the live editor, and inspected both the empty and populated inventory. The
  ten empty-slot symbols and starter-item art were distinct and stable, but the
  real saved `Embermere Recruit Pack` label clipped in its row and crowded the
  adjacent Equipment heading in the detail layout.
- Shortened the data-driven player-facing label to `Recruit Pack`, reproduced
  that value in `configure_starter_items.py`, saved the item package, and added
  the exact name to fresh-process icon validation. A second populated PIE
  capture accepted the corrected row and detail-heading fit. This keeps the
  underlying `RecruitPack` identity unchanged.
- Reused the existing resolver in the fixed `236x62` fantasy drag token and the
  temporary reward popup. Resolved item art now occupies a fixed `44x44` drag
  cell and a fixed `32x32` reward cell; category text and missing-item art remain
  fallbacks without changing drag payload or transaction identity.
- Expanded `Embermere.UI.InventoryDragDrop` and
  `Embermere.UI.IconPresentation` for resolved drag art, visual construction,
  and fixed reward-icon bounds. The authoritative no-hot-reload Mac build and
  fresh-process suite passed 22/22 with zero warnings or failures.
- Fresh-process icon validation passed all 14 textures, mappings, starter-item
  assignments, and the compact Recruit Pack name. Saved-zone validation passed
  62 upright Fab actors plus nine original placements; native traces again
  proved three clear gate lanes and solid gate/fence/end-stone collision.
- The interactive editor predates today's final relink. Clean-restart PIE still
  owns visual approval of the icon-bearing drag token, reward popup, and
  fallback appearance; the compact `Recruit Pack` copy is already accepted.
- The Codex automation read/update API stalled twice and was terminated without
  mutating `daily-embermere-rpg-build`. Its existing 8:00 AM heartbeat remains
  active and already treats this `Start Here` section as authoritative, so this
  handoff remains tomorrow's source of truth until the app endpoint recovers.
- 2026-07-23: clean-restart PIE accepted the fixed reward popup and a real
  Recruit Pack bag-to-Back drag on the July 22 module. The reward kept its
  `32x32` Marsh Tonic art and readable copy; the equipment transaction removed
  the item from the bag, populated the Back slot, raised HP to `105/105`, and
  posted one chat line. Slate cannot freeze its atomic drag mid-flight, so the
  populated drag-token appearance remains a physical-eye check.
- Extended the deterministic source family from 14 to 31 project-owned
  `128x128` textures: sixteen distinct starter-ability illustrations plus an
  explicit missing-ability fallback. Warrior uses ember/iron, Cleric
  gold/ivory, Ranger moss/leather, and Wizard frost/arcane while sharing the
  roadside family's frame language.
- Added direct soft icon references to every native ability definition,
  persisted matching assignments through `DA_EmbermereRules`, and extended
  `UEmbermereUiIconSet` with ability fallback resolution. The native hotbar now
  presents fixed `32x32` art inside the existing `92x64` slots, keeps
  cooldowns to a two-line layout, dims art and text together, and exposes
  data-driven description/power/mana/range/cooldown hover tooltips.
- Added `Embermere.UI.AbilityIconPresentation` and expanded the cooldown test.
  The authoritative no-hot-reload build succeeded and all 23 tests passed in a
  fresh process with zero failures. A separate fresh process reloaded all 31
  exact UI textures, all mappings, and all sixteen distinct ability references.
- Saved-zone validation remained green with 62 upright Fab actors and nine
  original placements. Native traces retained three clear gate lanes plus
  solid gate/fence/end-stone collision. The interactive editor now predates
  the final July 23 C++ link, so all-class hotbar appearance and tooltip fit
  are tomorrow's first clean-restart PIE acceptance pass.
- The Codex automation endpoint recovered. `daily-embermere-rpg-build` remains
  active at 8:00 AM and now starts from the 23-test ability-icon/hotbar handoff
  instead of the stale 21-test item-icon milestone.
- 2026-07-24: restarted Unreal onto the July 23 module, discovered and passed
  all 23 existing tests, and visually accepted Warrior, Cleric, Ranger, and
  Wizard hotbar palettes plus synchronized Warrior cooldown dimming/countdown.
- Added a data-driven ability-effect contract covering damage, healing, mana
  recovery, Attack Power buffs, Armor buffs, duration, and movement-speed
  multipliers. Saved rules now make Battle Shout, Ward, Snare, Nature's Focus,
  Frost Root, and Meditate functional instead of descriptive placeholders.
- Stats now expose effective Attack Power, Armor, and movement speed; damage
  consumes effective armor, attacks consume effective power, timed effects
  expire from world time, and vital initialization clears temporary effects so
  enemy respawn cannot preserve a root or snare.
- Player and enemy movement consume the shared multiplier. Live clean-PIE
  probes measured Snare at `0.50x`, Frost Root at `0.00x`, both offensive buffs
  at 18 effective Attack Power, Ward at 10 effective Armor, and Meditate
  restoring mana from 20 to 38.
- Added `Embermere.Combat.StarterAbilityEffects`, saved-data validation for all
  sixteen ability semantics, and deterministic
  `Scripts/configure_starter_abilities.py`. The authoritative no-hot-reload
  build succeeded, fresh headless automation passed 24/24, and UI/icon,
  saved-map, and road-boundary validators all passed without Python errors.
- 2026-07-25: generalized the deterministic UI generator and added a
  project-owned `128x160` armored-adventurer paper-doll backdrop in the shared
  stone/moss/iron/gold/ember language.
- Added a soft backdrop reference/resolver to `DA_EmbermereUiIconSet` and
  layered it beneath the unchanged ten-slot equipment grid through a
  hit-test-invisible native UMG overlay. Slot bounds, labels, icons, tooltips,
  drag targets, and transaction rules remain unchanged.
- Added `Embermere.UI.PaperDollPresentation`. The authoritative no-hot-reload
  build succeeded, fresh headless automation passed 25/25 with zero warnings
  or failures, and a second process reloaded the exact texture dimensions,
  texture settings, and saved mapping.
- Saved-zone validation retained 62 upright Fab actors and nine original-art
  placements; native traces retained all three clear gate lanes and required
  solid boundary collision. The running GUI editor still holds the prior
  module, so backdrop contrast and fit remain the next clean-restart PIE gate.
- 2026-07-26: accepted the centered paper-doll backdrop in clean PIE with an
  empty equipment grid; it remained readable beneath all ten fixed controls
  without moving the title, bonuses, footer, inventory, or hotbar.
- Added source-ability-backed timed-status snapshots to stats and registered
  them only after generic buff/control rules succeed. Player and selected-target
  HUD rows now render existing ability art, names, live countdowns, beneficial
  or harmful color, and hover descriptions without hard-coded class or ability
  IDs.
- Duplicate effect registration refreshes one record, world-time expiration
  removes it, and existing vital-reset/respawn paths clear both gameplay state
  and presentation metadata. Meditate remains instantaneous and creates no
  timed record.
- Added `Embermere.UI.TimedStatusPresentation`. The authoritative
  no-hot-reload build succeeded and fresh headless automation passed 26/26.
  UI-art, saved-map, and native road-boundary validators retained explicit
  success markers with no Python errors.
- The interactive editor now predates the final July 26 link. Clean-restart PIE
  still owns visual acceptance of the new status rows and the paper doll with
  Recruit Pack equipped.
- Refreshed the daily heartbeat to begin from this 26-test baseline and move
  next into clean-PIE status acceptance, restrained VFX, original village art,
  or concrete live combat tuning.
- 2026-07-27: accepted Battle Shout and Snare timed-status cells in clean PIE,
  including saved ability art, beneficial/harmful treatment, live countdowns,
  fixed bounds, and stable neighboring HUD layout. Meditate remains
  instantaneous and focused automation retains duplicate, expiration, respawn,
  death, and target-switch clearing.
- Accepted the occupied paper-doll state with Recruit Pack equipped: the Back
  slot uses its gold icon state, HP reads `105/105`, the bag is empty after the
  atomic transfer, and the backdrop remains readable without moving the title,
  bonuses, footer, or hotbar.
- Added `SM_EmbermereSupplyChest_01`, a 2,364-triangle deterministic Blender
  prop with five shared materials, one UV channel, two authored `UBX_` boxes,
  editable source, FBX, preview, and metrics. Its route-facing saved actor
  replaces `FabPass_Village_Crates_A` beside the vendor area.
- Saved-map validation now requires 61 upright Fab actors and ten original-art
  placements, including exact chest provenance, materials, bounds, collision,
  tag, and transform. Native traces in the initialized live editor proved the
  chest lid solid while retaining all three open gate lanes and required
  boundary collision.
- The authoritative no-hot-reload build succeeded; fresh commandlet automation
  passed 26/26 with zero failures or warnings; UI-art and saved-map validators
  emitted their success markers with no Python errors.
- Refreshed the 8:00 AM daily automation to start from the 61-Fab/10-original
  supply-chest baseline and to separate fresh-process persistence checks from
  initialized-live-world collision traces.
- 2026-07-27 evening: built Embermere's first original rigged creature,
  `SK_EmbermereMarshProwler_01`, from a reviewed deterministic Blender script.
  The saved source has 7,464 triangles, 3,878 vertices, 26 authored bones, five
  Stylized Classic marsh materials, applied transforms, and six actions:
  Idle, Walk, Run, Attack, Hit, and Death.
- Imported the skeletal mesh, skeleton, physics asset, materials, and six
  animation sequences into a project-owned Unreal folder. Generic enemy state
  now routes those animations without coupling combat, loot, target, quest, or
  respawn rules to the art asset. All three saved Prowler map instances were
  repaired and validated after discovering that a correct Blueprint CDO does
  not overwrite stale placed-instance component state.
- Completed the measured environment-grounding pass: ordinary art moved from
  the false `Z=20` baseline to the real `Z=0` surface; unsupported SoulCave
  accents and three visual enemy markers were removed; 21 foliage/tree
  placements received project-owned overrides; and the ground became a
  38-expression moss/earth material with a `300` cm worn-road half width.
- Added `SM_EmbermereMarshReedCluster_01`, a deterministic 1,012-triangle,
  four-material visual-only asset with a terrain-blended footprint. Four saved
  `NoCollision` placements add marsh identity without blocking the road or
  combat sightlines.
- Clean PIE exercised the real Prowler targeting, Strike, retaliation, death,
  target clear, Marsh Tonic loot, and respawn loop. Editor automation passed
  27/27 with zero failures, skips, or warnings. The no-hot-reload Mac build,
  fresh saved-map validator, road-boundary traces, and headless saved-Blender
  inspection all passed.
- Documented two honest follow-ups: straight-line autorun currently meets
  Mara's temporary village cluster, and several KiteDemo meshes retain absent
  internal vendor dependencies even though project-owned component overrides
  keep the visible map readable.
- 2026-07-28: reproduced the straight-line PlayerStart autorun stop at the
  supply chest, moved `Embermere_SupplyChest_Vendor_01` to
  `(-1740, -1180, 0)` without weakening its route-facing village composition,
  and saved the exact map transform.
- Added geometric validation requiring at least `225` cm of chest clearance
  from the spawn corridor and an initialized-world player-height trace proving
  the old blocked line is clear. Fresh PIE autorun moved well beyond the former
  stop point.
- Exercised the complete Marsh Prowler loop in clean PIE: real 6-damage
  retaliation killed the player and triggered recovery/protection plus
  return-home; a bounded zero-damage diagnostic then proved four Strikes,
  target clear, Marsh Tonic loot, death hold, hide, 12-second respawn, full HP,
  and untouched neighboring enemies.
- Added an asset-agnostic world-status presentation to every Embermere
  character. Eight fixed non-colliding segments consume the same successful
  status snapshots as the HUD, with orange-gold Attack Power, blue-white Armor,
  marsh-green Snare, frost-cyan Root, harmful priority, pulse/rotation, and
  dead/empty hiding.
- Added `Embermere.UI.WorldStatusVfxPresentation`; the authoritative
  no-hot-reload build succeeded and all 28 tests passed in a fresh commandlet.
  Saved-map validation, initialized-world road/chest traces, and fresh Blender
  Prowler inspection all retained explicit success markers.
- Clean PIE visually accepted Battle Shout's eight restrained orange-gold
  segments around the player alongside its status countdown, with no HUD
  movement or overlap. Ward, Nature's Focus, Snare, and Frost Root remain the
  next normal-route color/readability sweep rather than unverified claims.
- 2026-07-29: replaced the raised orange-gold rotating marker with an
  EverQuest-inspired cyan-blue target circle. Forty-eight overlapping
  non-colliding segments form a stationary complete read, resolve their radius
  from transformed capsule/mesh bounds plus `18` cm padding, and trace the
  supporting surface with a `16` cm effective clearance while selected. The
  larger value is visually flush in this prototype because the moss ground's
  rendered surface sits above the collision surface returned by the trace.
- Corrected the user-reported clean-PIE disappearance. The saved
  `BP_StarterEnemy` still inherited 24 older native segment templates while a
  native-only test constructed all 48. Runtime initialization now reuses saved
  segment components and creates any missing transient components until the
  exact 48-segment presentation contract is satisfied.
- Expanded `Embermere.Combat.TargetSelectionPresentation` to prove cyan color,
  continuity, restrained pulse, no rotation/collision, selection, target
  switching, clear behavior, and effective terrain clearance. Prowler
  presentation automation now constructs the actual Blueprint-generated class,
  proves runtime reconciliation to 48 non-colliding segments, and confirms its
  visual footprint expands beyond the generic minimum without becoming
  oversized.
- The no-hot-reload Mac build, fresh commandlet suite, restarted-editor MCP
  suite, saved-map validator, initialized-world road traces, and saved Blender
  Prowler inspection all passed. Corrected clean daylight PIE showed the cyan
  circle beneath a live selected Blueprint Prowler with its nameplate and HUD
  target frame intact. A final physical-eye sweep across the other two
  normal-route instances remains the honest acceptance check.
- 2026-07-30: retained the accepted cyan target circle across all three live
  Prowlers and added finite-world recovery below `Z=-1000`, including autorun
  cancellation, forced death through temporary immunity, frozen death movement,
  exact village recovery, full vitals, walking restoration, zero velocity, and
  three seconds of protection. The suite advanced to 29 tests.
- 2026-07-31: corrected harmful world auras that were structurally active but
  visually buried inside the Marsh Prowler. Harmful presentation now derives a
  bounded radius from transformed skeletal bounds and traces the support
  surface, resolving the live Prowler to a `94.352` cm inner radius at `21` cm
  above ground. Clean PIE accepted marsh-green Snare and lighter ice-white cyan
  Frost Root inside the unchanged outer target circle.
- Expanded `Embermere.UI.WorldStatusVfxPresentation` with the real Prowler mesh,
  footprint/grounding assertions, target-circle containment, and Root/Snare
  contrast checks. The authoritative no-hot-reload Mac build, all 29 tests,
  saved-zone and UI-art validators, and initialized-world route traces passed.
- 2026-08-01: built `SM_EmbermereFenwatchShelter_01`, Embermere's first full
  village-scale original module. The reviewed Blender source is 4,348 triangles
  at `438.0 x 296.782 x 369.5` cm with five shared materials, one UV channel,
  applied scale, a ground pivot, and four authored support boxes.
- Imported through classic `FbxFactory`, explicitly saved the mesh package, and
  placed `Embermere_FenwatchShelter_Mara_01` at `(-1740, -700, 0)`, yaw `-64`.
  It replaces the old Mara stone backdrop, mismatched market cover, and
  vendor/trainer cubes, bringing the map to 57 Fab actors and 15 original-art
  placements.
- Rejected the first technically valid shelter transform because clean PIE
  showed it hiding Mara. The accepted transform places the shelter behind her,
  retains the quest-marker/name read, and preserves the measured straight
  autorun route plus independent `W` cancellation.
- The no-hot-reload Mac build succeeded; fresh-process saved-map and UI-art
  validators emitted their success markers without `LogPython: Error`; all 29
  automation tests passed; and initialized-world traces proved the shelter
  center and spawn corridor clear plus all four supports solid.
- 2026-08-02: replaced Mara's stretched engine cube with the project-owned
  `SM_EmbermereFenwatchKeeper_Mara_01` static presentation. The reviewed
  Blender source is 3,280 triangles at `107.45 x 71.0 x 207.5` cm with six
  project-owned materials, one UV channel, applied scale, a ground pivot, and
  no collision.
- Imported through classic `FbxFactory`, explicitly saved the mesh and skin
  material packages, and reconciled both the real saved `BP_QuestGiver` SCS
  template and Mara's placed visual component. The actor and interactable
  gameplay ownership remain unchanged; only the art now uses grounded local
  offset `(0, 0, -140)`, PlayerStart-facing yaw `100`, unit scale, and
  `NoCollision`.
- Clean PIE accepted grounded feet, readable moss-cowl/staff/satchel silhouette,
  shelter composition, and unobstructed quest marker/name. The map now retains
  57 grounded upright Fab actors and 16 project-owned placements.
- Added `Embermere.NPC.FenwatchKeeperPresentation` against the actual saved SCS
  template. The authoritative no-hot-reload Mac build and all 30 tests passed;
  fresh-process saved-map and UI-art validators plus initialized-world route,
  shelter, boundary, and chest traces emitted their explicit success markers.
- 2026-08-03: built `SM_EmbermereFenwatchQuartermaster_01`, a deterministic
  3,632-triangle static merchant at `120.842 x 93.0 x 217.0` cm with six
  project-owned materials, one UV channel, applied scale, a ground pivot, and
  no collision. The accepted actor sits beside the supply chest at
  `(-1530, -1190, 0)`, yaw `100`, without obscuring Mara or the route.
- Added `AEmbermereNpcPresentationActor`, a reusable art-only wrapper with soft
  static/skeletal references, optional animation class, one shared authored
  transform, deterministic lane fallback, and permanent non-colliding visuals.
  It intentionally owns no interaction, vendor, trainer, dialogue, or quest
  behavior; the contract is recorded in `Docs/NPC_PRESENTATION_CONTRACT.md`.
- Added `Embermere.NPC.PresentationContract` and
  `Embermere.NPC.FenwatchQuartermasterPresentation`. The authoritative
  no-hot-reload Mac build and all 32 tests passed. Fresh-process saved-map and
  UI-art validators accepted 57 Fab actors plus 17 original placements, and
  initialized-world route/shelter/boundary/chest traces emitted their success
  marker without `LogPython: Error`.
- Clean PIE accepted the grounded chest-side quartermaster, Mara's readable
  shelter composition, and the open route. Separate transform-measured runs
  proved Q movement plus exact W and S autorun cancellation on the final map.
- 2026-08-04: added the first complete Fenwatch vendor service without moving
  economy rules into quartermaster art. `AEmbermereVendorServiceActor` owns an
  interactable and `UEmbermereVendorComponent` but no mesh or collision;
  `AEmbermereNpcPresentationActor` remains art-only.
- Added player copper, `UEmbermereVendorStockData`, finite/unlimited runtime
  stock, complete transaction preflight, exact charge/grant behavior, refund
  on an unexpected add failure, and mutation-free full-bag, sold-out, and
  insufficient-funds rejection.
- Saved `DA_FenwatchQuartermasterStock` with unlimited Marsh Tonic at 8 copper
  and one Recruit Pack at 30, plus the co-located art-free
  `Embermere_FenwatchQuartermaster_Service_01`. The focused fresh-process
  validator locks stock, transform, tags, references, and art/service
  separation.
- Built a fixed native `Fenwatch Supplies` panel with purse balance, four
  stable stock rows, item icons/details, Buy, close, readable two-line status,
  inventory exclusion, game/UI input, and bottom-left chat feedback.
- Added four focused tests for vendor transactions, service ownership, saved
  stock, and panel behavior. The authoritative no-hot-reload build and all
  36 tests passed; saved-map, UI-art, vendor, and initialized-world route
  validators passed without `LogPython: Error`.
- Clean live PIE proved normal `F` opening; 40 -> 32 copper tonic purchase;
  32 -> 2 finite Recruit Pack purchase; inventory updates; sold-out and
  insufficient-funds button states; non-overlapping result/footer copy; and
  close/input restoration.

## Asset Hunt

Start with free Unreal/Fab assets:

- stylized modular fantasy village kit;
- stylized forest/foliage pack;
- ruins/stone props;
- fantasy UI/icons;
- simple fantasy enemy or creature pack;
- weapon pack for sword, shield, staff, and bow.

Track candidates and install notes in [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md).

## Daily Wrap

Before stopping each day:

- build the project;
- run available automation tests;
- note anything requiring manual PIE verification;
- commit and push intentional changes;
- update this file with the next starting point.
