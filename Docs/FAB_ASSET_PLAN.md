# Fab Asset Plan

Embermere does not have final high-fantasy art assets installed yet. The current map now has a first local Fab/Epic art pass layered over the original greybox gameplay shell. This document tracks what to add from Fab/Marketplace and how to keep those assets replaceable. Original project-owned art follows the complementary [Blender asset pipeline](BLENDER_ASSET_PIPELINE.md).

## Current State

- We have a playable starter map: `/Game/Maps/L_Embermere_Prototype`.
- We have gameplay Blueprints for player, enemies, game mode, and a quest giver.
- We have a first local environment pass using imported Epic/Fab packs, but we do not yet have production-ready fantasy buildings, creatures, weapons, icons, VFX, or UI art.
- We should start with free assets, then buy focused paid packs only when they clearly save time or improve cohesion.
- Blender has begun replacing the most identity-defining temporary props and
  architecture one bounded asset family at a time; Fab remains useful for
  breadth and specialist content.

## Imported Local Packs

These packs are installed locally through Fab/Epic and referenced by the map. They are intentionally ignored by Git so the public repository does not redistribute raw marketplace content.

- `Content/KiteDemo/`: Open World Demo Collection; used for working outdoor foliage/rocks where the assets resolve cleanly in UE 5.8.
- `Content/SoulCave/`: Soul: Cave; used for stone, roots, cave/ruin mood pieces, fog/water accents, and fallback foliage.
- `Content/ParagonProps/`: Paragon Agora/Monolith props; used for ruins, rocks, portals, pillars, and high-fantasy landmark silhouettes.
- `Content/Scifi_desert_city/`: Science Fiction Desert City Kit; now limited to neutral crates, fabric, fences, tables, and stools. The three full building shells and two temporary lamps were removed after project-owned replacements became available.
- `Content/SampleMap/` and `Content/Lighting/`: supporting content imported with Paragon sample assets.

First local placement pass:

- `Scripts/place_fab_zone_pass.py` stores the idempotent placement list for the `FabPass_` art layer.
- `Scripts/place_fab_zone_pass_unreal.py` runs that placement list through Unreal Python and saves `/Game/Maps/L_Embermere_Prototype`.
- `Scripts/validate_fab_zone_pass_unreal.py` loads the saved map and verifies the `FabPass_` actor count, required gameplay anchors, encounter layout, original-art placements, moss foundation material, and exact Mac-friendly daylight settings.
- The pass removes the old visual-only village blockout buildings, road markers, and ruin blockout pieces, then creates 62 tagged `EmbermereFabPass` actors in `04_Fab_Zone_Pass` outliner folders. A separate project-owned layer places `Embermere_Waystone_Road_01`, `Embermere_EmberLamp_Mara_01`, `Embermere_EmberLamp_Road_01`, `Embermere_RoadSignpost_01`, `Embermere_RoadGate_01`, and two matching `Embermere_BoundaryFence_*` actors, all tagged `EmbermereOriginalArt`, for seven original placements replacing temporary props and defining the road-to-wilderness threshold.
- The script keeps Mara, PlayerStart, quest data, combat, HUD, hotbar, inventory, nameplates, and target ring intact. Starter-enemy home points are deliberately authored in the setup script so collision-safe encounter tuning is reproducible.
- The Unreal Python helper assigns rotation fields by name. Do not use positional `unreal.Rotator(...)` arguments here; the first pass mapped intended yaw into pitch and tilted the environment.
- Validation rejects any `FabPass_` actor with meaningful pitch or roll and requires all seven original placements, their exact meshes/tags/transforms, and clean classic-FBX bounds plus authored box colliders for the imported Blender props. `Scripts/validate_road_boundary_traces_unreal.py` separately proves three road-gate lanes clear, one gate support solid, and both boundary-fence centers solid.
- The three Marsh Prowler homes now form a collision-cleared solo-pull triangle at `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`, with a `525` cm aggro radius and at least `800` cm between homes. Native WorldStatic overlap queries were used to avoid hidden vendor rock/stair collision.
- Safe-area and combat-pocket bands plus enemy marker meshes are visual guides only. Their saved components use `NoCollision`, and the validator rejects a rebuild that turns those decorations into invisible blockers.
- The map now includes a first Mac-friendly daylight baseline: `SkyAtmosphere`, real-time movable skylight fill, and restrained height-fog color/density. The validator asserts the sun, skylight, fog, and atmosphere settings so rebuilt maps do not regress to the previous black-sky state.
- `/Game/Art/Embermere/Environment/M_EmbermereGround` gives the zone plane and both prototype area platforms a muted moss color. The setup script reapplies it and validation rejects missing overrides.

## How To Install Assets

Fab's official docs say products can be explored through the Fab website or Fab in the Epic Games Launcher. Free products can be added to your library after accepting the applicable EULA. For Unreal Engine products, Fab docs say the files should be downloaded/imported into the project from My Library through the Fab integration or Epic Games Launcher.

In Unreal 5.8:

1. Open `L_Embermere_Prototype`.
2. Open Fab from `Window > Fab` or the Content Drawer toolbar.
3. Sign in with the Epic account.
4. Search with the lanes below.
5. Add free candidates to the library first.
6. Import into the project under a vendor folder, for example `/Game/ThirdParty/Fab/<PackName>`.
7. Place art into the level by replacing blockout actors, not by moving gameplay logic into vendor folders.

2026-07-03 note: direct unauthenticated Fab search/API access from terminal automation is Cloudflare-gated. Use the signed-in Unreal Fab window or Epic Games Launcher for the first real import, then let Codex inspect and wire the imported assets once they are present in `/Game/ThirdParty/Fab/<PackName>`.

Sources:

- Fab purchasing/downloading docs: https://dev.epicgames.com/documentation/en-us/fab/purchasing-and-downloading-assets-in-fab
- Fab launcher export docs: https://dev.epicgames.com/documentation/en-us/fab/exporting-assets-from-fab-in-launcher

## First Practical Import Order

For the first import pass, prefer a tiny, reversible slice over a broad art dump:

1. Keep the current first-pass art layer focused and playable.
2. Verify scale, collision, nameplate readability, and route readability in PIE.
3. Replace the placeholder sci-fi village shells with a better UE-compatible stylized fantasy village pack when we find one.
4. Pick one simple UI/icon or VFX candidate only after the environment pass proves scale, collision, and performance are healthy.
5. Record future pack names, Fab URLs, licenses, install dates, and caveats in this file before committing map references.

Good first replacements:

- `Village_Hall_Blockout`
- `Inn_Blockout`
- `Road_Marker_*`
- `Ruin_Broken_Arch_*`

Avoid importing creature packs first unless they include animation clips for idle, walk/run, attack, hit, and death. A static environment kit gives us faster visual payoff with less gameplay risk.

## First Asset Lanes

### Village Kit

Search terms:

- `stylized fantasy village`
- `modular medieval village`
- `stylized medieval buildings`
- `fantasy tavern blacksmith modular`

Must have:

- UE 5 compatibility.
- Modular walls, roofs, doors, windows, fences, crates, barrels, signs.
- Collision that works for a third-person character.
- Consistent texel density and readable silhouettes.

Use in map:

- Replace `Village_Hall_Blockout`.
- Replace `Inn_Blockout`.
- Replace `Blacksmith_Blockout`.
- Add fences and props around the safe area.

### Forest And Road Dressing

Search terms:

- `stylized forest`
- `fantasy forest foliage`
- `stylized grass rocks trees`
- `forest path modular`

Must have:

- Trees, bushes, grass, rocks, stumps, logs.
- LODs or Nanite-appropriate meshes.
- Wind/materials that do not tank Mac performance.

Use in map:

- Dress the path between village and wilderness.
- Create visual boundary lines without invisible walls.
- Add landmark trees near the road.

### Ruins And Stone Props

Search terms:

- `fantasy ruins`
- `ancient stone ruins`
- `stylized ruins`
- `modular dungeon ruins`

Must have:

- Broken arches, pillars, wall chunks, rubble, carved stones.
- Good collision or simple collision that can be adjusted.

Use in map:

- Replace `Ruin_Broken_Arch_Left`, `Ruin_Broken_Arch_Right`, `Ruin_Broken_Arch_Top`, and `Ruin_Fallen_Stone`.

### Creatures

Search terms:

- `stylized fantasy creature`
- `goblin enemy`
- `swamp creature`
- `animated monster`

Must have:

- Skeletal mesh and animations for idle, walk/run, attack, hit, death.
- Reasonable scale against the default Unreal mannequin/player capsule.
- Clear silhouette for tab targeting.

Use in map:

- Replace the marker mesh inside `BP_StarterEnemy`.
- Keep the enemy Blueprint as the gameplay wrapper.

### Weapons And Starter Gear

Search terms:

- `fantasy weapons pack`
- `stylized sword shield staff bow`
- `medieval weapon pack`

Must have:

- Sword, shield, staff, bow at minimum.
- Consistent material style.

Use in project:

- Soft-reference weapons from race/class/equipment data later.
- Do not hardwire weapon meshes inside combat logic.

### UI Icons And VFX

Search terms:

- `fantasy skill icons`
- `rpg ability icons`
- `spell icons`
- `niagara magic vfx`
- `fantasy hit effects`

Must have:

- Enough icons for all starter abilities.
- Clear cooldown readability at small hotbar size.
- Niagara effects with scalable performance settings.

Use in project:

- Ability data points to icons/VFX with soft references.
- UI widgets use icons from data, not hardcoded textures.

## Import Rules

- Keep third-party content under `/Game/ThirdParty/Fab/<PackName>`.
- Keep Embermere gameplay assets under `/Game/Blueprints`, `/Game/Data`, `/Game/UI`, `/Game/Maps`, and later `/Game/Art/Embermere`.
- Never edit vendor assets directly unless the change is trivial. Duplicate into `/Game/Art/Embermere` for project-specific variants.
- Add a short note in this file when we commit to a pack: name, source URL, license, install date, intended use, and any caveats.
- Do not commit raw marketplace/Fab asset folders to this public repo. Commit scripts, map references, and documentation; require developers to install the packs locally.

## Buy Later Criteria

Paid assets are worth it when they solve a real problem:

- one cohesive village/environment pack beats stitching ten free packs together;
- one animated creature pack gives us attack/death loops quickly;
- one fantasy UI kit makes the hotbar/target frame readable;
- a VFX pack gives combat feedback faster than building Niagara from scratch.

Paid assets are not worth it yet when:

- they only look nice in screenshots but do not include collisions/LODs;
- they force a photoreal look away from Stylized Classic;
- they bundle too much unrelated content and make the project heavy;
- they require rewriting gameplay systems around the asset pack's own framework.
