# Embermere RPG

**Embermere** is a classic high-fantasy RPG prototype inspired by the feeling of early EverQuest and World of Warcraft: tab targeting, hotbar combat, readable fantasy silhouettes, dangerous wilderness just outside town, and that old-school sense that a tiny village can open into a much bigger world.

This project is also a build journal. We are learning Unreal Engine 5.8 in public, using Codex and GPT-5.5 as a development partner, and experimenting with Unreal MCP as a way to let AI help drive editor workflows, scene building, iteration, and QA.

![Embermere art direction: stylized classic, realistic dark fantasy, and painterly nostalgic fantasy concepts](Docs/Media/embermere-art-direction-triptych.png)

The current visual north star is the left side of this concept: **Stylized Classic** high fantasy with readable shapes, warm village light, chunky silhouettes, colorful wilderness, and a clear sense of where the adventure begins.

## The Game We Want To Build

The first playable slice is intentionally small:

- a shared starter zone called `L_Embermere_Prototype`
- a safe village hub with quest NPCs, vendors, and trainers
- a wilderness combat pocket with hostile starter enemies
- a small ruin landmark that gives the zone a reason to exist
- tab-target combat with nameplates, target frame, hotbar abilities, loot, XP, and quest progress

The long-term dream is a cozy-but-dangerous fantasy RPG that captures the social, exploratory, slightly mysterious feel of early online worlds while staying achievable for a small, AI-assisted build.

## The World Idea

Embermere begins at the edge of the known road: a lantern-lit village tucked against old woods, broken stone, marsh paths, and ruins that predate everyone living there. The first zone should feel friendly from the inn door and dangerous ten steps past the last fence.

The world tone we are chasing:

- classic high fantasy rather than grimdark
- inviting villages, readable roads, and landmarks you can navigate by memory
- wilderness that feels magical, old, and a little unsafe
- ruins that hint at deeper history without lore-dumping
- races and classes with strong silhouettes and tabletop-style identity
- combat that feels tactical and readable through tab targeting, hotbars, cooldowns, and clear enemy feedback

The starter zone is not meant to be huge. It is meant to be dense with promise: a place where one quest giver, one ruined tower, and a handful of hostile creatures can make the player feel like a bigger world is waiting.

## Current Prototype Foundation

The repo currently contains the C++ gameplay scaffold for:

- data-driven races, classes, and starter abilities
- Dwarf limited to Warrior/Cleric
- Bullywug limited to Warrior/Cleric/Ranger
- tab-target combat components
- classic MMO camera and mouse behavior
- `WASD` movement and `Q` autorun
- hotbar bindings for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`
- inventory, quest log, interactables, stats, combat, targeting, and hotbar components
- player/enemy character classes
- UMG base classes for character creation and HUD widgets
- native first-pass HUD panels for status, target, range state, quest tracking, dialogue, loot, and hotbar labels
- bottom-left chat/combat log for targeting, combat, quest, XP, inventory, and death/recovery feedback
- first hotbar cooldown enforcement and ready-time feedback
- starter enemy aggro, leash, return-home, attack, death, and respawn behavior
- player respawn with short damage protection for recovery safety
- structured inventory window with capacity, item rows, selected-item details, empty/reward states, `[`/`]` inspection cycling, and `I` show/hide behavior
- selected enemy UMG nameplate widget, HP bar/readout, and a flat animated 24-segment target ring
- live hotbar cooldown countdowns with unavailable-state dimming
- Unreal MCP setup notes and a local setup validator
- first editor-created Blueprints and rules data asset
- the saved starter-zone map `L_Embermere_Prototype`
- a corrected 65-actor local Fab/Epic art pass over the starter village, road, wilderness pocket, and ruin landmark, with upright rotations and an unobstructed PlayerStart/Mara route

## Starting Races

- Human
- Elf
- Dwarf
- Gnome
- Dark Elf
- Lizardman
- Ogre
- Bullywug

## Starting Classes

- **Warrior**: durable melee, threat, shield pressure
- **Cleric**: healing, smite, defensive blessings
- **Ranger**: bow/melee hybrid, snares, wilderness utility
- **Wizard**: roots, mana-heavy burst, arcane damage

## Asset Direction

The working visual north star is **Stylized Classic**: colorful high fantasy, readable silhouettes, lighter performance, and enough charm to avoid generic asset soup.

We are starting Unreal-first with Fab and Marketplace assets, then replacing or upgrading packs as the game identity sharpens. Gameplay systems are designed to stay asset-agnostic so art, VFX, icons, characters, and environments can be swapped without rewriting core mechanics.

The public repository intentionally ignores raw Fab/Epic marketplace asset folders. The saved map can reference locally imported packs, but the source asset packs themselves should be installed through Fab/Epic Launcher on each development machine rather than redistributed through GitHub.

See [Docs/ASSET_STRATEGY.md](Docs/ASSET_STRATEGY.md).

The current pack-by-pack shopping and import plan lives in [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md).

## Unreal And MCP Setup

The project targets Unreal Engine 5.8 and includes plugin configuration for:

- `ModelContextProtocol` / Unreal MCP
- `AllToolsets`
- `PythonScriptPlugin`
- `EnhancedInput`

Setup notes live in [Docs/UNREAL_SETUP.md](Docs/UNREAL_SETUP.md).

Continuing from a fresh Codex task? Start with
[Docs/THREAD_HANDOFF.md](Docs/THREAD_HANDOFF.md), then follow the live priorities
in [TODO.md](TODO.md).

After opening the project in Unreal, the intended MCP startup commands are:

```text
ModelContextProtocol.StartServer 8123
ModelContextProtocol.GenerateClientConfig Codex
```

Then validate locally:

```bash
zsh Scripts/check_unreal_setup.sh
```

## The Journey

This is not just a code repo. It is the record of building a fantasy RPG from zero Unreal experience into a playable prototype with modern AI-assisted development.

- [JOURNEY.md](JOURNEY.md) tracks decisions, milestones, and lessons.
- [TODO.md](TODO.md) keeps the daily next steps and automation handoff visible.
- [Docs/PLAYTESTING.md](Docs/PLAYTESTING.md) tracks the current editor smoke test.
- [Docs/UNREAL_LESSONS.md](Docs/UNREAL_LESSONS.md) captures Unreal-specific lessons learned during implementation.

## Status

Early playable Unreal prototype scaffold with MCP connected, a Fab-dressed starter-zone loop, tab-target combat, hotbar cooldown feedback, native HUD panels, bottom-left chat/combat feedback, first quest reward flow, inventory readout/toggle/detail text with bracket-key inspection cycling, and first native selected-target nameplate/ring feedback.

Next milestone: begin the first Fab art replacement pass and continue upgrading temporary target/inventory presentation into real fantasy UI treatment.
