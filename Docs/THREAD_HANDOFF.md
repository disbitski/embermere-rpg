# Embermere New-Thread Handoff

Last updated: 2026-08-13

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
12. `Docs/MARSH_PROWLER_ART_BRIEF.md` when creature work is active
13. `Docs/GROUNDING_AND_TERRAIN_PASS.md` for environment contact/readability
14. `JOURNEY.md` when historical detail is useful

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
  countdowns, and chat log;
- a grounded local Fab/Epic art pass with 56 upright environment actors and 19
  project-owned placements from an original Blender-built Embermere
  waystone/lamp/signpost/gate/fence/boundary-stone/chest/shelter/keeper/
  quartermaster/armsmaster/practice-dummy/reed family, a 38-expression moss/
  earth road material, and a Mac-friendly daylight baseline;
- a reusable art-only NPC wrapper with static and skeletal lanes, shared
  transforms, soft references, Anim Blueprint precedence, a
  skeleton-compatible single-node Idle lane, and no interaction or service
  ownership, now used by the saved production rigged Fenwatch keeper,
  armsmaster, and quartermaster with reversible static fallbacks. Mara's
  original Blueprint quest actor remains her sole gameplay authority;
- a separate Fenwatch vendor vertical slice with an art-free interactable
  service actor, data-driven stock/prices/sell values, earned player copper,
  atomic buy/sell/buyback rollback, fixed native stock UI, and saved ownership
  validation;
- a separate Fenwatch trainer vertical slice with an art-only rigged and
  Idle-animated armsmaster, art-free interactable service, data-driven Combat
  Drills, atomic copper-to-XP progression, fixed native training UI, and a
  matching solid-core practice dummy that owns no service authority;
- a versioned save-game contract that atomically captures and restores copper,
  XP, inventory/equipment identity, quest state, and finite vendor stock using
  stable identifiers plus validated soft paths. Buyback, combat, cooldowns,
  temporary effects, and position remain intentionally session-only;
- the first original rigged Marsh Prowler with six animations and
  asset-agnostic runtime presentation across all three saved enemy instances;
- 51 passing Unreal automation tests plus fresh-process keeper-rig,
  armsmaster-rig, quartermaster-rig, practice-dummy, trainer, vendor,
  UI-art/package, and
  saved-map validators, initialized-live-world route/collision traces,
  measured live NPC Idle advancement, and accepted two-session Chronicle
  proofs for both the full commerce/quest fixture and trainer-produced
  progression.

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

- `EmbermereCombatComponent`: ability execution and combat rules.
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

- `Save/EmbermereSaveGame.h`: versioned durable records for wallet, XP,
  inventory/equipment identity, quest state, and finite vendor stock.
- `Save/EmbermerePersistenceLibrary.*`: stable-ID capture, whole-snapshot
  preflight, atomic restore, slot save/load, and session-only-state reset.
- `Docs/SAVE_GAME_CONTRACT.md`: version 1 ownership, validation, lifecycle, and
  explicit non-goals.
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
  map retains 56 Fab actors after project-owned replacements and removal of
  unsupported accents and enemy markers.
- `Scripts/place_fab_zone_pass_unreal.py`: executes placement through Unreal
  Python and saves the map.
- `Scripts/validate_fab_zone_pass_unreal.py`: reloads and validates the saved
  map, actor count, upright rotations, gameplay anchors, collision-cleared
  encounter layout, 19 original-art placements, moss/earth terrain, and exact
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
  `1.525603` seconds. Her normal-range `F` dialogue and quest loop remains the
  next physical acceptance gate.

## Immediate Next Work

Start from the `Start Here` section of `TODO.md`. Confirm Unreal has the
2026-08-13 no-hot-reload rigged Fenwatch keeper, armsmaster, and quartermaster
module plus all three accepted skeletal-mesh/Skeleton/Idle sets, practice-dummy/map,
offering, Chronicle, Fenwatch
stock/service, item, quest, keeper, quartermaster, NPC wrapper, and Blueprint
packages, then confirm MCP/test discovery; restart only if the editor or test
registry proves stale.

First fresh-session checks:

1. Confirm Unreal has the latest module/map and open
   `L_Embermere_Prototype`; restart only when stale.
2. Start MCP on port `8123` and wait briefly for tool discovery. Prefer the
   dedicated startup flags documented above for unattended launches.
3. Run/discover all 51 tests, including the six economy/vendor tests,
   `Embermere.Trainer.TransactionRules`,
   `Embermere.Trainer.ServiceContract`,
   `Embermere.Trainer.FenwatchOfferingsData`,
   `Embermere.NPC.FenwatchArmsmasterPresentation`,
   `Embermere.NPC.FenwatchArmsmasterIdlePresentation`,
   `Embermere.NPC.FenwatchQuartermasterIdlePresentation`,
   `Embermere.NPC.FenwatchKeeperIdlePresentation`,
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
   - Mara marker/dialogue, quest, combat, reward, and inventory update;
   - enemy leash/return and player death/respawn protection;
   - bottom-left clipped chat log;
   - 56 grounded upright Fab actors plus 19 original placements from the
     waystone/lamp/signpost/gate/fence/boundary-stone/chest/shelter/keeper/
     quartermaster/armsmaster/practice-dummy/reed family;
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
  the PlayerStart route;
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
  a fresh world and second idempotent load without changing save version 1;
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
- proper Stylized Classic fantasy village buildings from a suitable signed-in
  UE-compatible pack;
- preserve the collision-cleared `525` cm solo-pull baseline while tuning
  leash, attack, damage, and respawn from normal-route PIE feel;
- player respawn timing/protection tuning only when normal-route play exposes a
  concrete feel issue;
- continued automation around player-facing combat and UI behavior.

## Daily Build Automation

The existing automation is named `daily-embermere-rpg-build` and runs at
8:00 AM America/New_York. It is a heartbeat attached to the original long task.

Before archiving the original task:

1. Create the new Embermere task from the prompt below.
2. Ask Codex in the new task to update/migrate `daily-embermere-rpg-build` so it
   targets the new task with the same 8:00 AM schedule.
3. Verify the automation card points to the new task.
4. Archive the original task only after the migration succeeds.

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

Then inspect git status and recent commits. Preserve the existing unstaged Config/DefaultEngine.ini and Config/DefaultInput.ini changes; do not stage, revert, or overwrite them unless we intentionally decide they are required.

Unreal may already be open with the project loaded. If it has not been restarted since the latest C++ build, ask me to restart it before authoritative PIE checks. Unreal MCP uses port 8123. Prefer launching UE 5.8 with:
-ModelContextProtocolStartServer -ModelContextProtocolPort=8123
The editor-console fallback is:
ModelContextProtocol.StartServer 8123

Prefer first-class Unreal MCP tool search. Use direct HTTP only as a fallback. Run Unreal commandlets sequentially, build C++ with -NoHotReloadFromIDE before authoritative headless tests, and save intentional map changes through Unreal asset APIs rather than simulated keyboard shortcuts.

Follow TODO.md's Start Here section. Confirm the 2026-08-13 rigged Fenwatch
keeper, armsmaster, and quartermaster module and all three accepted
skeletal-mesh/Skeleton/Idle sets, practice-dummy/map,
offering/Chronicle,
bounds-aware cyan target circle, finite-world recovery, grounded bounds-aware
world-status VFX,
Marsh Prowler, terrain, reeds, Fenwatch keeper, quartermaster, NPC wrapper,
vendor stock/service, item/quest economy data, Blueprint/map packages, and
route-repair map, then run all 51 tests, including the persistence round-trip,
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
38-expression moss/earth road, 56 grounded upright Fab actors, and all 19
original-art placements including four `NoCollision` reed clusters, the
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
live playback, and the reviewed static-mesh fallback. The saved offering exposes repeatable level-1
Combat Drills for 10 copper and 25 XP. Exercise the normal `F` trainer loop and
prove `40 -> 30` copper plus `0 -> 25` XP, insufficient-funds rejection,
fixed-panel bounds, chat, and Inventory/Chronicle/close handoff. Preserve the
2,824 Blender-source versus 2,800 post-import Unreal topology distinction.
Retain the accepted practice dummy at `(-1120, -1120, 0)`, yaw `45`: grounded
target face toward the armsmaster, five shared materials, 2,572 triangles,
solid base/core, clear arms, and no generic crate.
Retain the accepted version 1 persistence contract and live two-session proof:
`EmbermereSave` captured 22 copper, 125 XP, exact inventory/equipment identity,
completed Mara state, and exhausted finite Recruit Pack stock; a fresh PIE
session restored it through `EmbermereLoad`, and a second load produced no
duplication, repeated reward, or stat inflation. Buyback, combat, cooldowns,
temporary effects, and position remain intentionally session-only.
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
First prove
the accepted trainer-produced 30-copper/25-XP Chronicle state remains exact in
a fresh PIE world and a second idempotent load without changing save version 1.
Then retain grounded normal-camera keeper, armsmaster, and quartermaster
motion, clear markers/routes, advancing animation clocks, and all three static
fallbacks. Walk into Mara's normal `F` radius and complete her dialogue and
quest loop to physically prove the extracted art did not alter Blueprint-owned
gameplay. Tune only concrete Prowler or aura issues, or take the highest-value
next milestone when the path is clear.

The project should remain classic high fantasy with early EverQuest/WoW tab-target controls and a Stylized Classic art direction. Keep gameplay systems asset-agnostic and do not commit raw Fab/Marketplace packs.

Also migrate the existing daily-embermere-rpg-build 8:00 AM heartbeat from the old task to this new task before the old one is archived.
```

## Handoff Principle

The conversation is disposable. The repository, tests, handoff docs, and daily
TODO are the memory system. Update them whenever a lesson would otherwise exist
only in chat.
