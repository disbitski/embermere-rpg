# Fab Asset Plan

Embermere does not have final high-fantasy art assets installed yet. The current map is a greybox made from Unreal basic shapes plus project Blueprints. This document tracks what to add from Fab/Marketplace and how to keep those assets replaceable.

## Current State

- We have a playable blockout map: `/Game/Maps/L_Embermere_Prototype`.
- We have gameplay Blueprints for player, enemies, game mode, and a quest giver.
- We do not yet have production-ready fantasy buildings, foliage, creatures, weapons, icons, VFX, or UI art.
- We should start with free assets, then buy focused paid packs only when they clearly save time or improve cohesion.

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

Sources:

- Fab purchasing/downloading docs: https://dev.epicgames.com/documentation/en-us/fab/purchasing-and-downloading-assets-in-fab
- Fab launcher export docs: https://dev.epicgames.com/documentation/en-us/fab/exporting-assets-from-fab-in-launcher

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
