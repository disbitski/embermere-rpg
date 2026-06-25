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
- Added the Fab asset plan and editor playtest checklist.

## Principles

- Make the first slice playable before making it huge.
- Prefer readable fantasy assets over photoreal asset soup.
- Keep systems data-driven so races, classes, abilities, enemies, quests, and art can change without rewiring code.
- Document decisions as we go so the project keeps its memory.
