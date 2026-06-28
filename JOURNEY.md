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

## Principles

- Make the first slice playable before making it huge.
- Prefer readable fantasy assets over photoreal asset soup.
- Keep systems data-driven so races, classes, abilities, enemies, quests, and art can change without rewiring code.
- Document decisions as we go so the project keeps its memory.
