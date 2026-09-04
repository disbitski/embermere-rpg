# Embermere New-Thread Handoff

Last updated: 2026-09-04

Repository baseline: public `main`; inspect `git log -1` for the current pushed
handoff commit rather than relying on a self-referential hash in this file.

Workspace: `/Users/wizard/Documents/Unreal Game`
Repository: https://github.com/disbitski/embermere-rpg

## Purpose

This is the durable context artifact for continuing Embermere in a fresh Codex
task. It records the decisions, working systems, editor setup, asset constraints,
verification workflow, hard-earned Unreal lessons, and immediate next work that
would otherwise be trapped in the original long conversation.

For day-to-day priorities, `TODO.md` remains authoritative and more current than
this snapshot. A new task should read files in this order:

1. `Docs/THREAD_HANDOFF.md`
2. `TODO.md`
3. `README.md`
4. `Docs/PLAYTESTING.md`
5. `Docs/UNREAL_LESSONS.md`
6. `Docs/FAB_ASSET_PLAN.md`
7. `Docs/INVENTORY_INTERACTION_PLAN.md` when item interaction work is active
8. `Docs/BLENDER_ASSET_PIPELINE.md` when original-art work is active
9. `Docs/NPC_PRESENTATION_CONTRACT.md` when NPC/service work is active
10. `Docs/VENDOR_SERVICE_CONTRACT.md` when economy work is active
11. `Docs/SAVE_GAME_CONTRACT.md` when persistence/lifecycle work is active
12. `Docs/MULTI_QUEST_CONTRACT.md` when quest/content work is active
13. `Docs/QUEST_LEDGER_PRESENTATION_CONTRACT.md` when quest UI/focus work is active
14. `Docs/QUEST_OBJECTIVE_PRESENTATION_CONTRACT.md` when quest copy/tracker work is active
15. `Docs/CHARACTER_CREATION_CONTRACT.md` when identity/lifecycle work is active
16. `Docs/LEVEL_PROGRESSION_CONTRACT.md` when XP/level/growth work is active
17. `Docs/PRACTICE_TARGET_CONTRACT.md` when training-target work is active
18. `Docs/COMBAT_FEEDBACK_CONTRACT.md` when combat-result presentation is active
19. `Docs/MARSH_PROWLER_ART_BRIEF.md` when creature work is active
20. `Docs/GROUNDING_AND_TERRAIN_PASS.md` for environment contact/readability
21. `JOURNEY.md` when historical detail is useful

## One-Page State

Embermere is a single-player Unreal Engine 5.8 prototype with the feel of an
early-2000s tab-target MMORPG. It has a playable starter loop in
`L_Embermere_Prototype`: spawn near a village edge, find Mara Fenwatch, accept a
quest, travel toward a wilderness/ruin pocket, tab-target Marsh Prowlers, use
hotbar abilities, survive aggro and melee attacks, defeat enemies, return to
Mara, receive XP and an inventory reward, and inspect that reward.

The project currently includes:

- classic MMO movement, camera, autorun, mouse inversion, and tab targeting;
- data-driven race/class restrictions and starter abilities;
- functional data-driven starter utility effects: timed Attack Power and Armor
  buffs, Snare, Frost Root, and Meditate mana recovery;
- quest, inventory, equipment, stats, combat, targeting, interactable, and hotbar systems;
- hostile enemy aggro, chase, attack, leash, return-home, death, and respawn;
- player death, finite-world fall recovery, autorun cancellation, respawn,
  velocity/movement restoration, and short recovery damage immunity;
- a styled native HUD with player/target status, quest tracker, hotbar,
  clickable/draggable inventory rows and equipment slots, dialogue, loot
  feedback, nameplates, a bounds-aware cyan-blue emissive target circle,
  project-owned fantasy drag tokens, data-driven item/slot icons,
  sixteen data-driven starter-ability icons with tooltips, an illustrated
  paper-doll equipment backdrop, data-driven timed buff/control rows with live
  countdowns, chat log, and a fixed three-entry floating-damage observer fed by
  immutable post-commit combat results, plus a fixed eight-row native Quest
  Ledger with transient compact-tracker focus, quest-data-owned objective
  instructions in its selected detail and fixed compact tracker, and no quest
  mutation authority;
- a grounded local Fab/Epic art pass with 53 upright environment actors and 24
  project-owned placements from an original Blender-built Embermere
  waystone/lamp/signpost/gate/fence/boundary-stone/chest/shelter/keeper/
  quartermaster/armsmaster/practice-dummy/reed family, a 38-expression moss/
  earth road material, and a Mac-friendly daylight baseline;
- a reusable art-only NPC wrapper with static and skeletal lanes, shared
  transforms, soft references, Anim Blueprint precedence, a
  skeleton-compatible single-node Idle lane, and no interaction or service
  ownership, now used by the saved production rigged Fenwatch keeper,
  armsmaster, and quartermaster with reversible static fallbacks. Mara's
  original Blueprint quest actor remains her sole gameplay authority while
  the keeper wrapper optionally observes that state for a fixed contextual
  greeting;
- a separate Fenwatch vendor vertical slice with an art-free interactable
  service actor, data-driven stock/prices/sell values, earned player copper,
  atomic buy/sell/buyback rollback, fixed native stock UI, and saved ownership
  validation;
- a separate Fenwatch trainer vertical slice with an art-only rigged and
  Idle-animated armsmaster, art-free interactable service, two stable-ID
  repeatable Combat Drills tiers with data-driven level gates, atomic copper-
  to-XP progression, fixed native training UI, and a matching solid-core
  practice dummy that owns no service authority;
- a versioned save-game contract that atomically captures and restores confirmed
  race/class identity, copper, XP, inventory/equipment identity, a bounded
  keyed quest ledger, and finite vendor stock. Version 3 keeps stable semantic
  identity IDs, writes stable quest/objective records, validates the entire
  ledger before mutation, and reads version-1/version-2 singular quest history
  through explicit adapters without rewriting old slots. Version 1 still has
  its explicit Human Warrior identity fallback. Buyback, focused quest,
  combat, cooldowns, temporary effects, and position remain session-only;
- `Still Waters`, the first real parallel quest over that ledger: a separate
  art-free notice-board owner offers and turns it in, while a dedicated router
  converts only immutable committed communal-well rest success into exact
  `FenwatchStillWaters/FenwatchRestCompleted` progress. It grants `50` XP and
  `10` copper once; the board/well art, rest service/VFX, and trainer remain
  quest-free;
- a fixed `620x430` native Quest Ledger with eight stable `596x30` row cells,
  exact active/ready/completed state, mouse and keyboard selection, explicit
  transient focus, a bottom-right command above Chronicle, and mutually
  exclusive Inventory/Chronicle/service panel handoff. It consumes the keyed
  quest log read-only and neither serializes focus nor mutates progress or
  rewards;
- data-driven level progression that derives levels 1 through 5 from durable
  XP thresholds, combines validated race/class growth, rebuilds identity base
  stats atomically, applies equipment once, restores silently, and exposes
  level read-only to HUD, Trainer, and Chronicle without saving it twice. One
  removable live-only observer adds a class-colored twelve-segment ground
  effect without owning progression, collision, navigation, or save state;
- the first original rigged Marsh Prowler with six animations and
  asset-agnostic runtime presentation across all three saved enemy instances;
- a fixed native pre-play character picker that presents all eight races and
  four classes, exposes disabled combinations, atomically applies data-driven
  starter stats/abilities once, restores the normal controller/HUD path, and
  feeds the confirmed identity into the version-3 persistence contract;
- 85 passing Unreal automation tests plus fresh-process character-creation,
  derived-level progression, and
  character-identity and multi-quest persistence,
  combat-feedback,
  practice-target, keeper-greeting,
  keeper-rig,
  armsmaster-rig, quartermaster-rig, practice-dummy, trainer, vendor,
  UI-art/package, and
  saved-map validators, initialized-live-world route/collision traces,
  measured live NPC Idle advancement, accepted two-session Chronicle proofs
  for the full commerce/quest fixture, baseline trainer progression, and a
  richer Advanced Combat Drills result, plus live normal-camera acceptance of
  the class-colored level-up world effect.

This is still prototype art. The first black-sky problem is corrected, but the
scene remains mixed in style and is missing a cohesive fantasy village kit,
player/race characters, weapons, final ability VFX, audio, and final UI art.

## Game Vision

The target is a cozy-but-dangerous classic high-fantasy RPG inspired by the
feeling of early EverQuest and World of Warcraft:

- readable tab-target combat instead of action combat;
- dangerous wilderness close to a welcoming village;
- roads and landmarks navigable by memory;
- colorful, readable **Stylized Classic** fantasy rather than grimdark;
- an old world suggested by ruins and environment, not lore dumps;
- tactical hotbar abilities, cooldowns, threat, healing, roots, snares, and
  strong class identity;
- a small, polished first zone before expanding scope;
- Mac as the primary development/play target, with Windows 11 as a later
  supported build target.

The current version is single-player with MMO feel. Do not introduce networking
or backend scope until the core slice is stable and the user explicitly chooses
that milestone.

## Races And Classes

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

- **Warrior**: durable melee, threat, shield pressure.
- **Cleric**: healing, smite, defensive blessings.
- **Ranger**: bow/melee hybrid, snares, wilderness utility.
- **Wizard**: roots, mana-heavy burst, arcane damage.

Allowed combinations:

| Race | Warrior | Cleric | Ranger | Wizard |
| --- | --- | --- | --- | --- |
| Human | Yes | Yes | Yes | Yes |
| Elf | Yes | Yes | Yes | Yes |
| Dwarf | Yes | Yes | No | No |
| Gnome | No | Yes | No | Yes |
| Dark Elf | Yes | No | Yes | Yes |
| Lizardman | Yes | No | Yes | No |
| Ogre | Yes | Yes | No | No |
| Bullywug | Yes | Yes | Yes | No |

The matrix is data-driven. Preserve these explicit product decisions:

- Dwarves cannot be Rangers or Wizards.
- Bullywugs cannot be Wizards.
- `Bullywug` is acceptable during prototyping but should be renamed to an
  original equivalent before a public/commercial release.

## Controls

| Input | Behavior |
| --- | --- |
| `WASD` | Move |
| `Tab` | Cycle nearby hostile targets in front of the player |
| Hold right mouse | Rotate camera and character facing together |
| Hold left mouse | Rotate camera only |
| Hold both mouse buttons | Move forward |
| `Q` | Toggle autorun |
| `W` or `S` | Cancel autorun while applying manual movement |
| `Ctrl+M` | Toggle inverted/normal mouse Y; inverted is the prototype default |
| `1`, `2`, `3`, `4` | First four class abilities |
| `Alt+R`, `Alt+E`, `R`, `X`, `E`, `F` | Remaining hotbar slots; `F` is Interact |
| `I` | Show/hide inventory and switch cursor/game input mode |
| Mouse click or `[` and `]` | Select/cycle the inspected inventory stack |
| Inventory action button | Equip or unequip the selected eligible item |

The desired camera feel is early EverQuest/WoW third person. Do not replace it
with a modern over-the-shoulder action camera.

## Current Playable Slice

Map: `/Game/Maps/L_Embermere_Prototype`

Important gameplay actors/assets:

- PlayerStart near the village edge.
- Mara Fenwatch, the first quest giver, with a temporary gold quest marker.
- Three starter Marsh Prowlers in a collision-cleared solo-pull triangle in
  the wilderness pocket, using a `525` cm aggro radius.
- A village-to-road-to-wilderness route and an upgraded ruin landmark.
- Starter quest and reward item data assets.
- The tracked Recruit Pack reward is configured as level-1 Back-slot armor so
  the first Equip/Unequip action is reachable through the normal quest loop.
- Marsh Prowlers grant one stackable Marsh Tonic on death; it restores up to
  25 health and 10 mana through the inventory `Use` action.
- Blueprint children/wrappers for the C++ player, enemy, game mode, and quest
  giver systems.

Expected first quest loop:

1. Spawn and locate Mara.
2. Press `F` to accept the starter quest.
3. Follow the dressed road toward the wilderness pocket.
4. Press `Tab` to select a Marsh Prowler.
5. Use abilities, observe cooldown/range feedback, and defeat an enemy.
6. Confirm Marsh Tonic loot reaches inventory, then use one after taking damage.
7. Defeat three enemies, return to Mara, and press `F`.
8. Receive XP and a starter item.
9. Inspect the reward in the inventory window.

## UI And Combat Presentation

Implemented presentation includes:

- top-left player level, XP, HP, mana, and bars;
- target panel with target HP and ability range state;
- fixed `112x32` floating damage cells beside the world nameplate, capped at
  three rapid outcomes and cleared on expiry, switch, defeat, reset, or teardown;
- bottom-center 10-slot hotbar with stable key labels;
- cooldown enforcement, live countdown text, and dimmed cooling slots;
- bottom-left clipped six-line chat/combat log;
- quest tracker, dialogue panel, and loot/reward popup;
- Mara's temporary gold quest marker;
- native screen-space UMG enemy nameplate with selected marker, name, HP text,
  HP bar, and health-aware color;
- complete non-colliding 48-segment cyan-blue target circle using the tracked
  unlit opaque-emissive `M_EmbermereTargetRing` material, transformed visual
  bounds plus padding, and selected-state ground tracing;
- 700x330 structured inventory/equipment window with `Slots X / 24`, eight visible rows,
  selected-item details, quantity, stack limit, description, empty/reward state,
  clickable/draggable highlighted rows, cursor-aware input mode, keyboard footer, `I`
  toggle, bracket cycling, category/slot/level/effect metadata, ten clickable
  and drop-target paper-doll controls, aggregate bonuses, transactional Equip/Unequip, and safe
  consumable Use actions;
- net item-stat comparison against current gear or an empty destination slot,
  plus hover tooltips for populated bag rows and occupied equipment slots;
- item-identity drag payloads, bag-to-equipment and equipment-to-bag drop paths,
  plus a fixed category-sigil fantasy token and gold/red target feedback on the
  existing atomic transaction layer;
- explicit stable category/name sorting with selected-item and duplicate-stack
  occurrence preservation plus pending/active-drag protection;
- a project-owned data-driven icon family with explicit Recruit Pack and Marsh
  Tonic art, all ten paper-doll slot symbols, all sixteen starter-ability
  illustrations, category/slot/ability fallbacks, and fixed UI dimensions.
- a project-owned `128x160` armored-adventurer illustration, resolved through
  the same data asset and layered beneath the unchanged ten-slot grid as
  hit-test-invisible presentation.

These systems are functional first-pass UI. Dedicated fantasy UI materials,
final ability VFX, chat scrolling, and final responsive layout remain future
work. The paper-doll still needs clean-PIE contrast acceptance. The interaction contract is in
`Docs/INVENTORY_INTERACTION_PLAN.md`.

## Source Architecture

Runtime module: `Source/Embermere`

Core gameplay:

- `Characters/EmbermereCharacter.*`: player pawn behavior, movement/camera,
  autorun, combat-facing integration, death/recovery.
- `Characters/EmbermereEnemyCharacter.*`: enemy presentation, aggro/combat,
  leash/return, death/respawn, nameplate, target ring.
- `Game/EmbermerePlayerController.*`: input and player-facing control path.
- `Game/EmbermereGameMode.*`: game mode and starter setup.

Components:

- `EmbermereCombatComponent`: ability execution and combat rules plus immutable
  post-commit result publication; presentation observes but cannot mutate it.
- `EmbermereHotbarComponent`: slot definitions, cooldowns, activation.
- `EmbermereTargetingComponent`: tab-target selection/cycling.
- `EmbermereStatsComponent`: HP/mana/XP, damage, death, immunity.
- `EmbermereInventoryComponent`: item stacks and capacity.
- `EmbermereEquipmentComponent`: slot eligibility, equip/replace/unequip state.
- `EmbermereQuestLogComponent`: quest state and completion.
- `EmbermereInteractableComponent`: interaction range/behavior.
- `EmbermereWalletComponent`: player-owned copper.
- `EmbermereVendorComponent`: stock and bounded buyback state plus purchase,
  sell, and buyback preflight, commit, and rollback-safe transaction rules.

Data and types:

- `Data/EmbermereRulesData.*`: data-driven races, classes, and rules.
- `Data/EmbermereItemData.*`: item identity, category, equipment slot, level requirement, and stat bonuses.
- `Data/EmbermereUiIconSet.*`: item-category, equipment-slot, and missing-art
  icon resolution through soft texture references.
- `Data/EmbermereQuestData.h`: quest definition.
- `Data/EmbermereVendorStockData.h`: vendor name, item soft references,
  prices, and finite/unlimited initial quantities.
- `Types/EmbermereTypes.h`: shared gameplay enums/structs.
- `Types/EmbermereItemTypes.h`: item categories, equipment slots, and stat bonuses.
- `Interfaces/EmbermereTargetable.h`: targetable contract.

UI:

- `UI/EmbermerePlayerHudWidget.*`: native HUD and inventory layout.
- `UI/EmbermereEnemyNameplateWidget.*`: selected enemy nameplate.
- `UI/EmbermereCharacterCreationWidget.*`: race/class creation scaffold.
- `UI/EmbermereGameplayMessageLibrary.*`: routes gameplay feedback into HUD.
- `UI/EmbermereVendorStockButton.*`: indexed native stock-row interaction.

Vendor service:

- `Characters/EmbermereVendorServiceActor.*`: art-free interaction and vendor
  owner saved beside the independent quartermaster presentation.
- `Docs/VENDOR_SERVICE_CONTRACT.md`: ownership, transaction, validation, and
  current prototype limits.

Persistence:

- `Save/EmbermereSaveGame.h`: versioned durable records for confirmed
  race/class identity, wallet, XP, inventory/equipment identity, quest state,
  and finite vendor stock.
- `Save/EmbermerePersistenceLibrary.*`: stable-ID capture, whole-snapshot
  preflight, atomic restore, slot save/load, and session-only-state reset.
- `Docs/SAVE_GAME_CONTRACT.md`: version 2 ownership, validation, version 1
  compatibility interpretation, lifecycle, and explicit non-goals.
- `Scripts/validate_persistence_live_unreal.py`: two-phase live PIE proof that
  saves the real economy/quest/equipment state and restores it in a fresh PIE
  session without duplication.

Automation lives in:

- `Source/Embermere/Private/Tests/EmbermereRulesTests.cpp`

Prefer the existing C++/native UMG/data-driven patterns. Keep gameplay logic
outside vendor asset folders and environment meshes.

## Local Fab/Epic Assets

The following packs are installed locally and intentionally ignored by Git:

- `Content/KiteDemo/`: foliage, rocks, and outdoor dressing.
- `Content/SoulCave/`: cave stone, roots, ruin mood, fog/water accents.
- `Content/ParagonProps/`: Agora/Monolith ruins, rocks, portals, pillars.
- `Content/Scifi_desert_city/`: only neutral props such as crates, fabric,
  lamps, fences, tables, and stools.
- `Content/SampleMap/` and `Content/Lighting/`: supporting imported content.

The public repository must not redistribute raw Fab/Marketplace assets. A new
machine must install the packs through the signed-in Fab integration or Epic
Games Launcher. Saved map references may be missing until those packs exist.

Environment scripts:

- `Scripts/place_fab_zone_pass.py`: idempotent base placement recipe; the saved
  map retains 55 Fab actors after project-owned replacements and removal of
  unsupported accents and enemy markers.
- `Scripts/place_fab_zone_pass_unreal.py`: executes placement through Unreal
  Python and saves the map.
- `Scripts/validate_fab_zone_pass_unreal.py`: reloads and validates the saved
  map, actor count, upright rotations, gameplay anchors, collision-cleared
  encounter layout, 20 original-art placements, moss/earth terrain, and exact
  Mac-friendly daylight values.
- `Scripts/configure_starter_items.py`: idempotently migrates tracked starter
  item assets, currently the level-1 Back-slot Recruit Pack reward.

The three oversized sci-fi building shells were removed because they blocked
PlayerStart and Mara readability. The current zone still needs proper Stylized
Classic fantasy village buildings.

## Original Blender Assets

Blender is the active source tool for project-owned static props, ruins,
modular architecture, weapons, and later character art. The durable setup,
security model, technical contract, and acceptance checklist live in
`Docs/BLENDER_ASSET_PIPELINE.md`.

The first pilot is `SM_EmbermereWaystone_01`, built by the reviewed
`Scripts/blender/build_embermere_waystone.py` script. The script creates the
render mesh, two Unreal-named collision boxes, UVs, an FBX export, a `.blend`
source file, a preview render, and deterministic metrics. On 2026-07-13 Blender
5.1.2 and the pinned MCP bridge were installed, the script ran through the
approved-path flow, inline execution was proven blocked, and the resulting
1,340-triangle asset imported into Unreal with three materials, one UV channel,
and two explicit collision boxes. It now replaces `FabPass_Road_Stump_01` in
the saved starter map as `Embermere_Waystone_Road_01`.

The second original asset is `SM_EmbermereEmberLamp_01`, built by
`Scripts/blender/build_embermere_ember_lamp.py`. It reuses the waystone's pale
stone, moss, and ember language with a dark iron cage and warm faceted crystal.
The saved map contains Mara-side and road-side lamp instances replacing two
temporary sci-fi lamps. Unreal retains two authored `UBX_` box colliders; the
validator requires clean classic-FBX provenance, exact bounds, transforms,
tags, and both collision boxes before accepting the map.

The third original asset is `SM_EmbermereRoadSignpost_01`, built by
`Scripts/blender/build_embermere_road_signpost.py` and imported through the
explicit classic-FBX lane in `Scripts/import_embermere_road_signpost_unreal.py`.
It adds a broad timber sign silhouette while reusing stone, moss, iron, and
ember materials. The saved actor `Embermere_RoadSignpost_01` sits beside the
village road at `(20, -170, 0)`, yaw `22`; two authored boxes cover the base
and post while its overhead arms remain non-colliding. The importer explicitly
saves the new `M_EmbermereTimber` package so it cannot exist only in memory.

The fourth original asset is `SM_EmbermereRoadGate_01`, built by
`Scripts/blender/build_embermere_road_gate.py` and imported through
`Scripts/import_embermere_road_gate_unreal.py`. It reuses the complete
stone/moss/timber/iron/ember material family and frames the road into the
wilderness at `(1080, 540, 0)`, yaw `20`. Four authored boxes cover the two
footings and posts while the 250 cm center opening and overhead span remain
clear. Native traces and the saved-map validator both enforce that traversal
contract: traces prove the opening/support behavior, while validation locks the
four colliders, mesh bounds, materials, and actor transform.

The fifth original model type is `SM_EmbermereBoundaryFence_01`, built by
`Scripts/blender/build_embermere_boundary_fence.py` and imported through
`Scripts/import_embermere_boundary_fence_unreal.py`. It is a 2,632-triangle,
`78 x 328 x 180` cm low stone/moss/timber/iron/ember module with three authored
collision boxes. The saved south and north instances continue both sides of
the road gate at yaw `20`, bringing the map to seven original-art placements.
The saved-map validator locks their exact assets, transforms, tags, materials,
bounds, and colliders; a separate native-trace validator proves both fence
centers solid while three lanes through the gate remain clear.

The sixth original model type is `SM_EmbermereBoundaryStone_01`, built by
`Scripts/blender/build_embermere_boundary_stone.py` and imported through
`Scripts/import_embermere_boundary_stone_unreal.py`. It is a 1,872-triangle,
`96 x 92 x 253.731` cm rune-topped stone/moss/timber/iron/ember marker with two
authored collision boxes. South and north instances terminate the fence family
at gate-local Y `-570` and `570`, bringing the map to nine original-art
placements. The same integration pass relocates only the two vendor trees that
masked the south fence; exact validator assertions preserve those accepted
foliage transforms.

The seventh original model type is `SM_EmbermereSupplyChest_01`, built by
`Scripts/blender/build_embermere_supply_chest.py` and imported through
`Scripts/import_embermere_supply_chest_unreal.py`. It is a 2,364-triangle,
`180.0 x 119.0 x 123.1` cm timber/stone/iron/ember/moss village prop with one
UV channel and two authored body/lid boxes. The saved
`Embermere_SupplyChest_Vendor_01` actor at `(-1740, -1180, 0)`, yaw `108`,
replaces `FabPass_Village_Crates_A`. The validator locks classic-FBX provenance, exact
bounds, shared materials, collision, tag, transform, and old-crate removal; a
native live-editor trace proves the lid solid. A second player-height trace and
saved `225` cm corridor-clearance invariant lock the July 28 fix for the
reproduced straight-line autorun contact.

The eighth original model type is `SK_EmbermereMarshProwler_01`, built by
`Scripts/blender/build_embermere_marsh_prowler.py`. The saved source contains
7,464 triangles, 3,878 vertices, five materials, 26 authored bones, two
physics proxies, and Idle, Walk, Run, Attack, Hit, and Death actions. Unreal
retains a skeletal mesh, skeleton, physics asset, five materials, and six
animation sequences. Generic enemy state owns animation routing while the
existing capsule and gameplay components remain authoritative. The Blueprint
CDO and all three placed instances are validated independently.

The ninth original model type is
`SM_EmbermereMarshReedCluster_01`, built by
`Scripts/blender/build_embermere_marsh_reed_cluster.py` and imported through
`Scripts/import_embermere_marsh_reeds_unreal.py`. It is a 1,012-triangle,
`168 x 131.04 x 187.8` cm visual-only cluster with four project materials and
no collision. Four exact saved placements use `M_EmbermereGround` on their low
footprints and bring the project-owned level layer to 14 placements.

The tenth original model type is `SM_EmbermereFenwatchShelter_01`, built by
`Scripts/blender/build_embermere_fenwatch_shelter.py` and imported through
`Scripts/import_embermere_fenwatch_shelter_unreal.py`. It is a 4,348-triangle,
`438.0 x 296.782 x 369.5` cm open-sided stone/moss/timber/iron/ember shelter
with one UV channel and four authored support boxes. The saved
`Embermere_FenwatchShelter_Mara_01` actor at `(-1740, -700, 0)`, yaw `-64`,
replaces the old Mara stone backdrop, mismatched market cover, and vendor/
trainer cubes. Its center and roof span remain clear, bringing the map to 15
original-art placements.

The eleventh original model type is the Fenwatch keeper, initially built as
`SM_EmbermereFenwatchKeeper_Mara_01` by
`Scripts/blender/build_embermere_fenwatch_keeper.py`. Its production skeletal
lane reuses the exact 3,280-triangle, `107.45 x 71.0 x 207.5` cm Stylized
Classic silhouette, one UV channel, and six project-owned materials, then adds
nine authored bones, complete rigid weights, and a 109-frame, 30-fps, exact
3.6-second Idle. Classic FBX adds one imported Armature root. Mara's original
`BP_QuestGiver` remains at `(-2050, -850, 140)`, yaw `35`, with the exact
interactable, name, dialogue, quest, marker, and rewards. Its dormant SCS visual
retains local `(0, 0, -140)`, yaw `100`, unit scale, and `NoCollision` but no
render mesh. The colocated art-only wrapper sits at `(-2050, -850, 0)`, yaw
`135`, prefers the rig and Idle, and keeps the static mesh as a fallback. The
project-owned level count remains 16 for this model because its art tag follows
the rendering wrapper rather than being duplicated on gameplay.

The twelfth original model type is
`SM_EmbermereFenwatchQuartermaster_01`, built by
`Scripts/blender/build_embermere_fenwatch_quartermaster.py` and imported through
`Scripts/import_embermere_fenwatch_quartermaster_unreal.py`. It is a static
3,632-triangle, `120.842 x 93.0 x 217.0` cm Stylized Classic merchant visual
with one UV channel, six project-owned materials, and no collision. The saved
`Embermere_FenwatchQuartermaster_Vendor_01` actor uses the native
`AEmbermereNpcPresentationActor` at `(-1530, -1190, 0)`, yaw `100`, unit scale,
static-preferred mode, and the `EmbermereOriginalArt` tag. The wrapper owns
soft static/skeletal references and one shared visual transform but no
interaction, vendor, trainer, dialogue, or quest state. This brings the map to
17 original-art placements.

Later production additions are the rigged Fenwatch armsmaster, the solid-core
Fenwatch practice dummy, the project-owned Fenwatch vendor stall, and the
first closed Fenwatch cottage. The stall
is a `394 x 242 x 306` cm, 5,476-triangle stone/moss/timber/iron/ember module
with four solid support boxes and one solid counter. It replaces
`FabPass_Village_Fence_01` at `(-1530, -1430, 0)`, yaw `180`, behind the
quartermaster. The 180-degree yaw intentionally compensates for classic FBX's
local-Y mirror so the counter faces the customer approach. Fresh package
validation and initialized-world traces lock the four supports, counter,
service approach, and east bypass. The map now contains 55 Fab actors and 20
original-art placements before the cottage pass.

`SM_EmbermereFenwatchCottage_01` is a `580 x 422 x 503` cm,
6,616-triangle closed stone/moss/timber/iron/ember dwelling with two authored
body/doorstep collision boxes. It replaces `FabPass_Village_Fence_02` at
`(-2480, -260, 0)`, yaw `38`. Fresh validation locks classic-FBX provenance,
materials, bounds, collision, tag, and transform; initialized-world traces
prove the body/step solid, roof/chimney decoration clear, and both the direct
Mara route and west bypass open. The current map contains 54 Fab actors and 21
original-art placements: 14 solid world props/modules, three non-colliding NPC
presentations, and four non-colliding reed clusters.

`SM_EmbermereFenwatchTrainingWorkshop_01` extends that architecture with a
grounded open-front `460 x 270.403 x 369` cm, 5,624-triangle training shed. It
uses the same five materials and exactly four authored boxes for two front
posts, the rear wall, and the workbench; roof, trim, crest, weapons, and tools
remain visual-only. It replaces only `FabPass_Village_Fence_03` at
`(-690, -1030, 0)`, yaw `-100`. Fresh package/full-zone validation and live
native traces lock its provenance, placement, purposeful solids, clear center,
clear player-height bay, open armsmaster-to-dummy approach, and road-side east
bypass. The current map contains 53 Fab actors and 22 original-art placements:
15 solid world props/modules, three non-colliding NPC presentations, and four
non-colliding reed clusters.

The chosen community bridge is `djeada/blender-mcp-server`, pinned during
installation to commit `7eed33edf4aca2ab0ca84a6da27321f89f68b504`.
Configure its add-on with Safe Mode enabled, inline code disabled, localhost
port `9876`, and approved roots limited to this repository's `Scripts/blender`
and `ArtSource/Blender` directories. Do not install the popular unrestricted
raw-code bridge for this project.

## Unreal And Machine Setup

Known local installation:

- Unreal Engine 5.8:
  `/Users/Shared/Epic Games/UE_5.8`
- Project:
  `/Users/wizard/Documents/Unreal Game/Embermere.uproject`
- Xcode:
  `/Applications/Xcode.app`

Required Unreal plugins:

- Model Context Protocol (`ModelContextProtocol` / Unreal MCP)
- `AllToolsets`
- `PythonScriptPlugin`
- `EnhancedInput`

Validate setup with:

```bash
zsh Scripts/check_unreal_setup.sh
```

The Xcode Metal Toolchain is a recurring macOS failure point. If Unreal reports
that `metal` is missing:

```bash
xcodebuild -downloadComponent MetalToolchain
xcrun metal -v
```

If the toolchain downloaded and its mounted binary works but `xcrun metal -v`
still fails, reboot macOS before reinstalling Xcode or Unreal. The stale state
has previously been the MobileAsset/cryptex lookup, not a failed download.

## Unreal MCP Playbook

The project uses MCP port `8123`.

At the start of each Unreal editor session, run in the Unreal console:

```text
ModelContextProtocol.StartServer 8123
```

For unattended startup, UE 5.8 supports:

```text
-ModelContextProtocolStartServer -ModelContextProtocolPort=8123
```

On macOS, pass the `.uproject` after `open ... --args`; see
`Docs/UNREAL_SETUP.md` for the exact command. Generic `-ExecCmds` startup was
too early for the MCP module in testing.

Client config generation is normally one-time setup:

```text
ModelContextProtocol.GenerateClientConfig Codex
```

Expected project config:

```toml
[mcp_servers.unreal-mcp]
url = "http://127.0.0.1:8123/mcp"
```

Operational rules:

- Give the server a short warm-up after startup.
- Prefer first-class Unreal MCP tool search when available:
  `list_toolsets`, `describe_toolset`, and `call_tool`.
- Use direct JSON-RPC HTTP at `http://127.0.0.1:8123/mcp` only as a fallback.
- Unreal MCP calls execute serially on the game thread; do not overlap them.
- Use MCP to discover/run tests, start/stop PIE, capture Slate/viewport state,
  inspect actors, and call asset/editor APIs.
- A successful simulated `Cmd+S` only proves Slate received the input. It does
  not prove a map package reached disk.
- Persist intentional map changes through Unreal asset save APIs, confirm the
  package is clean, and reload it through a fresh headless validator.

The refreshed ChatGPT/Codex app exposed Unreal MCP as first-class tools and was
materially more reliable than the earlier raw HTTP workflow. Treat that as an
end-to-end harness improvement, not proof that a model alone fixed MCP.

## Build And Verification

Preferred Mac editor build after C++ changes:

```bash
"/Users/Shared/Epic Games/UE_5.8/Engine/Build/BatchFiles/Mac/Build.sh" \
  EmbermereEditor Mac Development \
  -Project="/Users/wizard/Documents/Unreal Game/Embermere.uproject" \
  -NoHotReloadFromIDE
```

Use `-WaitMutex` for a basic manual rebuild when Unreal reports that Embermere
cannot compile, but prefer `-NoHotReloadFromIDE` before authoritative headless
automation after C++ or test changes.

Important verification rules:

- Restart the interactive editor after a relink or interface-heavy C++ change.
  The open editor can keep a stale numbered hot-reload module.
- Run Unreal build, automation commandlets, and map validation sequentially.
  Parallel commandlets can race on shared UBT `Trace.uba` files.
- Prefer object-level `NewObject` tests when behavior does not require a world,
  but inspect a saved SCS node template when the contract belongs to a
  Blueprint-authored component template.
- Record whether a PIE/test result came from a fresh or stale editor session.
- Run the manual route in `Docs/PLAYTESTING.md` after a clean restart.

Current automation tests:

1. `Embermere.Rules.RaceClassMatrix`
2. `Embermere.Combat.TargetSelectionPresentation`
3. `Embermere.Combat.DeadCasterRejected`
4. `Embermere.Combat.StarterAbilityEffects`
5. `Embermere.Stats.DamageImmunity`
6. `Embermere.UI.EnemyNameplateWidget`
7. `Embermere.Enemy.LeashRules`
8. `Embermere.UI.InventoryToggle`
9. `Embermere.UI.HotbarCooldownDisplay`
10. `Embermere.UI.ChatLog`
11. `Embermere.Quests.CompletionRewards`
12. `Embermere.Equipment.SlotRules`
13. `Embermere.Equipment.StatApplication`
14. `Embermere.Inventory.ConsumableUse`
15. `Embermere.Enemy.LootRules`
16. `Embermere.Equipment.InventoryTransactions`
17. `Embermere.Inventory.CapacityTransactions`
18. `Embermere.UI.ItemComparison`
19. `Embermere.Inventory.IdentityActions`
20. `Embermere.UI.InventoryDragDrop`
21. `Embermere.Inventory.StableSorting`
22. `Embermere.Input.AutorunCancellation`
23. `Embermere.UI.IconPresentation`
24. `Embermere.UI.AbilityIconPresentation`
25. `Embermere.UI.PaperDollPresentation`
26. `Embermere.UI.TimedStatusPresentation`
27. `Embermere.Enemy.MarshProwlerPresentation`
28. `Embermere.UI.WorldStatusVfxPresentation`
29. `Embermere.Player.OutOfBoundsRecovery`
30. `Embermere.NPC.FenwatchKeeperPresentation`
31. `Embermere.NPC.PresentationContract`
32. `Embermere.NPC.FenwatchQuartermasterPresentation`
33. `Embermere.Vendor.TransactionRules`
34. `Embermere.Vendor.ServiceContract`
35. `Embermere.Vendor.FenwatchStockData`
36. `Embermere.UI.VendorPanel`
37. `Embermere.Vendor.SellBuybackTransactions`
38. `Embermere.Economy.FenwatchRewardsAndValues`
39. `Embermere.Persistence.RoundTrip`
40. `Embermere.Persistence.ValidationRollback`
41. `Embermere.Persistence.SlotInspection`
42. `Embermere.UI.SaveLoadPanel`
43. `Embermere.NPC.SkeletalIdlePresentation`
44. `Embermere.NPC.FenwatchArmsmasterPresentation`
45. `Embermere.NPC.FenwatchArmsmasterIdlePresentation`
46. `Embermere.Trainer.TransactionRules`
47. `Embermere.Trainer.ServiceContract`
48. `Embermere.Trainer.FenwatchOfferingsData`
49. `Embermere.UI.TrainerPanel`
50. `Embermere.NPC.FenwatchQuartermasterIdlePresentation`
51. `Embermere.NPC.FenwatchKeeperIdlePresentation`
52. `Embermere.NPC.ContextGreetingPresentation`
53. `Embermere.Combat.PracticeTargetPolicy`
54. `Embermere.Combat.PracticeTargetCombatReset`
55. `Embermere.Combat.ResultContract`
56. `Embermere.UI.CombatFeedbackPresentation`
57. `Embermere.UI.CharacterCreationInitialState`
58. `Embermere.UI.CharacterCreationRestrictions`
59. `Embermere.CharacterCreation.ConfirmationLoadout`
60. `Embermere.CharacterCreation.ControllerLifecycle`
61. `Embermere.Persistence.CharacterIdentityRoundTrip`
62. `Embermere.Persistence.CharacterIdentityRollback`
63. `Embermere.Persistence.LegacyV1CharacterFallback`
64. `Embermere.Progression.LevelRules`
65. `Embermere.Progression.LiveExperienceAndEquipment`
66. `Embermere.Progression.RewardOwners`
67. `Embermere.Progression.ValidationRollback`
68. `Embermere.UI.ExperienceProgressPresentation`
69. `Embermere.UI.LevelUpPresentation`
70. `Embermere.Trainer.LevelGatedProgression`
71. `Embermere.Trainer.LevelGatedPersistence`
72. `Embermere.UI.LevelUpWorldVfxPresentation`
73. `Embermere.Rest.ServiceContract`
74. `Embermere.Rest.RecoveryTransactions`
75. `Embermere.Rest.InterruptionAndCombat`
76. `Embermere.UI.RestWorldPresentation`
77. `Embermere.Quests.SingleSlotCompatibility`

Current verified baseline (2026-08-30):

- the no-hot-reload Mac editor build succeeded;
- an isolated commandlet and the restarted editor's first-class MCP runner each
  discovered and passed all 77 tests with no failed or
  skipped Embermere tests;
- the fresh 18-package aggregate emitted every expected success marker,
  retained the exact 53 grounded Fab plus 24 original-art baseline, and logged
  no Python error;
- clean PIE used the real Combat Drills transaction for `25` XP and Mara's
  original physical-F quest completion for another `125`, producing Human
  Warrior level `2` at `150` XP with exact `110/110` health, `53/53` mana,
  `12` Attack Power, unchanged hotbar, and matching Chronicle identity/level;
- clean PIE rejected Dwarf Ranger without silent correction, then accepted and
  saved Elf Wizard at exact `80/80` health, `110/110` mana, and Spark
  Bolt/Frost Root/Arcane Burst/Meditate hotbar state;
- Chronicle rejected a malformed version-2 slot without mutation. A fresh PIE
  Lizardman Ranger at `100/100` health and `60/60` mana then loaded the exact
  saved Elf Wizard identity twice without stat, hotbar, item, or reward drift;
- the live notice-board trace validator retained its three purposeful solid
  boxes, decorative clearance, and all four protected routes.

Historical verified baseline (2026-08-08):

- editor build succeeded with `-NoHotReloadFromIDE`;
- headless automation discovered and passed 43/43 with zero test failures,
  including saved starter-effect semantics and runtime contracts for timed
  power/armor buffs, Snare, Frost Root, Meditate, effective-stat consumption,
  respawn-safe temporary-effect clearing, fixed paper-doll presentation, and
  source-ability-backed timed-status presentation, all three saved Marsh
  Prowler instances, the world-status VFX contract, and finite-world player
  recovery, the real saved Fenwatch-keeper SCS presentation contract, the
  generic static-to-skeletal NPC wrapper contract, the real-asset single-node
  Idle contract, and the saved quartermaster presentation, plus vendor
  transactions, art/service separation, saved
  Fenwatch stock, native panel behavior, exact saved quest/item economy values,
  rollback-safe sell/buyback transactions, and versioned persistence
  round-trip plus whole-snapshot validation/rollback;
- a PIE-only swap on the accepted quartermaster wrapper used the real Marsh
  Prowler mesh and Idle sequence to prove the lightweight skeletal lane. The
  component remained visible and `NoCollision`, reported `playing=true` at
  `0.75x`, and advanced from `0.0` to `1.4153` seconds. Stopping PIE discarded
  the swap, while a fresh validator proved the saved quartermaster remained
  static with no animation reference;
- a real first PIE session ran the vendor sequence `40 -> 32 -> 35 -> 32 -> 2`,
  completed Mara's quest to `22` copper and `125` XP, retained one Marsh Tonic,
  one bagged Recruit Pack, one Back-equipped Recruit Pack, and exhausted finite
  Recruit Pack stock, then wrote `EmbermerePrototype.sav` through
  `EmbermereSave`;
- a fresh second PIE session restored that exact durable state through
  `EmbermereLoad`. Loading it a second time remained idempotent: no item
  duplication, repeated quest reward, equipment bonus inflation, or stock
  reset. Buyback history correctly cleared because version 1 defines it as
  session-only;
- the centered `460x260` Embermere Chronicle opened through `M`, inspected the
  same slot without mutation, and displayed `22` copper, `125` XP, two bag
  stacks, one equipped item, and completed quest state. Save exposed an
  overwrite confirmation and cancel path; Load exposed a separate replacement
  confirmation and restored through the same atomic persistence authority;
- empty, unreadable, unsupported-version, and load-time validation failures now
  surface readable persistence feedback. Inventory, Vendor, and Chronicle are
  mutually exclusive, and closing Chronicle restores game-only input. Console
  Save/Load commands remain debug fallbacks; autosave, deletion, profiles, and
  implicit migration remain intentionally absent;
- fresh-process vendor validation accepted the exact two stock rows, prices,
  finite/unlimited quantities, co-located service transform/tags/reference, no
  art on the service, and no service component on the presentation;
- clean PIE opened the service through normal `F`, bought Marsh Tonic from
  `40` to `32` copper, sold it by selected identity from `32` to `35`, bought
  it back from `35` to `32`, bought the one Recruit Pack from `32` to `2`, and
  rejected another tonic without mutation. The live quest component then paid
  the saved `20` copper reward exactly once, moving `2` to `22` alongside XP
  and the Recruit Pack reward;
- the fixed panel and chat exposed purchase, sale, buyback, sold-out, and
  insufficient-funds states without crossing the footer or hotbar. Inventory
  retained the bought-back tonic plus independently owned bought and rewarded
  Recruit Packs;
- target presentation automation proves cyan color, 48-segment continuity,
  restrained pulse, no rotation/collision, target switching, and clearing;
- the selected-target circle resolves its radius from transformed capsule and
  skeletal-mesh bounds plus `18` cm padding, then traces the supporting surface
  and keeps `16` cm effective collision-hit clearance. Clean daylight PIE
  accepted all three live Marsh Prowler runtime contracts and proved immediate
  `Tab` switching without obscuring the native nameplate or HUD target frame;
- fresh PIE forced a player with autorun enabled below the `Z=-1000` recovery
  plane and proved autorun off, both fall/recovery HUD messages, exact village
  spawn, full health, `MOVE_Walking`, zero velocity, and three seconds of
  protection;
- clean PIE accepted Battle Shout and Snare status rows with saved art,
  beneficial/harmful treatment, live countdowns, fixed bounds, and no
  neighboring HUD motion. Meditate remained instantaneous. The same session
  accepted Recruit Pack in the gold occupied Back slot over the centered paper
  doll with `105/105` HP, an empty post-transfer bag, and no title, bonus,
  footer, inventory, or hotbar movement;
- stats expose generic active-effect snapshots with source ability data,
  remaining world time, and beneficial/harmful intent. Combat registers them
  only after timed gameplay effects succeed; duplicate applications refresh
  one record, and existing expiration/vital-reset paths clear mechanics and
  presentation together. The HUD renders two fixed cells beneath player mana
  and two beneath selected-target HP using saved ability art, names,
  countdowns, and hover descriptions without switching on class or ability ID;
- every Embermere character owns eight fixed non-colliding presentation
  segments that read those same snapshots without owning gameplay. Attack
  Power resolves orange-gold, Armor blue-white, Snare marsh green, and Frost
  Root ice-white cyan; harmful state has deterministic priority and empty/dead
  state hides all segments. Harmful auras derive a bounded radius from
  transformed visual bounds and trace the support surface. Clean PIE accepted
  the Prowler at a `94.352` cm inner radius and `21` cm support clearance inside
  the unchanged outer target circle;
- `Scripts/configure_starter_abilities.py` reproduces the saved effect type,
  duration, movement multiplier, and player-facing descriptions. Fresh-process
  UI validation reloads and verifies those semantics for all sixteen abilities
  while rejecting placeholder behavior copy;
- 31 deterministic `128x128` project-owned icon textures, one deterministic
  `128x160` paper-doll texture, plus
  `DA_EmbermereUiIconSet` were imported and explicitly saved under
  `/Game/UI/Icons`; a fresh process reloaded exact texture settings, ten slot
  mappings, five category mappings, item/slot/ability missing-art paths,
  the paper-doll mapping, explicit Recruit Pack/Marsh Tonic references, and all
  sixteen distinct ability assignments. The saved player-facing item label remains the
  validated compact `Recruit Pack`, while its stable identity is
  `RecruitPack`;
- fresh-process UI-art validation retained its explicit success marker with no
  `LogPython: Error`;
- the validator reloaded the saved map and passed with 57 grounded upright
  `FabPass_` actors, 17 exact original-art placements, four `NoCollision` reed
  clusters, required gameplay anchors, the 38-expression moss/earth road
  material, and exact saved sun/skylight/fog values;
- Blender MCP generated and validated the 2,364-triangle supply chest; Unreal
  imported it through `FbxFactory`, reused all five project materials, retained
  two authored colliders, replaced the temporary vendor crate stack, and saved
  the route-facing actor at `(-1740, -1180, 0)`, yaw `108`;
- Blender MCP generated the 4,348-triangle Fenwatch shelter with five shared
  materials and four authored support boxes. Unreal imported it through classic
  `FbxFactory` and saved it behind Mara at `(-1740, -700, 0)`, yaw `-64`. Clean
  PIE rejected an earlier technically valid transform because it obscured Mara;
  the accepted composition retains her quest-marker/name read and removes the
  old backdrop, market cover, and vendor/trainer cubes;
- Blender MCP generated and validated the 3,280-triangle Fenwatch keeper.
  Unreal imported and explicitly saved its mesh and skin material, then
  reconciled the real saved `BP_QuestGiver` SCS template and Mara's serialized
  visual component. Clean PIE accepted grounded feet, PlayerStart-facing pose,
  staff/satchel silhouette, readable marker/name, and unchanged interaction;
- Blender MCP generated and validated the 3,632-triangle Fenwatch
  quartermaster. Unreal imported and explicitly saved its mesh and skin
  material, then placed the native dual-lane presentation wrapper beside the
  supply chest at `(-1530, -1190, 0)`, yaw `100`. Clean PIE accepted grounded
  contact, merchant readability, Mara separation, and the open route. Native
  tests prove static and skeletal resolution through one shared transform while
  rejecting collision and interaction ownership;
- final-map PIE moved from `(-2400, -1200, 90.15)` to approximately
  `(-1793.18, -831.38, 90.15)` under Q in two independent sessions. W and S
  each stopped all later movement at the measured transform. The quartermaster
  remained behind the route at the chest while Mara and her marker/name stayed
  readable under the shelter;
- native traces in the initialized live editor proved three gate lanes clear,
  one gate support solid, both fence centers solid, both boundary-stone cores
  solid, the supply-chest lid solid, all four shelter supports solid, the
  shelter center clear, and the old player-height spawn corridor clear.
  Saved-map validation independently requires at least `225` cm of
  geometric chest clearance. Fresh commandlets remain authoritative for
  package/map assertions, but a commandlet-loaded world did not register native
  collision bodies and must not be treated as a physics result;
- automatic movement is now applied separately from manual-axis cancellation,
  and the inventory drag visual is a fixed 236x62 fantasy token with resolved
  item art, category-sigil fallback, and item context. Reward feedback reuses
  the same resolver in a fixed 32x32 icon cell;
- starter Prowlers now use a `525` cm aggro radius and collision-cleared home
  points at `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`; a focused PIE
  probe proved one enemy moved and attacked while the other two stayed home;
- the three redundant enemy markers were removed; safe/combat area bands remain
  explicitly `NoCollision` in the saved map and setup script;
- the original Marsh Prowler retained 7,464 triangles, 26 authored bones, five
  materials, and six saved animations through a fresh Blender inspection and
  Unreal package reload. Clean PIE exercised target, Strike, real retaliation
  through player death/recovery, target clear, tonic loot, death hold, hide,
  full-health respawn, and return-home while untouched neighbors stayed home;
- a fresh editor loaded the 2026-07-18 controller module. Transform-based PIE
  checks proved `Q` autorun advances the player and independent `W` and `S`
  presses stop all subsequent movement. A July 22 populated-inventory capture
  accepted the slot/item art but rejected the old long Recruit Pack copy for
  row clipping and detail-heading crowding; a second capture accepted the
  corrected `Recruit Pack` row and detail fit. The icon-bearing reward popup and
  real Recruit Pack bag-to-Back flow were accepted on July 23. The populated
  drag token while in motion and physical `Ctrl+M` feedback remain honest
  physical-eye checks.

## Critical Unreal Lessons

Read `Docs/UNREAL_LESSONS.md` for the full record. The highest-risk lessons are:

1. **Save All noise:** Unreal can normalize config and generate local tokens.
   Inspect `git status`; commit only intentional config changes.
2. **Do not revert user/editor changes:** work with a dirty tree and preserve
   unrelated modifications.
3. **Stale hot reload:** an open editor can run old C++ or old test registration
   after a successful source build. Restart before authoritative PIE/MCP checks.
4. **Sequential commandlets:** do not run Unreal commandlets in parallel.
5. **Rotator argument order:** never rely on positional
   `unreal.Rotator(...)` arguments in placement scripts. Assign `.pitch`,
   `.yaw`, and `.roll` by name.
6. **Slate input is not persistence:** save through asset APIs and verify from a
   fresh process.
7. **World indicator clearance:** inspect both indicator and layered walkable
   surface Z values; visible components can still be buried inside a platform.
8. **Fab access:** unauthenticated terminal/API search is Cloudflare-gated.
   Asset acquisition happens through the signed-in Fab window/Launcher.
9. **Marketplace licensing:** use assets in the project, but never commit or
   redistribute raw vendor packs in the public repository.
10. **Authored Blender collision:** pin `unreal.FbxFactory()` for FBX assets
    with `UBX_`/`UCX_` collision, validate body setup before map placement, and
    recreate a partial Interchange mesh package if stale import provenance
    survives atomic replacement.
11. **Visual-only geometry:** marker cones and encounter bands must explicitly
    use `NoCollision`; persist that state in the setup script and validator.
12. **Encounter placement:** inspect native WorldStatic overlaps around the
    expected capsule, not only visible mesh bounds, before saving enemy homes.
13. **Generated material persistence:** save every imported/generated material
    package and verify it on disk; a live Unreal object is not durable proof.
14. **Movement ownership:** controller-driven autorun must not call the manual
    input cancellation handler. Share movement math, but keep automatic and
    real-player input paths behaviorally distinct and test both directions.
15. **Python trace shape:** UE 5.8 `line_trace_single` returns `HitResult` or
    `None`, not a boolean/result tuple. Use `to_dict()` for protected hit data.
16. **Commandlet Python failures:** a zero process exit is insufficient; require
    the validator success marker and reject `LogPython: Error` in its log.
17. **UI art has separate persistence gates:** generated PNGs, loaded Unreal
    objects, and saved packages are different states. Import/save explicitly,
    reload in a fresh process, and inspect texture source dimensions in
    `-NullRHI` automation instead of runtime resource size.
18. **Native interface test objects:** reflected `BlueprintNativeEvent`
    dispatch can differ on unattached native `NewObject<AActor>` fixtures.
    Prefer the native implementation for known native actors, preserve
    reflected fallback for other implementers, and recheck the real Blueprint
    actor in clean PIE.
19. **Placed Blueprint instances:** a correct Blueprint CDO does not rewrite
    stale serialized component state on actors already saved in a map. Update,
    save, and fresh-process validate the class and every placed instance.
20. **Material lifecycle:** build destructive material graphs outside PIE.
    Validate live structure, save the package, then reload it fresh; a failed
    PIE-time save can leave a broken in-memory graph over a healthy disk asset.
21. **MCP startup:** prefer
    `-ModelContextProtocolStartServer -ModelContextProtocolPort=8123`; on macOS
    put the `.uproject` after `open ... --args`.
22. **Vendor overrides:** a component material override can repair the visible
    level without repairing missing dependencies in the vendor mesh package.
    Preserve the warning and replace the source asset over time.
23. **Presentation dependency direction:** world VFX should subscribe to
    gameplay-owned successful-effect snapshots. Gameplay must not know that the
    VFX exists, and automation plus clean PIE answer technical and visual
    acceptance separately.
24. **Static PIE placement probes:** static components can silently reject
    runtime transform changes. Temporarily use `Movable` only for a bounded PIE
    diagnostic, discard it by stopping PIE, then apply the accepted transform
    to the editor world and validate saved geometry plus live traversal.
25. **World-aura placement:** component count, color, and visibility do not prove
    normal-camera readability. Resolve both the transformed creature footprint
    and the supporting surface, assert bounded containment in automation, then
    accept the result in clean PIE.
26. **Blueprint visual replacement:** a saved SCS component template and a
    placed map instance can retain different placeholder transforms, scale,
    collision, and mesh state. Use `SubobjectDataSubsystem` for editor-side SCS
    access, inspect the saved SCS node template in native tests, reconcile the
    serialized instance separately, and fresh-process validate both.
27. **NPC swap boundary:** static-to-skeletal flexibility needs an executable
    wrapper, not a comment. Share one transform across soft static/skeletal
    lanes, keep both visual-only and non-colliding, test mode resolution, and
    leave interaction, services, dialogue, and quests in separate gameplay
    owners.
28. **Saved animation is not live playback:** clearing an Anim Blueprint class
    can destroy the transient instance while leaving single-node mode
    unchanged. Store construction-safe `AnimationData`, explicitly reinitialize
    registered components, then prove `IsPlaying`, play rate, and two advancing
    positions in PIE. Property inspection alone can accept a frozen pose.

## Known Workspace State

At a completed daily handoff, `main` and `origin/main` should point to the same
latest intentional commit.

The worktree intentionally still shows:

```text
 M Config/DefaultEngine.ini
 M Config/DefaultInput.ini
```

These are Unreal Save All/config-normalization artifacts from the editor. They
were deliberately excluded from recent commits. A new task must not silently
stage, revert, or overwrite them. Review them only if a future change genuinely
requires those config values.

The latest intentional gameplay/map/docs work is already committed and pushed.

## 2026-08-09 Trainer And Armsmaster Update

Fenwatch now has a complete trainer vertical slice whose art, service, data,
player state, and UI remain separately owned.

- `Embermere_FenwatchArmsmaster_Trainer_01` is an art-only
  `AEmbermereNpcPresentationActor` at `(-1320, -920, 0)`, yaw `100`, unit
  scale, and `NoCollision`. Its grounded Stylized Classic shield/staff
  silhouette is readable from the normal village route.
- The reviewed Blender source is `154.5 x 87.0 x 228.0` cm with 2,824 source
  triangles and six project-owned materials. Classic `FbxFactory` import
  removes 24 degenerate triangles and persists a 2,800-triangle Unreal mesh;
  these are deliberately separate source and imported-topology contracts.
- The co-located, art-free
  `Embermere_FenwatchArmsmaster_Service_01` exclusively owns interaction and
  `UEmbermereTrainerComponent`. It has no static/skeletal mesh, collision,
  navigation, vendor, quest, or dialogue authority.
- `DA_FenwatchArmsmasterOfferings` defines one repeatable level-1 `Combat
  Drills` action costing 10 copper and granting 25 XP. Level, funds, malformed
  data, and XP overflow reject before mutation; an unexpected progression
  failure after spend refunds the exact copper.
- The fixed `500x300` native Fenwatch Training panel owns presentation and
  requests only. It keeps offering/detail/result/footer copy in bounds and
  hands off mutually exclusively to Inventory and Chronicle.
- Clean PIE proved 40 copper/0 XP becomes exactly 30 copper/25 XP, repeated
  training reaches zero without underflow, and an additional request reports
  insufficient funds without mutation. Inventory, Chronicle, and close paths
  all restore coherent cursor/input ownership.
- The no-hot-reload Mac build, all 48 automation tests, trainer/saved-map/UI
  validators, and initialized-world route traces passed. The map now retains
  57 grounded upright Fab actors plus 18 original-art placements.

Two durable tool lessons also came from the pass. Blender source topology and
Unreal post-import topology can both be correct acceptance metrics when the
importer removes degenerates. In Unreal MCP, the Output Log's Python textbox
may not appear in a root Slate snapshot; observing the `SOutputLog` subtree
directly exposes its internal `SMultiLineEditableTextBox` for supported
localhost automation without OS-level keystroke injection.

## 2026-08-10 Trainer Persistence And Practice-Dummy Update

Trainer-produced progression now has an accepted two-world Chronicle proof
without expanding save version 1. Fresh PIE trained once from 40 copper/0 XP to
30 copper/25 XP and saved through Chronicle. A second world proved its normal
40/0 baseline, restored 30/25 through confirmed Load, and stayed at 30/25 after
a second confirmed Load. Inventory, equipment, quest, finite vendor stock,
buyback, and reward state remained untouched; trainer offerings and panel state
remain transient.

Fenwatch also gained `SM_EmbermereFenwatchPracticeDummy_01`, built through the
reviewed deterministic Blender/classic-FBX lane:

- `252.0 x 100.879 x 245.0` cm, 2,572 triangles, one UV channel, zero
  non-manifold edges, unit scale, and a ground pivot;
- five existing stone/moss/timber/iron/ember materials;
- exactly two authored UBX boxes for a solid base and torso/core, with both
  outstretched arms intentionally clear;
- saved as `Embermere_FenwatchPracticeDummy_TrainingYard_01` at
  `(-1120, -1120, 0)`, yaw `45`, replacing `FabPass_Village_Crate_C`.

The live Blender bridge correctly rejected the scene-resetting build script, so
the reviewed script ran in a factory-clean headless process instead of weakening
Safe Mode or disturbing the open scene. The first technically valid Unreal
placement faced away from the armsmaster; viewport review rejected it, then the
deterministic integration script was corrected and rerun. All 48 tests passed,
fresh-process dummy/zone/trainer/vendor/UI validators passed without Python
errors, and initialized-world traces proved the dummy base/core solid and both
arms clear. The saved map now contains 56 Fab actors plus 19 original-art
placements.

## 2026-08-11 Rigged Armsmaster Idle Update

The accepted armsmaster wrapper now prefers a dedicated project-owned skeletal
lane while preserving the original static mesh as a reversible fallback.

- Blender retains the `154.5 x 87.0 x 228.0` cm grounded bounds, 2,824 source
  triangles, six materials, and adds nine authored bones with complete rigid
  weights plus a 97-frame, 30-fps, 3.2-second Idle.
- Classic skeletal `FbxFactory` preserves one Armature object as an imported
  root. Unreal therefore validates ten reference-skeleton bones, all nine
  authored names, and the authored `root` beneath that importer-owned node.
- The saved wrapper uses `SK_EmbermereFenwatchArmsmaster_01` and
  `A_EmbermereFenwatchArmsmaster_Idle`, stays `NoCollision`, keeps the static
  soft fallback, and does not gain any trainer authority.
- The importer replaces skeletal packages in place and reuses the valid saved
  Skeleton. Deleting and recreating both in one commandlet produced stale
  UObject state and is now explicitly avoided.
- The no-hot-reload build, all 49 tests, fresh rig/trainer/zone/dummy/UI
  validators, and initialized-world route traces passed.
- Clean PIE kept the Idle `playing=true` and advanced its position from
  `0.193888` to `1.670905` seconds while the grounded training-yard composition
  stayed readable.

## 2026-08-12 Rigged Quartermaster Idle Update

The quartermaster is now the second production NPC to adopt the wrapper's
skeletal lane without moving vendor behavior into art.

- The deterministic rig retains the reviewed `120.842 x 93.0 x 217.0` cm
  grounded bounds, 3,632 triangles, and six materials. It adds nine authored
  bones, complete rigid weights, and a 121-frame, 30-fps, exact 4.0-second
  merchant Idle.
- Classic FBX adds one imported Armature root, so Unreal validates ten
  reference bones while retaining all nine authored names and hierarchy.
- `Embermere_FenwatchQuartermaster_Vendor_01` prefers the exact rig and Idle,
  remains `NoCollision`, keeps its static soft fallback, and owns no marker,
  stock, transaction, UI, or persistence authority. The co-located art-free
  vendor service remains unchanged.
- UE 5.8 can silently switch replacement of an existing skeletal package to
  Interchange. Routine reruns now validate eligible classic-FBX packages
  without importing; intentional rebuilds use separate cleanup-only, fresh
  creation, and fresh validation Unreal processes.
- The no-hot-reload build and all 50 tests passed. Fresh quartermaster,
  armsmaster, vendor, trainer, zone, dummy, and UI validators plus initialized-
  world route traces passed.
- Clean PIE kept the quartermaster `playing=true` and advanced its Idle from
  `0.853735` to `2.195707` seconds. Normal-camera review retained grounded
  chest-side composition and an open village-service route.

## 2026-08-13 Rigged Fenwatch Keeper Update

Mara now uses the same production skeletal lane without replacing the
Blueprint actor that owns her quest gameplay.

- The rig retains the reviewed `107.45 x 71.0 x 207.5` cm grounded bounds,
  3,280 triangles, and six materials. It adds nine authored bones, complete
  rigid weights, and a 109-frame, 30-fps, exact 3.6-second Idle.
- Classic FBX adds one imported Armature root, so Unreal validates ten
  reference bones while retaining all nine authored names and hierarchy.
- `Quest_Giver_Mara_Fenwatch` remains the exact `BP_QuestGiver` gameplay actor
  with its interactable, name, dialogue, quest, marker, and rewards. Its old
  SCS static component keeps the accepted local transform and `NoCollision`
  state as a dormant template but renders no mesh.
- `Embermere_FenwatchKeeper_Mara_Presentation_01` is colocated at ground level,
  prefers the exact rig and Idle, remains `NoCollision`, retains the static
  fallback, and owns no interaction or quest component.
- The no-hot-reload build passed. Headless and fresh-editor MCP suites each
  passed 51/51, and fresh keeper plus full-zone validators retained the complete
  ownership, map, UI, service, and route baseline.
- Clean PIE kept Mara grounded beneath the shelter with her marker and name
  readable. The fresh-module animation clock advanced from `0.333814` to
  `1.525603` seconds. On 2026-08-14 a physical `F` press accepted the original
  Blueprint-owned quest, real Prowler combat advanced it to `3/3`, and the same
  original interactable completed the return. The wrapper remained art-only.

## 2026-08-15 Contextual Mara Greeting Update

Mara's rigged presentation wrapper now has an optional read-only greeting that
subscribes to the original quest authority without creating a second dialogue
or interaction path.

- `DQ_FirstSignsAtTheRuin` owns short available, active, ready, and completed
  greeting copy alongside its existing authoritative quest data.
- `Embermere_FenwatchKeeper_Mara_Presentation_01` holds an explicit reference
  to `Quest_Giver_Mara_Fenwatch`, observes the player's quest log, and shows a
  fixed `320x56` hit-test-invisible panel only within 420 cm.
- The wrapper still owns no interactable, quest component, marker, dialogue,
  progression, or reward behavior. Removing the greeting changes no quest
  result.
- The no-hot-reload build, all 52 tests, the dedicated greeting/keeper/full-zone
  validators, and the existing UI, NPC, vendor, trainer, practice-dummy, map,
  and initialized-world route validators passed.
- Clean PIE accepted hidden-out-of-range plus available, active, ready, and
  completed presentation. Physical `F` preserved the original dialogue and
  granted exactly 125 XP, 20 copper, and one Recruit Pack; a second `F` granted
  nothing.

## 2026-08-16 Fenwatch Cottage Update

Fenwatch now has its first closed project-owned dwelling rather than only
service shelters and boundary props.

- `Scripts/blender/build_embermere_fenwatch_cottage.py` deterministically
  creates `SM_EmbermereFenwatchCottage_01`: grounded `580 x 422 x 503` cm
  bounds, 6,616 triangles, one UV channel, zero non-manifold edges, five shared
  materials, and two UBX boxes for the closed body and doorstep.
- The tracked preview camera now uses `clip_end=5000`; the first shorter range
  clipped the roof. Full-resolution review also caught and fixed door-jamb gaps
  before import.
- Classic `FbxFactory` import explicitly saves the package and places
  `Embermere_FenwatchCottage_West_01` at `(-2480, -260, 0)`, yaw `38`, after
  removing only `FabPass_Village_Fence_02`.
- Fresh cottage and full-zone validators lock exact provenance, metrics,
  materials, two colliders, transform, tag, `730.1` cm route clearance, and
  `730.1` cm Mara separation. Live traces prove body/step blocking, decorative
  roof/chimney clearance, the direct PlayerStart-to-Mara lane, and west bypass.
- Clean PIE kept Mara, her contextual greeting, and all service silhouettes
  readable; physical `F` still opened the original Blueprint dialogue and
  accepted the quest.
- The no-hot-reload Mac build succeeded. Live MCP and fresh-process automation
  each passed 52/52, and all focused UI, rig, greeting, vendor, trainer,
  practice-dummy, stall, cottage, saved-map, and initialized-world validators
  passed. The map baseline is now 54 Fab actors plus 21 original placements.

## 2026-08-17 Fenwatch Training Workshop Update

Fenwatch's practice dummy now sits within a project-owned open-front training
workshop rather than beside a generic fence.

- `Scripts/blender/build_embermere_fenwatch_training_workshop.py` creates a
  grounded `460 x 270.403 x 369` cm, 5,624-triangle module with one UV channel,
  zero non-manifold edges, five shared materials, and four UBX boxes for the
  two front posts, rear wall, and workbench.
- Classic `FbxFactory` import explicitly saves the package and places
  `Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
  `(-690, -1030, 0)`, yaw `-100`, after removing only
  `FabPass_Village_Fence_03`.
- Fresh package/full-zone validators lock exact provenance, metrics,
  materials, transform, tag, four colliders, and spacing from the practice
  dummy, armsmaster, and road pine.
- Initialized-world traces prove the purposeful surfaces solid while keeping
  the open center, player-height bay, armsmaster-to-dummy approach,
  decorative roof/tools, and road-side east bypass clear. The first bypass
  trace began inside an existing road lamp; its endpoint was corrected before
  acceptance rather than moving valid art around a bad assertion.
- The no-hot-reload build and authoritative 52/52 suite passed with zero
  warnings. All focused package, full-zone, UI, NPC-rig, vendor, trainer,
  cottage, stall, workshop, and native route validators passed. Clean PIE
  preserved Mara's physical `F` quest acceptance and transform-proved both
  `W` and `S` autorun cancellation. The map baseline is now 53 Fab actors plus
  22 original placements.

## 2026-08-18 Fenwatch Practice-Target Update

The visible training-yard dummy and workshop remain project-owned world art.
A new native `AEmbermerePracticeTargetActor` is saved separately at the exact
dummy transform as removable gameplay authority.

- It exposes `Fenwatch Practice Target`, 150 health, normal ability damage,
  the native enemy nameplate, and the complete 48-segment cyan target circle.
- It owns no mesh, collision, navigation, AI, aggro, retaliation, leash, loot,
  XP, quest credit, trainer/service behavior, or save-version-1 state.
- `IEmbermereTargetable::ShouldGrantDefeatCredit` now separates targetability
  from objective reward eligibility. Ordinary enemies opt in; the practice
  target opts out.
- `EmbermereTargetableDispatch` deliberately routes Blueprint-generated
  targetables through `Execute_*` and native C++ targetables through their
  `_Implementation` path. Combat, targeting, player feedback, and HUD use the
  same rule.
- Clean PIE found and fixed an inherited-engine failure: collision-free
  `ACharacter` actors still fall under CharacterMovement gravity. The target
  now remains at `(-1120, -1120, 0)` with gravity zero, zero velocity, and
  `MOVE_None` through BeginPlay and reset.
- Six real hotbar Strikes dealt `28, 28, 28, 28, 28, 10`. Defeat cleared the
  current target and ring, left the separate visible dummy in place, and reset
  to `150/150` after three seconds. Player health stayed `100`, XP `0`, bag
  stacks `0`, and Mara objective progress `0`; `Tab` immediately reacquired
  the reset target.
- The no-hot-reload build, all 54 tests, aggregate 13-package validation,
  focused practice-target/full-zone validation, initialized-world workshop,
  cottage, stall, and road traces, and clean PIE passed. The art baseline
  remains 53 Fab actors plus 22 original-art placements because the native
  target is gameplay rather than art.

The durable boundary is documented in `Docs/PRACTICE_TARGET_CONTRACT.md`.
Placement and validation live in
`Scripts/place_fenwatch_practice_target_unreal.py`,
`Scripts/validate_fenwatch_practice_target_gameplay_unreal.py`, and the
sequential `Scripts/validate_saved_prototype_packages_unreal.py` aggregate.

## 2026-08-19 Floating Combat Feedback Update

Combat now publishes one immutable `FEmbermereCombatResult` only after a
primary outcome commits. It carries source, target, stable ability ID, result
kind, exact post-mitigation amount, and lethal state without replacing the
existing ability-used cooldown event or acquiring gameplay authority.

- `UEmbermereCombatFeedbackWidget` is a standalone hit-test-invisible native
  observer with three fixed `112x32` slots, newest-first ordering, fourth-entry
  eviction, and a deterministic 1.25-second rise/fade lifetime.
- Damage uses the exact applied amount. `MISS` is a supported presentation kind
  but is not published until a real hit-resolution rule produces it.
- Target switch, deselection, death, practice-target reset, invalid actor,
  expiry, and teardown clear stale entries. Target HP and clipped chat remain
  durable fallbacks.
- `IEmbermereTargetable::GetCombatFeedbackAnchorLocation` excludes screen-space
  nameplate bounds. Dispatch validates reflected Blueprint anchors and falls
  back to native placement when an older saved Blueprint returns a zeroed new
  interface value.
- Normal-camera PIE accepted the same fixed `104x30` `28` result beside the
  practice target and a saved Prowler, with a 16-pixel nameplate gap, readable
  cyan circle, exact HP/chat changes, and immediate clear behavior.
- The no-hot-reload build, all 56 tests, and the sequential 13-package
  aggregate validator passed. The map remains 53 grounded Fab actors plus 22
  original-art placements.

The durable boundary is documented in `Docs/COMBAT_FEEDBACK_CONTRACT.md`.

## 2026-08-20 Fenwatch Notice-Board Update

Fenwatch now has a compact project-owned civic roadside prop without adding a
new gameplay authority.

- `Scripts/blender/build_embermere_fenwatch_notice_board.py` produces
  `SM_EmbermereFenwatchNoticeBoard_01` at
  `286 x 93.927 x 277` cm, 3,684 triangles, one UV channel, clean topology,
  five shared Fenwatch materials, and three UBX boxes for the two supports and
  central panel.
- The roof, layered notices, pins, trim, and ember crest remain visual-only.
  The asset and placement own no quest marker, dialogue, interaction, reward,
  vendor, trainer, or persistence behavior.
- Classic `FbxFactory` import explicitly saved the mesh and map, placing
  `Embermere_FenwatchNoticeBoard_Road_01` at `(-1560, -260, 0)`, yaw `-35`.
- Focused validation locks source/provenance, metrics, materials, collision,
  project-owned tag, transform, and exact saved neighbor labels. Post-restart
  initialized-world traces prove both supports and the panel solid,
  decorative geometry clear, and the PlayerStart-to-Mara, village-to-road,
  quartermaster, and armsmaster routes open.
- Normal-route PIE accepted the grounded roadside composition. The
  no-hot-reload Mac build, all 56 tests, and a 14-validator sequential saved-
  package aggregate passed with explicit success markers and no
  `LogPython: Error`.

The current map baseline is 53 grounded Fab actors plus 23 original-art
placements.

## 2026-08-21 Character-Creation Update

The existing data-driven race/class scaffold is now a real pre-play gate.

- `UEmbermereCharacterCreationWidget` presents all eight races and four
  classes in a fixed native `940x560` modal while the ordinary HUD is hidden.
- Invalid options remain visible and disabled. Selecting Ranger before Dwarf,
  or Wizard before Bullywug, leaves the invalid pending class visible and
  explicitly explained rather than silently correcting it.
- `UEmbermereRulesData` remains the only owner of legality, class starting
  attributes, and four starter ability IDs. The widget owns pending choice;
  the character revalidates and atomically applies the accepted state once;
  the controller owns modal/input/HUD lifecycle.
- Clean PIE rejected Dwarf Ranger and Bullywug Wizard, then accepted Elf Wizard
  at exact `80/80` health, `110/110` mana, and Spark Bolt, Frost Root, Arcane
  Burst, and Meditate. The modal cleared and chat reported
  `Journey begun: Elf Wizard`.
- Human Warrior remains the reversible construction fallback. Save version 1
  intentionally does not persist identity or creation completion.
- The no-hot-reload build, all 60 tests, the fresh 14-package aggregate, and
  initialized-world notice-board route traces passed.

The durable boundary is documented in `Docs/CHARACTER_CREATION_CONTRACT.md`.
The next bounded milestone is an explicit save version 2 identity contract,
including stable race/class IDs, validated atomic restore, idempotence, and a
documented version 1 Human Warrior fallback before any schema changes land.

## 2026-08-22 Character-Identity Persistence Update

Save version `2` now makes a deliberately confirmed race/class pair durable.

- Race and class serialize as stable semantic IDs, not enum ordinals, display
  text, inferred stats, or hotbar contents.
- Load resolves IDs through current `UEmbermereRulesData`, validates legality,
  class attributes, and all starter abilities before any live mutation.
- One atomic commit replaces identity, class base stats, full vitals, and the
  first four hotbar abilities before equipment and durable progression restore.
- Version `1` remains loadable through an explicit current-rules Human Warrior
  compatibility interpretation. The old slot is not rewritten or implicitly
  migrated.
- Chronicle shows race/class read-only and labels the version-1 legacy fallback.
- Three focused tests cover v2 round-trip/idempotence, malformed/unknown/illegal
  rollback, and the version-1 fallback. All 63 tests and the fresh 14-validator
  aggregate passed.
- Clean PIE saved Elf Wizard, established a genuinely different Lizardman
  Ranger in a fresh world, then restored Elf Wizard twice at exact `80/80`
  health, `110/110` mana, and the Wizard starter hotbar without drift. A
  preexisting malformed v2 slot was rejected safely with Load disabled.

## 2026-08-23 Derived-Level Progression Update

Level is now a deterministic runtime result of durable XP, not another saved
field.

- `DA_EmbermereRules` owns cumulative thresholds `0`, `100`, `250`, `450`,
  and `700` for the first level-5 cap plus distinct finite, nonnegative
  race/class growth profiles.
- Stats owns XP and derived level. Live grants atomically resolve identity base
  growth, preserve absolute missing health/mana, and emit exact XP plus one
  multi-level-aware level-up message.
- Save/load resolves stable identity and candidate XP before mutation,
  validates equipment against the candidate saved level, rebuilds base stats,
  then applies equipment once. Load is silent and idempotent; version `1`
  still derives level through its explicit current-rules Human Warrior fallback.
- HUD, Trainer requirements, and Chronicle consume the authoritative Stats
  result. None owns threshold math or serializes level.
- Four focused progression tests brought the suite to 67. The no-hot-reload
  build, progression and persistence groups, full 67-test suite, standalone
  progression package validator, and 15-package aggregate all passed.
- Clean PIE retained Human Warrior's exact level-1 baseline, used real Combat
  Drills for `25` XP, completed Mara's original quest for another `125`, and
  reached level `2` at `150` XP with exact `110/110` health, `53/53` mana,
  `12` Attack Power, `12` Strength, `9` Spirit, `11.25` Agility, and `7.75`
  Intellect. Chronicle read the same identity/level and durable owner state.

## 2026-08-24 Progression Presentation And UI Recovery Update

- Stats now publishes one read-only XP/threshold/cap snapshot. The HUD renders
  a fixed `260x8` gold progress bar, while a separate fixed `360x76`, 2.75-
  second live-only observer handles one-level, multi-level, and cap feedback.
  Save restoration updates steady state silently and never replays celebration.
- Character creation now owns one idempotent controller move/look lock even
  when both `OnPossess` and `BeginPlay` request the modal. Dwarf Warrior clean
  PIE confirmed Inventory close returns movement, look, a hidden cursor, and
  first-press classic mouse control.
- Chronicle moved to a `140x38` bottom-right command and now distinguishes live
  `Current Journey` from read-only `Saved Journey`. Its `500x320` panel reserves
  a fixed saved-summary region above nonoverlapping Save/Load actions.
- Two presentation tests brought the authoritative suite to 69. The build,
  full commandlet, progression validator, and 15-package aggregate passed.

## 2026-08-25 Level-Gated Trainer Update

- `DA_FenwatchArmsmasterOfferings` now contains repeatable level-1 Combat
  Drills (`10` copper, `25` XP) and repeatable level-2 Advanced Combat Drills
  (`20` copper, `50` XP), each with a unique stable ID.
- The service validates the complete asset and owns level/currency/XP/
  repeatability preflight plus atomic commit/refund. UMG keeps unavailable rows
  visible and inspectable but owns no level math or transaction authority.
- Clean PIE kept Advanced locked at level 1 with exact rejection and zero
  mutation, then refreshed the same panel to ready when Stats derived level 2.
  The final HUD request changed `50` copper / `100` XP to `30` / `150` exactly.
- A live pass found and fixed stale rejection copy after the action had become
  valid. Action state and status text now come from the same current preflight.
- Two focused tests brought the suite to 71. The no-hot-reload build, complete
  fresh commandlet, focused trainer validator, progression validator, and
  15-package aggregate all passed with no `LogPython: Error` and retained the
  53 Fab plus 23 original-art baseline.

## 2026-08-26 Advanced Persistence And Level-Up World VFX Update

- A clean Elf Wizard world completed Mara's authoritative reward, ran one real
  Advanced Combat Drills transaction, equipped the quest Recruit Pack, and
  saved through Chronicle at exact level `2`, `175` XP, and `40` copper.
- A genuinely fresh Dwarf Warrior world proved level `1`, `0` XP, and `40`
  copper before two confirmed Chronicle loads restored exact Elf Wizard
  identity, base/equipment stats, Wizard hotbar, completed quest, untouched
  finite vendor stock, wallet, and XP without replay, duplication, transient
  trainer state, schema expansion, or drift.
- The player now observes the existing post-commit live level event through
  twelve lazily created project-material segments. They resolve color from the
  confirmed class, expand and rotate for `1.6` seconds, remain non-colliding and
  navigation-free, then clear on expiry, death, or teardown. Silent load never
  broadcasts the event and cannot replay the effect.
- Clean PIE used four real Combat Drills transactions to reach Human Warrior
  level `2` at `100` XP, proved all twelve orange-gold segments, and visually
  accepted their grounded first-person read plus Inventory-close handoff.
- One focused test brought the suite to 72. The no-hot-reload build, complete
  fresh commandlet, focused trainer validator, progression validator, and
  15-package aggregate all passed with exact markers and no
  `LogPython: Error`, retaining the 53 Fab plus 23 original-art baseline.

## 2026-08-27 Fenwatch Communal-Well Update

- `Scripts/blender/build_embermere_fenwatch_communal_well.py` produces the
  grounded `SM_EmbermereFenwatchCommunalWell_01` at
  `348 x 220 x 322` cm, 6,760 triangles, one UV channel, clean topology, and
  five shared Fenwatch stone, moss, timber, iron, and ember materials.
- Four curb sections and two uprights own the only six UBX collision boxes.
  The open shaft, roof, axle, rope, bucket, crank, trim, and crest remain
  decorative and non-colliding.
- Classic `FbxFactory` import explicitly saved the mesh and map and placed
  `Embermere_FenwatchCommunalWell_SouthCommons_01` at
  `(-950, -1600, 0)`, yaw `-135`, as presentation-only project-owned art.
- Focused and full-zone validation lock classic-FBX provenance, exact bounds,
  topology, materials, collider count, transform, tag, collision profile, and
  spacing from Mara, PlayerStart, the quartermaster, armsmaster, practice
  dummy, workshop, and vendor stall. Initialized-world traces prove all six
  purposeful surfaces solid, all decorative/open-shaft samples clear, and six
  village or service circulation lanes open.
- Clean PIE accepted the grounded south-commons composition. Character
  creation confirmed, Inventory closed cleanly, Q moved from
  `(-2400, -1200)` to `(-1793.188, -831.382)`, and W stopped all later
  movement. The no-hot-reload build, all 72 isolated tests, focused trainer
  validator, progression validator, and fresh 16-package aggregate passed with
  exact markers and no `LogPython: Error`.

The current map baseline is 53 grounded Fab actors plus 24 original-art
placements. The next bounded gate should define a separate art-free well/rest
service with explicit recovery, resource, cooldown, UI, and rollback rules if
that interaction genuinely improves the playable loop. Otherwise add one
smaller matching Fenwatch prop. The well mesh itself must remain removable and
free of interaction, recovery, reward, quest, or persistence authority.

## 2026-08-28 Communal-Well Rest-Service Update

- Added `UEmbermereRestServiceData`, `UEmbermereRestServiceComponent`, and the
  art-free `AEmbermereRestServiceActor`. The saved service is colocated with the
  accepted static well at `(-950, -1600, 0)`, yaw `-135`, but carries no mesh,
  collision, navigation, or `EmbermereOriginalArt` tag.
- The service owns a `300` cm interaction range, `1.5` second stationary
  channel, `35` cm movement interruption, complete start/commit preflight,
  exact full-Health/full-Mana atomic recovery, and a `12` second session-only
  cooldown. The static well remains removable presentation art and owns none
  of those decisions.
- Rejections cover malformed data/vitals, range, death, live enemy engagement,
  full resources, cooldown, duplicate requests, movement, and teardown without
  partial mutation. The stationary practice target deliberately does not count
  as combat.
- Clean PIE used the real `F` path to restore exactly `30 Health` and `20 Mana`
  from `70/100` and `30/50`, showed fixed prompt/start/result chat, and rejected
  a full-resource repeat. Initialized-world traces retained all six purposeful
  well surfaces, clear decoration/open shaft, and all protected routes.
- Added three focused tests, bringing the authoritative suite to `75/75` with
  zero failures or skips. The no-hot-reload build, focused rest/well/trainer/
  progression validators, fresh 17-package aggregate, full-zone validator,
  and live native traces passed with exact markers and no `LogPython: Error`.
  Save version `2` and the 53 Fab plus 24 original-art baseline are unchanged.
- A first teardown test incorrectly called `EndPlay` on a component that had
  never entered `BeginPlay`, triggering Unreal's lifecycle assertion. Cleanup
  now lives in a shared private reset helper that production `EndPlay` and the
  focused test can each reach through their valid boundary. After that crash,
  Crash Reporter inherited the MCP launch flags and briefly owned port `8123`;
  checking the listener process before restarting avoided misdiagnosing the
  healthy editor.

## 2026-08-29 Communal-Well Rest-Presentation Update

- Added the native, removable
  `AEmbermereRestPresentationActor` and saved
  `Embermere_FenwatchCommunalWell_RestPresentation_01` beside the existing
  well art and art-free service. The observer references the service but owns
  no interaction, eligibility, channel rule, cooldown, recovery mutation,
  chat, quest, reward, or persistence state.
- The observer lazily creates exactly twelve transient project-material plane
  segments. The live channel is restrained cyan at a `46` cm radius inside the
  open shaft. Only a committed `Success` produces the mint bloom, expanding
  from `52` to at most `120` cm for exactly `1.1` seconds. All geometry is
  `NoCollision`, navigation-free, overlap-free, and shadow-free.
- Added a native C++ mirror of the existing immutable rest outcome before the
  established dynamic Blueprint broadcast. Native observers now have a
  deterministic subscription lane while existing Blueprint/chat consumers
  remain unchanged and no second outcome calculation exists.
- `Embermere.UI.RestWorldPresentation` proves deferred geometry, fixed count
  and bounds, duplicate safety, service-owned atomic recovery, success-only
  completion, deterministic expiry, interruption/death/unbind/teardown
  clearing, and zero gameplay authority. The authoritative suite is now
  `76/76`.
- Clean PIE visually accepted the real cyan shaft channel and mint committed
  bloom at normal camera distance. The service alone restored exactly
  `35 Health` and `20 Mana`; the observer remained clear of the roof, marker,
  route, chat, and HUD. The no-hot-reload build, focused validators, fresh
  18-package aggregate, full-zone validation, and initialized-world well,
  notice-board, workshop, cottage, stall, and road traces all passed. Save
  version `2` and the 53 Fab plus 24 original-art baseline remain unchanged.

## 2026-08-30 Single-Quest Compatibility And Version-3 Plan

- Inspection proved that `UEmbermereQuestLogComponent` and save version `2`
  each own one quest record. Mara's completed record is durable no-replay
  history, so a second quest cannot safely replace it or live outside
  persistence. `Docs/MULTI_QUEST_CONTRACT.md` now defines the deliberate
  version-3 keyed ledger, version-1/2 read adapters, complete atomic validation,
  and the bounded future `Still Waters` content slice.
- Quest acceptance now returns `Success`, `InvalidQuest`, `AlreadyTracked`, or
  `OccupiedByOtherQuest`. Only an exact same-quest revisit may attempt turn-in.
  A different valid offer produces fixed occupied-slot feedback and cannot
  replace or complete the current quest.
- `Embermere.Quests.SingleSlotCompatibility` proves wrong-giver rejection,
  matching Mara completion, unchanged wallet/XP on rejection, completed-history
  retention, version-2 capture/restore, and no schema expansion. The suite is
  now `77/77` in both an isolated commandlet and the restarted editor's MCP
  runner.
- The no-hot-reload Mac build, focused rest-presentation/rest-service/well/
  trainer/progression validators, and fresh 18-package aggregate passed with
  exact markers and no `LogPython: Error`. The aggregate retained 53 grounded
  Fab actors plus 24 original-art placements.
- Clean normal-route PIE confirmed Human Warrior startup, Inventory close,
  physical Q movement plus W cancellation, and real `F` acceptance of `First
  Signs at the Ruin` through `Quest_Giver_Mara_Fenwatch`. The compatibility
  guard did not disturb Mara's original authority or UI path.

## 2026-08-31 Save-Version-3 Multi-Quest Foundation

- `UEmbermereQuestLogComponent` now owns a bounded eight-record `QuestStates`
  ledger keyed by stable quest ID. Exact quest/objective APIs own progress and
  completion; reward preflight remains atomic and exactly once.
- Existing `ActiveQuest` Blueprint/HUD consumers remain compatible through a
  derived projection selected by transient `FocusedQuestId`. Neither focus nor
  projection is serialized. Mara's contextual observer queries
  `FirstSignsAtTheRuin` directly.
- Save version `3` serializes stable quest ID, asset, objective ID, progress,
  and completion records. The complete ledger resolves before mutation;
  duplicate, missing, mismatched, invalid-progress, contradictory,
  mixed-format, and over-capacity candidates reject atomically.
- Versions `1` and `2` remain readable through zero-or-one singular-record
  adapters that use the same validator and never rewrite the old source slot.
- `Embermere.Quests.MultiQuestRuntime`,
  `Embermere.Persistence.MultiQuestRoundTrip`,
  `Embermere.Persistence.LegacyQuestCompatibility`, and
  `Embermere.Persistence.MultiQuestValidationRollback` bring the suite to `80`.
  The no-hot-reload build, isolated commandlet `80/80`, restarted-editor MCP
  `80/80`, five focused validators, fresh 18-package aggregate, full-zone
  validation, and initialized-world well/board/workshop/cottage/stall/road
  traces all passed with no Python errors.
- Clean PIE retained Human Warrior creation and Inventory close. Q moved from
  `(-2400,-1200)` to `(-1793.189,-831.382)` and W cancelled there. Physical `F`
  accepted Mara's original quest; live QuestLog inspection showed exactly one
  keyed Mara record, matching transient focus, and the compatibility projection.

## 2026-09-01 Still Waters Parallel Quest

- Added `DQ_FenwatchStillWaters` with stable quest/objective IDs
  `FenwatchStillWaters/FenwatchRestCompleted`, one committed-rest requirement,
  four-state copy, `50` XP, `10` copper, and no item reward.
- Added the separate art-free native
  `AEmbermereRestQuestServiceActor`, saved as
  `Embermere_FenwatchNoticeBoard_StillWatersService_01` at the notice board,
  plus `UEmbermereRestQuestObjectiveRouterComponent`. The owner exclusively
  offers/turns in; the router forwards only immutable rest `Success`. The
  notice-board mesh, well art, rest service/presentation, and trainer remain
  free of quest components and authority.
- Added `Embermere.Quests.StillWatersServiceContract`,
  `Embermere.Quests.StillWatersRestRouting`, and
  `Embermere.Persistence.StillWatersRoundTrip`, bringing the suite to `83`.
  The no-hot-reload build, isolated commandlet `83/83`, restarted-editor MCP
  `83/83`, focused validators, sequential 19-package aggregate, full-zone
  validation, and initialized-world well/board/workshop/cottage/stall/road
  traces passed.
- Clean PIE used physical `F` to accept Still Waters, then Mara, proving two
  active records at `0/1` and `0/3`. Moving `75` cm interrupted one real rest
  without progress or partial recovery; one stationary committed rest advanced
  only Still Waters to `1/1`. Physical notice-board turn-in paid exact `50` XP
  and `10` copper once while Mara remained active; repeated `F` paid nothing.
- The isolated `EmbermereStillWatersLiveProbe` save slot captured completed
  Still Waters plus active Mara, diverged live state, and restored the exact
  ledger, `50` XP, and `50` copper twice without replay. It then deleted itself
  and restored the controller's real Chronicle slot target.

## 2026-09-02 Native Quest Ledger And Transient Focus

- Added a fixed `620x430` native Quest Ledger with eight stable `596x30`
  record rows, exact active/ready/completed state, objective progress, a fixed
  empty state, and one explicit transient focus action. The `140x38`
  bottom-right command sits eight pixels above Chronicle; `J`, Up/Down, Enter,
  row clicks, and the focus button share the controller-owned input lifecycle.
- Added a small native row-button type that reports its fixed occupied-row
  index. Selection remains widget-local; only explicit focus resolves that
  authoritative record's stable quest ID and calls the quest log. Focus changes
  only the `ActiveQuest` compatibility projection and is absent from save
  version `3`.
- Added `Embermere.UI.QuestLedgerPresentation` and
  `Embermere.UI.QuestLedgerFocusLifecycle`, bringing the suite to `85`. The
  no-hot-reload build, isolated commandlet `85/85`, restarted-editor MCP
  `85/85`, six focused package validators, fresh 19-package aggregate,
  full-zone validator, and initialized-world well/board/workshop/cottage/stall/
  road traces all passed.
- Clean PIE accepted Mara and Still Waters independently, used real Prowler
  damage plus physical `F` well recovery to make Still Waters ready, and paid
  exactly `50` XP plus `10` copper through physical notice-board turn-in. The
  ledger showed active Mara `0/3` beside completed Still Waters `1/1`; physical
  Up/Down and Enter switched the compact tracker both ways without record or
  reward mutation. `I`, `J`, and `M` proved exclusive peer-panel handoff.

## 2026-09-03 Selected-Quest Detail And Reward Surface

- Extended the native ledger to one fixed `620x550` panel while preserving all
  eight `596x30` record rows. One fixed `596x120` selected-record region now
  reads quest-owned title, wrapped/clipped description, exact state copy,
  objective progress, XP, copper, and optional reward-item display name.
- Selection remains local and refreshes details only. Enter or the focus action
  remains the explicit stable-ID request that changes the compact tracker. No
  focus, quest, reward, wallet, XP, inventory, equipment, service, or save
  authority moved into the widget.
- Added `Embermere.UI.QuestLedgerDetailsPresentation` and
  `Embermere.UI.QuestLedgerDetailLifecycle`, bringing the suite to `87`. The
  no-hot-reload build, isolated commandlet `87/87`, restarted-editor MCP
  `87/87`, focused validators, fresh 19-package aggregate, and full-zone
  validation passed while retaining 53 grounded Fab plus 24 original-art
  actors.
- Clean PIE created both records through their real interaction owners and
  completed Still Waters through a real communal-well channel and notice-board
  turn-in. The fixed surface showed exact `50 XP | 10 copper | No item reward`
  for completed Still Waters and `125 XP | 20 copper | Recruit Pack` for active
  Mara. Up/Down changed only details, Enter explicitly changed focus, and `I`
  replaced the ledger with Inventory without overlap or mutation.

## 2026-09-04 Quest-Owned Objective Instructions

- Added `UEmbermereQuestData::ObjectiveInstructions` as descriptive metadata
  independent of stable objective ID and numeric requirement. Mara owns exact
  `Defeat 3 Marsh Prowlers.` copy; Still Waters owns exact `Complete a rest at
  the communal well.` copy. Both saved packages were updated explicitly.
- The compact tracker consumes title, authoritative progress, and the authored
  instruction inside fixed corrected `260x68` clipped bounds. The unchanged
  `596x120` selected detail consumes the same field. Missing or blank metadata
  reports `Objective details unavailable.` without changing quest validation,
  progress, rewards, focus, or save version `3`.
- Added `Embermere.UI.QuestObjectiveDisplayPresentation`, bringing the suite to
  `88`. The no-hot-reload build, isolated `88/88`, focused package validator,
  sequential 20-package aggregate, full-zone validator, and initialized-world
  well/board/workshop/cottage/stall/road traces passed without Python errors.
- Clean PIE physically accepted both quests, showed each exact instruction in
  the compact tracker and fixed ledger detail, and used Up to select Mara while
  Still Waters remained focused. The ledger geometry, records, rewards, routes,
  and 53 Fab plus 24 original-art baseline remained intact.

## Immediate Next Work

The September 4 Astra continuation supersedes the older milestone below:
the native quest-update observer and Blender-rendered journal emblem are now
implemented, with `91/91` final automation and a 21-package aggregate.
Read `Docs/QUEST_UPDATE_PRESENTATION_CONTRACT.md` and the dated Astra
evaluation report. After local unlock, fresh Q traversal plus W cancellation
reached Mara for real F acceptance. Injected temporary objective progress
provided 1/3 and 3/3 visual fixtures, then real F committed exactly 125 XP,
20 copper, and one Recruit Pack. Completion/reward/level-up surfaces had clear
separation, and Inventory suppressed the notice without replay on close.
Finish the real Prowler/Still Waters routes, remaining peer panels, and held
mouse/Ctrl+M checks. The widget tree and repeated full version-3 restore are
covered by tests. Do not equate injected objective progress with combat play.
Do not overwrite the user's Chronicle slot: read-only inspection currently
rejects contradictory legacy quest state in that v3 file. No save/load occurred.
The long Still Waters compact-tracker instruction can clip; its full ledger
detail remains the fallback. Map counts and placements are unchanged.

Start from the `Start Here` section of `TODO.md`. Confirm Unreal has the
2026-09-04 no-hot-reload quest-update and quest-objective modules, saved journal
texture, and quest
packages, the 2026-09-03 selected-quest detail module, the 2026-09-02 native
Quest Ledger module, the 2026-09-01 Still
Waters quest/service/router module and saved
quest/map packages, the 2026-08-31 save-version-3 multi-quest module, plus the accepted
2026-08-29 rest-presentation observer,
rest-service data/map, and communal-well art packages, the 2026-08-26
class-colored level-up world-VFX module, and the
2026-08-25 level-gated trainer and progression-presentation module, serialized rules
asset, accepted notice-board
asset/map package,
combat-feedback module, quest/map packages, all
three accepted skeletal-mesh/Skeleton/Idle sets, the Fenwatch vendor stall and
closed cottage plus training workshop,
practice dummy plus native practice target, offering, Chronicle, Fenwatch
stock/service, item, keeper,
quartermaster, NPC wrapper, and Blueprint packages, then confirm MCP/test
discovery; restart only if the editor or test registry proves stale.

First fresh-session checks:

1. Confirm Unreal has the latest module/map and open
   `L_Embermere_Prototype`; restart only when stale.
2. Start MCP on port `8123` and wait briefly for tool discovery. Prefer the
   dedicated startup flags documented above for unattended launches.
3. Run/discover all 91 tests, including
   `Embermere.Quests.LiveUpdateContract`,
   `Embermere.UI.QuestUpdatePresentation`,
   `Embermere.UI.QuestUpdateLifecycle`,
   `Embermere.UI.QuestObjectiveDisplayPresentation`,
   `Embermere.UI.QuestLedgerDetailsPresentation`,
   `Embermere.UI.QuestLedgerDetailLifecycle`,
   `Embermere.UI.QuestLedgerPresentation`,
   `Embermere.UI.QuestLedgerFocusLifecycle`,
   `Embermere.Quests.StillWatersServiceContract`,
   `Embermere.Quests.StillWatersRestRouting`,
   `Embermere.Persistence.StillWatersRoundTrip`,
   `Embermere.Quests.MultiQuestRuntime`,
   `Embermere.Persistence.MultiQuestRoundTrip`,
   `Embermere.Persistence.LegacyQuestCompatibility`,
   `Embermere.Persistence.MultiQuestValidationRollback`,
   `Embermere.UI.RestWorldPresentation`,
   `Embermere.Rest.ServiceContract`,
   `Embermere.Rest.RecoveryTransactions`,
   `Embermere.Rest.InterruptionAndCombat`,
   `Embermere.UI.LevelUpWorldVfxPresentation`,
   `Embermere.Progression.LevelRules`,
   `Embermere.Progression.LiveExperienceAndEquipment`,
   `Embermere.Progression.RewardOwners`,
   `Embermere.Progression.ValidationRollback`,
   `Embermere.UI.CharacterCreationInitialState`,
   `Embermere.UI.CharacterCreationRestrictions`,
   `Embermere.CharacterCreation.ConfirmationLoadout`,
   `Embermere.CharacterCreation.ControllerLifecycle`,
   `Embermere.Persistence.CharacterIdentityRoundTrip`,
   `Embermere.Persistence.CharacterIdentityRollback`,
   `Embermere.Persistence.LegacyV1CharacterFallback`,
   `Embermere.Combat.ResultContract`,
   `Embermere.UI.CombatFeedbackPresentation`,
   `Embermere.Combat.PracticeTargetPolicy`,
   `Embermere.Combat.PracticeTargetCombatReset`, the six economy/vendor tests,
   `Embermere.Trainer.TransactionRules`,
   `Embermere.Trainer.ServiceContract`,
   `Embermere.Trainer.FenwatchOfferingsData`,
   `Embermere.Trainer.LevelGatedProgression`,
   `Embermere.Trainer.LevelGatedPersistence`,
   `Embermere.NPC.FenwatchArmsmasterPresentation`,
   `Embermere.NPC.FenwatchArmsmasterIdlePresentation`,
   `Embermere.NPC.FenwatchQuartermasterIdlePresentation`,
   `Embermere.NPC.FenwatchKeeperIdlePresentation`,
   `Embermere.NPC.ContextGreetingPresentation`,
   `Embermere.UI.TrainerPanel`,
   `Embermere.Persistence.RoundTrip`,
   `Embermere.Persistence.ValidationRollback`,
   `Embermere.Persistence.SlotInspection`,
   `Embermere.UI.SaveLoadPanel`,
   `Embermere.NPC.FenwatchKeeperPresentation`,
   `Embermere.NPC.PresentationContract`,
   `Embermere.NPC.SkeletalIdlePresentation`, and
   `Embermere.NPC.FenwatchQuartermasterPresentation`.
4. Start PIE and verify:
   - damage Health and spend Mana, approach the separate communal-well service,
     and use real `F`. Retain the fixed prompt, exact `1.5` second stay-still
     copy, atomic full-vitals result, exact restored amounts, full-resource and
     cooldown rejection, movement/combat interruption, practice-target
     exclusion, and unchanged well collision/routes. Retain the twelve cyan
     shaft segments during the real channel and the fixed `1.1`-second mint
     bloom only after committed recovery; interruption, death, unbind, expiry,
     teardown, and fresh/load worlds must clear it. The static mesh must own no
     service component, the service must own no art or collision, and the
     observer must own no interaction or mutation;
   - use physical `F` at the road notice board to accept `Still Waters`, then
     accept Mara independently. Interrupt one real well channel and verify
     `0/1`, commit one stationary channel to reach `1/1`, turn in at the board
     for exactly `50` XP and `10` copper once, and retain Mara unchanged. Do
     not use the user's Chronicle slot for the double-load proof;
   - the fixed creation modal precedes play, all eight races and four classes
     remain visible, Dwarf Ranger and Bullywug Wizard stay disabled without
     silent correction, and valid confirmation restores the HUD/input path;
     recheck Elf Wizard at `80/80` health, `110/110` mana, and the exact four
     Wizard starter abilities;
   - Chronicle shows confirmed race/class read-only. Save a non-default legal
     identity, begin a fresh world with a different legal pair, load twice, and
     prove exact identity, base stats, starter hotbar, progression, and
     repeated-load idempotence; malformed identity keeps Load disabled;
   - retain Human Warrior's exact level-1 baseline, train once to `25` XP, then
     complete Mara's real quest to reach level `2` at `150` XP. Recheck exact
     `110/110` health, `53/53` mana, `12` Attack Power, unchanged Warrior
     hotbar, read-only Chronicle level, and silent repeated load;
   - all three Marsh Prowlers retain the project-owned skeletal mesh and route
     Idle, Walk, Run, Attack, Hit, and Death from generic enemy state; verify
     paws/terrain contact, swamp palette, target-ring/nameplate clearance,
     combat, tonic loot, hide, and respawn;
   - retain all four accepted class palettes across sixteen starter-ability
     illustrations, fixed `32x32` art inside unchanged `92x64` slots, key/name
     fit, empty and Interact stability, data-driven hover tooltips, and
     synchronized icon/text cooldown dimming with two-line countdown copy;
   - Battle Shout and Nature's Focus grant `+8 Attack Power` for 10 seconds;
     Ward grants `+10 Armor` for 10 seconds; Snare deals light damage and
     halves movement for 6 seconds; Frost Root deals light damage and stops
     movement for 4 seconds; Meditate restores 18 missing mana. Confirm chat,
     cooldown, natural expiration, and respawn clearing;
   - retain the accepted Battle Shout, Ward, and Nature's Focus treatment
     beneath player mana with
     existing ability art, names, beneficial coloring, live countdowns, and
     hover descriptions; Snare and Frost Root appear beneath selected-target
     HP with harmful coloring. Confirm duplicate refresh, expiry/respawn/target
     clearing, fixed two-cell row bounds, and no neighboring HUD movement.
     Meditate remains instantaneous and creates no timed cell;
   - retain the eight-segment presentation-only world aura sourced from those
     same successful-effect snapshots. Battle Shout and Nature's Focus use
     orange-gold, Ward blue-white, Snare marsh green, and Frost Root a lighter
     ice-white cyan. Harmful presentation scales from transformed creature
     bounds and traces the support surface; the accepted Prowler result is a
     `94.352` cm inner radius at `21` cm above the hit, inside the calm outer
     target circle. Harmful status takes visual priority and empty/dead state
     hides all segments;
   - all ten empty equipment-slot icons, Recruit Pack and Marsh Tonic row/detail
     art, occupied-slot art, category fallback, text/tooltips, fixed icon sizes,
     the illustrated adventurer beneath the unchanged slot grid, and no
     inventory/footer/hotbar layout shift. Retain the accepted empty and
     Recruit-Pack-occupied compositions and confirm the backdrop remains
     decorative and does not obscure occupied controls. Confirm
     `Recruit Pack` fits its row and detail header without clipping or crowding
     `Equipment`;
   - retain the proven `Q` plus independent `W`/`S` cancellation behavior;
     press physical `Ctrl+M` and confirm inversion feedback because Slate's
     atomic chord cannot span the controller's later `PlayerTick` poll;
   - structured inventory layout, empty state, reward inspection, clickable row
     selection, selected-row highlight, 700px equipment/bonus pane, all ten slot
     controls, Recruit Pack drag/click bag-to-Back transfer, slot-click or
     equipment-to-bag drag unequip, gold/red drop states, full-bag
     rejection, stat changes, row/slot hover tooltips, net item comparison,
     category/name Sort ordering and selection preservation, Sort disabled
     during drag, the fixed icon-bearing fantasy drag token for armor and
     consumables, category-sigil fallback, fixed icon-bearing reward popup,
     Marsh Tonic loot/use, `I`,
     `[`/`]`, and cursor capture/release;
   - hotbar dimming and live cooldown countdown;
   - native nameplate and health-aware color;
   - complete stationary cyan-blue 48-segment emissive target circle sized to
     the Prowler footprint, clearing its paws and supporting surface;
   - Mara's greeting remains hidden beyond 420 cm and uses the accepted
     available, active, ready, and completed copy and marker colors within
     range, without moving HUD or world-marker layout;
   - Mara marker/dialogue, quest, combat, reward, and inventory update;
   - enemy leash/return and player death/respawn protection;
   - bottom-left clipped chat log;
   - Mara's accepted ownership proof: physical `F` quest acceptance, real
     Prowler objective progress, original-interactable completion, and no quest
     or interaction authority on the rigged wrapper;
   - the vendor stall at `(-1530, -1430, 0)`, yaw `180`, with a customer-facing
     counter, four solid supports, clear service approach and east bypass, and
     unchanged normal-`F` Fenwatch Supplies ownership on the art-free service;
   - 53 grounded upright Fab actors plus 24 original placements from the
     waystone/lamp/signpost/gate/fence/boundary-stone/chest/shelter/keeper/
     quartermaster/armsmaster/vendor-stall/practice-dummy/cottage/workshop/
     notice-board/reed family;
     inspect the route-facing chest at `(-1740, -1180, 0)`, its solid lid, the
     Fenwatch shelter behind Mara at `(-1740, -700, 0)`, its four solid supports
     and clear center, Mara's grounded front-facing non-colliding Fenwatch
     keeper with readable marker/name, the non-colliding quartermaster beside
     the supply chest with no interaction authority, its co-located art-free
     vendor service, four visual-only reed
     clusters, clear
     gate lanes and solid fences/end stones,
     the moss/earth road, a navigable spawn/Mara route, readable enemy pocket,
     supported ruin silhouette, and balanced daylight/fog under the
     atmospheric sky.
   - approach the quartermaster and press `F`; confirm 40 starting copper,
     tonic at 8 with sell value 3, one Recruit Pack at 30 with sell value 12,
     tonic purchase/sale/buyback at `40 -> 32 -> 35 -> 32`, pack purchase at
     `32 -> 2`, exact wallet/inventory/finite-stock/buyback changes, sold-out
     and insufficient-funds states, fixed non-overlapping result/footer copy,
     chat, and close/input restoration;
   - complete Mara's quest and confirm its saved 20-copper reward pays exactly
     once alongside XP and Recruit Pack, with repeat completion rejected.
   - approach the armsmaster and press `F`; confirm the fixed Fenwatch Training
     panel, one data-driven Combat Drills offering, 40-to-30 copper and 0-to-25
     XP transaction, exact chat/panel feedback, insufficient-funds rejection,
     and clean Inventory/Chronicle/close handoffs. The visual actor must remain
     art-only while the co-located service owns all interaction/progression.
     Confirm the project-owned 3.2-second Idle is playing and advancing,
     grounded feet and staff remain planted, the marker and route stay clear,
     and the static soft-reference fallback remains intact.
   - inspect `Embermere_FenwatchPracticeDummy_TrainingYard_01` at
     `(-1120, -1120, 0)`, yaw `45`; retain its grounded target-facing
     composition, five shared materials, solid base/core, clear arms, open
     route spacing, and absence of the replaced generic crate.
   - inspect `Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
     `(-690, -1030, 0)`, yaw `-100`; retain its five-material open-front read,
     solid front posts/rear wall/workbench, clear player-height bay,
     visual-only roof/tools, open dummy approach and road-side bypass, and
     absence of `FabPass_Village_Fence_03`.
   - run the documented two-session persistence flow: save the accepted
     `22`-copper/`125`-XP/equipped-pack state, start fresh PIE, load it, and
     prove exact item identity, quest, finite stock, stats, and idempotent
     second load. Confirm buyback does not persist.
   - run the trainer-specific Chronicle lane: train once from `40`/`0` to
     `30`/`25`, save, start fresh at `40`/`0`, then confirm two loads each
     restore exactly `30`/`25` without items, equipment, quest, vendor-stock,
     buyback, reward, or schema mutation.
5. Walk from the village into the new Prowler triangle. Confirm each `525` cm
   pull stays solo, visual markers/bands do not block movement, and an enemy can
   leash and return home normally.
6. Inspect the accepted road approach to both boundary fences and rune-topped
   end stones. Confirm the relocated south-side trees preserve depth without
   masking the threshold.
7. Confirm the already-added precise daylight, moss/earth-ground, original-art,
   collision, and encounter-layout validator assertions still match the
   clean-restart visual result.

High-value milestones after that:

- build a fixed native multi-quest ledger/focus panel over the accepted two
  records. `QuestStates` remains authoritative; UI may only read records and
  request transient focus. Cover empty/one/two/eight-record layouts, exact
  active/ready/completed copy, mouse/keyboard and peer-panel handoff, focus
  switching without mutation, completed history, teardown, and save/load
  non-persistence. In clean PIE, switch the HUD projection between Mara and
  Still Waters without changing either record or replaying rewards;
- retain the accepted art-free practice-target contract around the visible
  dummy, including exact stationary transform, normal hotbar damage,
  three-second reset, target clear/reacquisition, and zero retaliation, loot,
  XP, inventory, quest, trainer, or persistence mutation;
- retain the accepted immutable combat-result and fixed floating-feedback
  presentation on Prowlers and the practice target, including exact amounts,
  rapid-result cap, expiry, target/death/reset clearing, and chat/nameplate
  fallbacks; do not emit `MISS` until authoritative resolution produces it;
- retain the accepted 48-segment cyan target circle and the `Z=-1000`
  finite-world recovery contract; fresh PIE has accepted target switching,
  autorun cancellation, exact village respawn, full health, walking, zero
  velocity, and protection feedback;
- tune Prowler animation timing, transitions, material balance, or physics only
  when normal-route PIE exposes a concrete issue;
- retain the resolved straight-line autorun route and chest clearance;
- extend sparse `NoCollision` marsh dressing only where route and combat
  sightlines remain clear;
- retain the accepted beneficial and harmful world-aura palettes, grounding,
  and target-circle separation; polish only concrete readability issues;
- retain Mara's accepted rigged Fenwatch keeper and verify its grounded Idle,
  marker/name clearance, normal-range `F` dialogue, and full quest flow from
  the PlayerStart route; retain the contextual greeting as a removable
  read-only observer across all four quest states with no reward replay;
- retain the accepted Chronicle one-slot lifecycle, readable empty/malformed/
  rejected-version feedback, overwrite/load confirmations, panel handoff, and
  two-session idempotence; retain console commands as debug fallbacks and keep
  buyback session-only;
- retain the proven Blender waystone/lamp/signpost/gate/fence/end-stone/chest/
  shelter/keeper/quartermaster/armsmaster/practice-dummy lane and its
  deterministic scripts, FBX checks, original-art tags, and route composition;
- retain the accepted rigged keeper/quest, armsmaster/trainer, and
  quartermaster/vendor ownership boundaries and transaction contracts;
  retain the accepted trainer-produced 30-copper/25-XP Chronicle restore across
  a fresh world and second idempotent load without adding trainer-specific
  serialized state;
- retain the accepted production armsmaster skeletal lane: exact mesh,
  ten-bone imported hierarchy containing all nine authored bones, 3.2-second
  Idle, live advancing clock, static fallback, and no collision or service
  authority;
- rerun the trainer-to-Chronicle proof with the rigged presentation active,
  then consider another NPC rig only when it adds enough visual value to earn
  the same deterministic import, fresh-package, fallback, and live-clock gates;
- optional restrained rune/soft-edge texture treatment for the dedicated
  target-circle material after all three normal-route Prowlers pass the
  physical-eye sweep;
- build the next cohesive Fenwatch village module through the deterministic
  Blender/classic-FBX/package/validation lane without obscuring Mara's
  greeting, vendor service, trainer yard, or normal routes;
- proper Stylized Classic fantasy village buildings from a suitable signed-in
  UE-compatible pack;
- preserve the collision-cleared `525` cm solo-pull baseline while tuning
  leash, attack, damage, and respawn from normal-route PIE feel;
- player respawn timing/protection tuning only when normal-route play exposes a
  concrete feel issue;
- continued automation around player-facing combat and UI behavior.

## Daily Build Automation

The automation is named `daily-embermere-rpg-build` and runs at 8:00 AM
America/New_York. Its prompt should be refreshed after each accepted build so
the next run names the current module, test count, verification baseline, and
best bounded milestone rather than replaying stale work.

Daily working agreement:

- start with `TODO.md` and this handoff;
- work expansively when the path is clear;
- use first-class Unreal MCP when available;
- build, run tests, use PIE, and validate map changes;
- keep vendor/config noise out of commits;
- commit and push intentional work;
- update `TODO.md` with the next day's best starting point;
- wrap before the five-hour usage window becomes too constrained for a safe
  build/test/commit cycle.

## Copy-Ready New Task Prompt

Use this as the first message in the fresh Embermere task:

```text
We are continuing the Embermere RPG Unreal Engine 5.8 prototype in:
/Users/wizard/Documents/Unreal Game

This is a continuation of a long prior build thread. Treat the repository as the durable source of truth instead of asking me to restate its history.

Start by reading, in order:
1. Docs/THREAD_HANDOFF.md
2. TODO.md
3. README.md
4. Docs/PLAYTESTING.md
5. Docs/UNREAL_LESSONS.md
6. Docs/FAB_ASSET_PLAN.md
7. Docs/NPC_PRESENTATION_CONTRACT.md
8. Docs/VENDOR_SERVICE_CONTRACT.md
9. Docs/TRAINER_SERVICE_CONTRACT.md
10. Docs/SAVE_GAME_CONTRACT.md
11. Docs/PRACTICE_TARGET_CONTRACT.md
12. Docs/COMBAT_FEEDBACK_CONTRACT.md
13. Docs/CHARACTER_CREATION_CONTRACT.md
14. Docs/LEVEL_PROGRESSION_CONTRACT.md
15. Docs/MULTI_QUEST_CONTRACT.md
16. Docs/QUEST_LEDGER_PRESENTATION_CONTRACT.md

Then inspect git status and recent commits. Preserve the existing unstaged Config/DefaultEngine.ini and Config/DefaultInput.ini changes; do not stage, revert, or overwrite them unless we intentionally decide they are required.

Unreal may already be open with the project loaded. If it has not been restarted since the latest C++ build, ask me to restart it before authoritative PIE checks. Unreal MCP uses port 8123. Prefer launching UE 5.8 with:
-ModelContextProtocolStartServer -ModelContextProtocolPort=8123
The editor-console fallback is:
ModelContextProtocol.StartServer 8123

Prefer first-class Unreal MCP tool search. Use direct HTTP only as a fallback. Run Unreal commandlets sequentially, build C++ with -NoHotReloadFromIDE before authoritative headless tests, and save intentional map changes through Unreal asset APIs rather than simulated keyboard shortcuts.

Follow TODO.md's Start Here section. Confirm the 2026-09-02 no-hot-reload
native Quest Ledger module, the 2026-09-01 Still Waters
quest/service/router module and saved quest/map packages, the
2026-08-31 save-version-3 multi-quest module, class-colored level-up world-VFX module, level-gated trainer and
progression-presentation module, serialized rules asset, character-identity
persistence and quest/map packages, the accepted notice-board,
vendor-stall,
cottage, training-workshop, practice-dummy, and native practice-target map packages,
and all three accepted skeletal-mesh/Skeleton/Idle sets,
offering/Chronicle,
bounds-aware cyan target circle, finite-world recovery, grounded bounds-aware
world-status VFX,
Marsh Prowler, terrain, reeds, Fenwatch keeper, quartermaster, NPC wrapper,
vendor stock/service, item/quest economy data, Blueprint/map packages, and
route-repair map, then run all 85 tests, including the two Quest Ledger
presentation/focus tests, the three Still Waters service/routing/round-trip
tests, the four multi-quest runtime, round-trip,
legacy-adapter, and rollback tests, the class-colored level-up
world-VFX presentation test, the four progression tests,
the four character-creation lifecycle/restriction/loadout tests, the three
character-identity persistence tests, combat-result and floating-feedback
presentation, the two practice-target policy/combat-reset tests,
contextual-greeting,
persistence round-trip,
validation/rollback, slot-inspection, native Chronicle panel, trainer
transaction/service/offering, static armsmaster presentation, production
armsmaster and quartermaster Idle presentations, and trainer-panel contracts.
Retain the
original rigged Prowler across all three instances and verify Idle, Walk, Run,
Attack, Hit, Death, terrain contact, target presentation, combat, tonic loot,
hide, and respawn. Retain the accepted fixed player and
selected-target status rows with saved ability art, names, live countdowns,
beneficial/harmful colors, hover descriptions, duplicate refresh, and clearing
on expiration, respawn, death, and target switch without neighboring HUD
movement. Retain the data-driven eight-segment world aura that reads those same
successful-effect snapshots without owning gameplay: orange-gold Attack Power,
blue-white Armor, marsh-green Snare, ice-white cyan Root, harmful priority, and
empty/dead hiding. Harmful auras now derive a bounded radius from transformed
visual bounds and trace the support surface; clean PIE accepted the Prowler at
`94.352` cm radius and `21` cm support clearance inside the unchanged outer cyan
target circle. The latest clean PIE also
proved Q autorun plus independent W and S
cancellation, all four starter-class hotbar palettes, Warrior cooldown
presentation, the icon-bearing reward popup, a real Recruit Pack bag-to-Back
drag, and the centered paper doll beneath both empty and occupied slot states
with fixed layout and noninteractive behavior. Retain Battle
Shout and Nature's Focus at +8 Attack Power for 10 seconds, Ward at +10 Armor
for 10 seconds, Snare at half movement for 6 seconds, Frost Root at zero
movement for 4 seconds, and Meditate restoring 18 missing mana. Retain all
sixteen fixed 32x32 ability illustrations, the unchanged 92x64 hotbar layout,
data-driven tooltips, empty-slot stability, and F Interact. Retain the accepted
item/slot art, compact Recruit Pack copy, reward popup,
inventory/footer/hotbar bounds, and physically inspect the populated fantasy
drag token while in motion. Physically verify Ctrl+M feedback. Recheck the
accepted gate/fence/end-stone threshold, exact south-side foliage cleanup, the
700px inventory and ten-slot paper doll, identity-preserving Sort,
bag-to-equipment and equipment-to-bag drag/drop with gold/red feedback,
click/keyboard fallbacks, item comparison and hover tooltips, full-bag failure
feedback, Marsh Tonic enemy loot and Use behavior, cursor mode, native enemy
nameplate, bounds-aware surface-traced cyan-blue target circle, quest/reward
loop, enemy
leash, respawn protection, chat clipping, atmospheric daylight and the
38-expression moss/earth road, 53 grounded upright Fab actors, and all 24
original-art placements including the vendor stall, closed west cottage,
open-front training workshop, roadside notice board, south-commons communal
well, four
`NoCollision` reed clusters, the
open-sided Fenwatch shelter at `(-1740, -700, 0)`, yaw `-64`, and the
  grounded front-facing non-colliding rigged Mara keeper wrapper with its
  3.6-second Idle, marker/name clear, and original Blueprint quest authority,
  the
non-colliding Fenwatch quartermaster at `(-1530, -1190, 0)`, yaw `100`, using
the production skeletal lane of the reusable NPC presentation wrapper with an
exact 4.0-second Idle, measured live playback, reviewed static fallback, and no
service or interaction authority, plus its co-located art-free vendor service
and saved stock asset. Exercise the normal `F` vendor loop: 40 starting copper,
tonic at
8/sell 3, one Recruit Pack at 30/sell 12, tonic purchase/sale/buyback at
`40 -> 32 -> 35 -> 32`, pack purchase at `32 -> 2`, exact wallet/inventory/
stock/buyback mutation, sold-out and insufficient-funds states, fixed result/
footer copy, chat, and close/input restoration. Complete Mara's quest and prove
its 20-copper reward pays exactly once. Re-run the six focused economy/vendor
tests and fresh-process validator.
Retain the accepted Fenwatch armsmaster and trainer separation: the grounded,
non-colliding art-only rigged armsmaster and co-located art-free service sit at
`(-1320, -920, 0)`, yaw `100`; the skeletal lane has nine authored Blender
bones plus one imported Unreal Armature root, an exact 3.2-second Idle, measured
live playback, and the reviewed static-mesh fallback. The saved offering list
exposes repeatable level-1 Combat Drills for 10 copper and 25 XP plus repeatable
level-2 Advanced Combat Drills for 20 copper and 50 XP. Exercise the normal `F`
trainer loop and prove the visible level-1 lock, live level-2 unlock, exact
advanced transaction, insufficient-funds rejection, current status copy,
fixed-panel bounds, chat, and Inventory/Chronicle/close handoff. Preserve the
2,824 Blender-source versus 2,800 post-import Unreal topology distinction.
Retain the accepted practice dummy at `(-1120, -1120, 0)`, yaw `45`: grounded
target face toward the armsmaster, five shared materials, 2,572 triangles,
solid base/core, clear arms, and no generic crate.
Retain the accepted version 3 persistence contract and live two-session proof:
Chronicle captured stable Elf Wizard identity alongside durable progression; a
fresh Lizardman Ranger session restored the exact saved Elf Wizard class stats,
hotbar, and progression twice without duplication, repeated reward, or stat
inflation. Version 1 remains loadable as current-rules Human Warrior without
rewriting the old slot. Buyback, combat, cooldowns, temporary effects, and
position remain intentionally session-only.
Also retain
the route-facing supply chest at `(-1740, -1180, 0)` with solid authored lid
collision, at least 225 cm of saved spawn-corridor clearance, and a clear live
player-height trace. Walk the
normal route and prove each 525 cm Prowler pull stays solo while visual
band geometry remains non-colliding. Inspect the shelter from PlayerStart and
prove Mara's name/quest marker, keeper, open center, four supports, and straight
autorun route remain readable. Retain the accepted player-facing Chronicle:
plain `M`, exact slot summary, overwrite/load confirmations and cancel paths,
empty/rejected feedback, mutually exclusive panel handoff, and console-command
fallbacks over the same atomic contract. Retain both wrapper production
skeletal/Idle lanes and the accepted vendor and trainer ownership contracts.
Retain both accepted trainer Chronicle lanes: the baseline `30`-copper/`25`-XP
state and the richer Elf Wizard Advanced state at level `2`, `175` XP, `40`
copper, equipped Recruit Pack, completed quest, and untouched finite stock.
Each fresh-world second load must remain exact without trainer-specific
serialized state.
Then retain grounded normal-camera keeper, armsmaster, and quartermaster
motion, clear markers/routes, advancing animation clocks, and all three static
fallbacks. Retain the accepted Mara proof: physical `F` opened and accepted the
Blueprint-owned quest, real Prowler combat advanced it to `3/3`, and the same
original interactable completed the return while the rigged wrapper remained
art-only. Retain the fixed contextual greeting hidden outside 420 cm and its
available, active, ready, and completed states inside range; it must remain a
read-only observer with no interaction, dialogue, quest, marker, or reward
authority. Recheck the vendor stall's customer-facing counter, solid supports,
clear service approach and east bypass, and normal `F` vendor opening. Retain
the accepted workshop at `(-690, -1030, 0)`, yaw `-100`, with its four solid
purposeful surfaces, clear open bay, dummy approach, decorative clearance, and
east bypass. Retain the accepted art-free practice target at the dummy's exact
transform: stationary, non-colliding, normally targetable and damageable,
three-second reset, and no AI, retaliation, loot, XP, quest credit, service, or
persistence authority. Retain the accepted asset-agnostic floating combat-
feedback presentation shared by Prowlers and the practice target: immutable committed
results, exact applied amounts, fixed three-entry bounds, 1.25-second expiry,
target/death/reset clearing, and durable chat/nameplate fallbacks. Retain the
accepted presentation-only notice board, its three purposeful colliders, clear
decorative geometry, and four protected routes. Retain the accepted fixed
character-creation picker, all eight races/four classes, explicit disabled
Dwarf Ranger and Bullywug Wizard paths, exact data-driven starter stats and
hotbars, one-shot confirmation, and controller input/HUD handoff. Retain the
accepted derived-level contract: thresholds `0/100/250/450/700`, level-5 cap,
rules-owned race/class growth, candidate-level equipment validation, silent
idempotent restore, and no serialized level. Retain the fixed XP/level-up
presentation, the twelve-segment class-colored live world effect, and the two-
offering level-gated trainer. Retain the accepted communal well at its exact
south-commons transform, with only purposeful curb/upright collision and every
protected route clear. Retain its separate art-free rest service and
presentation observer, then retain Still Waters through its own separate
art-free notice-board owner and committed-success router. The board/well art,
rest service/VFX, and trainer must remain removable and quest-free. Retain the
accepted fixed selected-quest detail, exact reward summary, and distinct
quest-data-owned objective instructions in the fixed `260x68` tracker and
unchanged detail region. Next, define one presentation-only quest-update
observer over immutable post-commit acceptance, progress, ready, and completion
results. Keep chat, tracker, greeting, and ledger as durable fallbacks; use
fixed short-lived bounds, bounded rapid updates, teardown/death/load clearing,
and no replay on silent restore. Preserve stable IDs, quest/reward authority,
transient selection/focus, save version `3`, peer-panel lifecycle, and zero
mutation. Do not add another quest, abandonment, sorting, map tracking, naming,
appearance, autosave, profiles, deletion, or implicit migration in that slice.

The project should remain classic high fantasy with early EverQuest/WoW tab-target controls and a Stylized Classic art direction. Keep gameplay systems asset-agnostic and do not commit raw Fab/Marketplace packs.

Refresh the existing daily-embermere-rpg-build 8:00 AM heartbeat with the
current commit, 88-test and 20-validator baseline, accepted Quest Ledger,
selected-quest details, and quest-owned objective instructions,
character creation,
v3 identity/multi-quest persistence, derived-level progression, level-gated trainer,
Advanced Chronicle proof, class-colored world VFX, and next bounded milestone
before ending the run.
```

## Handoff Principle

The conversation is disposable. The repository, tests, handoff docs, and daily
TODO are the memory system. Update them whenever a lesson would otherwise exist
only in chat.
