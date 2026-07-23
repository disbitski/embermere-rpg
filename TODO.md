# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

For a fresh Codex task or context reset, read [Docs/THREAD_HANDOFF.md](Docs/THREAD_HANDOFF.md) before this daily file.

## Start Here

- Restart Unreal if it predates the 2026-07-23 starter-ability icon, hotbar
  layout, and tooltip C++ build, then confirm the current module/map and MCP
  listener on port `8123`.
  Confirm Blender and its localhost bridge only when another original-art pass
  is selected.
- Discover and run all 23 tests. The authoritative 2026-07-23 no-hot-reload
  build and headless run passed 23/23; saved-map validation passes with 62 upright
  `FabPass_` actors, nine original-art placements, visual-only encounter
  markers, exact daylight, and the collision-cleared starter layout.
- In clean PIE, visually approve the new hotbar presentation. The active
  starter abilities must show fixed `32x32` illustrations inside the unchanged
  `92x64` slots, with readable two-line key/name text, no hotbar growth, and
  no overlap. Trigger a cooldown and confirm both icon/text dimming plus the
  live countdown. Hover each active slot and confirm its data-driven tooltip
  includes description, power, mana, meter-based range or Self, and cooldown.
  Exercise Warrior, Cleric, Ranger, and Wizard so all sixteen icons receive a
  real viewport check; empty slots remain stable and `F` retains Interact.
- In clean PIE, open the inventory and visually approve the new project-owned
  icon family: fixed `18x18` bag/slot icons, the `42x42` detail icon, all ten
  empty-slot symbols, Recruit Pack and Marsh Tonic art, occupied-slot item art,
  readable text/tooltips, and no row, footer, panel, or hotbar layout shift. The
  saved player-facing label is now `Recruit Pack`; confirm it fits both its row
  and detail header without clipping or crowding `Equipment`. Also check that
  an item without direct art uses its category fallback.
- Retain the accepted reward-popup and bag-to-Back flow, then physically inspect
  the one state Slate cannot freeze: the populated fantasy drag token in
  motion. It should retain fixed `236x62` bounds and show the resolved item
  illustration in its `44x44` sigil cell. The accepted reward popup should
  retain its fixed `32x32` item icon and readable text. Category-sigil and
  missing-art fallbacks must remain readable when no texture resolves.
- In clean PIE, retain the verified input baseline: `Q` advances the player and
  independent `W` and `S` presses stop all later movement. Physically press
  `Ctrl+M` and confirm inversion feedback; Slate's atomic chord cannot span the
  controller's later `PlayerTick` poll, so MCP cannot prove that specific UI
  message without changing the input implementation.
- Complete the populated-inventory interaction check for Recruit Pack and Marsh
  Tonic: readable item/context text, no panel or hotbar overlap, gold valid-drop
  state, red wrong-slot state, unchanged click/keyboard fallbacks, and atomic
  rollback. The icon-bearing drag-token automation contract already passes.
- Recheck the accepted road threshold through `Embermere_RoadGate_01`, between
  both boundary fences, and inside the two new rune-topped boundary stones.
  Native validation already proves three clear center lanes plus solid gate,
  fence, and end-stone collision. The two masking trees now sit in the south
  foliage band without erasing the authored silhouette.
- Walk normally to the Prowler homes at `(1900,300)`, `(1700,1100)`, and
  `(2500,1300)`. Confirm each 525 cm pull stays solo, enemies cross visual-only
  marker geometry, and the two idle Prowlers remain home. Tune leash/respawn
  only when live feel exposes a concrete issue.
- After visual acceptance, continue with the highest-value bounded slice:
  implement real starter root/snare/buff/mana-recovery behavior and class-loop
  coverage, add a restrained illustrated paper-doll backdrop, build a compact
  original Blender village prop, or begin cohesive fantasy village architecture
  when a suitable signed-in UE pack is available.

## Full Manual Regression Checklist

- Restart Unreal before manual PIE when the editor predates the 2026-07-23
  ability-icon/hotbar build. Current headless code passes all 23 tests.
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
  - `Embermere.Enemy.LeashRules`
  - `Embermere.Enemy.LootRules`
  - `Embermere.Equipment.SlotRules`
  - `Embermere.Equipment.InventoryTransactions`
  - `Embermere.Equipment.StatApplication`
  - `Embermere.Inventory.CapacityTransactions`
  - `Embermere.Inventory.ConsumableUse`
  - `Embermere.Inventory.IdentityActions`
  - `Embermere.Inventory.StableSorting`
  - `Embermere.Input.AutorunCancellation`
  - `Embermere.Quests.CompletionRewards`
  - `Embermere.Rules.RaceClassMatrix`
  - `Embermere.Stats.DamageImmunity`
  - `Embermere.UI.ChatLog`
  - `Embermere.UI.EnemyNameplateWidget`
  - `Embermere.UI.HotbarCooldownDisplay`
  - `Embermere.UI.IconPresentation`
  - `Embermere.UI.InventoryDragDrop`
  - `Embermere.UI.InventoryToggle`
  - `Embermere.UI.ItemComparison`
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
- Manually verify selected-target world readability in PIE:
  - `Tab` shows the selected enemy's UMG nameplate, selected marker, HP text, HP bar, and saturated orange-gold segmented mesh target ring above the visual combat-pocket band rather than buried inside it;
  - the nameplate accent/HP bar changes toward red/orange as enemy HP falls;
  - switching targets clears the old enemy marker;
  - killing the selected enemy clears the target and marker.
- Manually verify the live control fix in PIE:
  - `W`/`S` should cancel autorun.
  - `Ctrl+M` should toggle mouse Y inversion and show a temporary message.
  - The fix now runs through the pawn/controller path known to receive live movement input, and the project builds successfully.
- Manually verify hotbar cooldown feedback in PIE:
  - Press `1` to use `Strike`.
  - Confirm the slot dims and displays its live cooldown countdown.
  - Press `1` again before cooldown expires and confirm the bottom-left chat/combat log shows a ready-time message.
  - Confirm non-empty failed hotbar activations show an `Unable to use ...` message instead of silently failing.
- Manually verify the new Mara marker in PIE:
  - Mara should show a temporary gold `!` and name marker above her.
- Manually verify the first `FabPass_` environment layer in PIE:
  - the map has a blue atmospheric sky, readable ambient fill, and muted moss foundation instead of the previous black sky/white ground presentation;
  - all 62 remaining Fab art-pass actors stand upright, the original waystone sits at road elevation, both original ember lamps replace the former sci-fi lamps, and the original road signpost reads clearly beside the route;
  - the three oversized sci-fi building shells are gone and PlayerStart has an unobstructed view toward Mara;
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
- first-pass selected-target UMG nameplate widget plus a flat 24-segment rotating/pulsing emissive gold target ring;
- first-pass hotbar cooldown enforcement and ready-time feedback;
- live hotbar slot countdown and unavailable-state dimming;
- fixed hotbar ability illustrations with class-specific palettes, data-driven
  soft references, accessible stat/description tooltips, and missing-art
  fallback;
- first local Fab/Epic environment pass over the village, road, wilderness pocket, and ruin landmark;
- a project-owned Blender waystone, ember-lamp, road-signpost, road-gate, and boundary-fence family with reviewed scripts, editable source, FBX, previews, authored collision, and seven validated saved placements;
- a project-owned fantasy inventory drag token with category sigils, contextual item text, and fixed bounds;
- first Mac-friendly sky, ambient fill, fog-readability correction, and muted moss foundation material;
- automation coverage for the race/class matrix, quest completion rewards, selected-target presentation, dead-caster rejection, enemy leash and loot rules, equipment slot/stat/transaction rules, atomic inventory capacity, consumable use, item comparison/tooltips, identity-based inventory and drag/drop actions, autorun cancellation, damage immunity, enemy nameplate widget, chat log, hotbar cooldown display, item/slot icon presentation, ability icon presentation, and inventory toggle.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The environment is upright, spawn-safe, and readable, while inventory/equipment now has clickable and draggable gear, project-owned drag and icon presentation, stable explicit sorting, hover inspection, item comparison, and lossless transactional RPG rules rather than display-only state. Starter combat feeds inventory through Marsh Tonic drops, closing the first damage-loot-recovery loop. Clean PIE has verified the full quest, a real bag-to-Back drag, the icon-bearing reward popup, identity-preserving Sort, solo Prowler pulls, collision-cleared encounter markers, targeting, the original-art road family, and transform-proven `W`/`S` autorun cancellation. The new sixteen-ability hotbar art family, populated drag token while in motion, and physical `Ctrl+M` feedback remain the immediate clean-restart visual checks. The world remains stylistically mixed without real fantasy village buildings or final character art, and several class abilities still use placeholder effects rather than their intended root, snare, buff, or recovery behavior.

## Next Work

- Replace temporary selected-target text with better world readability:
  - manually verify the new UMG nameplate widget, selected marker, HP bar, and HP-aware accent color in PIE;
  - visually verify the flat animated 24-segment ring using `M_EmbermereTargetRing`, then decide whether rune/soft-edge texture art warrants a decal revision;
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
    consider a restrained paper-doll body backdrop.
- Finish starter-class combat identity:
  - visually approve all sixteen new ability icons and class palettes in the
    fixed hotbar layout after a clean editor restart;
  - verify hover tooltips and cooldown dimming/countdown without layout shift;
  - replace placeholder root, snare, defensive buff, offensive buff, and mana
    recovery effects with bounded functional behavior plus tests.
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
  lamps, road signpost, road gate, boundary fences, and end stones into compact
  village pieces before attempting rigged characters or creatures.
- Preserve the collision-cleared 525 cm solo-pull baseline while tuning leash,
  return-home speed, damage, and respawn timing only from normal-route PIE.
- Tune player respawn delay, protection duration, and recovery rules after in-editor playtesting.
- Keep automation coverage growing around cooldowns, death/respawn, targeting, and hotbar behavior.
- Add a restrained paper-doll backdrop after the complete item/slot/ability
  icon family survives clean-restart PIE.

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
