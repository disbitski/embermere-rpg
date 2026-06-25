# Embermere RPG

**Embermere** is a classic high-fantasy RPG prototype inspired by the feeling of early EverQuest and World of Warcraft: tab targeting, hotbar combat, readable fantasy silhouettes, dangerous wilderness just outside town, and that old-school sense that a tiny village can open into a much bigger world.

This project is also a build journal. We are learning Unreal Engine 5.8 in public, using Codex and GPT-5.5 as a development partner, and experimenting with Unreal MCP as a way to let AI help drive editor workflows, scene building, iteration, and QA.

## The Game We Want To Build

The first playable slice is intentionally small:

- a shared starter zone called `L_Embermere_Prototype`
- a safe village hub with quest NPCs, vendors, and trainers
- a wilderness combat pocket with hostile starter enemies
- a small ruin landmark that gives the zone a reason to exist
- tab-target combat with nameplates, target frame, hotbar abilities, loot, XP, and quest progress

The long-term dream is a cozy-but-dangerous fantasy RPG that captures the social, exploratory, slightly mysterious feel of early online worlds while staying achievable for a small, AI-assisted build.

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
- Unreal MCP setup notes and a local setup validator

Unreal Engine is still being installed on the target machine, so editor-created assets such as maps, Blueprints, widgets, imported Fab assets, and MCP-driven level work are next.

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

See [Docs/ASSET_STRATEGY.md](Docs/ASSET_STRATEGY.md).

## Unreal And MCP Setup

The project targets Unreal Engine 5.8 and includes plugin configuration for:

- `ModelContextProtocol` / Unreal MCP
- `AllToolsets`
- `PythonScriptPlugin`
- `EnhancedInput`

Setup notes live in [Docs/UNREAL_SETUP.md](Docs/UNREAL_SETUP.md).

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
- [TOMORROW.md](TOMORROW.md) keeps the immediate next steps visible.

## Status

Early prototype scaffold. Not playable yet.

Next milestone: finish Unreal installation, open the project, generate the MCP config, create the first map and Blueprints, then start building the tiny Embermere starter zone.
