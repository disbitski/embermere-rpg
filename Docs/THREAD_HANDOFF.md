# Embermere New-Thread Handoff

Last updated: 2026-07-13

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
- quest, inventory, equipment, stats, combat, targeting, interactable, and hotbar systems;
- hostile enemy aggro, chase, attack, leash, return-home, death, and respawn;
- player death, respawn, and short recovery damage immunity;
- a styled native HUD with player/target status, quest tracker, hotbar,
  clickable inventory rows and equipment slots, dialogue, loot feedback, nameplates, an emissive
  target ring, and chat log;
- a first local Fab/Epic art pass with 64 upright environment actors, the first
  original Blender-built Embermere waystone, and a Mac-friendly atmospheric
  daylight baseline;
- 16 passing Unreal automation tests and a headless Fab/map/lighting validator.

This is still prototype art. The first black-sky problem is corrected, but the
scene remains mixed in style and is missing a cohesive fantasy village kit,
final characters, animations, weapons, icons, VFX, audio, and final UI art.

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
- Three starter Marsh Prowlers in the wilderness pocket.
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
  clickable highlighted rows, cursor-aware input mode, keyboard footer, `I`
  toggle, bracket cycling, category/slot/level/effect metadata, ten clickable
  paper-doll controls, aggregate bonuses, transactional Equip/Unequip, and safe
  consumable Use actions.

These systems are functional programmer art. Dedicated fantasy UI materials,
icons, illustrated body-slot art, item comparison, drag/drop, chat scrolling,
and final responsive layout remain future work.

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

- `Scripts/place_fab_zone_pass.py`: idempotent 65-actor placement recipe.
- `Scripts/place_fab_zone_pass_unreal.py`: executes placement through Unreal
  Python and saves the map.
- `Scripts/validate_fab_zone_pass_unreal.py`: reloads and validates the saved
  map, actor count, upright rotations, gameplay anchors, moss foundation, and
  exact Mac-friendly daylight values.
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
4. `Embermere.Stats.DamageImmunity`
5. `Embermere.UI.EnemyNameplateWidget`
6. `Embermere.Enemy.LeashRules`
7. `Embermere.UI.InventoryToggle`
8. `Embermere.UI.HotbarCooldownDisplay`
9. `Embermere.UI.ChatLog`
10. `Embermere.Quests.CompletionRewards`
11. `Embermere.Equipment.SlotRules`
12. `Embermere.Equipment.StatApplication`
13. `Embermere.Inventory.ConsumableUse`
14. `Embermere.Enemy.LootRules`
15. `Embermere.Equipment.InventoryTransactions`
16. `Embermere.Inventory.CapacityTransactions`

Latest verified baseline (2026-07-13):

- editor build succeeded with `-NoHotReloadFromIDE`;
- automation passed 16/16 with zero errors and zero warnings;
- headless zone validator passed with 64 upright `FabPass_` actors, the exact
  original waystone mesh/tag/transform, required gameplay anchors, moss-ground
  overrides, and exact saved sun/skylight/fog values;
- live pre-relink PIE confirmed the blue atmospheric sky, readable ambient fill,
  muted moss ground, inventory show/hide, target/nameplate, and chat clipping;
- clean PIE verified the 2026-07-11 inventory shell, Mara quest acceptance,
  target/nameplate, raised target ring, moss/daylight balance, and chat clipping;
- clean pre-relink PIE verified Mara quest acceptance, combat progression, a real
  Marsh Tonic drop, targeting/nameplate, daylight/moss, and the three-column window;
- restart before visually testing the newly linked ten-slot paper doll, bag/gear
  transfers, full-bag rejection message, and stat changes.

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

Start from the `Start Here` section of `TODO.md`. The immediate manual boundary
is a clean editor restart because the previous editor session predates the
latest linked C++ module.

First fresh-session checks:

1. Restart Unreal and open `L_Embermere_Prototype`.
2. Start MCP on port `8123` and wait briefly for tool discovery.
3. Run/discover all 16 tests.
4. Start PIE and verify:
   - structured inventory layout, empty state, reward inspection, clickable row
     selection, selected-row highlight, 700px equipment/bonus pane, all ten slot
     controls, Recruit Pack bag-to-Back transfer, slot-click unequip, full-bag
     rejection, stat changes, Marsh Tonic loot/use, `I`,
     `[`/`]`, and cursor capture/release;
   - hotbar dimming and live cooldown countdown;
   - native nameplate and health-aware color;
   - flat rotating/pulsing 24-segment emissive target ring clearing the raised combat platform;
   - W/S autorun cancel and `Ctrl+M` inversion feedback;
   - Mara marker/dialogue, quest, combat, reward, and inventory update;
   - enemy leash/return and player death/respawn protection;
   - bottom-left clipped chat log;
   - 64 upright Fab actors plus the original road waystone, clear spawn/Mara route, readable road/enemy pocket,
     a ruin that does not trap the player, muted moss ground, and balanced
     daylight/fog under the atmospheric sky.
5. Confirm the already-added precise daylight and moss-ground validator
   assertions still match the clean-restart visual result.

High-value milestones after that:

- illustrated body-slot art, item tooltips/comparison, and drag/drop planning on top of the atomic transaction model;
- expand the proven Blender lane into a small matching road/village prop family
  while preserving deterministic scripts, FBX checks, and original-art tags;
- optional rune/soft-edge texture treatment for the dedicated target-ring
  material;
- proper Stylized Classic fantasy village buildings from a suitable signed-in
  UE-compatible pack;
- enemy aggro/leash/attack/damage/respawn tuning from real PIE feel;
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

Follow TODO.md's Start Here section. The first priority is a clean-restart PIE verification of the 700px inventory and ten-slot paper doll, Recruit Pack bag-to-Back transfer and click-to-unequip flow, full-bag failure feedback, Marsh Tonic enemy loot and Use behavior, cursor mode, hotbar cooldown countdown, native enemy nameplate, raised saturated emissive target ring, movement/camera fixes, quest/reward loop, enemy leash, respawn protection, chat clipping, atmospheric daylight/moss-ground balance, and the corrected 65-actor Fab environment. Then continue into starter-enemy tuning, item comparison/drag-drop planning, and the highest-value next milestone when the path is clear.

The project should remain classic high fantasy with early EverQuest/WoW tab-target controls and a Stylized Classic art direction. Keep gameplay systems asset-agnostic and do not commit raw Fab/Marketplace packs.

Also migrate the existing daily-embermere-rpg-build 8:00 AM heartbeat from the old task to this new task before the old one is archived.
```

## Handoff Principle

The conversation is disposable. The repository, tests, handoff docs, and daily
TODO are the memory system. Update them whenever a lesson would otherwise exist
only in chat.
