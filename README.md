# Embermere RPG

**Embermere** is a classic high-fantasy RPG prototype inspired by the feeling of early EverQuest and World of Warcraft: tab targeting, hotbar combat, readable fantasy silhouettes, dangerous wilderness just outside town, and that old-school sense that a tiny village can open into a much bigger world.

This project is also a build journal. We are learning Unreal Engine 5.8 in public, using ChatGPT Codex as a development partner, and experimenting with Unreal MCP as a way to let AI help drive editor workflows, scene building, iteration, and QA.

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
- inventory, equipment, quest log, interactables, stats, combat, targeting, and hotbar components
- player/enemy character classes
- an asset-agnostic NPC presentation wrapper with interchangeable static and
  skeletal visual lanes, shared authored transforms, soft art references,
  Anim Blueprint precedence, a skeleton-validated single-node Idle lane, and
  no quest/service/interaction ownership; an optional read-only contextual
  greeting observes quest state while keeping all mutation on the existing
  authority actor
- a separate data-driven Fenwatch vendor service with player copper,
  finite/unlimited stock, complete buy/sell/buyback preflight, rollback-safe
  transactions, native fixed stock UI, inventory/chat feedback, and no art
  ownership
- a separate data-driven Fenwatch trainer service with stable lesson IDs,
  authored level gates and repeatability, complete level/currency/XP preflight,
  rollback-safe progression, two visible Combat Drills tiers in a fixed native
  training panel, inventory/Chronicle handoff, and no art ownership
- a separate art-free Fenwatch practice target that shares normal tab-target,
  hotbar, nameplate, and cyan-circle systems while remaining stationary,
  non-colliding, non-retaliating, reward-free, quest-credit-free, and safely
  resettable independently of the visible dummy and training workshop
- an immutable post-commit combat-result event plus a fixed three-entry native
  floating-feedback observer shared by Prowlers and the practice target; exact
  damage briefly rises beside the world nameplate while chat and target HP stay
  authoritative fallbacks, and presentation owns no hit resolution or rewards
- data-driven item sell values plus an earned-currency loop: Mara's first quest
  grants copper exactly once, selected bag items can be sold by identity, and
  the latest sale can be bought back at its recorded price
- a versioned save-game contract for confirmed race/class identity, copper,
  XP, inventory/equipment identity, quest progress/completion, and finite
  vendor stock; version 2 resolves stable race/class IDs through current rules,
  restores class stats and starter hotbars atomically, keeps version 1 loadable
  through an explicit Human Warrior fallback, rejects malformed records without
  partial state, and keeps buyback session-only
- data-driven level progression derived only from durable XP, with validated
  cumulative thresholds for levels 1 through 5, distinct race/class growth,
  atomic identity-base recomputation, one additive equipment layer, exact live
  level-up feedback, silent idempotent load, a fixed next-threshold XP bar, a
  live-only multi-level-aware celebration panel, a class-colored twelve-segment
  ground effect that tears down after `1.6` seconds, and read-only HUD/Chronicle
  presentation without serializing duplicate level state or calculating level
  inside UMG
- a real native pre-play character picker over the data-driven rules matrix:
  all eight races and four classes remain visible, invalid combinations stay
  visibly disabled, confirmation atomically applies starting stats and the
  four starter abilities, and controller-owned modal input returns cleanly to
  the normal HUD/game path; Human Warrior remains the reversible fallback
- native first-pass HUD panels for status, target, range state, quest tracking, dialogue, loot, and hotbar labels
- a fixed quest-owned four-state Mara greeting below her world marker that is
  range-gated, hit-test-invisible, non-colliding, and presentation-only
- bottom-left chat/combat log for targeting, combat, quest, XP, inventory, and death/recovery feedback
- first hotbar cooldown enforcement and ready-time feedback
- starter enemy aggro, leash, return-home, attack, death, and respawn behavior
- player respawn with short damage protection plus a finite-world recovery
  plane that cancels autorun, freezes death movement, clears velocity, and
  restores walking at the village
- structured inventory/equipment window with clickable and draggable item rows, a fixed project-owned icon-bearing fantasy drag token, ten clickable/drop-target paper-doll slots over a data-driven illustrated adventurer backdrop, gold/red drop feedback, equipment-to-bag return, aggregate bonuses, cursor-aware game/UI input, capacity, selected-item effects, icon-bearing reward feedback, empty/reward states, `[`/`]` inspection cycling, `I` show/hide behavior, transactional Equip/Unequip, and consumable Use actions
- data-driven project-owned item and equipment-slot icons with explicit soft
  item references, category/missing-art fallbacks, fixed row/detail/slot sizes,
  deterministic source generation, and durable Unreal texture packages
- sixteen data-driven project-owned starter-ability icons plus an explicit
  fallback, fixed hotbar art cells, class-specific palettes, synchronized
  cooldown dimming, and accessible ability-stat tooltips
- functional data-driven starter utility effects: timed Attack Power and Armor
  buffs, six-second Snare, four-second Frost Root, Meditate mana recovery,
  effective-stat combat consumption, and respawn-safe effect clearing
- data-driven player and selected-target timed-status rows that reuse ability
  art, show live countdowns and hover descriptions, distinguish beneficial from
  harmful effects, refresh duplicate applications, and clear with the same
  expiration/respawn rules as gameplay
- eight-segment non-colliding world-status auras that subscribe to those same
  successful-effect snapshots; harmful auras ground to the supporting surface,
  scale from the creature footprint, distinguish marsh-green Snare from
  ice-white Frost Root, and remain presentation-only
- atomic inventory capacity and quantity operations with rollback-safe equipment replacement and full-bag unequip rejection
- idempotent equipment stat application for health, mana, armor mitigation, and power
- selected enemy UMG nameplate widget, HP bar/readout, and a bounds-aware
  cyan-blue 48-segment target circle that traces the supporting surface and
  uses a dedicated emissive Embermere material
- live hotbar cooldown countdowns with unavailable-state dimming
- Unreal MCP setup notes and a local setup validator
- first editor-created Blueprints and rules data asset
- a tracked level-1 Back-slot Recruit Pack quest reward for the first equipment action
- stackable Marsh Tonic recovery items dropped by Marsh Prowlers, connecting combat loot to the inventory `Use` action
- the saved starter-zone map `L_Embermere_Prototype`
- the first original rigged enemy, `SK_EmbermereMarshProwler_01`, with
  deterministic Blender source, 7,464 triangles, 26 authored bones, five
  swamp-fantasy materials, six imported animations, three validated saved
  instances, and asset-agnostic runtime presentation
- a corrected 53-actor local Fab/Epic art pass plus 23 placements from
  Embermere's original Blender-built waystone, ember-lamp, road-signpost,
  road-gate, boundary-fence, rune-topped boundary-stone, supply-chest, and
  Fenwatch shelter, vendor stall, first closed cottage, open-front training
  workshop, and roadside notice board; Mara's rigged Fenwatch keeper; the
  rigged Fenwatch quartermaster and armsmaster; a practice dummy; and four
  visual-only marsh-reed clusters over the starter village, road, wilderness
  pocket, and ruin landmark, with grounded rotations,
  deliberate collision, a navigable PlayerStart/Mara route, a readable gate
  silhouette, a project-owned moss/earth/path material, collision-cleared solo
  enemy pulls, and a validated Mac-friendly daylight/atmosphere baseline

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

We are starting Unreal-first with Fab and Marketplace assets, then replacing or upgrading packs as the game identity sharpens. Blender is our original-asset workshop for distinctive Embermere props, ruins, modular architecture, weapons, and eventually characters. Gameplay systems are designed to stay asset-agnostic so art, VFX, icons, characters, and environments can be swapped without rewriting core mechanics.

The public repository intentionally ignores raw Fab/Epic marketplace asset folders. The saved map can reference locally imported packs, but the source asset packs themselves should be installed through Fab/Epic Launcher on each development machine rather than redistributed through GitHub.

See [Docs/ASSET_STRATEGY.md](Docs/ASSET_STRATEGY.md).

The current pack-by-pack shopping and import plan lives in [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md).

The original Blender and Blender MCP workflow lives in
[Docs/BLENDER_ASSET_PIPELINE.md](Docs/BLENDER_ASSET_PIPELINE.md). Its first
family now includes a project-owned stylized waystone shrine, matching ember
lamps, a timber road signpost, a traversable road gate, low boundary fences,
rune-topped end stones, a route-facing village supply chest, an open-sided
Fenwatch shelter, Mara's grounded rigged Stylized Classic Fenwatch keeper,
the rigged Fenwatch quartermaster and armsmaster, a matching vendor stall,
practice dummy, closed Fenwatch cottage, open-front training workshop,
roadside notice board, visual-only marsh reeds, and the fully rigged Marsh
Prowler, each built from reviewed scripts,
validated in Blender, imported through an explicit Unreal contract, and placed
as a swappable project-owned layer over the local Fab environment.

The reusable art-only static/skeletal NPC boundary lives in
[Docs/NPC_PRESENTATION_CONTRACT.md](Docs/NPC_PRESENTATION_CONTRACT.md).

The visible-dummy versus art-free combat authority boundary lives in
[Docs/PRACTICE_TARGET_CONTRACT.md](Docs/PRACTICE_TARGET_CONTRACT.md).

The post-commit combat-result and floating-presentation boundary lives in
[Docs/COMBAT_FEEDBACK_CONTRACT.md](Docs/COMBAT_FEEDBACK_CONTRACT.md).

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

The generated client config remains a one-time setup step. For unattended or
daily starts, Unreal 5.8 can launch the listener with:

```text
-ModelContextProtocolStartServer -ModelContextProtocolPort=8123
```

On macOS, pass the project path after `--args` when launching through `open`;
see [Docs/UNREAL_SETUP.md](Docs/UNREAL_SETUP.md) for the exact command.

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
- [Docs/VENDOR_SERVICE_CONTRACT.md](Docs/VENDOR_SERVICE_CONTRACT.md) records
  the merchant art/service/economy boundary and transaction guarantees.
- [Docs/TRAINER_SERVICE_CONTRACT.md](Docs/TRAINER_SERVICE_CONTRACT.md) records
  the armsmaster art/service/progression boundary and transaction guarantees.
- [Docs/COMBAT_FEEDBACK_CONTRACT.md](Docs/COMBAT_FEEDBACK_CONTRACT.md) records
  the immutable combat-outcome and short-lived world-presentation boundary.
- [Docs/SAVE_GAME_CONTRACT.md](Docs/SAVE_GAME_CONTRACT.md) records the versioned
  progression schema, validation/rollback rules, session-only state, and live
  PIE verification lane.

## Field Notes And Podcast

The code, assets, and build journal show what Embermere is becoming. These
Real World AI Lab field notes tell the story behind that work: how Codex,
Blender MCP, Unreal MCP, deterministic validation, and human judgment became a
repeatable game-development workflow.

- [From My Amiga 500 To Blender MCP: Building Embermere's First Original Asset](https://github.com/disbitski/real-world-ai-lab/blob/main/field-notes/2026-07-14-amiga-blender-mcp-embermere.md)
  follows the project from an Amiga-rooted creative journey to Embermere's
  first original waystone and a guarded, inspectable Blender-to-Unreal asset
  pipeline.
- [From One Waystone to a World: The Acceptance Loop Behind Embermere](https://github.com/disbitski/real-world-ai-lab/blob/main/field-notes/2026-07-22-embermere-asset-acceptance-loop.md)
  shows how one successful experiment became a related family of world assets,
  and why technical validation still needs playtesting, taste, and final human
  acceptance.
- [From Props to Predators: Building Embermere's First Animated Creature](https://github.com/disbitski/real-world-ai-lab/blob/main/field-notes/2026-07-28-embermere-first-animated-creature.md)
  expands the workflow into rigging, six animation states, Unreal runtime
  integration, and the first original Marsh Prowler without surrendering
  gameplay authority to presentation.

Prefer to listen? [The Age We Build, Season 1, Episode 6: From My Amiga 500 To
Blender MCP: Building Embermere's First Original
Asset](https://podcast.thedavedev.com/episodes/amiga-blender-mcp-embermere)
turns the first field note into a wider conversation about AI-assisted game
development, MCP guardrails, provenance, validation, creative identity, and
why human taste becomes more important as building gets easier.

## Status

Early playable Unreal prototype scaffold with Unreal and Blender MCP connected,
a fixed player-facing race/class picker that initializes data-driven starter
stats and hotbars before play, plus save version 2 persistence for that
confirmed identity with a backward-compatible version 1 Human Warrior fallback,
a rules-owned `0/100/250/450/700` XP curve that derives levels 1 through 5,
combines race/class stat growth, restores silently and idempotently, and exposes
the result read-only through status and Chronicle,
a daylight starter-zone loop mixing a local Fab pass with an original Embermere
waystone/lamp/signpost/gate/fence/end-stone/chest/shelter/keeper/
quartermaster/armsmaster/vendor-stall/cottage/workshop/notice-board/reed family,
the first original rigged and animated Marsh
Prowler, grounded moss/earth road presentation,
collision-cleared solo-pull tab-target combat, a fully illustrated data-driven
starter hotbar with cooldown feedback, tooltips, timed buffs, root, snare, mana
recovery, live player/target status indicators, and restrained data-driven
world-status auras, native HUD panels,
bottom-left chat/combat feedback, first quest reward and starter-enemy loot
flow, a clickable and draggable illustrated paper-doll equipment/inventory
window with atomic transactions, project-owned data-driven art reused across
rows, slots, details, drag feedback, rewards, and the equipment backdrop, safe
consumable actions, native selected-target nameplate/emissive-ring feedback,
and a reusable static-to-skeletal NPC presentation contract demonstrated by
the grounded Fenwatch keeper, quartermaster, and armsmaster around the village
service cluster. The training yard applies the same ownership discipline to
combat: the project-owned dummy and workshop remain replaceable art while a
colocated native practice target accepts real class abilities, clears on
defeat, resets after three seconds, and grants no retaliation, loot, XP, quest
progress, or service authority. Both it and the real Prowlers now consume the
same immutable post-commit combat result for fixed-size floating damage text;
rapid results stay capped, and deselection, defeat, reset, expiry, and teardown
clear presentation without changing the durable chat or target-frame facts. A
colocated art-free service turns the
quartermaster into Embermere's first
working merchant without coupling stock, prices, currency, transactions, or
interaction to the model. The playable loop now earns `20` copper from Mara's
first quest and supports rollback-safe selling and latest-item buyback through
the same fixed vendor panel. The native `Embermere Chronicle` now gives players
a deliberate one-slot Save/Load surface with non-mutating slot inspection,
overwrite/load confirmation, exact progression summaries, and readable
rejection feedback. Explicit `EmbermereSave` and `EmbermereLoad` commands remain
debug fallbacks. Version 2 persists confirmed race/class, copper, XP, exact
item/equipment identity, completed quest, and finite merchant stock across
fresh PIE worlds. Chronicle presents identity read-only; repeated loads do not
duplicate class stats, starter abilities, rewards, or equipment bonuses.
Version 1 slots remain loadable as Human Warrior without being silently
rewritten. Level remains derived from XP rather than serialized: the first
rules-owned curve reaches level 2 at `100` XP and caps at level 5 at `700`,
then combines race/class growth before applying equipment once. Trainer and
Mara still own only their XP grants; HUD and Chronicle consume the resulting
level read-only. A second removable observer consumes only the post-commit live
transition and briefly expands twelve project-material segments around the
player's feet; it owns no progression, collision, navigation, or persistence
state and never replays on load. See
[Docs/LEVEL_PROGRESSION_CONTRACT.md](Docs/LEVEL_PROGRESSION_CONTRACT.md).

The NPC wrapper's skeletal/Idle lane is now in production on three matching
Fenwatch characters. The armsmaster has `2,824` source triangles and a
3.2-second Idle; the quartermaster retains its reviewed 3,632-triangle merchant
silhouette and adds a 4.0-second Idle; Mara's 3,280-triangle keeper adds a
3.6-second Idle. Each deterministic rig has nine authored bones, complete rigid
weights, six materials, and one reviewed Armature root added by classic FBX in
Unreal. Live PIE proved all three saved animation clocks advancing. Their
original static meshes remain reversible fallbacks. The separate trainer and
vendor services retain their authority, while Mara's original Blueprint quest
actor remains the sole owner of her interaction, dialogue, quest, marker, and
rewards. Mara's rigged wrapper can now observe that authority and render
available, active, ready, and completed context copy without owning or replaying
any quest behavior.
Its data-driven training list now keeps repeatable level-1 Combat Drills at
`10` copper for `25` XP and exposes repeatable level-2 Advanced Combat Drills
at `20` copper for `50` XP. Locked lessons remain visible with service-owned
rejection copy, then refresh immediately when the Stats-derived level changes,
without moving progression or interaction authority into the model or UMG.
The richer persistence gate now saves a real Advanced result through Chronicle,
starts a fresh world with a deliberately different identity, and restores the
exact identity, stats, equipment, hotbar, quest, stock, copper, and XP twice
without trainer-specific schema or drift.
Autosave, deletion, multiple profiles, migrations, and position persistence
remain later product decisions.
