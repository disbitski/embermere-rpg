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

## Principles

- Make the first slice playable before making it huge.
- Prefer readable fantasy assets over photoreal asset soup.
- Keep systems data-driven so races, classes, abilities, enemies, quests, and art can change without rewiring code.
- Document decisions as we go so the project keeps its memory.
