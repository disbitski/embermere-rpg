# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

For a fresh Codex task or context reset, read [Docs/THREAD_HANDOFF.md](Docs/THREAD_HANDOFF.md) before this daily file.

## Start Here

- Restart Unreal before manual PIE if the editor has been open since the 2026-07-12 paper-doll, equipment-stat, Marsh Tonic loot/use, and target-ring-color C++ build. The current source builds and headless automation passes, but the open editor still has the pre-build module loaded.
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
  - `Embermere.Equipment.StatApplication`
  - `Embermere.Inventory.ConsumableUse`
  - `Embermere.Quests.CompletionRewards`
  - `Embermere.Rules.RaceClassMatrix`
  - `Embermere.Stats.DamageImmunity`
  - `Embermere.UI.ChatLog`
  - `Embermere.UI.EnemyNameplateWidget`
  - `Embermere.UI.HotbarCooldownDisplay`
  - `Embermere.UI.InventoryToggle`
- Manually verify the styled first-pass HUD in PIE:
  - player HP, mana, XP, health bar, and mana bar are visible;
  - target panel appears after `Tab` and shows target HP plus range state;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - all hotbar slots show stable labels for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`;
  - Mara interaction shows the dialogue panel;
  - the structured inventory window starts empty and shows its title, `Slots 0 / 24`, clickable item list, selected-item detail pane, quest-reward hint, and keyboard/mouse footer;
  - the inventory starts in cursor-aware game/UI mode; pressing `I` hides it, hides the cursor, and restores classic game-only mouse input;
  - clicking a populated row or pressing `[` and `]` changes the inspected inventory stack after multiple stacks exist;
  - Mara's Recruit Pack identifies as level-1 Back armor with `+5 HP, +1 Armor`; equipping it updates the Back slot, bonus totals, HP to `105/105`, the `[E]` bag marker, action label, and chat, while unequipping cleanly restores base stats;
  - the 700x330 inventory/equipment window keeps all three columns and its footer inside the shaded panel without overlapping the hotbar;
  - defeating a Marsh Prowler drops one stackable Marsh Tonic, posts a loot message, and adds it to inventory;
  - after taking damage, selecting Marsh Tonic and clicking `Use` restores up to 25 health and 10 mana and consumes one tonic, while full resources disable use and preserve inventory;
  - quest completion/reward shows the loot popup and the inventory panel lists the reward item;
  - combat, target, quest, XP, inventory, mouse, cooldown, and death/recovery messages appear clipped as single-line rows inside the bottom-left chat/combat log instead of overlapping the top-left player status panel or spilling beyond the chat panel border.
- Manually verify selected-target world readability in PIE:
  - `Tab` shows the selected enemy's UMG nameplate, selected marker, HP text, HP bar, and saturated orange-gold segmented mesh target ring above the raised combat platform rather than buried inside it;
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
  - all 65 remaining art-pass actors stand upright instead of inheriting intended yaw as pitch;
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
- mouse-clickable inventory rows and cursor-aware game/UI input while the inventory is open;
- data-driven item categories, paper-doll equipment slots, level requirements, stat-bonus fields, and a first Equip/Unequip inventory action;
- visible equipment slot/bonus pane, idempotent health/mana/armor/power application, equipped-row markers, armor mitigation, and safe consumable depletion;
- data-driven Marsh Tonic consumables dropped by starter enemies so recovery is reachable through the normal combat loop;
- first-pass selected-target UMG nameplate widget plus a flat 24-segment rotating/pulsing emissive gold target ring;
- first-pass hotbar cooldown enforcement and ready-time feedback;
- live hotbar slot countdown and unavailable-state dimming;
- first local Fab/Epic environment pass over the village, road, wilderness pocket, and ruin landmark;
- first Mac-friendly sky, ambient fill, fog-readability correction, and muted moss foundation material;
- automation coverage for the race/class matrix, quest completion rewards, selected-target presentation, dead-caster rejection, enemy leash and loot rules, equipment slot/stat rules, consumable use, damage immunity, enemy nameplate widget, chat log, hotbar cooldown display, and inventory toggle.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The environment is upright, spawn-safe, and readable, while inventory/equipment now has meaningful RPG rules rather than display-only slots. Starter combat now feeds inventory through Marsh Tonic drops, closing the first damage-loot-recovery loop. The newly linked 700px paper-doll pane, live stat changes, loot/use flow, and saturated target ring still need visual PIE review after a clean editor restart. The world remains stylistically mixed without real fantasy village buildings or final character art.

## Next Work

- Replace temporary selected-target text with better world readability:
  - manually verify the new UMG nameplate widget, selected marker, HP bar, and HP-aware accent color in PIE;
  - visually verify the flat animated 24-segment ring using `M_EmbermereTargetRing`, then decide whether rune/soft-edge texture art warrants a decal revision;
  - tune screen-space widget size/height against camera distance.
- Improve inventory presentation:
  - verify the 700x330 bag/detail/equipment layout, Recruit Pack stat changes, `[E]` marker, and Equip/Unequip action in PIE;
  - manually verify mouse row/action clicks, cursor capture/release, and bracket-key item cycling once inventory has multiple stacks;
  - verify Marsh Prowler deaths grant Marsh Tonic, repeated drops stack, and `Use` heals damaged players without wasting a full-resource tonic;
  - decide whether equipped gear should remain represented in the bag or move through a full inventory/equipment transaction model;
  - add richer graphical slot controls after the three-column text paper doll survives PIE.
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
- Tune starter enemy aggro, leash radius, return-home speed, attack range, damage, and respawn timing after in-editor playtesting.
- Tune player respawn delay, protection duration, and recovery rules after in-editor playtesting.
- Keep automation coverage growing around cooldowns, death/respawn, targeting, and hotbar behavior.
- Add graphical body-slot art or individual slot controls after the text paper doll survives PIE.

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
