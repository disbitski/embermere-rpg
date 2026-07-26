# Embermere New-Thread Handoff

Last updated: 2026-07-24

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
7. `JOURNEY.md` when historical detail is useful

## One-Page State

Embermere is a single-player Unreal Engine 5.8 prototype with the feel of an
early-2000s tab-target MMORPG. It has a playable starter loop in
`L_Embermere_Prototype`: spawn near a village edge, find Mara Ashwick, accept a
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
- player death, respawn, and short recovery damage immunity;
- a styled native HUD with player/target status, quest tracker, hotbar,
  clickable/draggable inventory rows and equipment slots, dialogue, loot feedback, nameplates, an emissive
  target ring, project-owned fantasy drag tokens, data-driven item/slot icons,
  sixteen data-driven starter-ability icons with tooltips, an illustrated
  paper-doll equipment backdrop, data-driven timed buff/control rows with live
  countdowns, and chat log;
- a first local Fab/Epic art pass with 62 upright environment actors and nine
  placements from an original Blender-built Embermere waystone, ember-lamp,
  timber-signpost, traversable-road-gate, matching boundary-fence family, and
  rune-topped boundary stones, plus a Mac-friendly atmospheric daylight
  baseline;
- 26 passing Unreal automation tests plus fresh-process UI-art package,
  saved-map, and road-boundary validators.

This is still prototype art. The first black-sky problem is corrected, but the
scene remains mixed in style and is missing a cohesive fantasy village kit,
final characters, animations, weapons, final ability VFX, audio, and
final UI art.

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
- Mara Ashwick, the first quest giver, with a temporary gold quest marker.
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
- flat 24-segment rotating/pulsing target ring using the tracked unlit opaque-emissive
  `M_EmbermereTargetRing` material;
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

Data and types:

- `Data/EmbermereRulesData.*`: data-driven races, classes, and rules.
- `Data/EmbermereItemData.*`: item identity, category, equipment slot, level requirement, and stat bonuses.
- `Data/EmbermereUiIconSet.*`: item-category, equipment-slot, and missing-art
  icon resolution through soft texture references.
- `Data/EmbermereQuestData.h`: quest definition.
- `Types/EmbermereTypes.h`: shared gameplay enums/structs.
- `Types/EmbermereItemTypes.h`: item categories, equipment slots, and stat bonuses.
- `Interfaces/EmbermereTargetable.h`: targetable contract.

UI:

- `UI/EmbermerePlayerHudWidget.*`: native HUD and inventory layout.
- `UI/EmbermereEnemyNameplateWidget.*`: selected enemy nameplate.
- `UI/EmbermereCharacterCreationWidget.*`: race/class creation scaffold.
- `UI/EmbermereGameplayMessageLibrary.*`: routes gameplay feedback into HUD.

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

- `Scripts/place_fab_zone_pass.py`: idempotent 62-actor placement recipe.
- `Scripts/place_fab_zone_pass_unreal.py`: executes placement through Unreal
  Python and saves the map.
- `Scripts/validate_fab_zone_pass_unreal.py`: reloads and validates the saved
  map, actor count, upright rotations, gameplay anchors, collision-cleared
  encounter layout, nine original-art placements, moss foundation, and exact
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
village road at `(20, -170, 20)`, yaw `22`; two authored boxes cover the base
and post while its overhead arms remain non-colliding. The importer explicitly
saves the new `M_EmbermereTimber` package so it cannot exist only in memory.

The fourth original asset is `SM_EmbermereRoadGate_01`, built by
`Scripts/blender/build_embermere_road_gate.py` and imported through
`Scripts/import_embermere_road_gate_unreal.py`. It reuses the complete
stone/moss/timber/iron/ember material family and frames the road into the
wilderness at `(1080, 540, 20)`, yaw `20`. Four authored boxes cover the two
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
foliage transforms without changing the 62-actor Fab count.

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
- Prefer object-level `NewObject` tests when behavior does not require a world.
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

Latest verified baseline (2026-07-26):

- editor build succeeded with `-NoHotReloadFromIDE`;
- headless automation discovered and passed 26/26 with zero test failures,
  including saved starter-effect semantics and runtime contracts for timed
  power/armor buffs, Snare, Frost Root, Meditate, effective-stat consumption,
  respawn-safe temporary-effect clearing, fixed paper-doll presentation, and
  source-ability-backed timed-status presentation;
- clean PIE accepted the centered paper doll beneath an empty ten-slot grid
  without title, bonus, footer, inventory, or hotbar movement. The final July
  26 status relink makes the running GUI editor stale, so live status-row and
  occupied-slot contrast remain one clean-restart visual gate;
- stats expose generic active-effect snapshots with source ability data,
  remaining world time, and beneficial/harmful intent. Combat registers them
  only after timed gameplay effects succeed; duplicate applications refresh
  one record, and existing expiration/vital-reset paths clear mechanics and
  presentation together. The HUD renders two fixed cells beneath player mana
  and two beneath selected-target HP using saved ability art, names,
  countdowns, and hover descriptions without switching on class or ability ID;
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
- the validator reloaded the saved map and passed with 62 upright `FabPass_`
  actors, nine exact original-art placements, visual-only encounter geometry,
  required gameplay anchors, moss-ground overrides, and exact saved
  sun/skylight/fog values;
- Blender MCP generated and validated the 1,872-triangle boundary-stone module;
  Unreal imported it through `FbxFactory`, reused all five project materials,
  retained two authored colliders, and saved two exact fence-end placements;
- native traces proved three gate lanes clear, one gate support solid, both
  fence centers solid, and both boundary-stone cores solid. A fresh road capture
  shows the complete threshold after two masking trees moved within the south
  foliage band; their exact transforms are validator-owned;
- automatic movement is now applied separately from manual-axis cancellation,
  and the inventory drag visual is a fixed 236x62 fantasy token with resolved
  item art, category-sigil fallback, and item context. Reward feedback reuses
  the same resolver in a fixed 32x32 icon cell;
- starter Prowlers now use a `525` cm aggro radius and collision-cleared home
  points at `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`; a focused PIE
  probe proved one enemy moved and attacked while the other two stayed home;
- enemy markers plus safe/combat area bands are explicitly `NoCollision` in the
  saved map and setup script;
- a fresh editor loaded the 2026-07-18 controller module. Transform-based PIE
  checks proved `Q` autorun advances the player and independent `W` and `S`
  presses stop all subsequent movement. A July 22 populated-inventory capture
  accepted the slot/item art but rejected the old long Recruit Pack copy for
  row clipping and detail-heading crowding; a second capture accepted the
  corrected `Recruit Pack` row and detail fit. The icon-bearing reward popup and
  real Recruit Pack bag-to-Back flow were accepted on July 23. The populated
  drag token while in motion, occupied paper-doll contrast, new timed-status
  rows, and physical `Ctrl+M` feedback remain honest physical-eye checks.

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

## Immediate Next Work

Start from the `Start Here` section of `TODO.md`. Restart Unreal if it predates
the 2026-07-26 timed-status presentation C++ link, then confirm MCP/test
discovery against the current module.

First fresh-session checks:

1. Confirm Unreal has the latest module/map and open
   `L_Embermere_Prototype`; restart only when stale.
2. Start MCP on port `8123` and wait briefly for tool discovery.
3. Run/discover all 26 tests.
4. Start PIE and verify:
   - retain all four accepted class palettes across sixteen starter-ability
     illustrations, fixed `32x32` art inside unchanged `92x64` slots, key/name
     fit, empty and Interact stability, data-driven hover tooltips, and
     synchronized icon/text cooldown dimming with two-line countdown copy;
   - Battle Shout and Nature's Focus grant `+8 Attack Power` for 10 seconds;
     Ward grants `+10 Armor` for 10 seconds; Snare deals light damage and
     halves movement for 6 seconds; Frost Root deals light damage and stops
     movement for 4 seconds; Meditate restores 18 missing mana. Confirm chat,
     cooldown, natural expiration, and respawn clearing;
   - Battle Shout, Ward, and Nature's Focus appear beneath player mana with
     existing ability art, names, beneficial coloring, live countdowns, and
     hover descriptions; Snare and Frost Root appear beneath selected-target
     HP with harmful coloring. Confirm duplicate refresh, expiry/respawn/target
     clearing, fixed two-cell row bounds, and no neighboring HUD movement.
     Meditate remains instantaneous and creates no timed cell;
   - all ten empty equipment-slot icons, Recruit Pack and Marsh Tonic row/detail
     art, occupied-slot art, category fallback, text/tooltips, fixed icon sizes,
     the illustrated adventurer beneath the unchanged slot grid, and no
     inventory/footer/hotbar layout shift. Retain the accepted empty-state
     composition, then confirm the backdrop remains decorative and does not
     obscure occupied controls. Confirm
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
   - flat rotating/pulsing 24-segment emissive target ring clearing the raised combat platform;
   - Mara marker/dialogue, quest, combat, reward, and inventory update;
   - enemy leash/return and player death/respawn protection;
   - bottom-left clipped chat log;
   - 62 upright Fab actors plus nine original placements from the waystone,
     ember-lamp, timber-signpost, road-gate, boundary-fence, and boundary-stone
     family; clear gate lanes and solid fences/end stones; clear spawn/Mara route,
     readable road/enemy pocket,
     a ruin that does not trap the player, muted moss ground, and balanced
     daylight/fog under the atmospheric sky.
5. Walk from the village into the new Prowler triangle. Confirm each `525` cm
   pull stays solo, visual markers/bands do not block movement, and an enemy can
   leash and return home normally.
6. Inspect the accepted road approach to both boundary fences and rune-topped
   end stones. Confirm the relocated south-side trees preserve depth without
   masking the threshold.
7. Confirm the already-added precise daylight, moss-ground, original-art,
   collision, and encounter-layout validator assertions still match the
   clean-restart visual result.

High-value milestones after that:

- add restrained class/status VFX for the now-visible timed effects without
  coupling presentation assets to gameplay rules;
- expand the proven Blender waystone/lamp/signpost/gate/fence/end-stone lane
  into compact village-prop modules while preserving
  deterministic scripts, FBX checks, and original-art tags;
- optional rune/soft-edge texture treatment for the dedicated target-ring
  material;
- proper Stylized Classic fantasy village buildings from a suitable signed-in
  UE-compatible pack;
- preserve the collision-cleared `525` cm solo-pull baseline while tuning
  leash, attack, damage, and respawn from normal-route PIE feel;
- player respawn timing/protection tuning;
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

Then inspect git status and recent commits. Preserve the existing unstaged Config/DefaultEngine.ini and Config/DefaultInput.ini changes; do not stage, revert, or overwrite them unless we intentionally decide they are required.

Unreal may already be open with the project loaded. If it has not been restarted since the latest C++ build, ask me to restart it before authoritative PIE checks. Unreal MCP uses port 8123 and is started inside Unreal with:
ModelContextProtocol.StartServer 8123

Prefer first-class Unreal MCP tool search. Use direct HTTP only as a fallback. Run Unreal commandlets sequentially, build C++ with -NoHotReloadFromIDE before authoritative headless tests, and save intentional map changes through Unreal asset APIs rather than simulated keyboard shortcuts.

Follow TODO.md's Start Here section. Restart onto the 2026-07-26 timed-status
presentation module, run all 26 tests, and inspect the new fixed player and
selected-target status rows for saved ability art, names, live countdowns,
beneficial/harmful colors, hover descriptions, duplicate refresh, and clearing
on expiration, respawn, death, and target switch without neighboring HUD
movement. The latest clean PIE already proved Q autorun plus independent W and
S cancellation, all four starter-class hotbar palettes, Warrior cooldown
presentation, the icon-bearing reward popup, a real Recruit Pack bag-to-Back
drag, and the centered paper doll beneath an empty slot grid. Confirm occupied
paper-doll contrast, fixed layout, and noninteractive behavior. Retain Battle
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
nameplate, raised saturated emissive target ring, quest/reward loop, enemy
leash, respawn protection, chat clipping, atmospheric daylight/moss-ground
balance, 62 upright Fab actors, and all nine original-art placements. Walk the
normal route and prove each 525 cm Prowler pull stays solo while visual
marker/band geometry remains non-colliding. Then add restrained VFX, build a
compact original Blender village prop, pursue cohesive fantasy architecture,
tune concrete combat/respawn issues, or take the highest-value next milestone
when the path is clear.

The project should remain classic high fantasy with early EverQuest/WoW tab-target controls and a Stylized Classic art direction. Keep gameplay systems asset-agnostic and do not commit raw Fab/Marketplace packs.

Also migrate the existing daily-embermere-rpg-build 8:00 AM heartbeat from the old task to this new task before the old one is archived.
```

## Handoff Principle

The conversation is disposable. The repository, tests, handoff docs, and daily
TODO are the memory system. Update them whenever a lesson would otherwise exist
only in chat.
