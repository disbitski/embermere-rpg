# Embermere Journey

This is the running build journal for Embermere: a classic high-fantasy, tab-target RPG prototype built in Unreal Engine with Codex and Unreal MCP.

## 2026-06-24 - Project Seed

We decided to build a small early-2000s MMO-inspired RPG slice: third-person camera, tab targeting, hotbar combat, a starter village, wilderness enemies, a simple quest loop, and swappable Unreal-first assets.

Key design calls:

- Working title: Embermere.
- Art direction: Stylized Classic high fantasy.
- First version: single-player with MMO feel, not real multiplayer.
- Engine: Unreal Engine 5.8.
- AI/editor bridge: Unreal MCP.
- Asset approach: Unreal-first via Fab/Marketplace, with gameplay kept asset-agnostic.

Initial playable fantasy:

- One village safe area.
- One wilderness combat pocket.
- One ruin landmark.
- One quest NPC.
- Starter enemies with tab targeting, nameplates, loot, and respawn.
- First quest: accept, defeat/recover objective, return for XP and item.

Starting races:

- Human
- Elf
- Dwarf
- Gnome
- Dark Elf
- Lizardman
- Ogre
- Bullywug

Starting classes:

- Warrior
- Cleric
- Ranger
- Wizard

Important race/class rules:

- Dwarf: Warrior, Cleric only.
- Bullywug: Warrior, Cleric, Ranger only.

Initial implementation:

- Created the UE 5.8 project scaffold.
- Added C++ gameplay components for stats, combat, targeting, hotbar, inventory, quests, and interactables.
- Added classic MMO input bindings.
- Added player/enemy character classes and game mode.
- Added UMG base classes for character creation and HUD widgets.
- Added Unreal/MCP setup docs and validator script.

## 2026-06-25 - Unreal MCP Online

Unreal Engine 5.8 is installed and the project opens on macOS after finishing the Xcode and Metal Toolchain setup. We started Unreal MCP on port `8123`, generated the Codex client config, and confirmed Codex can call Unreal editor toolsets.

First editor-created assets:

- `/Game/Blueprints/BP_EmbermereCharacter`
- `/Game/Blueprints/BP_StarterEnemy`
- `/Game/Blueprints/BP_EmbermereGameMode`
- `/Game/Data/DA_EmbermereRules`
- `/Game/Maps/L_Embermere_Prototype`

First map pass:

- Saved the starter map as `L_Embermere_Prototype`.
- Built a greybox village safe area, wilderness combat pocket, road markers, and ruin landmark.
- Placed a `PlayerStart`, quest/vendor/trainer placeholders, three `BP_StarterEnemy` instances, visible enemy markers, and lighting.
- Added `Scripts/setup_prototype_level.py` so the blockout can be rebuilt from Unreal's Python console.

First playable loop hooks:

- Added a first quest giver Blueprint, starter quest data, and starter reward item data.
- Wired `F` as a prototype interaction fallback when the tenth hotbar slot is empty.
- Added temporary on-screen messages for dialogue, quest acceptance/progress/completion, and ability hits.
- Added a race/class automation test for the starting matrix, including Dwarf Ranger and Bullywug Wizard restrictions.
- Added prototype enemy defeat and respawn behavior so the wilderness pocket can be tested repeatedly.
- Added first-pass enemy aggro, chase, and melee attack behavior for the greybox wilderness pocket.
- Added temporary XP and reward item messages so quest completion has visible payoff before the real loot UI exists.
- Added temporary target feedback for selected enemy name, HP, and range state.
- Added temporary player death feedback and autorun shutdown when the controlled character dies.
- Added prototype player respawn at the spawn point after a short delay.
- Added an automation test proving quest completion grants XP and the reward item.
- Updated classic controls so `W`/`S` cancel autorun and mouse Y starts inverted with `Ctrl+M` as the toggle.
- Added the Fab asset plan and editor playtest checklist.

## 2026-06-27 - Daily Build Cadence

We created a daily automation rhythm around `TODO.md`: start with the handoff, build as much as the Pro window reasonably allows, verify with builds/tests/PIE where possible, commit/push intentional work, and leave the next starting point documented.

First presentation/readability pass:

- Added a runtime world marker for interactable actors.
- Quest givers now show a temporary gold `!` and name marker above them in Play In Editor, making Mara easier to find without committing the dirty saved map package.
- Verified the project builds, the two Embermere automation tests pass, and PIE boots cleanly after the marker change.

## 2026-06-28 - First HUD Overlay

The prototype now creates a native first-pass player HUD at runtime. It is intentionally simple and data-bound so we can see core MMO state before investing in final UI art.

HUD pass:

- Shows player HP, mana, and XP in the viewport.
- Shows the current tab target and target HP when a target is selected.
- Shows active quest title and objective progress.
- Shows the first four hotbar slots plus the `F` interact reminder.
- Builds from C++ by default, while still allowing a future Blueprint HUD class override.

Verification:

- Built the project successfully after the HUD changes.
- Ran the Embermere quest reward and race/class matrix automation tests: 2 passed, 0 failed.
- Booted PIE through MCP and confirmed the starter map, game mode, and Enhanced Input initialized without gameplay or Blueprint errors.

## 2026-06-28 - Styled HUD Panels

We pushed the HUD from raw text toward a playable early-MMO layout. It is still programmer art, but the screen now has stable panels and clearer combat/quest feedback.

UI pass:

- Added fixed status, target, quest, dialogue, loot, and hotbar sections.
- Added health/mana/target HP progress bars.
- Expanded the hotbar to show all configured prototype slots: `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`.
- Added target range state for the first hostile ability.
- Routed Mara dialogue into a temporary dialogue panel.
- Routed inventory reward events into a temporary loot popup.

Verification:

- Built successfully after the richer native UMG layout.
- Ran the Embermere automation tests again: 2 passed, 0 failed.
- Booted PIE through MCP after the UI changes; the map, game mode, and Enhanced Input initialized cleanly with no UMG/runtime errors in the log.

Lesson learned:

- Pure C++ `UUserWidget` layouts need their widget tree built during `RebuildWidget()`, not only in `NativeConstruct()`. The HUD object can exist and PIE can boot cleanly while the visible Slate tree is still empty. We captured the long-term note in [Docs/UNREAL_LESSONS.md](Docs/UNREAL_LESSONS.md).

## 2026-06-29 - Target Readability And Inventory Panel

The prototype gained two more pieces of MMO readability: selected enemies are easier to identify in the world, and quest rewards are inspectable without relying only on temporary debug text.

Targeting pass:

- Extended targetables with selected/unselected notifications.
- Added native C++ dispatch for target selection so C++ targetables do not depend on Blueprint interface event routing.
- Marsh Prowlers now show a temporary overhead `TARGET` marker plus name and HP text when selected.
- Target presentation clears when switching targets, clearing the target, or killing the selected enemy.
- Added automation coverage for selected-target presentation.

Inventory pass:

- Added a top-right native HUD inventory panel.
- The panel starts as `Inventory / Empty`.
- Reward item stacks now appear in the panel after quest completion.

Verification:

- Built the project successfully after the targeting and inventory changes.
- Ran stable automation tests through MCP: race/class matrix and quest completion rewards passed.
- Ran PIE HUD smoke through MCP and confirmed visible `Level 1`, `Inventory`, and full hotbar text including `Alt+R`.
- The new target-presentation automation test was added, but the already-running editor retained stale hot-reload state during the daily run. Next verification should restart Unreal before running `Embermere.Combat.TargetSelectionPresentation`.

## 2026-06-30 - Target Ring And Inventory Toggle

We verified the previous target-presentation work in a fresh editor session, then tightened the next layer of playability without importing art yet.

Verification:

- Rediscovered the Embermere automation tests in Unreal MCP.
- Confirmed the stale `Embermere.Combat.TargetClearsOnDeath` registration was gone.
- Ran the current pre-change test set: target presentation, quest reward completion, and race/class matrix all passed.

Target readability pass:

- Added a prototype selected-target ground ring under selected enemies.
- Kept the overhead `TARGET`, name, and HP text, but added HP-aware nameplate coloring.
- Left the ground ring intentionally asset-light for now; the next art pass should replace it with a real decal/mesh/material.

Inventory pass:

- Added `I` as a show/hide toggle for the native inventory panel.
- Added the `Inventory (I)` hint to the panel header.
- Added automation coverage for inventory toggle state.

Test hardening:

- Changed the target-presentation automation test to use transient objects instead of spawning hot-reloaded actors in a transient world. This preserves the selection contract coverage while avoiding an Unreal hot-reload component registration failure.

Verification after changes:

- Built successfully.
- In the already-open editor after hot reload, quest reward, race/class matrix, and inventory toggle tests passed. The target-presentation test still hit Unreal's hot-reloaded actor registration path in that live editor session, so the next run should restart Unreal before treating that test result as authoritative.
- PIE HUD smoke verified visible `Level 1`, `Inventory (I)`, and full hotbar text including `Alt+R`.

## 2026-07-01 - Mesh Target Ring And Inventory Details

We started by validating the previous day's hot-reload-sensitive target work in MCP.

Verification:

- Rediscovered the current Embermere automation tests.
- Ran target presentation, quest reward completion, race/class matrix, and inventory toggle: 4 passed, 0 failed.
- After today's C++ build, the same four tests still passed. The already-open editor emitted no-world warnings from the hot-reloaded target test path, so a clean restart remains the best way to confirm warning-free automation.

Target readability pass:

- Replaced the debug-drawn target ring with actual static mesh segment components attached to the enemy.
- Kept the implementation asset-light by using engine basic cube meshes and an engine debug material.
- The ring remains replaceable: later Fab/decal/material work can swap this component treatment without touching targeting logic.

Inventory pass:

- Expanded the inventory panel slightly.
- Added detail text for the first visible inventory stack, using item description when available or stack limit as a fallback.

Verification after changes:

- Built successfully.
- PIE HUD smoke verified visible `Level 1`, `Inventory (I)`, and full hotbar text including `Alt+R`.
- Log scan found no current gameplay or Blueprint errors.

## 2026-07-02 - Target Marker Polish And Inventory Inspection

We tightened the first-pass selected-target and inventory presentation while keeping the systems asset-agnostic.

Target readability pass:

- Replaced the loud overhead `TARGET` text with a smaller selected marker.
- Routed selected-target nameplate text through a reusable helper: enemy name plus `HP current/max`.
- Added HP-aware presentation color so wounded enemies shift toward orange/red.
- Swapped the segmented target ring off the engine debug material and onto a gold-tinted basic material path.
- Added automation assertions for target presentation text and low-health color behavior.

Inventory pass:

- Expanded the inventory panel footprint.
- Changed reward inspection from description-only text to a clearer first-item inspection block with item name and stack count.

Verification:

- Built successfully after the C++ changes.
- Ran the four current Embermere automation tests through Unreal MCP: target presentation, quest reward completion, race/class matrix, and inventory toggle all passed.
- Started and stopped PIE through MCP after the build. Because the editor stayed open through multiple C++ builds, the target test still reported hot-reload no-world warnings; the next visual/manual pass should restart Unreal before treating warning absence as authoritative.

## 2026-07-03 - Native Enemy Nameplate Widget

We moved selected enemy presentation another step away from text-render programmer art and toward proper MMO UI.

Target readability pass:

- Added `UEmbermereEnemyNameplateWidget`, a native UMG widget for selected enemies.
- Mounted the nameplate on enemies with a screen-space `UWidgetComponent`.
- The widget shows a selected marker, enemy name, HP text, and an HP progress bar.
- HP-aware accent coloring now drives the UMG nameplate and health bar.
- Kept the old text-render nameplate/marker components as fallback if the widget path is unavailable.

Verification:

- Built successfully after adding the widget component.
- Ran the current MCP automation suite: target presentation, quest reward completion, race/class matrix, enemy nameplate widget, and inventory toggle all passed with no warnings.
- Booted and stopped PIE through MCP. Log scan found no current Embermere gameplay or Blueprint errors; only old engine self-test log strings and the macOS audio sample-rate warning appeared.

Asset pass note:

- Direct unauthenticated Fab search/API calls from terminal automation hit Cloudflare security checks.
- The first real asset import should use the signed-in Unreal Fab window or Epic Games Launcher, then Codex can inspect and integrate the imported content.

## 2026-07-03 - Bottom-Left Chat And Combat Log

We moved gameplay feedback away from Unreal's top-left debug overlay so it no longer collides with the player status panel.

HUD pass:

- Added a native bottom-left chat/combat log panel to `UEmbermerePlayerHudWidget`.
- The log keeps the seven most recent lines visible near the lower-left of the screen, above the hotbar.
- Routed target, combat, quest, XP, inventory reward, dialogue, mouse inversion, death, recovery, enemy defeat, and enemy respawn messages through the HUD log.
- Kept the engine debug overlay only as a fallback if the HUD is unavailable.

Verification:

- Built successfully after the HUD message routing.
- Ran clean headless automation for the full Embermere suite: 6 passed, 0 failed, 0 warnings.
- Live MCP automation in the already-open editor still saw stale hot-reloaded registrations and only listed the older four-test set, so the next manual PIE check should restart Unreal before judging the new chat log visually.

## 2026-07-04 - Chat Log Clipping

We tightened the first-pass chat/combat log after playtesting showed text could spill below the shaded panel border.

HUD pass:

- Added explicit clipping to the chat panel, message stack, and individual chat text rows.
- Reduced visible chat history to the configured message limit.
- Set a fixed wrap width for chat rows so long combat lines wrap inside the shaded panel.
- Kept the panel intentionally WIP; final height, line count, and fantasy styling still belong to a later UI art pass.

Verification:

- Built successfully after the clipping change.
- Ran clean headless automation for the full Embermere suite: 6 passed, 0 failed, 0 warnings.
- MCP was reachable, but because this was a fresh C++ build, visual PIE verification should happen after restarting Unreal.

Asset pass note:

- No local `/Game/ThirdParty/Fab` import was present during this run, so there was nothing for Codex to integrate yet.
- The next asset step remains a signed-in Fab/Epic Launcher import of one small stylized environment or village pack.

## 2026-07-05 - Inventory Inspection Cycling

We made the top-right inventory panel more useful before the full inventory screen exists.

Inventory pass:

- Added bracket-key inspection cycling with `[` and `]`.
- The inventory panel now shows `Inspecting X/Y` and marks the selected stack with `>`.
- Selection wraps forward and backward across multiple stacks.
- The panel still stays compact and programmer-art, but it now supports more than a single reward item.

Verification:

- Built successfully after the inventory cycling change.
- Expanded `Embermere.UI.InventoryToggle` to cover selection advance and wraparound.
- Ran clean headless automation for the full Embermere suite: 6 passed, 0 failed, 0 warnings.

## 2026-07-05 - First Local Fab Zone Pass

We moved the starter zone out of pure greybox for the first time.

Art pass:

- Integrated the locally imported Epic/Fab packs already installed in the project: KiteDemo, SoulCave, ParagonProps, and Scifi_desert_city.
- Added `.gitignore` rules so raw marketplace asset folders stay local and are not redistributed through the public repo.
- Added reusable placement scripts for a tagged `FabPass_` environment layer.
- Replaced the old visual-only village blockout buildings, road markers, and ruin blockout props with 68 placed environment actors.
- Used Paragon and Soul Cave assets for ruins, stone, roots, fog/water accents, and high-fantasy landmark shapes.
- Used Kite/Soul nature assets for road and wilderness dressing.
- Used the sci-fi desert kit sparingly for neutral prototype village shells, crates, fabric, lamps, fences, tables, and stools.
- Kept gameplay actors in place: PlayerStart, Mara, starter enemies, quest flow, HUD, inventory, target UI, and combat logic were not intentionally moved.

Implementation note:

- The first MCP placement attempt kicked off heavy one-time mesh/texture compilation and the MCP listener dropped before returning a result.
- A fallback Unreal Python commandlet path completed the pass and saved the map.
- Some Kite pine/bush assets exist on disk but did not resolve cleanly through the UE 5.8 headless asset registry, so the placement script currently uses confirmed-working substitutes.

Verification:

- The fallback placement completed with 68 created `FabPass_` actors and 0 skipped actors.
- The headless map validator passed with 68 `FabPass_` actors and required gameplay anchors intact.
- Built successfully and ran clean headless automation for the full Embermere suite: 6 passed, 0 failed, 0 warnings.
- Manual PIE still needs a fresh editor restart and local asset load check before treating collision, scale, and style cohesion as final.

## 2026-07-06 - Cooldowns, Death Safety, And Test Hygiene

We tightened the core combat loop before doing more map-art polish.

Combat pass:

- Combat execution now rejects abilities from dead characters.
- Hotbar slots now track ability cooldowns after successful activation.
- Reusing a cooling-down hotbar ability posts a bottom-left ready-time message instead of silently doing nothing.
- Failed non-empty hotbar activation now posts an `Unable to use ...` message, which should help early playtests distinguish cooldown, range, target, and mana issues.

HUD pass:

- The bottom-left chat/combat log now uses clipped single-line rows, so long combat lines should stay inside the shaded panel instead of wrapping below its border.

Automation pass:

- Added `Embermere.Combat.DeadCasterRejected` to cover the death/recovery combat contract.
- The full headless Embermere suite now has 7 tests.

Build lesson:

- Commandlet automation was initially loading stale assertion text from an older hot-reload module while the editor was open.
- Building with `-NoHotReloadFromIDE` cleaned numbered hot-reload dylibs and relinked the base module, after which the current tests ran correctly.

Verification:

- Built successfully with `-NoHotReloadFromIDE`.
- Ran clean headless automation for the full Embermere suite: 7 passed, 0 failed, 0 warnings.
- Manual PIE still needs a fresh editor restart before verifying the FabPass layer, cooldown message, chat row clipping, and live UI behavior.

## 2026-07-07 - Enemy Leashing

We tightened starter enemy behavior for the dressed FabPass zone.

Enemy AI pass:

- Added configurable enemy leash radius, return-home radius, and return-home speed.
- Enemies now drop aggro and return toward their spawn when pulled too far from their home pocket.
- Idle enemies that drift away can return home instead of staying displaced.
- Enemy death and respawn now clear the return-home state.

Automation pass:

- Added `Embermere.Enemy.LeashRules` to cover leash and return-home distance thresholds.
- The full headless Embermere suite now has 8 tests.

Build lesson:

- Running multiple Unreal commandlets in parallel caused a shared UBT `Trace.uba` race.
- Added a lesson to keep `UnrealEditor-Cmd` build/test/validation runs sequential.

Verification:

- Confirmed local Fab/Epic folders are present: KiteDemo, SoulCave, ParagonProps, and Scifi_desert_city.
- Built successfully with `-NoHotReloadFromIDE`.
- Ran clean headless automation for the full Embermere suite: 8 passed, 0 failed, 0 warnings.
- Reran the FabPass validator sequentially after the parallel commandlet race; it exited successfully.
- Manual PIE still needs a fresh editor restart before validating leash feel, collision, scale, route readability, and UI behavior in the actual FabPass level.

## 2026-07-08 - Respawn Protection

We added a small recovery safety pass for the prototype death loop.

Stats pass:

- Added temporary damage immunity to `UEmbermereStatsComponent`.
- Damage immunity blocks `ApplyDamage()` while active and can be cleared explicitly.
- `InitializeVitals()` clears any previous immunity, so respawn/recovery starts from a known state.

Respawn pass:

- Player respawn now grants a short configurable protection window.
- The recovery chat message now states the protection duration.

Automation pass:

- Added `Embermere.Stats.DamageImmunity`.
- The full headless Embermere suite now has 9 tests.

Verification:

- Built successfully with `-NoHotReloadFromIDE`.
- Ran clean headless automation for the full Embermere suite: 9 passed, 0 failed, 0 warnings.
- FabPass validator still exits successfully when run sequentially.
- Manual PIE still needs a fresh editor restart before validating the recovery message, actual damage protection feel, leash behavior, and FabPass collision/readability.

## 2026-07-09 - Inventory Empty And Reward States

We tightened the first-pass inventory HUD so the reward flow is clearer before building a full inventory screen.

Inventory pass:

- Added `GetInventoryDisplayText()` as a reusable HUD API.
- The live inventory panel and automation now share the same text-building path.
- Empty inventory now explicitly says quest rewards appear there.
- Multi-stack inventory now includes the bracket-key inspection hint in the generated display text.
- Selected stack details still show item name, quantity, stack limit, and description.

Automation pass:

- Expanded `Embermere.UI.InventoryToggle` to assert empty-state text, selected stack labels, stack quantities, max-stack display, item descriptions, and the bracket-key inspection hint.
- The full headless Embermere suite remains at 9 tests, with stronger inventory assertions.

Verification:

- Built successfully with `-NoHotReloadFromIDE`.
- Ran clean headless automation for the full Embermere suite: 9 passed, 0 failed, 0 warnings.
- FabPass validator still exits successfully when run sequentially.
- Direct MCP initialized, but raw `tools/call` streaming did not return usable editor-control output in this session. Manual PIE still needs a fresh editor restart before validating the live inventory panel, recovery behavior, leash feel, and FabPass collision/readability.

## 2026-07-09 - Native MCP, Upright World, And Real Inventory

The refreshed ChatGPT desktop app loaded Unreal MCP as first-class tools, turning the editor workflow from a direct-HTTP fallback into a live inspect/change/verify loop.

Environment recovery:

- Ran the existing nine-test baseline through Unreal MCP: 9 passed, 0 failed, 0 warnings.
- Used Slate screenshots to discover that PlayerStart was buried behind oversized, tilted Fab architecture.
- Traced the cause to positional `unreal.Rotator` construction mapping intended yaw values into pitch.
- Corrected all 68 placed actor rotations, including three Euler-normalized values above 90 degrees.
- Removed the three oversized sci-fi building shells and restored PlayerStart's readable route toward Mara.
- Updated the placement helper to assign named pitch/yaw/roll fields.
- Strengthened the map validator and saved the level through Unreal's asset API.
- Fresh headless validation passed with 65 upright Fab actors and required gameplay anchors intact.

Player-facing UI and targeting:

- Replaced the compact inventory text block with a structured MMO-style inventory window: capacity header, eight visible item rows, selected-item detail pane, description, empty/reward state, and keyboard footer.
- Preserved `I` show/hide and `[`/`]` selection cycling.
- Changed the selected-target ground ring from 18 chunky cube segments to 24 flat plane segments with subtle rotation and pulse.
- Added live hotbar cooldown countdowns and dimmed cooling slots.
- Added `Embermere.UI.HotbarCooldownDisplay`; the full headless suite now contains 10 tests.

Verification:

- Built successfully with `-NoHotReloadFromIDE`.
- Ran the final suite: 10 passed, 0 failed, 0 warnings.
- Reran the corrected Fab map validator successfully.
- The interactive editor still needs one clean restart before final visual PIE review of the newly linked inventory, hotbar, and target-ring code.

Knowledge loop:

- Published [When Unreal MCP Started Feeling Native](https://github.com/disbitski/real-world-ai-lab/blob/main/field-notes/2026-07-09-unreal-mcp-feels-native.md) with two flashcards.
- Added a private LinkedIn draft that separates observed end-to-end improvement from unproven model-only attribution.

## 2026-07-10 - Daylight, Mouse Inventory, And A Real Ring Material

We used the clean first-class Unreal MCP connection to verify yesterday's baseline and then pushed three visible prototype systems forward.

Live verification:

- Rediscovered and ran all ten Embermere tests in the restarted editor: 10 passed with zero warnings.
- Captured the running HUD and confirmed the structured inventory and corrected 65-actor Fab layer were present.
- Confirmed the map's largest readability problem was environmental rather than UI: it had no atmospheric sky, its captured skylight was black, and its fog inscattering was black.

Lighting pass:

- Added `SkyAtmosphere` to the saved prototype map and foundational level rebuild helper.
- Changed the skylight to movable real-time capture with soft lower-hemisphere fill.
- Reduced the heavy black fog and added restrained cool ambient plus warm directional inscattering.
- Preserved the existing directional sun and Mac-friendly rendering path.
- Captured PIE again and confirmed a blue sky plus materially better village/route readability.
- Extended the fresh-process Fab validator to require the atmosphere actor; it still passes with 65 upright actors and all gameplay anchors intact.

Inventory pass:

- Replaced passive inventory text rows with real non-focusable buttons.
- Clicking a populated row now selects that stack and refreshes the detail pane.
- Selected rows receive a restrained gold background while empty/invalid rows cannot be clicked.
- Inventory visibility now switches the controller between cursor-aware game/UI input and classic game-only mouse input.
- Preserved `I` show/hide plus `[`/`]` keyboard inspection and added direct-selection automation assertions.

Targeting pass:

- Created `/Game/Art/Embermere/Targeting/M_EmbermereTargetRing`, a tracked unlit additive gold material with runtime `Color` and authoring-time `EmissiveStrength` parameters.
- Wired enemy ring segments to the Embermere material with the engine basic material retained as a fallback.
- Added automation coverage proving enemies resolve the dedicated material.

Verification:

- Built successfully with `-NoHotReloadFromIDE` after the UI/controller/material wiring.
- Ran final clean headless automation: 10 passed, 0 failed, 0 warnings.
- Reran the saved-map validator after the atmosphere requirement; it passed.
- The interactive editor must be restarted once more before visual PIE verification of the newly linked clickable rows and emissive target ring.

## 2026-07-11 - Moss Ground, Equipment Rules, And First Item Actions

We started in the live editor, ran the existing ten-test suite through first-class Unreal MCP, and used PIE plus object inspection to close several presentation gaps.

World and targeting pass:

- Replaced the nearly white prototype foundation with `/Game/Art/Embermere/Environment/M_EmbermereGround`, a tracked muted-moss material applied to the zone plane and both raised area platforms.
- Changed the target-ring material from additive to opaque unlit/emissive so gold survives bright outdoor surfaces.
- Traced the missing ring to geometry rather than visibility: its constructor offset placed the segments inside the raised combat-pocket cylinder.
- Raised the source default for future enemy constructions and documented the layered-surface diagnostic.
- Added ground-material assignment to the level rebuild script and fresh-process validator.

Equipment and item-action foundation:

- Added data-driven item categories, paper-doll slots, level requirements, and health/mana/armor/power bonus fields.
- Added `UEmbermereEquipmentComponent` with equip eligibility, one-item-per-slot replacement, query, and unequip rules.
- Mounted equipment state on Embermere characters without coupling it to vendor meshes or marketplace assets.
- Expanded inventory inspection with category, primary action, slot, and required-level text.
- Added a real inventory action button that equips or unequips the selected eligible item and posts the result to the bottom-left chat log.
- Migrated Mara's tracked Recruit Pack reward to level-1 Back armor so the action is reachable through the normal quest loop.
- Added `Embermere.Equipment.SlotRules` and expanded inventory automation around level gating and Equip/Unequip state.

Durability and verification:

- Recorded the exact successful daylight values in the rebuild script.
- Strengthened saved-map validation to assert sun, skylight, fog, moss ground, 65 upright Fab actors, and gameplay anchors.
- Built successfully with `-NoHotReloadFromIDE`.
- Ran the final suite: 11 passed, 0 failed, 0 warnings.
- Fresh-process map validation passed with the complete daylight and ground baseline.
- The desktop safety layer correctly refused to close the live Unreal session without explicit confirmation, so the new C++ ring clearance and Equip/Unequip button still need a clean-restart PIE visual check.

## 2026-07-12 - Paper Doll, Real Gear Stats, And Safe Consumables

We opened with a clean editor/MCP baseline, discovered the newly linked equipment suite, and passed all eleven existing tests live before touching source.

Clean PIE verification:

- Confirmed the blue atmospheric sky, muted moss ground, corrected 65-actor environment, and unclipped 510x330 inventory shell.
- Accepted Mara's quest through Slate input and verified quest tracker, dialogue/chat feedback, and cursor-aware UI mode.
- Teleported to the combat pocket through MCP, selected a Marsh Prowler, and confirmed the native nameplate and target ring now clear the raised platform.
- Tuned the runtime ring toward a more saturated orange-gold and lowered material emissive strength from `2.5` to `0.75` so outdoor exposure preserves color instead of washing it white.

Equipment and stats:

- Expanded the inventory to a 700x330 three-column layout with bag rows, selected-item details/actions, and a visible equipment slot plus aggregate-bonus pane.
- Added aggregate equipment stat calculation and character-driven refresh on every equipment change.
- Applied health, mana, armor, and power bonuses idempotently by replacing the previous aggregate instead of stacking repeated updates.
- Preserved missing health/mana when maximums change and added a bounded armor-mitigation curve.
- Updated the Recruit Pack to grant `+5 HP` and `+1 Armor`, with effect text and an equipped bag-row marker.

Consumables:

- Added data-driven health/mana recovery effects and a real `Use` action path.
- Successful use applies recovery, consumes exactly one item, refreshes selection, and posts to chat.
- Full-resource use is rejected without consuming inventory.
- Added effect summaries for both gear and consumables.

Verification:

- Added `Embermere.Equipment.StatApplication` and `Embermere.Inventory.ConsumableUse`.
- Built successfully with `-NoHotReloadFromIDE`.
- Added a stackable Marsh Tonic data asset that restores up to 25 health and 10 mana.
- Wired Marsh Prowlers to editable loot item, quantity, and drop-chance settings; their death now grants the tonic to player inventory and posts a loot message.
- Added `Embermere.Enemy.LootRules` for deterministic drop policy, stack delivery, and no-drop behavior.
- Final headless automation passed 14/14 with zero warnings.
- Fresh saved-map/Fab/daylight validation remained green.
- The editor now predates the final C++ relink, so the new 700px paper-doll pane, stat changes, Marsh Tonic drop/use flow, and saturated ring need one clean-restart PIE pass tomorrow.

## 2026-07-13 - Clickable Gear Slots And Lossless Inventory Transactions

We began in a clean live MCP session and verified the player-facing baseline before changing source.

Live verification:

- Ran the existing 14-test suite successfully in the editor.
- Used PIE to accept Mara's quest, target and defeat a Marsh Prowler, advance the objective, and receive a real Marsh Tonic stack.
- Confirmed the blue atmospheric sky, muted moss ground, corrected Fab route, target/nameplate, chat log, hotbar, and three-column inventory were still healthy.

Equipment and inventory pass:

- Replaced the equipment text list with ten stable paper-doll slot buttons arranged as a compact body layout.
- Occupied slots show the item in gold and provide the single explicit click-to-unequip path.
- Equipping now removes one item from the bag; replacement returns the old item; unequipping returns the equipped item only when capacity exists.
- Added preflight and rollback behavior so a full bag cannot lose, duplicate, or partially move equipment.
- Made general inventory add/remove operations atomic as well, preventing failed loot, reward, consumable, and future vendor operations from leaving partial mutations.

Verification:

- Added `Embermere.Equipment.InventoryTransactions` for normal transfer, replacement, unequip, full-bag rejection, and replacement rollback.
- Added `Embermere.Inventory.CapacityTransactions` for atomic add/remove capacity and quantity boundaries.
- Built the Mac editor target with `-NoHotReloadFromIDE`.
- Final headless automation passed 16/16 with zero warnings and zero failures.
- Final saved-map validation passed with 65 upright Fab actors, gameplay anchors, moss ground, and exact daylight baseline intact.
- The interactive editor predates the final relink, so tomorrow starts with a clean-restart visual pass over the slot grid and bag/equipment flow.

## 2026-07-13 - Original Blender Asset Lane

We researched the Blender MCP workflow demonstrated in the shared X article
and adopted it as a complementary art lane rather than a wholesale replacement
for Fab.

Decisions:

- Use Blender first for original static props, ruins, modular architecture,
  signs, lamps, fences, and later weapons.
- Keep Fab for broad foliage, temporary environment coverage, specialist
  content, and animation-heavy assets while our original pipeline matures.
- Build one bounded asset at a time from a written technical brief and require
  deterministic mesh checks plus an Unreal PIE pass. A convincing render is
  evidence, not acceptance.
- Avoid the popular unrestricted raw-code Blender bridge after confirming its
  open arbitrary-code execution report.
- Use the structured `djeada/blender-mcp-server` bridge, pin the reviewed
  version, enable Safe Mode, disable inline code, and run only tracked scripts
  from approved project roots.

Implementation:

- Added `Docs/BLENDER_ASSET_PIPELINE.md` with Mac setup, Codex MCP config,
  security boundaries, source layout, Unreal FBX conventions, and the asset
  acceptance checklist.
- Added `ArtSource/Blender/README.md` to establish ownership and repository
  rules for original source art.
- Added `Scripts/blender/build_embermere_waystone.py`, an idempotent first
  pilot that builds a stylized ember-rune waystone, explicit Unreal collision,
  UVs, validation metrics, a preview render, `.blend` source, and FBX export.
- Kept the Blender MCP entry out of project config until its executable exists,
  so Codex startup does not gain a known-dead server.

Completed first asset loop:

- Installed Blender 5.1.2 and the selected bridge at reviewed commit
  `7eed33edf4aca2ab0ca84a6da27321f89f68b504`.
- Enabled Safe Mode, disabled inline code, bound the add-on to localhost, and
  limited approved scripts to Embermere's Blender source directories.
- Verified all 27 structured MCP tools, proved an approved tracked script can
  run, and proved inline code is rejected.
- Generated the first waystone source, FBX, preview, and deterministic metrics;
  revised its lighting after the first preview rendered too dark.
- Imported the 1,340-triangle mesh into Unreal with three materials, one UV
  channel, ground-level bounds, and two explicit box colliders.
- Replaced the temporary road stump with `Embermere_Waystone_Road_01` and
  separated it from the third-party art layer with `EmbermereOriginalArt`.
- Caught a real placement-order bug when snap-to-ground used the outgoing stump
  as its surface. Corrected the actor to road elevation and added its Z value to
  the saved-map validator.
- Rebuilt the editor, passed all 16 automation tests with zero warnings, and
  passed fresh map validation with 64 upright Fab actors plus the original
  waystone.

The lane is now proven end to end: reviewed Blender source script to editable
`.blend`, preview, FBX, Unreal import, map placement, collision, saved-map
validation, and gameplay regression tests.

## 2026-07-14 - Item Comparison And Drag-Safe Actions

The inventory moved from inspection-only metadata toward the decision support
expected in a classic fantasy RPG.

Implementation:

- Added net HP, mana, armor, and power comparison between an inspected bag item
  and the item currently equipped in its destination slot.
- Added concise hover tooltips to populated bag rows and occupied paper-doll
  slots without enlarging the fixed inventory window.
- Compacted detail metadata so comparison fits inside the existing 700x330
  three-column layout.
- Refactored inventory actions to resolve a live item identity and added an
  explicit target-slot action. Wrong-slot and stale payload requests are
  rejected before any transaction begins.
- Documented the drag payload, bag/equipment drop rules, stable sorting
  contract, implementation order, and edge-case matrix in
  `Docs/INVENTORY_INTERACTION_PLAN.md`.

Verification:

- Added `Embermere.UI.ItemComparison` for upgrades, downgrades, empty slots,
  tooltip content, and consumable presentation.
- Added `Embermere.Inventory.IdentityActions` for matching-slot equip,
  wrong-slot rejection, and stale-payload rejection without duplication.
- Built with `-NoHotReloadFromIDE`; headless automation passed 18/18.
- First-class MCP in the open editor listed only 14 tests, objectively proving
  that the interactive session still held an old module and must restart before
  visual PIE verification.

## 2026-07-15 - Real Drag/Drop And The Ember Lamp Family

The inventory interaction plan became a bounded playable gesture, and the
original Blender lane grew from a one-off proof into the beginning of a visual
family.

Inventory implementation:

- Added a typed `UEmbermereItemDragDropOperation` carrying item identity,
  bag/equipment source, and source equipment slot instead of a fragile row
  index.
- Made bag rows drag sources, matching paper-doll slots drop targets, and the
  bag list an equipment-return target while preserving row clicks, slot clicks,
  action buttons, `[`/`]`, and `I`.
- Added compact item-name drag feedback, gold valid targets, restrained red
  invalid targets, bag-return highlighting, and footer status inside the fixed
  700x330 window.
- Kept all mutations on the existing atomic equipment/inventory paths so stale
  identity, wrong slot, under-level, and full-bag failures cannot lose or
  duplicate gear.
- Added `Embermere.UI.InventoryDragDrop`; the final suite passed 19/19.

Original-art implementation:

- Added `Scripts/blender/build_embermere_ember_lamp.py` and generated editable
  `.blend`, FBX, preview, and metrics for a 254 cm stone/moss/iron lamp with a
  warm faceted ember crystal.
- Blender validation reported 2,184 triangles, one UV channel, applied scale,
  no non-manifold edges, a ground pivot, and two named `UBX_` collision boxes.
- Imported the lamp and `M_EmberLampIron` under the project-owned prototype
  village folder, reused the waystone stone/moss/ember materials, and replaced
  the two temporary sci-fi lamps with Mara-side and road-side original actors.
- Unreal retained 2,168 triangles and both authored box colliders. MCP asset
  imaging and live level captures confirmed the high-fantasy silhouette,
  scale, warm crystal, and road readability.

Pipeline lesson:

- The first generic import silently used UE 5.8 Interchange and persisted
  `bCollision=false`, producing zero colliders despite correct `UBX_` names.
- Pinning `unreal.FbxFactory()` imported both boxes, but atomic replacement kept
  stale Interchange provenance. Deleting only the partial mesh package and
  recreating the same path through the classic factory produced clean
  `FbxStaticMeshImportData` before the map was loaded.
- The import script now validates provenance, bounds, and collision before any
  actor replacement or level save. The saved-map validator repeats those checks
  from a fresh process.

Verification:

- The authoritative no-hot-reload Mac editor build succeeded.
- Headless automation passed 19/19 with no test failures.
- Saved-map validation passed with 62 upright third-party `FabPass_` actors,
  three tagged original-art placements, gameplay anchors, moss ground, and the
  exact daylight baseline intact.
- Clean-restart PIE remains the next manual boundary because the open editor
  predates the final drag/drop relink.

## 2026-07-16 - Clean Quest Regression And Stable Bag Sorting

The previous day's interaction work survived a complete live loop, then the bag
gained a predictable player-controlled order without weakening item identity.

Clean PIE verification:

- Ran the full Mara quest from acceptance through three Marsh Prowler defeats,
  tonic stacking, return, XP, Recruit Pack reward, and inventory update.
- Verified target nameplate/ring, damage and target clear on death, bracket
  cycling, item comparison, valid Back-slot drag, wrong-slot rejection, and
  equipment-to-bag return.
- Confirmed `Q` autorun and `W` cancellation in live input, plus readable
  Mara-side and road-side ember lamps. Slate's atomic key synthesis could not
  hold `Ctrl` into the later game tick used by the inversion toggle, so
  `Ctrl+M` remains a manual PIE check.

Inventory implementation:

- Added an explicit compact Sort control rather than silently reordering loot.
- Stable sorting uses weapon, armor, consumable, quest, and misc category
  priority, then case-insensitive display name.
- Selection follows the same item identity and duplicate-stack occurrence
  after rows move.
- Sorting is disabled from initial bag press through active drag completion, so
  a row cannot move under the pointer.
- Added `Embermere.Inventory.StableSorting` for category order, alphabetical
  order, equal-key stability, duplicate occurrence selection, and no-op repeat
  sorting.

Verification:

- The authoritative Mac editor build succeeded with `-NoHotReloadFromIDE`.
- Headless automation passed 20/20, including all prior drag/drop and atomic
  inventory transaction coverage.
- Fresh-process saved-map validation passed with 62 upright Fab actors, three
  original-art placements, gameplay anchors, moss ground, and the exact
  daylight baseline intact.
- Restart Unreal before visually checking the newly linked Sort button and its
  active-drag disabled state.

## 2026-07-17 - Clean Pulls And The Embermere Signpost

The starter encounter received its first collision-informed feel pass, and the
original roadside art family gained a navigation silhouette of its own.

Live gameplay verification:

- Ran all 20 current tests in the clean editor and again through authoritative
  headless automation; every test passed.
- Completed Mara's full acceptance, three-kill, tonic-stack, return, XP, and
  Recruit Pack loop in PIE.
- Verified identity-preserving Sort, bracket cycling, Back-slot equip/unequip
  drag paths, wrong-slot rejection, target clear, and W autorun cancellation.
- Measured the existing combat cadence before tuning: Strike deals 28, a
  Prowler deals 6 every two seconds, and one enemy takes roughly four hits over
  six seconds. The cadence felt useful; the real problem was accidental group
  pulls.

Encounter pass:

- Reduced the default starter-enemy aggro radius from `900` to `525` cm and
  added exact automation assertions for aggro, damage, and attack cadence.
- Native WorldStatic overlap probes found hidden collision from vendor rocks,
  stairs, and ruin dressing at the old and first-proposed home points.
- Moved the three Prowler homes to the collision-clear triangle
  `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`, preserving at least 800 cm
  spacing inside the visual wilderness pocket.
- Explicitly disabled collision on enemy marker cones and the safe/combat area
  cylinders. A focused PIE probe showed the pulled enemy moving and attacking
  while both neighboring enemies remained exactly at home.

Original art:

- Built `SM_EmbermereRoadSignpost_01` through the reviewed Blender MCP
  `script_path` lane: editable `.blend`, FBX, preview, deterministic metrics,
  1,828 triangles, one UV channel, five intentional materials, no non-manifold
  edges, and two authored `UBX_` boxes.
- Imported through explicit `FbxFactory`, saved the new
  `M_EmbermereTimber` package, reused the waystone/lamp material family, and
  placed `Embermere_RoadSignpost_01` beside the village route.
- The base and post collide while the broad directional arms remain overhead
  and non-colliding. Unreal thumbnail and viewport inspection confirmed a clear
  classic-fantasy road silhouette.

Verification and lessons:

- The no-hot-reload Mac editor build succeeded and headless automation passed
  20/20 with no test failures.
- The tracked validator reloaded the saved map and passed 62 upright Fab actors,
  four original-art placements, exact encounter homes, non-colliding visual
  guides, gameplay anchors, moss ground, and daylight assertions.
- A final fresh commandlet validator launch stalled in macOS app-launch services
  before Unreal produced a log, so the same tracked validator was run through
  the live editor's explicit saved-map reload instead of overstating that run.
- The day reinforced three durable rules: test gameplay homes against native
  collision rather than visual bounds, declare all decorative geometry
  `NoCollision`, and explicitly save generated material packages before
  trusting the current editor session.

## 2026-07-18 - A Real Road Gate And Clear Autorun Ownership

The roadside prop family gained its first traversable threshold, inventory
dragging stopped looking like debug text, and a subtle movement-ownership bug
received an architectural fix instead of another input special case.

Movement and UI:

- Separated controller-driven autorun movement from the manual forward-axis
  notification. Automatic ticks now add movement directly, while any real
  nonzero `W` or `S` input owns cancellation.
- Added `Embermere.Input.AutorunCancellation` for forward cancellation,
  backward cancellation, and idle preservation.
- Replaced the compact inventory drag label with a fixed 236x62 Embermere
  token using category sigils, warm item names, slot/level or consumable-effect
  context, and restrained moss/gold presentation.
- Extended drag/drop automation to cover the armor sigil, Back-slot context,
  and accent color while preserving every atomic transaction and click/key
  fallback.

Original art:

- Built `SM_EmbermereRoadGate_01` through the reviewed Blender MCP script lane:
  editable `.blend`, FBX, preview, deterministic metrics, 3,296 triangles, one
  UV channel, five shared fantasy materials, no non-manifold edges, and four
  authored `UBX_` support boxes.
- Imported through explicit classic `FbxFactory`, reused the existing
  stone/moss/timber/iron/ember family, and placed the gate on the road into the
  wilderness at `(1080, 540, 20)`, yaw `20`.
- Native traces proved the 250 cm center path is unobstructed while the support
  remains solid. The gate now frames the transition from village safety to the
  Prowler pocket without changing gameplay placement.

Verification:

- The authoritative Mac editor build succeeded with `-NoHotReloadFromIDE`.
- Headless automation passed 21/21, including the new autorun contract and
  expanded fantasy drag-token assertions.
- Saved-map validation passed 62 upright third-party actors, five exact
  original-art placements, gate import/material/collision bounds, encounter
  homes, visual-only guides, gameplay anchors, moss ground, and daylight.
- The interactive editor remained on the previous C++ module, so clean-restart
  live verification of both `W` and `S` cancellation plus the final drag-token
  appearance is deliberately carried into the next session.

## 2026-07-19 - The Gate Became A Boundary

The first traversable threshold gained a matching modular fence family, and
the controller fix crossed its final live-verification boundary.

Original art:

- Built `SM_EmbermereBoundaryFence_01` through the reviewed Blender MCP
  `script_path` lane with editable source, FBX, preview, deterministic metrics,
  2,632 triangles, one UV channel, no non-manifold edges, five shared fantasy
  materials, a ground pivot, and three authored `UBX_` boxes.
- Imported it through explicit classic `FbxFactory`, reused the gate's
  stone/moss/timber/iron/ember materials, and saved south and north instances
  beside `Embermere_RoadGate_01`.
- The two fences continue the gate silhouette without narrowing its center
  opening. A road-approach capture showed the north module and open route
  clearly; an existing tree partly masks the south module from that angle and
  is now a deliberate foliage-composition review.

Verification:

- The authoritative Mac editor build succeeded with `-NoHotReloadFromIDE`,
  and both headless automation and the clean live editor passed all 21 tests
  with no failures or warnings.
- Fresh-process saved-map validation passed 62 upright third-party actors plus
  seven exact original-art placements, including the fence mesh provenance,
  five materials, bounds, three colliders, tags, and transforms.
- Added a native trace validator that proves three lanes through the gate are
  clear, a gate support blocks, and both fence centers block.
- Restarted Unreal onto the current controller module and used short-lived MCP
  key events plus timed player-transform reads in PIE. `Q` advanced the player;
  both `W` and `S` independently stopped all later movement and held an
  identical transform for a full second.

Pipeline lessons:

- UE 5.8 Python `line_trace_single` returns `HitResult` or `None`, not a
  boolean/result tuple. Protected hit data is reliably available through
  `to_dict()`.
- A commandlet can exit zero while an invoked Python validator logs an
  exception. Acceptance therefore requires both a known success line and no
  `LogPython: Error` entries.
- Slate's atomic modifier chord cannot verify a control that polls modifier
  state during a later game tick. Physical `Ctrl+M` feedback remains a manual
  PIE check; that tool limitation is not evidence of a gameplay regression.

## 2026-07-20 - The Boundary Gained Its End Stones

The gate/fence experiment became a complete roadside threshold, and the first
composition problem was fixed without disturbing validated gameplay geometry.

Original art:

- Built `SM_EmbermereBoundaryStone_01` through the reviewed Blender MCP
  `script_path` lane with editable `.blend`, FBX, preview, deterministic
  metrics, 1,872 triangles, one UV channel, no non-manifold edges, five shared
  fantasy materials, a ground pivot, and two authored `UBX_` boxes.
- Imported it with explicit classic `FbxFactory`, saved the mesh package, and
  placed symmetric rune-topped south/north markers outside the two boundary
  fences. The map now has six original model types and nine placed instances.
- Moved only `FabPass_Road_Pine_05` and
  `FabPass_Wild_Tree_South_01` farther into the south foliage band. A fresh
  Unreal road capture now shows both fences, both end stones, and the complete
  gate opening while preserving all 62 Fab actors.

Acceptance:

- Fresh-process map validation passed exact boundary-stone classic-FBX import,
  bounds, materials, two colliders, tags, transforms, both foliage transforms,
  all gameplay anchors, the encounter triangle, and the daylight baseline.
- Native traces proved three gate lanes clear, one gate support solid, both
  fence centers solid, and both boundary-stone cores solid.
- The authoritative no-hot-reload Mac build succeeded, and the current live
  editor discovered and passed all 21 tests with no failures or warnings.
- Fresh PIE transform probes again proved `Q` movement and independent `W` and
  `S` cancellation, each holding an identical location for one second after
  the stop input. Physical `Ctrl+M` and populated-item drag-token appearance
  remain manual checks.

Pipeline lesson:

- A successful Slate key call does not prove the game viewport had focus.
  Refresh the accessibility snapshot, click the current viewport before each
  measured key, and compare game state before and after the action. That turned
  an apparent autorun failure into a reproducible focus diagnosis instead of a
  speculative gameplay change.

## 2026-07-21 - The Inventory Gained A Visual Language

The first item and paper-doll art pass moved Embermere beyond labeled boxes
without coupling inventory rules to any one texture family.

Presentation:

- Generated 14 original `128x128` icons for Recruit Pack, Marsh Tonic, all ten
  equipment slots, and explicit missing-item/missing-slot states. The tracked
  standard-library generator makes the raster source deterministic and uses the
  same moss, stone, iron, gold, leather, and ember language as the roadside
  Blender family.
- Added `UEmbermereUiIconSet` as the presentation contract. Item data may point
  directly to soft icon references; otherwise the resolver uses category,
  equipment-slot, and missing-art fallbacks without changing inventory or
  equipment logic.
- Integrated icons into fixed `18x18` bag rows and equipment cells plus a
  `42x42` selected-item header. Existing item names and tooltips remain the
  accessible fallback, and every fixed icon container keeps the inventory
  geometry stable when art is absent.

Persistence and verification:

- Imported the raster family explicitly under `/Game/UI/Icons`, configured it
  for UI use with no mip generation, saved every texture package, created and
  saved the icon-set data asset, and persisted direct Recruit Pack/Marsh Tonic
  assignments.
- A fresh Unreal process reloaded all 14 textures at exact dimensions and
  verified ten slot mappings, five category fallbacks, both missing-art paths,
  and both starter-item references.
- Added `Embermere.UI.IconPresentation`. The authoritative Mac build succeeded
  with `-NoHotReloadFromIDE`, and the complete headless suite passed 22/22.
  Saved-zone and native road-boundary validation also remained green with 62
  upright Fab actors, nine original placements, and all three gate lanes open.
- The interactive editor still predates the final C++ link, so actual icon
  scale, clipping, tooltip readability, and fallback appearance remain an
  explicit clean-restart PIE acceptance pass rather than an inferred success.

Pipeline lesson:

- Generated pixels, loaded objects, and saved Unreal packages are three
  different states. Keep the source generator, explicit import/save step,
  fresh-process asset validator, runtime resolver test, and human PIE review as
  separate gates. `-NullRHI` can report zero runtime texture resource size, so
  editor tests should inspect `Texture->Source` while the fresh Python validator
  checks Unreal's Blueprint-visible dimensions.

## 2026-07-22 - The Screenshot Rejected A Passing Build

The icon family passed every technical gate, then the first populated screen
found a presentation bug those gates were never meant to judge.

Visual acceptance:

- Restarted the editor onto the July 21 icon module, passed all 22 live tests,
  and inspected the empty inventory. All ten paper-doll symbols were distinct,
  stable, and readable.
- Populated the bag with Recruit Pack and Marsh Tonic through the current data
  and captured the real panel. The icons held their bounds, but the saved
  `Embermere Recruit Pack` copy clipped in its compact row and crowded the
  adjacent Equipment heading.
- Preserved the stable `RecruitPack` identity while shortening only the
  player-facing label to `Recruit Pack`. The configuration script now
  reproduces that choice, and the fresh-process validator treats it as part of
  the presentation contract. A second populated PIE capture accepted the
  corrected row and detail-heading fit.

Presentation reuse:

- Routed the existing item-icon resolver into the fixed `236x62` fantasy drag
  token. Resolved art occupies a stable `44x44` cell; the category sigil remains
  the fallback when no image resolves.
- Added the same data-driven art to the temporary reward popup in a fixed
  `32x32` cell, preserving its existing bounds, timing, and text behavior.
- Kept drag payload identity and all atomic inventory/equipment transactions
  unchanged. This was a presentation extension, not a second gameplay path.

Verification:

- The authoritative no-hot-reload Mac build succeeded, and fresh-process
  automation passed 22/22 with zero warnings or failures.
- Fresh icon validation reloaded all 14 textures, every mapping and item
  assignment, plus the compact Recruit Pack label. Saved-zone validation still
  passed 62 upright Fab actors and nine original placements, while native
  traces retained all three clear gate lanes and solid boundary collision.
- The open editor predates the final July 22 relink, so actual drag-token and
  reward-popup appearance remains a deliberate clean-restart PIE check rather
  than an inferred acceptance. The compact item copy itself is visually
  accepted.

Pipeline lesson:

- Automation establishes technical eligibility; a populated screenshot judges
  whether the result belongs in the interface. Test the real saved strings and
  real neighboring panels, then turn the accepted copy back into deterministic
  configuration and validation so the visual fix remains durable.

## 2026-07-23 - Every Starter Class Gained A Visual Voice

The item-art contract expanded into a complete ability language instead of a
Warrior-only sample.

Presentation:

- Generated seventeen additional deterministic `128x128` textures: all sixteen
  starter abilities plus an explicit missing-ability fallback. Warrior uses
  ember and iron, Cleric gold and ivory, Ranger moss and leather, and Wizard
  frost and arcane while retaining the roadside family's shared frame.
- Added direct soft references to native and saved ability definitions, plus a
  shared fallback resolver in `UEmbermereUiIconSet`. The hotbar remains
  asset-agnostic and never switches on class or ability ID.
- Mounted fixed `32x32` art cells inside the existing `92x64` hotbar slots.
  Cooldowns dim icon and text together, and the changing countdown stays in a
  two-line label instead of growing the bar.
- Added hover tooltips from the same ability data: description, power, mana,
  meter-based range or Self, and cooldown. Empty slots retain fixed geometry
  and `F` remains the Interact command.

Acceptance:

- Before the new build, clean PIE accepted the fixed Marsh Tonic reward popup
  and a real Recruit Pack bag-to-Back drag. The transaction removed the bag
  item, populated Back, raised HP to `105/105`, and posted one chat message.
- The authoritative Mac build succeeded with `-NoHotReloadFromIDE`. Fresh
  automation passed 23/23, including the new
  `Embermere.UI.AbilityIconPresentation` contract and expanded cooldown/tooltip
  assertions.
- A separate fresh process reloaded all 31 exact UI textures, ten equipment
  mappings, five category mappings, three missing-art paths, two starter-item
  assignments, and sixteen distinct ability assignments.
- Saved-zone and native road-boundary validation remained green with 62 upright
  Fab actors, nine original placements, three open gate lanes, and all required
  collision. The interactive editor predates the final link, so all-class
  hotbar appearance remains tomorrow's clean-restart visual gate.
- Refreshed the 8:00 AM daily automation to start from this 23-test
  ability-icon baseline and move next into functional class effects,
  paper-doll art, an original prop, or cohesive village architecture.

Pipeline lesson:

- A complete family is a better architecture test than one attractive icon.
  Distinct class palettes expose naming, persistence, fallback, fixed-layout,
  cooldown, and tooltip assumptions across every current ability while the
  gameplay system remains untouched.

## 2026-07-24 - The Utility Buttons Became Class Mechanics

The completed hotbar art family made the next gap obvious: Snare, Frost Root,
Ward, Battle Shout, Nature's Focus, and Meditate looked distinct but still
shared placeholder combat behavior. Today the saved rules became the authority
for what those buttons actually do.

Gameplay contract:

- Added explicit damage, healing, mana-recovery, Attack Power buff, and Armor
  buff effect types to the data-driven ability definition, alongside duration
  and movement-speed multiplier fields.
- Battle Shout and Nature's Focus now grant `+8 Attack Power` for 10 seconds.
  Ward grants `+10 Armor` for 10 seconds and feeds the existing armor
  mitigation formula.
- Snare deals light damage and applies `0.50x` movement for 6 seconds. Frost
  Root deals light damage and applies `0.00x` movement for 4 seconds.
- Meditate restores 18 missing mana and reports only the amount actually
  restored.
- Player movement, enemy chase, and enemy return-home movement consume the
  shared movement multiplier. World-time expiration restores baseline values,
  and vital initialization clears all temporary effects so respawned enemies
  never inherit old control state.

Acceptance:

- Restarted Unreal onto the July 23 module, passed all 23 existing live tests,
  and visually accepted the Warrior, Cleric, Ranger, and Wizard icon palettes.
  Warrior cooldown dimming and its fixed two-line countdown also held in PIE.
- Saved all sixteen ability semantics through a deterministic configuration
  script and expanded the fresh-process UI validator to reject mismatched
  effects, durations, movement multipliers, or placeholder descriptions.
- Added `Embermere.Combat.StarterAbilityEffects`. The authoritative
  no-hot-reload build succeeded and fresh headless automation passed 24/24.
- In a second clean editor session, live probes measured Snare at `0.50x`
  movement, Frost Root at `0.00x`, both offensive buffs at 18 effective Attack
  Power, Ward at 10 effective Armor, and Meditate restoring mana from 20 to 38.
- UI/icon, saved-map, and road-boundary validators all retained their success
  markers with no `LogPython: Error`; the environment stayed at 62 upright Fab
  actors, nine original placements, and three clear gate lanes.

Pipeline lesson:

- Put ability identity in saved data, make runtime systems consume generic
  effects, and validate both the package and the lived result. A passing struct
  assertion proves eligibility; a clean PIE measurement proves the enemy
  really stopped moving.

## 2026-07-25 - The Paper Doll Became A Picture

The equipment grid already behaved like an RPG system, but it still read as ten
carefully arranged buttons. Today Embermere gained a restrained original
adventurer illustration behind those controls without giving the artwork any
authority over inventory behavior.

Presentation contract:

- Generalized the standard-library UI generator so fixed square icons and
  bounded rectangular presentation art share one deterministic source lane.
- Generated a transparent `128x160` armored silhouette in Embermere's
  stone/moss/iron/gold/ember language, including the shared ember rune.
- Added a soft paper-doll reference and resolver to
  `DA_EmbermereUiIconSet`. The native HUD layers the fixed image beneath the
  existing ten-slot grid through a hit-test-invisible overlay.
- Preserved every slot's `18x18` icon, `62x31` button bounds, label, tooltip,
  click action, drop target, and atomic bag/equipment transaction.

Acceptance:

- The no-hot-reload Mac editor build succeeded.
- A fresh Unreal process imported and explicitly saved the new texture and data
  reference; a second process reloaded the exact `128x160` source, UI texture
  group, no-mipmap setting, and mapping.
- Added `Embermere.UI.PaperDollPresentation`. Fresh headless automation passed
  25/25 with zero warnings or failures.
- Saved-zone validation retained 62 upright Fab actors and nine original-art
  placements. Native road traces retained three clear gate lanes and all
  required solid supports, fences, and boundary stones.
- The running interactive editor still holds the July 24 module, so backdrop
  contrast behind empty and occupied slots remains the next clean-restart PIE
  judgment rather than a claimed visual acceptance.

Pipeline lesson:

- Decorative art should enrich the system it sits behind, not acquire hidden
  gameplay responsibilities. Deterministic source, persisted asset references,
  fixed layout contracts, and a final human visual check keep those concerns
  separate.

## 2026-07-26 - Timed Effects Became Visible

The starter abilities already changed real combat state, but their duration
lived only in chat and memory. Today Embermere gained a generic presentation
contract that lets a player inspect what is helping them and what is controlling
their selected target without teaching the HUD class-specific rules.

Presentation contract:

- Stats now expose lightweight active-effect snapshots containing the source
  ability, remaining world time, and beneficial/harmful intent.
- Combat registers that metadata only after the generic timed buff or movement
  effect succeeds. Reapplying the same ability refreshes one record instead of
  creating duplicate icons.
- The HUD renders two fixed `128x32` cells beneath player mana and two beneath
  selected-target health, reusing each ability's saved icon, display name,
  description, and countdown.
- Beneficial effects use moss/gold presentation; harmful control effects use
  ember/frost presentation. Meditate stays instantaneous and creates no timed
  record.
- Expiration, vital reinitialization, player recovery, enemy respawn, target
  death, and target switching all consume the same gameplay-owned state rather
  than maintaining a second UI timer.

Acceptance:

- Before relinking, clean PIE accepted the new `128x160` paper doll behind an
  empty equipment grid. It remained centered and readable without moving or
  obscuring the title, bonuses, footer, inventory, or hotbar.
- Added `Embermere.UI.TimedStatusPresentation`, covering source ability
  identity, icon reuse, beneficial/harmful classification, countdown copy,
  fixed icon/slot dimensions, and respawn-style clearing.
- The authoritative no-hot-reload Mac build succeeded and fresh headless
  automation passed 26/26.
- Fresh-process UI-art validation retained all 31 saved icon textures, the
  paper-doll package, item mappings, and sixteen ability semantics. Saved-zone
  validation retained 62 upright Fab actors and nine original placements.
  Native traces retained three clear gate lanes and all required road-boundary
  collision.
- The final C++ link makes the open editor stale, so tomorrow's clean-restart
  visual gate is the live status rows plus paper-doll contrast with Recruit
  Pack equipped.

Pipeline lesson:

- Register presentation metadata only after a gameplay rule succeeds, and let
  UI read snapshots rather than infer mechanics from names. The gameplay
  contract stays authoritative while icons, colors, layout, and future VFX
  remain replaceable.

## 2026-07-27 - Status Acceptance And A Village Supply Chest

The timed-effect system crossed its human acceptance gate, and the original-art
lane moved from roadside infrastructure into the village itself.

Live acceptance:

- Clean PIE showed Battle Shout beneath player mana with its saved icon,
  beneficial treatment, name, live countdown, and fixed footprint.
- Ranger Snare appeared beneath selected-target health with harmful treatment
  and the same data-driven layout. Neighboring HUD panels did not move.
- Meditate remained instantaneous with no false status cell. Focused automation
  retains duplicate refresh, expiration, respawn, death, and target-switch
  clearing.
- Recruit Pack was added and equipped through the real inventory/equipment
  components. The gold occupied Back slot, `105/105` HP, empty post-transfer
  bag, and illustrated paper doll all remained readable inside the unchanged
  `700x330` window.

Original village art:

- Added `SM_EmbermereSupplyChest_01` through the reviewed Blender script,
  FBX, classic `FbxFactory`, package-save, map-placement, and validation lane.
- The chest is 2,364 triangles, `180.0 x 119.0 x 123.1` cm, one UV channel,
  five shared Embermere materials, and two authored body/lid `UBX_` boxes.
- Its saved actor replaces `FabPass_Village_Crates_A` beside the vendor table,
  bringing the map to 61 upright local Fab actors and ten original-art
  placements.
- The first technically valid yaw hid the lock from the normal approach.
  Inspecting from PlayerStart led to the accepted yaw `108`, which exposes the
  lock and timber face while leaving Mara's route clear.
- A live native trace hit the authored lid collision at the expected height.
  Saved-map validation now locks classic-FBX provenance, bounds, materials,
  collider count, tag, transform, and removal of the old crate stack.

Verification:

- The authoritative no-hot-reload Mac editor build succeeded.
- Fresh commandlet automation discovered and passed all 26 tests with zero
  failures, skipped tests, or warnings.
- Fresh UI-art and saved-map validators emitted explicit success markers with
  no Python errors.
- A commandlet-loaded world did not register even known gate collision bodies,
  while the same traces passed in the initialized live editor. Static package
  contracts therefore remain fresh-process checks; native physics behavior is
  verified in a live initialized world.

Pipeline lesson:

- Technical validity makes an asset eligible, not accepted. Route-facing
  readability, physical behavior, and visual belonging still require the real
  world, the real camera, and human judgment.

## 2026-07-27 Evening - The Marsh Prowler Entered Embermere

The first creature pass and the first measured terrain-contact pass landed
together. The starter enemy is no longer a placeholder shape, and the road no
longer relies on one flat green plane to imply an environment.

Original creature:

- Built `SK_EmbermereMarshProwler_01` from a reviewed deterministic Blender
  script using the user's swamp dire-wolf concept as mood and silhouette
  guidance rather than an exact copy.
- The saved source has 7,464 triangles, 3,878 vertices, five marsh-fantasy
  materials, 26 authored bones, two physics proxies, and six actions: Idle,
  Walk, Run, Attack, Hit, and Death.
- Exported the skeletal mesh and each in-place action, then explicitly saved
  the Unreal skeletal mesh, skeleton, physics asset, materials, and six
  animation packages in a project-owned folder.
- Added asset-agnostic animation roles to the generic enemy character. The
  existing capsule, targeting, combat, loot, quest, leash, death, and respawn
  systems remain the authorities.
- Repaired all three saved level instances after discovering that the correct
  Blueprint CDO did not overwrite their stale serialized mesh component state.
- Clean PIE exercised Tab selection, Strike, Prowler retaliation, hit and death
  presentation, target clear, Marsh Tonic loot, hide, and respawn.

Ground and marsh identity:

- Measured the real foundation at `Z=0` and lowered 67 ordinary art actors from
  the inherited `Z=20` convention instead of trying to hide real gaps with
  grass.
- Removed two unsupported SoulCave ruin accents and three redundant enemy
  marker meshes.
- Applied project-owned component material overrides to 21 KiteDemo foliage
  placements, preserving raw vendor packages.
- Rebuilt `M_EmbermereGround` as a texture-free 38-expression moss/earth
  material with broad crossed variation and a route-aligned `300` cm half-width
  worn path.
- Built `SM_EmbermereMarshReedCluster_01`, a deterministic 1,012-triangle
  visual-only asset. The first preview's pale base technically passed but read
  like a planter, so the accepted version shares the terrain material on its
  low footprint. Four `NoCollision` clusters now dress the road and combat
  pocket without blocking sightlines.

Pipeline lessons:

- Blueprint class defaults and saved placed instances are different
  persistence gates.
- Material graph edits belong outside PIE. A failed PIE-time save can leave a
  broken live graph over a healthy disk package; restart before judging it.
- UE 5.8's dedicated
  `-ModelContextProtocolStartServer -ModelContextProtocolPort=8123` flags are
  reliable for startup. On macOS, the project path must follow `open --args`.
- A component override can make an incomplete vendor mesh visibly usable while
  fresh commandlets still report its missing internal dependencies. Keep both
  truths: accept the temporary presentation and plan the source replacement.
- Fresh package validators, initialized-world physics traces, and human PIE
  review answer different questions. None substitutes for the others.

Verification:

- The initialized editor passed all 27 Embermere automation tests with zero
  failures, skips, or warnings.
- The authoritative no-hot-reload Mac editor build succeeded.
- Fresh saved-map and road-boundary validators emitted their success markers
  with no Python errors.
- A fresh headless Blender inspection reloaded the saved Prowler source and
  confirmed topology, scale, bones, materials, and all six actions.
- The accepted map now contains 59 grounded upright Fab actors and 14
  project-owned placements. One honest follow-up remains: normal-route PIE
  should reproduce a straight-line autorun contact near Mara before moving the
  temporary village blocker.

## 2026-07-28 - The Route Cleared And Status Effects Entered The World

Today closed the two honest follow-ups left by the Prowler pass: the direct
spawn route now has a measured clearance contract, and timed effects no longer
exist only in HUD cells.

Route diagnosis:

- A bounded `Q` autorun probe reproduced the exact stop near
  `(-1646.9, -879.0, 90.15)` while autorun itself remained active. The issue
  was world collision, not input state.
- The blocker was `Embermere_SupplyChest_Vendor_01`. Because its component was
  static, a PIE transform probe silently stayed put until mobility was
  temporarily changed to `Movable` for the diagnostic.
- The accepted `(-1740, -1180, 0)` transform, yaw `108`, preserved the chest's
  lock-facing village read, terrain contact, and solid body/lid collision while
  clearing the direct route.
- The import script now reproduces that transform. Saved-map validation requires
  at least `225` cm of geometric spawn-corridor clearance, and the live native
  trace suite proves both the chest lid solid and the old player-height line
  clear. Fresh PIE autorun traveled well beyond the former stop point.

Prowler acceptance:

- Real retaliation at 6 damage every 2 seconds exercised player death,
  recovery protection, and enemy return-home behavior.
- A bounded zero-damage diagnostic then held the route safe long enough to
  prove four Strikes, target clear, Marsh Tonic loot, death hold, hide,
  12-second full-health respawn, and continued isolation of the other two
  Prowlers.
- The skeletal mesh, Idle/Walk/Run/Attack/Hit/Death routing, capsule, target
  ring, nameplate, loot, quest, leash, and respawn rules remained independent.

World-status presentation:

- Every Embermere character now owns eight fixed non-colliding plane segments
  using the existing project-owned target-ring material.
- The segments read only `GetActiveStatusEffects()`: Attack Power is
  orange-gold, Armor blue-white, Snare marsh green, and Frost Root frost cyan.
  Harmful effects take deterministic priority; empty or dead state hides the
  complete aura.
- Rotation and restrained pulse belong entirely to presentation. Combat and
  stats remain the authority for success, duration, power, movement, refresh,
  expiration, death, and respawn clearing.
- Clean PIE accepted Battle Shout as eight readable orange-gold segments around
  the player alongside its live HUD countdown, without moving the HUD or hiding
  the character.

Verification:

- The authoritative no-hot-reload Mac editor build succeeded.
- Fresh commandlet automation discovered and passed all 28 tests, including
  `Embermere.UI.WorldStatusVfxPresentation`.
- Fresh saved-map validation, initialized-world road/chest traces, and fresh
  Blender Prowler inspection all emitted explicit success markers.

Pipeline lessons:

- A successful input call does not prove movement input failed; actor
  transforms and collision isolation reveal the actual boundary.
- Temporarily changing mobility is useful for PIE diagnosis only. Stop PIE,
  apply the accepted transform to the editor world, save explicitly, and prove
  persistence fresh.
- HUD and world VFX can share gameplay-owned presentation metadata without
  either becoming a second gameplay system.
- Technical eligibility, physical route proof, normal-camera readability, and
  human acceptance remain distinct gates.

## 2026-07-29 - The Target Circle Learned The Creature And The Ground

The selected-target marker now reads like the classic MMO affordance we wanted
instead of a generic animated effect.

Target-circle pass:

- Replaced the rotating orange-gold 24-segment marker with 48 overlapping
  non-colliding plane segments in a restrained cyan-blue emissive treatment.
  The circle remains stationary and uses only a subtle pulse, keeping target
  identity distinct from the rotating timed-status world auras.
- Removed the fixed creature-size assumption. Runtime radius now starts from a
  conservative `96` cm minimum, expands from capsule and transformed skeletal
  mesh bounds, and adds `18` cm of visual padding. The Marsh Prowler therefore
  clears its paws and long silhouette without hard-coding a wolf-only value.
- Removed the fixed-ground assumption while selected. A downward visibility
  trace resolves the supporting surface and keeps the circle at least `16` cm
  above the hit. That value is visually flush in the prototype because the
  rendered moss surface sits above the collision surface returned by the
  trace. The previous offset remains a fallback when no initialized world or
  surface hit exists.
- Kept targeting authoritative. The circle owns no collision, navigation, aggro,
  combat, status, or target-selection rules; it only mirrors selected state.

Verification:

- `Embermere.Combat.TargetSelectionPresentation` now proves the 48-segment cyan
  contract, overlap, restrained pulse, no rotation/collision, initial hiding,
  selection, switching, and clearing. The Prowler presentation test proves its
  resolved circle grows beyond the generic minimum but remains bounded.
- The no-hot-reload Mac editor build succeeded. Fresh commandlet automation
  passed 28/28, and the restarted GUI editor rediscovered and passed the same
  28/28 suite with no failures, skips, or warnings.
- Fresh saved-map validation retained 59 grounded upright Fab actors, 14
  project-owned placements, three saved Prowlers, reeds, moss/earth ground, and
  daylight. Initialized-world traces retained the clear spawn corridor, three
  clear gate lanes, solid boundary family, and solid supply chest.
- A saved Blender inspection reloaded the 7,464-triangle, 26-bone Prowler source
  and retained its exact dimensions and grounded paw minimum.
- Clean daylight PIE selected a live Marsh Prowler and showed the cyan circle
  around its footprint with the native nameplate and HUD target frame intact.
  The remaining honest visual check is a normal-route physical-eye sweep across
  the other two saved Prowlers and harmful world-status colors.

Pipeline lessons:

- A world indicator needs two contracts: what it surrounds and what it rests
  on. Capsule-only sizing and fixed Z offsets answer neither reliably after art
  or terrain changes.
- Target identity benefits from calmer motion than timed-effect presentation.
  Color, footprint, and immediate switch/clear behavior did more useful work
  than continuous rotation.
- A forced PIE start transform can bypass the normal PlayerStart and land in an
  invalid or non-walkable location. Use it for bounded camera diagnostics, but
  do not treat that result as a saved-map regression when fresh package
  validation, initialized-world traces, and the normal route disagree.

Same-day PIE correction:

- A user-run clean PIE session selected a Marsh Prowler and showed the
  nameplate and HUD target frame but no visible circle. That screenshot
  overruled the earlier technical acceptance.
- Runtime inspection showed valid material, selected state, and visible
  components. Raising the ring proved that the visibility trace was hitting
  collision below the rendered moss surface: `3` cm remained buried, `10` cm
  was still effectively invisible, and `16` cm produced the intended
  visually flush cyan circle.
- A second discrepancy was hiding behind green native tests. A native
  `AEmbermereEnemyCharacter` constructed 48 segments, but the saved
  `BP_StarterEnemy` generated class retained 24 older inherited component
  templates. Recompiling the Blueprint did not rebuild that native subobject
  template set.
- `PostInitializeComponents` now reconciles the actual runtime actor to exactly
  48 segments, reusing inherited components and creating only missing
  transient components. Every segment is reasserted as non-colliding,
  non-navigating, shadowless presentation.
- The Prowler automation now instantiates the real Blueprint-generated class
  and proves the reconciled component contract. The authoritative
  no-hot-reload build and all 28 tests passed, and corrected clean PIE showed
  the cyan circle beneath the live selected Prowler.

## 2026-07-30 - The Circle Held, And The World Got An Edge

Today's acceptance sweep kept the corrected target circle unchanged and found a
different problem that only sustained gameplay input could reveal.

Target-circle acceptance:

- Clean normal-route PIE confirmed all three live Marsh Prowlers reconcile to
  exactly 48 stationary, non-colliding cyan-blue segments with the accepted
  16 cm effective collision-hit clearance.
- The visible circle reads around the selected Prowler's footprint alongside
  the native nameplate and HUD target frame. `Tab` immediately moved the
  selected presentation from the first Prowler to the third and hid the old
  ring.
- Runtime inspection retained the restrained pulse, zero rotation, cyan color,
  presentation-only collision, and bounds-aware sizing. No speculative
  retuning was made after the normal-camera pass.

Finite-world recovery:

- A sustained `Q` autorun probe was measured through transforms instead of
  trusting the synthetic key call. It traveled beyond the finite prototype
  foundation and reached roughly `(37258, 23234, -248980)`, proving the player
  could fall forever.
- Added a controller-owned `Z=-1000` recovery plane. Crossing it cancels
  autorun and calls an explicit stats-component death path that bypasses active
  damage immunity, preventing a protected player from remaining out of bounds.
- Death disables movement; the existing five-second recovery then teleports
  with physics semantics, clears velocity, restores `MOVE_Walking`, initializes
  full vitals, and applies the existing three-second damage protection.
- Fresh PIE forced the player below the recovery plane while autorun was active
  and proved both HUD messages, exact village spawn
  `(-2400, -1200, 90.15)`, `100/100` health, autorun off, walking restored, and
  zero velocity.

Verification:

- The authoritative no-hot-reload Mac editor build succeeded.
- Fresh commandlet automation discovered and passed all 29 tests with no
  failures, skips, or warnings, including
  `Embermere.Player.OutOfBoundsRecovery`.
- Saved-zone, UI-art, and initialized-world road-boundary validators emitted
  their explicit success markers with no Python errors.

Pipeline lessons:

- A synthetic input success is only an event claim. Transforms reveal whether
  the game actually moved and where it eventually went.
- Finite prototype maps need an explicit out-of-bounds gameplay contract before
  traversal becomes expansive enough to expose their edges.
- Respawn correctness includes location, vitals, movement mode, velocity,
  input state, and protection. A teleport alone is not a complete recovery.

## 2026-07-31 - The Aura Found The Ground

The harmful status aura was mechanically active, test-green, and almost
invisible. All eight segments existed, owned no collision, read the correct
Snare metadata, and cleared with the gameplay effect. They also orbited the
character root, which put them inside the Marsh Prowler's torso from the normal
camera.

The correction gave world presentation the same two contracts already proven
by the target circle: what it surrounds and what it rests on. Harmful aura
radius now comes from bounded transformed skeletal bounds, while a visibility
trace resolves the supporting surface. On the live Prowler that produces a
`94.352` cm inner ring at `21` cm above ground, safely inside the unchanged
48-segment cyan target circle.

Clean PIE accepted marsh-green Snare and a deliberately lighter ice-white cyan
Frost Root (`0.46, 0.92, 1.0`). The latter separates control-state motion from
the calm outer selection ring without changing combat, duration, movement, or
targeting rules.

The authoritative no-hot-reload Mac build succeeded, all 29 tests passed, both
fresh-process package validators emitted their success markers, and the
initialized-world road trace retained every collision and clearance contract.

Lesson: component count and color can prove eligibility, but they cannot prove
normal-camera readability. A creature-bound world effect needs both a visual
footprint contract and a support-surface contract before human playtesting can
decide that it belongs.

## 2026-08-01 - Fenwatch Got A Roof

No concrete Prowler regression appeared in the accepted normal-route baseline,
but clean PIE exposed a village problem immediately: Mara still stood among a
grey stone backdrop, a mismatched market cover, and placeholder service cubes.
The next bounded milestone became Embermere's first full architectural module.

The deterministic Blender build produced
`SM_EmbermereFenwatchShelter_01`, an open-sided gabled shelter measuring
`438.0 x 296.782 x 369.5` cm. It contains 4,348 triangles, one UV channel, no
non-manifold edges, applied scale, a ground pivot, five existing
stone/moss/timber/iron/ember materials, and four `UBX_` support boxes. The roof
and center intentionally own no collision.

The classic `FbxFactory` lane retained all four authored boxes and saved the
mesh package. The first placement at `(-2180, -1060)`, yaw `160`, passed import,
bounds, material, collision, tag, and transform checks. It also hid Mara from
the gameplay camera. Clean PIE rejected that technically eligible placement.

The accepted actor now sits at `(-1740, -700, 0)`, yaw `-64`, behind Mara. Her
name and quest marker read in the foreground while the shelter supplies the
first coherent village silhouette. The old backdrop, market cover, and
vendor/trainer cubes are gone. A measured autorun probe moved the player from
`(-2400, -1200, 90.15)` to `(-1793.188, -831.382, 90.15)` and an independent
`W` press stopped all later movement at the same transform.

The saved zone now contains 57 grounded upright Fab actors and 15 original-art
placements. Fresh-process map and UI-art validators passed, initialized-world
traces proved the shelter center and spawn route clear plus all four supports
solid, the no-hot-reload Mac build succeeded, and all 29 tests passed.

Lesson: architectural acceptance is contextual. A valid mesh at a valid
transform can still fail the quest composition. Deterministic contracts make an
asset eligible; normal-camera play decides whether it helps the world.

## 2026-08-02 - Mara Became A Fenwatch Keeper

Mara's quest logic, dialogue, marker, and rewards already worked, but her world
presentation was still a stretched engine cube. Today's bounded art pass kept
all of those systems untouched and replaced only the visual component with the
first original Embermere humanoid silhouette.

The deterministic Blender script produced
`SM_EmbermereFenwatchKeeper_Mara_01`, a static `107.45 x 71.0 x 207.5` cm
keeper with 3,280 triangles, one UV channel, no non-manifold edges, applied
scale, a ground pivot, and six project-owned materials. Moss cowl, timber-brown
clothing, pale skirt, ember crest, staff, and satchel connect her to the
Fenwatch shelter and roadside family without pretending this first pass is a
finished rigged character.

The classic `FbxFactory` lane saved the mesh and the new skin material, then
reconciled both `BP_QuestGiver`'s saved SCS component template and Mara's
serialized map instance. That second step mattered: the old cube left behind a
`-140` cm visual offset, stretched scale, world-facing rotation, and blocking
collision. The accepted keeper uses the grounded offset intentionally, unit
scale, local yaw `100` toward PlayerStart, and `NoCollision`, while the existing
interactable component remains authoritative.

Clean PIE accepted Mara's grounded feet, front-facing silhouette, shelter
composition, and unobstructed gold marker/name. The initialized-world validator
retained the clear spawn corridor, shelter center, four solid supports, all
three gate lanes, and the rest of the road boundary. The no-hot-reload Mac build
succeeded, all 30 tests passed, and fresh-process map and UI validators emitted
their success markers. The map now contains 57 grounded upright Fab actors and
16 project-owned placements.

Lesson: replacing a Blueprint visual is a two-layer persistence problem. The
saved class template and the placed actor can each carry different historical
overrides. Inspect both, clear inherited placeholder state deliberately, then
let fresh-process tests and normal-camera play decide whether the new art truly
belongs.

## 2026-08-03 - Fenwatch Gained A Quartermaster Contract

Mara's accepted keeper proved that project-owned humanoid art could replace a
placeholder without taking over quest logic. The next question was whether we
could repeat that visual language for a future service NPC without baking a
vendor implementation into a static mesh or one-off actor.

The deterministic Blender pass produced
`SM_EmbermereFenwatchQuartermaster_01`, a stocky static quartermaster measuring
`120.842 x 93.0 x 217.0` cm. It contains 3,632 triangles, one UV channel, no
non-manifold edges, applied scale, a ground pivot, and six project-owned
materials. A moss cap and shoulder wrap, pale apron, beard, ledger, pouches,
keys, and ember details distinguish the merchant silhouette while keeping it
inside the Fenwatch shelter/roadside family.

This asset introduced a reusable native boundary rather than another special
case. `AEmbermereNpcPresentationActor` owns soft static and skeletal art
references, an optional animation class, one shared visual transform, and a
permanent `NoCollision` presentation contract. It owns no interaction, stock,
currency, trainer, dialogue, or quest behavior. The accepted static instance,
`Embermere_FenwatchQuartermaster_Vendor_01`, sits beside the supply chest at
`(-1530, -1190, 0)`, yaw `100`; the same wrapper can later select a rigged lane
without moving the service location or changing gameplay ownership.

Clean PIE accepted grounded contact, the chest-side merchant read, Mara's
unobstructed shelter silhouette, and the open PlayerStart route. Independent
measured runs moved under `Q` from `(-2400, -1200, 90.15)` to approximately
`(-1793.18, -831.38, 90.15)` and then held the exact transform after `W` and
`S` cancellation. Initialized-world traces retained every route and collision
contract. The no-hot-reload Mac build succeeded, all 32 tests passed, and
fresh-process map/UI validators accepted 57 Fab actors plus 17 project-owned
placements.

Lesson: asset agnosticism becomes real only when the replacement boundary is
executable. A dual-lane visual wrapper, exact persistence tests, and a strict
absence of service authority let today's static NPC become tomorrow's animated
NPC without turning art into gameplay architecture.

## 2026-08-04 - Fenwatch Opened For Trade

The quartermaster began as a deliberate test of whether Embermere could swap
NPC art without inheriting gameplay responsibilities. Today that boundary paid
off: the accepted static merchant stayed untouched while a separate invisible
service actor gained interaction and vendor behavior.

- Added `UEmbermereVendorStockData`, a player wallet component, an art-free
  vendor service actor, and a vendor component with finite or unlimited stock.
- Defined `DA_FenwatchQuartermasterStock` with unlimited Marsh Tonic at 8
  copper and one Recruit Pack at 30 copper. Players start this prototype slice
  with 40 copper.
- Made purchases transactional. The vendor validates the whole request,
  affordability, stock, and bag capacity before mutation; unexpected item-add
  failure refunds the exact charge; finite stock changes only after success.
- Added a fixed native `Fenwatch Supplies` panel with resolved item art, purse,
  stock counts, detail copy, Buy/close controls, two-line result space, and
  bottom-left chat feedback. The first live screenshot rejected a technically
  working version because its wrapped purchase text crossed the footer; the
  accepted layout reserves fixed status space and keeps every line inside.
- Kept the architecture executable: the presentation actor has no vendor or
  interaction component, while the service actor has no static/skeletal mesh,
  collision, or navigation effect. The saved map co-locates them without
  making either responsible for the other.
- Added deterministic configuration and fresh-process validation for the stock
  asset and saved service actor, including exact rows, prices, quantities,
  transform, tags, references, and absence of art/service leakage.
- Added four automation tests for transaction rollback, service ownership,
  saved stock, and panel behavior. The no-hot-reload Mac build and all 36 tests
  passed alongside saved-map, UI-art, vendor, and initialized-world route
  validators.
- Clean PIE exercised the actual `F` path. A tonic changed 40 to 32 copper and
  entered the bag; the one Recruit Pack changed 32 to 2 and sold out; selecting
  tonic then showed an insufficient-funds disabled state. Chat, inventory,
  fixed panel copy, and close/input restoration all held.

The transferable lesson is the same one that shaped Embermere's asset lane:
separate ownership makes iteration cheap. Art can become skeletal, stock can
change, and the UI can be reskinned without rewriting the transaction core.

## 2026-08-05 - Copper Entered The Loop

Fenwatch's first merchant could spend the player's prototype purse, but the
world could not replenish it and the vendor could not take items back. Today's
economy pass closed both sides without moving a rule into the quartermaster art
or HUD.

- Added data-driven sell values: Marsh Tonic sells for `3` copper and Recruit
  Pack for `12`. Quest items and zero-value items are explicitly unsellable.
- Mara's first quest now grants `20` copper exactly once alongside `125` XP and
  its Recruit Pack, so currency is earned through the playable quest loop.
- Added authoritative sell and bounded buyback preflight to the existing vendor
  component. Sales remove the exact selected item identity, credit the wallet,
  and record buyback only after commit. Buyback checks purse and full quantity
  capacity before restoring that same identity at the recorded price.
- Every unexpected second-step failure has an inverse operation: a failed
  removal retracts the sale credit, while a failed buyback add refunds the
  charge. Rejected paths leave wallet, inventory, stock, and buyback unchanged.
- Extended the fixed `500x325` panel with Sell selected and latest Buyback while
  preserving its stock/detail/status/footer bounds and chat feedback.

Clean PIE exercised the real runtime state: tonic purchase `40 -> 32`, sale
`32 -> 35`, buyback `35 -> 32`, finite Recruit Pack purchase `32 -> 2`, and an
insufficient-funds retry that held at `2`. The live quest component then moved
`2 -> 22`, posted `Reward: 20 copper`, granted XP and the reward item, and
rejected a second completion without another payout. The inventory finished
with the bought-back tonic and both independently owned Recruit Packs.

The no-hot-reload Mac build succeeded, all `38/38` automation tests passed,
saved economy data and map/UI validators emitted their exact success markers,
and initialized-world route traces retained the accepted Fenwatch composition.

Lesson: rollback is not one generic undo. Each ownership boundary needs a
preflight, a deliberate commit order, and an exact inverse for the mutation it
performs. Buyback history is valid only after the sale itself has committed.

## 2026-08-06 - Progress Survived The World

The Fenwatch economy finally gave Embermere progression worth keeping. Today's
work made that state survive beyond the objects that created it.

- Added format-version `1` save records for copper, XP, exact inventory stacks,
  equipped item identity/slot, quest progress/completion, and persistent vendor
  stock.
- Paired every item, quest, and stock soft path with its authored stable ID.
  The Fenwatch service now owns `FenwatchQuartermaster`, a world-stable identity
  independent of the quartermaster model and transform.
- Made load a two-phase transaction. Every path, ID, quantity, slot, level,
  capacity, quest, vendor-set, and finite-stock rule resolves before any live
  owner changes. Unsupported or malformed candidates leave wallet, bag,
  equipment, quest, stock, and existing buyback untouched.
- Defined buyback as session-only. A successful load clears it along with
  target, cooldown, and temporary-effect state, restores equipped bonuses
  idempotently, and initializes full health/mana. World position remains the
  fresh session's safe spawn.
- Added explicit `EmbermereSave` and `EmbermereLoad` console commands with HUD
  feedback plus a tracked two-session live validator.

The runtime acceptance deliberately crossed a world boundary. Fresh PIE built
the real commerce chain (`40 -> 32 -> 35 -> 32 -> 2`), completed Mara's quest
to reach `22` copper and `125` XP, equipped one Recruit Pack, left one pack and
one tonic in the bag, exhausted finite stock, and wrote
`EmbermerePrototype.sav`. After PIE ended, a second fresh world began at its
normal defaults, loaded the file, and restored every identity and stat. Loading
the same file again did not duplicate items, rewards, XP, copper, or equipment
bonuses.

The no-hot-reload Mac build succeeded, all `40/40` automation tests passed,
fresh-process vendor/UI/map validators retained their exact success markers,
and initialized-world traces kept the spawn corridor, Fenwatch shelter, and
three gate lanes clear.

Lesson: deserialization is not acceptance. A save file is an untrusted
multi-owner transaction; make the entire candidate eligible before committing
any part of it, then prove it against a genuinely fresh world.

## 2026-08-07 - The Chronicle Gave Persistence A Player Surface

Version 1 already survived a real world boundary, but only console commands
could reach it. Today's pass added `Embermere Chronicle`, a centered fixed
one-slot Save/Load panel that requests the proven persistence contract without
becoming another owner of durable state.

- `InspectSaveSlot` reads slot metadata without mutating gameplay and reports
  empty, valid, unreadable, or unsupported-version state.
- The accepted slot summary showed `22` copper, `125` XP, two bag stacks, one
  equipped item, and completed quest state.
- Save requires explicit overwrite confirmation when the local slot exists;
  Load always confirms replacement of the current session. Both have real
  cancel paths.
- Inventory, Vendor, and Chronicle hand off cursor/input mode without overlap.
  Plain `M` toggles Chronicle while `Ctrl+M` keeps its existing mouse-inversion
  behavior.
- Empty and rejected slots expose persistence-layer feedback. Console
  `EmbermereSave` and `EmbermereLoad` remain debug fallbacks; autosave,
  deletion, profiles, and implicit migration remain intentionally out of scope.

The authoritative no-hot-reload Mac build succeeded. Fresh commandlet and live
editor runs each passed all `42/42` tests, including new slot-inspection and
fixed-panel contracts. UI-art, vendor, saved-map, and initialized-world route
validators retained their success markers. A clean first PIE session exercised
the native overwrite/cancel and load-confirmation paths, and a second fresh PIE
session restored the exact accepted state twice without duplication, reward
replay, stock reset, or equipment-stat inflation.

Lesson: save UI is a lifecycle and communication layer, not a serialization
authority. Let it inspect, confirm, and report; let the already-tested atomic
contract decide whether a world may change.

## 2026-08-08 - The NPC Wrapper Learned To Idle

The quartermaster proved that one art-only wrapper could hold either static or
skeletal presentation, but the skeletal lane still lacked a bounded ambient
animation contract. Today's pass added a soft Idle animation, explicit looping
and play rate, skeleton compatibility checks, Anim Blueprint precedence, and a
resolved animation mode without moving any service rule into art.

The real lesson arrived after the first green build. Native automation proved
the exact Marsh Prowler mesh, Idle asset, single-node mode, loop flag, saved
playing flag, `0.75` rate, shared transform, and `NoCollision`. Fresh package
validators also passed. Live PIE still sat at frame zero.

Reading UE 5.8's local engine source exposed the lifecycle gap:
`SetAnimInstanceClass(nullptr)` clears the transient instance but leaves the
component in `AnimationSingleNode` mode. `OverrideAnimationData()` then stores
correct construction data without seeing a mode change, so no new
`UAnimSingleNodeInstance` is created. The wrapper now stores the durable data
and explicitly calls `InitAnim(true)` when the component is registered.

The repeated PIE-only swap then reported `playing=true`, `0.75x`,
`NoCollision`, and advanced from `0.0` to `1.4153` seconds on the real Prowler
Idle clip. Stopping PIE discarded the diagnostic swap, while the fresh
Fenwatch validator proved the saved quartermaster remained on its accepted
static lane.

The no-hot-reload Mac build succeeded, all `43/43` tests passed, and UI-art,
Fenwatch, and saved-zone validators retained their exact success markers.

Lesson: configuration, serialized intent, and active runtime state are three
different claims. A swappable animation lane is not accepted until the runtime
clock moves.

## 2026-08-09 - The Armsmaster Trained Progression, Not Art

The static-to-skeletal wrapper was ready for another NPC, but today's goal was
not merely to place a guard-shaped model. Embermere needed a trainer whose
visual identity, interaction, offering data, currency spend, XP reward, and UI
could all evolve independently.

- Added an art-free Fenwatch trainer service and data-driven offering asset.
  The first bounded `Combat Drills` action requires level 1 and converts 10
  copper into 25 XP.
- Made training atomic. Level, funds, malformed data, and XP overflow reject
  before mutation; an unexpected progression failure after spend refunds the
  exact copper amount.
- Added a fixed `500x300` native Fenwatch Training panel with selected-offering
  details, purse, action/rejection feedback, bracket selection, close control,
  and mutually exclusive Inventory, Vendor, and Chronicle handoff.
- Built the project-owned Fenwatch armsmaster through the deterministic Blender
  lane. The grounded source has 2,824 triangles and six materials; classic FBX
  import removes 24 degenerate triangles and saves a 2,800-triangle Unreal
  mesh. Both metrics are now separate tested contracts.
- Saved the art-only armsmaster wrapper and the invisible trainer service as
  separate co-located actors at `(-1320, -920, 0)`, yaw `100`.

The first asset preview technically passed but clipped the ember tip of the
staff, so the authored camera was corrected before import. In Unreal, the full
48-test suite passed with no warnings or failures. Fresh trainer, zone, and UI
validators emitted their exact success markers, and initialized-world traces
retained the clear spawn corridor, shelter center, and three gate lanes.

Clean PIE then proved the real loop: 40 copper and 0 XP became 30 copper and 25
XP, repeated training reached zero copper without an illegal mutation, the
panel displayed its insufficient-funds state, and Inventory and Chronicle each
replaced the trainer cleanly. The armsmaster remained grounded, readable, and
non-colliding behind the native panel.

One MCP detail was worth keeping. Unreal's visible bottom Python input did not
receive a Slate ref from the root snapshot. Observing the `SOutputLog` subtree
directly exposed its internal `SMultiLineEditableTextBox`, allowing the live
trace validator to run through supported localhost MCP instead of OS-level
keystroke injection.

Lesson: model, service, data, player state, and UI are separate acceptance
claims. Let art establish identity, let deterministic owners decide whether a
transaction is eligible, and prove the integrated result in the world.

## 2026-08-10 - Training Progress Survived, Then Gained A Yard

The trainer already mutated the same wallet and XP owners that Chronicle knew
how to save, but that architectural argument was only a hypothesis until the
whole lifecycle ran across two worlds.

Fresh PIE began at 40 copper and 0 XP. One real Combat Drills transaction made
that exactly 30 copper and 25 XP, and Chronicle saved through its normal player-
facing control. After PIE stopped, a second world proved its untouched 40/0
baseline before Load restored 30/25. A second confirmed load stayed at 30/25.
No item, equipment, quest, vendor-stock, buyback, reward, or schema mutation
appeared. Trainer offerings and panel state remained transient, and save version
1 remained unchanged.

With progression durable, the armsmaster needed a place that looked like
training rather than a generic crate. The new Fenwatch practice dummy came
through the same deterministic Blender/classic-FBX lane as the roadside family:

- `252.0 x 100.879 x 245.0` cm, 2,572 triangles, one UV channel, no
  non-manifold edges, applied scale, and a ground pivot;
- five shared Fenwatch materials rather than a new one-off palette;
- two authored collision boxes for the base and torso/core, with both
  outstretched sparring arms intentionally clear;
- one exact saved placement at `(-1120, -1120, 0)`, yaw `45`, replacing
  `FabPass_Village_Crate_C`.

The live Blender bridge refused the script because it clears the active scene.
That was a useful success, not an obstacle: Safe Mode protected the artist's
open file, while a factory-clean headless Blender process gave the reviewed
script the isolated context it required.

The first Unreal placement then passed every numeric check but aimed the target
away from the armsmaster. Human viewport review rejected it. Instead of leaving
an in-memory rotation behind, the deterministic placement script was corrected
and rerun, making the accepted composition reproducible.

All 48 tests passed. Fresh-process practice-dummy, saved-zone, trainer, vendor-
economy, and UI-art validators emitted their success markers with no Python
errors. The saved zone now contains 56 Fab placements and 19 original-art
placements, while initialized-world traces prove the dummy core solid and both
arms clear.

Lesson: safe tool refusal, deterministic eligibility, and human composition
review are all production gates. None is a substitute for the others.

## 2026-08-11 - The Armsmaster Became Embermere's First Rigged NPC

The temporary quartermaster experiment had proven that the presentation wrapper
could play a skeletal Idle. Today tested the harder claim: could a production
NPC adopt its own rig and animation without trainer behavior noticing?

The deterministic rigged build reused the reviewed armsmaster geometry and
produced a grounded `154.5 x 87.0 x 228.0` cm SkeletalMesh source with 2,824
triangles, six existing Fenwatch materials, nine authored bones, complete rigid
one-bone weights, and a restrained 97-frame Idle at 30 fps. The exact 3.2-second
clip moves the spine, neck, head, and shield hand while keeping feet and staff
planted. Blender Safe Mode stayed enabled; the scene-resetting build ran in a
factory-clean headless process.

The import pass earned three useful lessons:

- Unreal Python does not expose `USkeletalMesh.set_material()`. The importer
  updates each skeletal-material struct and writes the array back before saving.
- Deleting a SkeletalMesh and Skeleton immediately before recreating them in
  one commandlet can leave stale UObject references. The accepted importer
  replaces packages in place, reuses the valid Skeleton, and rejects a mesh
  whose expected Skeleton is missing.
- Classic FBX preserved Blender's Armature object as one additional root.
  Blender still authors nine reviewed bones; Unreal deliberately validates ten
  reference-skeleton bones, all nine authored names, and the authored `root`
  beneath the imported root.

The saved `Embermere_FenwatchArmsmaster_Trainer_01` now prefers
`SK_EmbermereFenwatchArmsmaster_01` and its exact Idle while retaining the
original static mesh as a reversible fallback. It remains `NoCollision` and
owns no marker, interaction, offering, wallet, XP, panel, or persistence state.
The co-located trainer service did not change.

Fresh rig, trainer, zone, practice-dummy, UI-art, and initialized-world route
validators all passed. The no-hot-reload build passed, and the expanded suite
finished 49/49. Clean PIE supplied the final gate: the Idle remained
`playing=true` while its clock advanced from `0.193888` to `1.670905` seconds,
with the grounded silhouette and training-yard route intact.

Lesson: a reusable architecture becomes real when the second implementation
uses it without special treatment. Art changed lanes; trainer authority did
not move.

## 2026-08-12 - The Quartermaster Proved The Rig Pipeline Could Repeat

The armsmaster established Embermere's first production skeletal NPC. Today's
question was whether the same contract could animate a second service character
without accidentally moving merchant behavior into its model.

The deterministic Blender build reused the accepted quartermaster parts and
produced the same grounded `120.842 x 93.0 x 217.0` cm silhouette, 3,632
triangles, six materials, and clean topology. It added nine authored bones,
complete rigid one-bone weights, and a restrained 121-frame Idle at 30 fps. The
exact 4.0-second clip plants the feet while giving the spine, head, ledger hand,
and free hand enough motion to feel alive. Blender Safe Mode remained intact;
the reviewed reset ran in a factory-clean process.

The importer revealed a useful UE 5.8 trap. Replacing an existing skeletal
package could silently attach Interchange import data even when the script
explicitly supplied `FbxFactory`. The accepted lifecycle is stricter now:
routine reruns validate and preserve already eligible classic-FBX packages;
intentional rebuilds run narrow cleanup, fresh classic-FBX creation, and fresh
validation in three separate Unreal processes. This avoids both stale UObject
references and importer-provenance drift.

The saved `Embermere_FenwatchQuartermaster_Vendor_01` now prefers the exact
skeletal mesh and Idle while retaining its original static mesh as a reversible
fallback. It remains `NoCollision` and owns no interaction, stock, pricing,
transactions, UI, or persistence. The separate vendor service did not change.

The no-hot-reload build passed and the suite finished 50/50. Fresh
quartermaster-rig, armsmaster-rig, vendor, trainer, zone, practice-dummy, and
UI-art validators passed. Initialized-world traces retained the spawn corridor,
shelter, three gate lanes, solid supply chest and dummy core, and clear dummy
arms. Clean PIE supplied the final proof: the quartermaster remained
`playing=true` while its animation position advanced from `0.853735` to
`2.195707` seconds, grounded beside the chest with the route open.

Lesson: idempotence is not repeatedly asking an importer to do work. It is
knowing when to validate, when to rebuild, and keeping those lifecycles far
enough apart that one cannot silently contaminate the next.

## 2026-08-13 - Mara Moved, Her Quest Did Not

The two service NPCs had already proven Embermere's production skeletal lane,
but Mara carried a harder architectural constraint. Her art still lived inside
`BP_QuestGiver`, alongside the interactable that owns her name, dialogue, quest,
marker, and rewards. Upgrading the keeper could not mean replacing that actor
or quietly giving a visual wrapper duplicate gameplay authority.

The deterministic Blender build reused the exact accepted keeper parts. The
rigged source retains 3,280 triangles, six materials, grounded
`107.45 x 71.0 x 207.5` cm bounds, and clean topology. It adds nine authored
bones, complete rigid weights, and a restrained 109-frame Idle at 30 fps. The
3.6-second loop moves the torso, head, and arms while keeping Mara's feet and
staff planted. Classic FBX adds the now-documented importer-owned Armature root,
so Unreal validates ten reference bones while retaining all authored names.

The saved migration kept `Quest_Giver_Mara_Fenwatch` and its interactable
unchanged. Its old SCS static component remains as a dormant transform template
with no render mesh. A colocated `AEmbermereNpcPresentationActor` now owns only
the rig, Idle, static fallback, shared visual transform, and project-art tag.
It remains non-colliding and has no quest or interaction component.

The no-hot-reload build passed, followed by 51/51 headless tests and 51/51 in a
freshly restarted editor through MCP. Fresh keeper and full-zone validators
passed. Clean PIE kept the silhouette grounded beneath the shelter and the gold
marker/name unobstructed; a fresh-module probe advanced the Idle from
`0.333814` to `1.525603` seconds. The remaining honest playtest is to walk into
Mara's real `F` radius and complete her dialogue and quest loop.

Lesson: asset-agnostic presentation is not permission to erase existing
ownership. Fingerprint the gameplay actor, extract only its art, and prove both
halves after the migration.

## 2026-08-14 - Mara Worked, Then Fenwatch Gained A Shop

Mara's rig migration had passed every structural gate, but today closed the
honest gameplay gate. A real `F` press opened the original Blueprint-owned
dialogue and accepted her quest. Real combat against the three saved Marsh
Prowlers advanced it to `3/3`, and the same original interactable completed the
return for 125 XP, 20 copper, and one Recruit Pack. Her separate rigged wrapper
stayed grounded, animated, non-colliding, and free of quest authority.

With that ownership proof complete, the quartermaster's generic fence became
the next obvious visual weak point. The new Fenwatch vendor stall came through
the deterministic Blender and classic-FBX pipeline at `394 x 242 x 306` cm,
5,476 triangles, five shared materials, and five authored collision boxes.
Its pale moss roof, timber frame, stone feet, iron counter, and ember crest now
give Fenwatch Supplies a place in the world without changing the vendor service.

The first technically valid placement was backwards. Classic FBX mirrored
Blender local Y, and a native collision trace found the counter on the wrong
side even though the viewport composition looked plausible. Rotating the saved
actor to yaw 180, rerunning exact package validation, and proving four supports,
the counter, customer approach, and east bypass turned that failure into a
durable placement contract. Focused PIE then opened the existing vendor panel
through normal `F` interaction.

The map now contains 55 grounded Fab actors and 20 original-art placements.
The no-hot-reload build, all 51 tests, fresh package validators, and live route
traces passed. The lesson is simple: an asset's authored front is gameplay
information, and collision can reveal directional mistakes that a screenshot
misses.

## 2026-08-15 - Mara Learned To Greet Without Taking Over

Fenwatch already had a working quest giver and a separate rigged keeper, but a
useful world NPC needs to communicate before and between full dialogue panels.
The risky shortcut would have been to put another quest state machine on the
art wrapper. Instead, Mara's existing quest data now owns four short pieces of
context copy, and the wrapper only observes the authoritative player quest-log
delegate and the exact `BP_QuestGiver` actor.

The native `320x56` screen-space greeting is opt-in, range-gated to `420` cm,
hit-test-invisible, `NoCollision`, and navigation-free. It presents a gold `!`
when the quest is available, cyan `...` while active, green `?` when ready to
turn in, and muted green `+` after completion. The panel stays beneath Mara's
existing marker and name and disappears outside range without moving the HUD.

Clean PIE supplied the ownership proof. Physical `F` still opened the original
Blueprint dialogue and accepted the quest. The authoritative quest component
advanced to `3/3`, the same interactable completed the return for exactly 125
XP, 20 copper, and one Recruit Pack, and a second `F` did not replay rewards.
The observer changed visual state after each authoritative transition but never
called a quest mutation itself.

The no-hot-reload build passed, all 52 tests passed, fresh greeting, keeper,
zone, vendor, trainer, UI, and NPC-rig validators passed, and initialized-world
traces retained the vendor approach, spawn corridor, shelter, gate, boundary,
chest, and practice-dummy collision contracts. The lesson is that reactive
presentation can make a world feel more alive without becoming a second owner
of gameplay truth.

## 2026-08-16 - Fenwatch Gained Its First Home

The service cluster finally behaved like a village, but it still lacked the
most basic settlement signal: a place where somebody could live. The new
Fenwatch cottage turns the far side of Mara's road into an inhabited edge while
leaving the keeper, vendor, trainer, and normal route readable.

The deterministic Blender build produced a grounded `580 x 422 x 503` cm
closed dwelling at 6,616 triangles with one UV channel, clean topology, the
existing five-material Fenwatch family, and two authored collision boxes for
the house body and doorstep. The moss roof, chimney, windows, trim, and ember
crest remain visual-only. The first preview exposed a new acceptance failure:
the camera's far clip hid the upper roof even though the geometry contract
passed. The tracked script now sets a `5000` clip range, and the next full-size
review caught and repaired visible door-jamb gaps before import.

Classic `FbxFactory` import explicitly saved the static-mesh package and placed
`Embermere_FenwatchCottage_West_01` at `(-2480, -260, 0)`, yaw `38`, replacing
only `FabPass_Village_Fence_02`. Exact package validation locked bounds,
topology, materials, two colliders, tag, and transform. Native initialized-
world traces proved the body and step solid, roof and chimney decoration clear,
the PlayerStart-to-Mara lane open, and a west-side bypass available.

Clean PIE showed the cottage reading beside, rather than over, Mara's service
cluster. A real `F` press still reached the original Blueprint-owned dialogue
and accepted `First Signs at the Ruin`. The no-hot-reload build succeeded; both
the live MCP suite and authoritative fresh-process suite passed 52/52; all
saved UI, NPC rig, greeting, vendor, trainer, practice-dummy, stall, cottage,
and full-zone validators passed. The map now contains 54 grounded Fab actors
and 21 original-art placements.

Lesson: a technically valid house is not a village module until its preview,
collision, composition, traversal, and neighboring gameplay ownership all pass
together.

## 2026-08-17 - The Training Yard Gained An Architectural Home

The practice dummy and armsmaster already made Fenwatch's training loop
legible, but they still stood beside a generic fence. Today's module had a
different job from the closed cottage: frame an activity, expose its tools,
and keep the playable space open.

The deterministic Blender build produced an open-front
`460 x 270.403 x 369` cm workshop at 5,624 triangles with one UV channel,
clean topology, five shared Fenwatch materials, and four authored collision
boxes. Only the two front supports, rear wall, and workbench are solid. The
roof, trim, ember crest, sword, spear, and small tools remain visual-only so a
decorative silhouette cannot become an invisible cage.

Classic `FbxFactory` import explicitly saved the static mesh and level, then
replaced only `FabPass_Village_Fence_03` with
`Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
`(-690, -1030, 0)`, yaw `-100`. Exact validation locked 5,624 triangles, four
colliders, five materials, unit scale, project ownership, and deliberate
spacing from the practice dummy, armsmaster, and nearby road pine.

The live route pass exposed a test bug worth keeping. The first east-bypass
trace started inside the existing road lamp, so it reported a collision before
it could ask anything useful about the workshop. Correcting the trace origin,
instead of moving accepted world art, produced the real proof: solid posts,
rear wall, and bench; clear open bay, armsmaster-to-dummy approach, decorative
roof/tools, and road-side bypass.

Clean PIE preserved the real Mara `F` quest handoff and transform-proved both
`W` and `S` autorun cancellation. The no-hot-reload build passed, the fresh
automation report finished 52/52 with zero warnings, every focused and
full-zone package validator passed, and all initialized-world route suites
passed. Fenwatch now contains 53 grounded Fab actors and 22 original-art
placements.

Lesson: open architecture is successful when it makes an activity feel like a
place while leaving gameplay more legible, not more constrained.

## 2026-08-18 - The Dummy Became A Target Without Becoming An Enemy

The Fenwatch practice dummy already looked like part of the training yard, but
giving that mesh combat authority would have fused art, collision, trainer
services, quest progress, and rewards into one hard-to-replace object. We kept
the Blender dummy and workshop exactly where they were and placed a separate
native, art-free gameplay actor at the dummy's transform.

`AEmbermerePracticeTargetActor` now reuses Embermere's actual tab-target,
hotbar, stats, native nameplate, and 48-segment cyan-circle path. It has 150
health and resets after three seconds, but it owns no mesh, collision,
navigation, AI, aggro, retaliation, leash, loot, XP, quest credit, trainer
interaction, or save state. A new `ShouldGrantDefeatCredit` target policy keeps
target eligibility separate from reward eligibility, while a centralized
dispatch helper deliberately handles both native C++ targetables and
Blueprint-generated targetables.

Fresh PIE found the kind of failure that construction tests rarely reveal: an
`ACharacter` with collision disabled still has CharacterMovement gravity. The
invisible target fell hundreds of thousands of centimeters below its visible
dummy while every serialized policy looked correct. The accepted actor now
freezes gravity, velocity, and movement in construction, BeginPlay, and reset,
and its validator locks `MOVE_None` at the exact authored transform.

The real gameplay proof used six hotbar Strikes. Damage landed as
`28, 28, 28, 28, 28, 10`; the target cleared on defeat, the visible dummy stayed
in place, and the actor returned at `150/150` after three seconds. Player health
remained `100`, XP remained `0`, inventory remained empty, and Mara objective
progress remained `0`. `Tab` immediately reacquired the reset target with its
nameplate and cyan circle.

The no-hot-reload Mac build passed, and a truly isolated commandlet run passed
all 54 tests. Focused, aggregate saved-package, full-zone, and initialized-world
trace validators passed as well. Two more operational lessons came with that
proof: close the real editor window and confirm MCP is down before calling a
commandlet authoritative, and give UnrealBuildTool host access to its trace
directory instead of mistaking sandboxed trace-rotation failure for a compile
error.

The useful rule is broader than a training dummy: an actor may be targetable
without being a hostile reward source, and world art may visualize combat
without owning it.

## 2026-08-19 - Combat Published A Fact, Then The UI Let It Go

The practice target could already take real damage, but its only immediate
feedback lived in the target frame and bottom-left log. Rather than letting a
widget infer hits from cooldowns or changing health, combat now publishes one
immutable `FEmbermereCombatResult` after each outcome is committed. The event
contains the source, target, ability, result kind, exact post-mitigation amount,
and lethal state. It applies nothing and owns no targeting, AI, loot, quest,
reward, or persistence behavior.

A standalone native UMG observer consumes only damage and future miss results.
It keeps three fixed slots, places newest first, evicts the oldest on a fourth
rapid result, rises and fades over 1.25 seconds, and clears on expiry, target
switch, defeat, practice-target reset, invalidation, or teardown. Target HP and
the clipped chat remain the durable facts.

PIE exposed two useful Unreal traps. First, generic actor bounds included the
screen-space nameplate and pushed the practice-target anchor more than ten
meters upward. The targetable contract now owns a deliberate presentation
anchor derived from visible character geometry. Second, the saved
`BP_StarterEnemy` predates that new BlueprintNativeEvent, so its generated
thunk returned a zero vector until resave. Dispatch now validates reflected
anchors and falls back to the inherited native implementation instead of
forcing an unrelated Blueprint package rewrite.

Normal-camera PIE accepted the same fixed `104x30` floating `28` beside the
practice target and a saved Marsh Prowler, with a 16-pixel nameplate gap,
readable cyan target circle, updated target HP, and intact chat. Deselect and
lethal reset removed it immediately. The no-hot-reload build passed, all 56
tests passed, and the sequential 13-package aggregate validator retained the
53 grounded Fab plus 22 original-art baseline.

Lesson: combat should decide what happened once. Short-lived presentation may
show that fact, but it should never reconstruct or own it.

## 2026-08-20 - Fenwatch Put Its Story Beside The Road

The combat-feedback slice was already accepted, so today's bounded art pass
returned to the village itself. Fenwatch had homes, services, a training yard,
and a road threshold, but no small civic object suggesting that people share
news, warnings, and work beyond the one quest currently implemented.

The deterministic Blender build produced
`SM_EmbermereFenwatchNoticeBoard_01`: a grounded
`286 x 93.927 x 277` cm roadside module at 3,684 triangles, with one UV
channel, clean topology, five existing Fenwatch materials, and three authored
collision boxes. Only the two supports and central panel are solid. The moss
roof, layered notice cards, pins, iron trim, and ember crest remain decorative.

Classic `FbxFactory` import explicitly saved the mesh and level, then placed
`Embermere_FenwatchNoticeBoard_Road_01` at `(-1560, -260, 0)`, yaw `-35`.
The board is deliberately presentation-only: it owns no quest marker,
dialogue, interaction, reward, vendor, trainer, or persistence behavior.

Focused validation caught two guessed neighbor labels before the aggregate
run. Reading the saved map established the real labels,
`PlayerStart_Embermere_Village` and `FabPass_Road_Flowers_01`, and converted
the spatial checks from assumptions into durable contracts. Post-restart
native traces then proved both supports and the panel solid, decorative layers
clear, and the PlayerStart-to-Mara, village-to-road, quartermaster, and
armsmaster routes open.

Normal-route PIE accepted the board's grounded road-side composition. The
no-hot-reload Mac build succeeded, the isolated commandlet passed all 56 tests,
and the aggregate now runs 14 focused package validators with explicit success
markers and no Python errors. The map contains 53 grounded Fab actors and 23
original-art placements.

Lesson: world art can imply stories without owning them. A convincing notice
board makes Fenwatch feel inhabited; a separate future service can decide
whether the player may ever read it.

## 2026-08-21 - Character Creation Became A Real Gate, Not A Second Rules Engine

The project already had eight race records, four class records, starter
abilities, and the Dwarf/Bullywug restriction matrix. What it did not have was
a player-facing lifecycle that could present those facts before play without
quietly inventing different rules in UI code.

The new native `940x560` pre-play picker keeps every race and class visible.
Changing race does not silently repair an invalid pending class. Dwarf Ranger
and Bullywug Wizard therefore remain selected, visibly disabled, explicitly
explained, and impossible to confirm. The widget owns only pending choice;
`UEmbermereRulesData` still owns legality, attributes, and abilities, while the
character revalidates and atomically applies the accepted identity exactly once.

Class starting attributes are now data-driven alongside starter abilities.
`AEmbermerePlayerController` owns the modal lifecycle: it hides the ordinary
HUD, blocks play, then restores game-only input and the normal HUD only after
the character accepts a legal pair. Human Warrior remains the reversible
construction fallback, and save version 1 deliberately remains unchanged.

Clean PIE proved both forbidden combinations and accepted Elf Wizard. The
modal cleared, the HUD returned, chat reported `Journey begun: Elf Wizard`,
health and mana became exactly `80/80` and `110/110`, and the hotbar resolved
Spark Bolt, Frost Root, Arcane Burst, and Meditate from existing rules data.
The no-hot-reload Mac build succeeded, all 60 automation tests passed in an
isolated commandlet, the fresh 14-package aggregate retained 53 grounded Fab
actors plus 23 original-art placements, and the initialized-world notice-board
trace suite kept all four protected routes clear.

Lesson: creation UI should make invalid state legible, then ask authoritative
gameplay to accept one choice. It should never become a friendlier-looking
duplicate of the rules engine.

## 2026-08-22 - Character Identity Crossed The Save Boundary

The creation gate proved that a race/class choice could initialize gameplay
correctly, but it was still temporary. Save version `2` now records the
deliberately confirmed race and class as stable semantic IDs rather than enum
ordinals, display names, inferred stats, or hotbar contents.

Load resolves those IDs through current rules, validates the pair, class
definition, starting attributes, and every starter ability before touching the
live world. Only then does one atomic commit rebuild identity, class base stats,
full vitals, and the starter hotbar before restoring equipment and durable
progression. Repeated load therefore replaces the same owners instead of
stacking class stats, abilities, equipment bonuses, or quest rewards.

Backward compatibility is an explicit interpretation rather than an implicit
migration. A version `1` save remains readable as current-rules Human Warrior,
and loading it does not rewrite the old file. Chronicle consumes the result
read-only: version `2` slots show race/class, while version `1` slots clearly
label the legacy fallback.

The tests cover version-2 round-trip and repeated-load idempotence, unknown and
illegal IDs, malformed records, complete rollback, and the version-1 fallback.
The no-hot-reload build passed, all 63 automation tests passed, and the fresh
14-validator package aggregate completed without Python errors.

PIE supplied the harder proof. A malformed preexisting version-2 slot was
safely rejected with Load disabled. Elf Wizard was then confirmed and saved at
exactly `80/80` health, `110/110` mana, and the Wizard starter hotbar. In a
fresh world, Lizardman Ranger first established a genuinely different live
identity. Loading replaced it with the exact Elf Wizard state, and a second
load remained unchanged.

Lesson: compatibility is a named policy, not a side effect. Validate the whole
semantic identity before mutation, then prove restoration by loading over a
meaningfully different live state.

## 2026-08-23 - Level Became A Derived Fact

Save version 2 made race and class durable, but XP was still only a number. The
next question was whether level should become another saved field. It should
not. Embermere now derives level from the durable XP it already owns, using a
validated cumulative curve of `0`, `100`, `250`, `450`, and `700` XP for the
first five levels.

Rules data owns that curve and distinct per-level race/class growth. Stats owns
live XP and the resulting level. A legal identity now resolves its base values
as class starting attributes plus combined race/class growth for every gained
level; equipment remains a separate additive layer applied exactly once. Live
gains preserve absolute missing health and mana, report exact XP, and emit one
multi-level-aware level-up message. Load performs the same resolution silently,
so it cannot replay celebration, rewards, or stat growth.

The persistence preflight exposed the sharpest lesson. An equipped item cannot
be validated against the character's current pre-load level, because the save
may imply a different level. The loader now derives the candidate level from
saved identity and XP first, validates equipment against that candidate state,
then commits identity, XP, base stats, hotbar, and one equipment layer
atomically. Repeated load replaces the same facts instead of stacking them.

Clean PIE used the real Trainer transaction for `25` XP, then Mara's original
physical-F quest flow for another `125`. Human Warrior reached level `2` at
`150` XP with exact `110/110` health, `53/53` mana, `12` Attack Power, `12`
Strength, `9` Spirit, `11.25` Agility, and `7.75` Intellect while retaining the
Warrior hotbar. Chronicle independently read `Human Warrior | Level 2`, `50`
copper, `150 XP`, one bag stack, no equipment, and the completed quest.

The no-hot-reload build passed, all 67 automation tests passed, and the fresh
15-package validator chain retained the 53 grounded Fab plus 23 original-art
world baseline.

Lesson: do not persist a consequence when its durable inputs and authoritative
rules can reproduce it. Derive once, validate the candidate world, then commit
every dependent owner together.

## 2026-08-24 - Progress Became Visible Without Becoming Authority

The first level curve worked, but the HUD still exposed XP as a bare total. We
needed the player to understand what `25 XP` meant without letting UMG acquire
its own copy of the curve. Stats now publishes one read-only presentation
snapshot containing the derived level, total XP, current and next thresholds,
cap state, and normalized within-level progress. The HUD consumes that result
as `XP 25 / 100` and a fixed gold bar; it never reads the rules asset or derives
a level itself.

Celebration follows a different path from steady state. A separate
hit-test-invisible observer listens only to a post-commit live level-transition
event, then shows a fixed `360x76` panel for 2.75 seconds. One large grant can
say `Advanced 3 levels`; the cap uses explicit copy. Save restoration performs
the same authoritative stat resolution but emits no live transition, so loading
can update the steady bar without replaying a level-up.

Two focused tests cover level-1 progress, `99/100`, exact-threshold rollover,
within-level normalization, cap copy, multi-level feedback, fixed geometry,
expiry, teardown, and silent restore. The no-hot-reload build passed, all 69
tests passed in a fresh commandlet, and the full 15-package validator retained
the 53 grounded Fab plus 23 original-art world baseline.

Lesson: steady state and celebration are different contracts. Publish one
authoritative snapshot for the former and one live-only event for the latter;
then persistence can restore facts without reenacting history.

## 2026-08-24 - One Modal Had Two Input Locks

A real Dwarf Warrior playtest found something the first character-creation
tests had missed: after confirmation and closing Inventory, the cursor was gone
but the mouse could not turn the camera. Live controller inspection showed both
movement and look were still ignored. Resetting those flags repaired the
running PIE session immediately and proved this was input ownership, not an
Inventory widget failure.

Character creation could be requested from both `OnPossess` and `BeginPlay`.
Unreal's ignore-input calls are reference-counted, so the modal had acquired
two locks and confirmation had released only one. The controller now tracks one
idempotent character-creation suppression state and changes Unreal's counters
only when that owned state changes. Returning from Inventory also leaves the
capture mouse-down unconsumed, allowing the first right-mouse press to rotate
instead of acting as a discarded viewport-capture click.

The same playtest exposed a separate layout issue: the Chronicle command
overlapped the top-right Inventory. It is now a larger fixed `140x38` control
anchored to the bottom-right with a `24`-pixel margin. A fresh-module PIE pass
accepted Dwarf Warrior, closed Inventory, proved move/look active and cursor
hidden, then reopened Inventory and verified the Chronicle stayed clear of the
Inventory, hotbar, and chat. The no-hot-reload build and all 69 tests passed.

Lesson: UI visibility is not proof that input ownership balanced. Treat every
modal lock as an owned resource, test duplicate lifecycle entry, and finish
with the exact human transition that originally failed.

## 2026-08-24 - Chronicle Learned The Difference Between Now And Saved

The Dwarf Warrior playtest uncovered another subtle truth after the input fix.
Chat said `Journey begun: Dwarf Warrior`, but Chronicle said `Elf Wizard`.
Nothing had corrupted character creation: Chronicle was correctly inspecting an
older Elf Wizard save slot. The problem was that the panel displayed saved-slot
metadata as though it described the live session.

Chronicle now names both owners. A gold `Current Journey` row reads the possessed
Dwarf Warrior directly, while `Saved Journey | Local slot` continues to inspect
the existing Elf Wizard save without mutation. The actions are now directional
too: `Save Current` and `Load Saved`. This lets the player understand the state
before deciding which side should replace the other.

The same report exposed a physical layout bug. The third saved-summary line for
bag stacks, equipment, and quest state shared space with the buttons. The panel
grew from `460x260` to `500x320`; the saved summary now owns a fixed `460x78`
region followed by a 12-pixel gap and a separate action row. Clean PIE recreated
the exact current Dwarf Warrior versus saved Elf Wizard state, showed both
clearly, and kept the full summary above the buttons without touching the slot.
The no-hot-reload build and all 69 tests passed.

Lesson: current state and persisted state are different products, even when
their values happen to match. Name both owners, label the direction of every
mutation, and test with intentionally different data.

## 2026-08-25 - A Locked Lesson Became A Live Progression Contract

Fenwatch Training began as one repeatable copper-to-XP transaction. Today it
became a real level-gated list without letting the panel acquire level rules.
The offerings asset now owns two unique stable IDs and explicit repeatability:
Combat Drills remains level 1 at 10 copper for 25 XP, while Advanced Combat
Drills requires level 2 and converts 20 copper into 50 XP. The service validates
the complete asset, asks Stats for the current derived level, preflights wallet
and XP overflow, commits atomically, and refunds the spend if progression ever
fails unexpectedly.

The first live pass exposed a subtle observer bug. Advanced correctly unlocked
when Stats reached level 2 and the Train action enabled, but the panel still
displayed its old `requires level 2` sentence. Eligibility authority was right;
the presentation snapshot was stale. Refresh now recomputes both the action and
status copy from the same service preflight. Clean PIE retained the visible
level-1 lock with zero mutation, then accepted a real Advanced transaction from
50 copper / 100 XP to 30 copper / 150 XP without reopening the panel.

Two focused tests brought the suite to 71. The no-hot-reload build, full fresh
commandlet suite, exact Fenwatch trainer validator, standalone progression
validator, and sequential 15-package aggregate all passed without Python
errors while retaining the 53 Fab plus 23 original-art map baseline.

Lesson: a disabled control and its explanation are one presentation contract.
When authority changes, refresh both from the same current preflight; otherwise
the UI can tell yesterday's truth beside today's valid action.

## Principles

- Make the first slice playable before making it huge.
- Prefer readable fantasy assets over photoreal asset soup.
- Keep systems data-driven so races, classes, abilities, enemies, quests, and art can change without rewiring code.
- Document decisions as we go so the project keeps its memory.
