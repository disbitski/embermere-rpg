# Asset Strategy

Embermere is Unreal-first for the prototype.

## Direction

The working style is Stylized Classic:

- readable silhouettes
- colorful high fantasy
- lighter performance profile
- easier cohesion across mixed Fab and Marketplace packs
- appropriate for tab-target combat readability

## Selection Rules

Prefer UE 5-ready Fab or Marketplace packs that provide:

- modular fantasy buildings
- foliage and terrain props
- village props
- ruins and dungeon pieces
- humanoid fantasy character meshes
- creatures for starter enemies
- weapons, VFX, SFX, and UI icons
- clean collision, LODs, and reasonable scale

Paid packs are fine later when they clearly save time or improve quality.

## Acquisition And Install Flow

Assets will be installed through Epic Games Launcher, Fab, and Unreal Editor once your Epic account and UE 5.8 are set up.

1. Find candidate packs in Fab or the Marketplace.
2. Check the license, supported Unreal version, file size, and whether the pack is content-only or includes Blueprints/code.
3. Add free packs to the Epic account library, or purchase paid packs only after we inspect whether they match Embermere.
4. Use Epic Games Launcher to add the asset pack to the `Embermere.uproject` project.
5. Open the project in Unreal and let it compile/import shaders as needed.
6. Keep imported packs under their original vendor folder in `/Game`, then create Embermere-specific wrapper Blueprints, materials, and data references elsewhere.
7. Use Unreal MCP to place, inspect, and iterate on imported assets once MCP is connected.

I can help choose, vet, import, organize, and wire assets after Unreal is installed. Anything that requires account login, clicking purchase, or accepting paid checkout has to be done by you. Once the assets are in the project or your library, I can take over the boring integration work.

## Starter Asset Shopping List

For the first playable slice, prioritize:

- one stylized modular village/building kit
- one stylized forest/foliage/environment kit
- one ruin/stone prop kit
- one humanoid character base or fantasy NPC pack
- one small creature/enemy pack suitable for starter wilderness combat
- one weapon pack with sword, shield, staff, bow, and simple one-handed weapons
- one stylized fantasy UI/icon pack
- one spell/VFX pack for holy, nature, arcane, and weapon impact effects
- one ambience/SFX pack for village, forest, combat hits, UI, and magic casts

## System Boundary

Gameplay code should not depend on a specific art pack.

- Race data points to meshes, portraits, and materials.
- Class/ability data points to icons, VFX, and SFX.
- NPCs and enemies use swappable mesh and animation references.
- Quest and combat logic lives in components and data assets, not in environment art Blueprints.

This keeps us free to start with free assets and upgrade to better packs without rebuilding the game.
