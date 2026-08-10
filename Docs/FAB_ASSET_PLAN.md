# Fab Asset Plan

Embermere does not have final high-fantasy art assets installed yet. The current map now has a first local Fab/Epic art pass layered over the original greybox gameplay shell. This document tracks what to add from Fab/Marketplace and how to keep those assets replaceable. Original project-owned art follows the complementary [Blender asset pipeline](BLENDER_ASSET_PIPELINE.md).

## Current State

- We have a playable starter map: `/Game/Maps/L_Embermere_Prototype`.
- We have gameplay Blueprints for player, enemies, game mode, and a quest giver.
- We have a first local environment pass using imported Epic/Fab packs plus a
  project-owned rigged starter creature, terrain material, icons, original
  prop/ground-cover family, one open-sided Fenwatch shelter, and Mara's
  grounded Stylized Classic keeper plus matching quartermaster and armsmaster
  presentations.
  The quartermaster now has a separate project-owned vendor service and stock
  data asset; the armsmaster has a separate trainer service and offerings data
  asset. A project-owned practice dummy now gives that service cluster a
  readable training-yard focal point. Service actors do not change the
  19-piece art baseline.
  We still do
  not have a cohesive production-ready fantasy building kit, player/race art,
  weapons, audio, or final UI skinning.
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
- `Scripts/validate_fab_zone_pass_unreal.py` loads the saved map and verifies the
  `FabPass_` actor count, required gameplay anchors, encounter layout,
  original-art placements, moss/earth material graph, and exact Mac-friendly
  daylight settings.
- The pass removes the old visual-only village blockout buildings, road
  markers, ruin blockout pieces, unsupported SoulCave accents, three enemy
  marker meshes, the Mara stone backdrop, the mismatched market cover, and the
  vendor/trainer cubes. The practice-dummy pass also replaces the remaining
  generic `FabPass_Village_Crate_C`, leaving 56 tagged `EmbermereFabPass` actors in
  `04_Fab_Zone_Pass` outliner folders. A separate project-owned layer contains
  eleven solid waystone/lamp/signpost/gate/fence/boundary-stone/supply-chest/
  shelter placements, the solid-core practice dummy, Mara's non-colliding
  Fenwatch keeper, the non-colliding Fenwatch quartermaster and armsmaster,
  plus four visual-only marsh-reed clusters, for 19 original-art placements.
- The script keeps Mara, PlayerStart, quest data, combat, HUD, hotbar, inventory, nameplates, and target ring intact. Starter-enemy home points are deliberately authored in the setup script so collision-safe encounter tuning is reproducible.
- The Unreal Python helper assigns rotation fields by name. Do not use positional `unreal.Rotator(...)` arguments here; the first pass mapped intended yaw into pitch and tilted the environment.
- Validation rejects any `FabPass_` actor with meaningful pitch or roll and
  requires all 19 original placements, their exact meshes/tags/transforms, the
  expected solid-prop colliders, and explicit `NoCollision` on the four reeds.
  It also verifies the keeper's exact static mesh, local offset/facing, unit
  scale, and `NoCollision` state on both the saved Blueprint SCS template and
  the placed Mara actor, without moving quest or interaction logic into art.
  The quartermaster assertion independently locks the native presentation
  wrapper class, static lane, exact transform, mesh metrics, six materials,
  project-owned tag, zero collision, and absence of interaction authority.
  `Scripts/validate_fenwatch_vendor_unreal.py` independently locks the
  co-located art-free vendor service, its stock asset, prices, quantities,
  transform, tags, and ownership separation.
  `Scripts/validate_fenwatch_trainer_unreal.py` independently locks the
  co-located art-free trainer service, one Combat Drills offering, imported
  armsmaster mesh, transform, tags, and ownership separation.
  `Scripts/validate_fenwatch_practice_dummy_unreal.py` locks the project-owned
  target's dimensions, topology, five shared materials, two authored support/
  core collision boxes, exact target-facing transform, and replacement of the
  generic crate.
  It also locks the two foliage transforms that reveal the accepted south-fence
  silhouette and rejects restoration of the replaced crate and unsupported
  accents. `Scripts/validate_road_boundary_traces_unreal.py` separately proves
  three road-gate lanes clear, one gate support solid, both boundary-fence
  centers solid, both boundary-stone cores solid, the supply-chest lid solid,
  all four Fenwatch-shelter supports solid, and the shelter center clear when
  run in the initialized live editor world. It also proves the practice-dummy
  base and torso/core solid, both outstretched arms clear, and the old
  PlayerStart-to-Mara player-height line clear after the chest moved to
  `(-1740, -1180, 0)`; the saved-map validator independently requires at least
  `225` cm of geometric corridor clearance.
- The three Marsh Prowler homes now form a collision-cleared solo-pull triangle at `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`, with a `525` cm aggro radius and at least `800` cm between homes. Native WorldStatic overlap queries were used to avoid hidden vendor rock/stair collision.
- Safe-area and combat-pocket bands are visual guides only. Their saved
  components use `NoCollision`; the redundant enemy marker meshes were
  removed.
- The map now includes a first Mac-friendly daylight baseline: `SkyAtmosphere`, real-time movable skylight fill, and restrained height-fog color/density. The validator asserts the sun, skylight, fog, and atmosphere settings so rebuilt maps do not regress to the previous black-sky state.
- `/Game/Art/Embermere/Environment/M_EmbermereGround` is now a project-owned
  38-expression moss/earth material with broad macro variation and a
  route-aligned path whose `PathHalfWidthCm` is `300`. The setup script
  reapplies it and validation rejects missing parameters, connections, or
  overrides.
- Twenty-one KiteDemo components use project-owned material overrides for
  current readability. Some source meshes still log absent internal vendor
  dependencies in a fresh process; those overrides do not repair the raw
  package graph. Replace incomplete meshes over time and never commit or resave
  raw vendor packs merely to hide those warnings.

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
3. Extend the accepted Fenwatch shelter into a cohesive Stylized Classic village
   family. Mara, the quartermaster, and the armsmaster now have project-owned
   presentations, the training yard has its first project-owned dummy, and the
   latter two NPCs use separate bounded service actors. The next art step can
   give the accepted armsmaster wrapper dedicated rigged/Idle art without
   coupling art to service rules, then extend the yard or settlement only when
   the added composition remains useful.
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
- The first item/body-slot lane is now project-owned under `/Game/UI/Icons`:
  Recruit Pack, Marsh Tonic, ten equipment slots, and category/missing-art
  fallbacks resolve through `UEmbermereUiIconSet`. Do not replace that
  data-driven contract when evaluating a later UI or ability-icon pack.
- Fab remains useful for broad starter ability/spell icon and Niagara coverage;
  Embermere-owned item/equipment art establishes the palette and fallback lane.

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
