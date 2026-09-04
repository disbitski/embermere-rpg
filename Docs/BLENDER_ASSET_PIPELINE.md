# Blender Asset Pipeline

Embermere uses Blender as an original-asset workshop alongside Unreal and Fab.
The goal is not to replace every marketplace pack. It is to give the game its
own recognizable props, modular architecture, ruins, weapons, and eventually
characters while retaining Fab assets for broad coverage and specialist art.

## Decision

Adopt a Blender-to-Unreal pipeline with a local community MCP bridge.

- Start with static props and modular environment pieces.
- Build one bounded asset at a time from a written brief.
- Inspect dimensions, topology, pivots, UVs, materials, and collision with
  deterministic checks in addition to rendered screenshots.
- Export FBX for Unreal 5.8, then validate scale, collision, and readability in
  the actual Embermere map.
- Keep original source scripts, `.blend` files, previews, and exported source
  files eligible for the public repository. These are ours, unlike raw Fab
  packs.
- Continue using Fab for foliage breadth, temporary architecture, animation
  sets, audio, and other areas where a mature pack saves substantial time.

The X article that prompted this work demonstrates the useful loop: describe a
3D object, let the model build it in Blender, render it, inspect the result, and
iterate. That loop is real, but the render is not sufficient proof that a game
asset is shippable. Embermere adds explicit technical validation and an Unreal
PIE pass before accepting an asset.

## MCP Choice

Use `djeada/blender-mcp-server`, pinned to a reviewed commit when installed.
It provides named scene, material, render, export, history, and script tools;
supports FBX export; connects to Codex over STDIO; and keeps its Blender bridge
on `127.0.0.1:9876`.

The popular `ahujasid/blender-mcp` proves the concept and has broad adoption,
but its primary workflow exposes unrestricted Blender Python execution. An open
security report documents that this can execute operating-system commands with
the Blender process's privileges. Do not install that server for Embermere.

The selected bridge can also execute Python, so configure it defensively:

- enable **Safe Mode**;
- disable **Allow Inline Code**;
- approve only this repository's `Scripts/blender` and `ArtSource/Blender`
  directories;
- keep the bridge bound to localhost;
- keep Codex write tools approval-gated;
- run only reviewed, tracked scripts from the approved root.

This gives us reproducible procedural builds without accepting arbitrary code
from a prompt, reference file, or downloaded scene.

## Local Architecture

```text
Codex / ChatGPT desktop
  -> STDIO MCP server
  -> JSON/TCP on 127.0.0.1:9876
  -> Blender MCP add-on
  -> reviewed Scripts/blender/*.py
  -> .blend + preview + FBX
  -> Unreal import under /Game/Art/Embermere
  -> Unreal MCP inspection and PIE validation
```

Unreal and Blender use separate MCP servers. Unreal remains available at
`127.0.0.1:8123`; Blender uses a local STDIO process that connects to the
Blender add-on on port `9876`.

## Mac Setup

Verified machine state on 2026-07-13:

- Blender 5.1.2 is installed at `/Applications/Blender.app` with CLI access at
  `/opt/homebrew/bin/blender`.
- `uv` is installed at `/Users/wizard/.local/bin/uv`.
- `djeada/blender-mcp-server` is installed locally under ignored
  `Tools/External/blender-mcp-server` and pinned to
  `7eed33edf4aca2ab0ca84a6da27321f89f68b504`.
- The Blender add-on is enabled on `127.0.0.1:9876` with Safe Mode on, inline
  code off, and only the two project roots approved.
- The MCP SDK discovered 27 structured Blender tools; scene inspection and a
  tracked approved script succeeded, while inline execution was rejected.
- Docker is not installed and is not required for the selected bridge.

### 1. Install Blender

Download the current stable Apple Silicon build from:

- https://www.blender.org/download/

Move `Blender.app` to `/Applications`. Launch it once so macOS can complete its
first-run checks.

### 2. Install The MCP Server

Keep the community server local and outside source control:

```bash
cd "/Users/wizard/Documents/Unreal Game"
mkdir -p Tools/External
git clone https://github.com/djeada/blender-mcp-server.git \
  Tools/External/blender-mcp-server
cd Tools/External/blender-mcp-server
git checkout 7eed33edf4aca2ab0ca84a6da27321f89f68b504
uv venv --python 3.11 .venv
uv pip install --python .venv/bin/python -e .
./scripts/build_addon_zip.sh
```

The pinned commit is the version reviewed while introducing this pipeline.
Re-review release notes and security-sensitive execution code before upgrading.

### 3. Install The Blender Add-on

In Blender:

1. Open `Edit > Preferences > Add-ons`.
2. Choose `Install from Disk`.
3. Select
   `Tools/External/blender-mcp-server/dist/blender_mcp_bridge.zip`.
4. Enable **Blender MCP Bridge**.
5. Enable **Safe Mode**.
6. Disable **Allow Inline Code**.
7. Set **Approved Script Roots** to:

```text
/Users/wizard/Documents/Unreal Game/Scripts/blender;/Users/wizard/Documents/Unreal Game/ArtSource/Blender
```

8. Confirm the viewport sidebar's `MCP` panel reports that the bridge is
   listening on `127.0.0.1:9876`.

### 4. Add Blender To Project MCP Config

After the server exists, add this next to `unreal-mcp` in
`.codex/config.toml`:

```toml
[mcp_servers.blender]
command = "/Users/wizard/Documents/Unreal Game/Tools/External/blender-mcp-server/.venv/bin/blender-mcp-server"
cwd = "/Users/wizard/Documents/Unreal Game/Tools/External/blender-mcp-server"
enabled = true
required = false
startup_timeout_sec = 20
tool_timeout_sec = 180
default_tools_approval_mode = "prompt"
disabled_tools = ["blender_python_exec_async"]
```

Restart ChatGPT Codex after changing MCP configuration. The desktop app, Codex
CLI, and IDE extension share the same local MCP configuration.

### 5. Verify The Connection

With Blender open and its bridge listening:

1. Inspect the empty scene.
2. Create and delete a test cube through the structured object tools.
3. Render a 512x512 test image into `ArtSource/Blender/SmokeTest`.
4. Run the first tracked script by `script_path`, not inline code.
5. Confirm file access outside the two approved roots is rejected.

## Repository Layout

```text
ArtSource/Blender/
  README.md
  Environment/
    WaystoneShrine/
      SM_EmbermereWaystone_01.blend
      SM_EmbermereWaystone_01.fbx
      SM_EmbermereWaystone_01_preview.png
    EmberLamp/
    RoadSignpost/
    RoadGate/
    BoundaryFence/
    BoundaryStone/
    SupplyChest/
    FenwatchShelter/
    MarshReedCluster/
Scripts/blender/
  build_embermere_waystone.py
  build_embermere_ember_lamp.py
  build_embermere_road_signpost.py
  build_embermere_road_gate.py
  build_embermere_boundary_fence.py
  build_embermere_boundary_stone.py
  build_embermere_supply_chest.py
  build_embermere_fenwatch_shelter.py
  build_embermere_marsh_reed_cluster.py
Content/Art/Embermere/
  Environment/PrototypeVillage/
```

Generated backup files such as `.blend1` and `.blend2` are ignored. Intentional
source, interchange, and preview artifacts are not ignored automatically.
Review their size before committing. Use Git LFS if original binary art grows
large enough to make normal Git history unhealthy.

## Unreal Technical Contract

For the first static-prop lane:

- author in centimeters and verify import at scale `1.0`;
- use Z as up in Blender and test the FBX conversion once before standardizing;
- apply object transforms before export;
- place the render mesh pivot at ground level near the placement center;
- prefix render meshes with `SM_`;
- prefix simple collision with `UBX_`, or convex collision with `UCX_`, using
  the exact render-mesh name;
- provide at least one UV channel and inspect lightmap generation in Unreal;
- triangulate deliberately before final export;
- keep material slots few, named, and reusable;
- validate collision in the Static Mesh Editor and with the player capsule;
- place accepted assets under `/Game/Art/Embermere`, never under a vendor
  folder.

Epic's Unreal 5.8 FBX documentation says the pipeline supports materials,
custom collision, multiple UV sets, vertex colors, and LODs. It also documents
the `UBX_`, `UCP_`, `USP_`, and `UCX_` collision naming conventions and notes
that Unreal's FBX importer uses FBX 2020.2:

- https://dev.epicgames.com/documentation/en-us/unreal-engine/fbx-static-mesh-pipeline-in-unreal-engine

## Asset Acceptance Checklist

An asset is not accepted because a beauty render looks good. It must pass:

- silhouette reads at gameplay camera distance;
- dimensions match the written brief;
- pivot and origin are deliberate;
- transforms are applied;
- normals face correctly;
- no accidental non-manifold geometry;
- triangle count fits the asset class;
- UV channel exists and has no accidental zero-area islands;
- material slots are intentional;
- collision names and shapes import correctly;
- no hidden cameras, lights, reference planes, or preview geometry are exported;
- FBX imports at expected scale and orientation;
- Mac editor performance remains healthy;
- the asset does not obscure Mara, enemies, nameplates, target rings, or routes;
- the original source and generation script have clear ownership and licensing.

## First Pilot Asset

Build `SM_EmbermereWaystone_01`, a stylized mossy standing-stone shrine with a
warm ember rune. It will test:

- Embermere's original visual identity;
- multi-material static geometry;
- bevel and silhouette quality;
- explicit collision;
- centimeters, pivot, and orientation;
- render/inspect/revise loops;
- FBX import into Unreal;
- replacement of one temporary road or village marker without touching
  gameplay.

The tracked build script is `Scripts/blender/build_embermere_waystone.py`.
It has now run through the approved `script_path` flow, been revised after a
too-dark first render, and imported into:

```text
/Game/Art/Embermere/Environment/PrototypeVillage
```

Verified pilot result:

- dimensions: approximately `163.7 x 92.0 x 198.9` cm;
- render mesh: 1,340 triangles with one UV channel and no non-manifold edges;
- materials: stone, moss, and ember rune;
- transforms: applied at scale `1.0`, ground-level pivot, Z-up;
- collision: two imported `UBX_` boxes, retained by Unreal as two box elements;
- Unreal bounds after import: minimum Z `0`, maximum Z approximately `196.4`;
- saved actor: `Embermere_Waystone_Road_01` at `(-690, -25, 20)`, yaw `30`;
- replacement: removed `FabPass_Road_Stump_01`, leaving 64 third-party
  `FabPass_` actors plus one original tagged `EmbermereOriginalArt` actor;
- verification: Mac editor build succeeded, saved-map validation passed, and
  all 16 Embermere automation tests passed with zero warnings.

The first live placement exposed a useful edge case: snapping the waystone
before deleting the stump placed it on top of the outgoing prop. The actor was
corrected to the authored road elevation, and Z is now part of the validator.

The pilot survived the technical checklist. The next original family should be
small road and village props that reuse its stone, moss, and ember language:
signposts, lamps, low boundary stones, and a matching quest shrine.

## Second Asset: Ember Lamp

`SM_EmbermereEmberLamp_01` extends the waystone language into a repeatable
village/road prop: pale beveled stone, moss steps, dark iron, and a warm faceted
ember crystal. Its reviewed source is
`Scripts/blender/build_embermere_ember_lamp.py`; generated source and exchange
files live under `ArtSource/Blender/Environment/EmberLamp`.

Verified result:

- dimensions: approximately `66.3 x 66.3 x 254.0` cm with a ground pivot;
- Blender mesh: 2,184 triangles, one UV channel, no non-manifold edges, applied
  scale, and four intentional material slots;
- Unreal mesh: 2,168 post-import triangles and two retained `UBX_` box
  colliders;
- shared materials: existing waystone stone, moss, and ember materials, plus
  project-owned `M_EmberLampIron`;
- saved actors: `Embermere_EmberLamp_Mara_01` and
  `Embermere_EmberLamp_Road_01`, both tagged `EmbermereOriginalArt`;
- replacement: removed the two temporary sci-fi lamps, leaving 62 third-party
  `FabPass_` actors plus three project-owned placements;
- verification: asset thumbnail and level viewport inspected through Unreal
  MCP, Mac editor build succeeded, 19/19 automation tests passed, and the
  fresh-process map validator passed exact bounds, import provenance, authored
  collision, actor tags, and transforms.

### UE 5.8 FBX Import Lesson

Passing `FbxImportUI` to a generic `AssetImportTask` is not enough to guarantee
the classic FBX path in UE 5.8. The generic Interchange importer accepted the
lamp but persisted `bCollision=false`, so both correctly named `UBX_` meshes
were discarded. Pinning `unreal.FbxFactory()` made Unreal triangulate both
collision models and retain them as box elements.

An atomic replacement of the already-created Interchange asset kept its old
`InterchangeAssetImportData` even after the classic factory rebuilt collision.
The durable import script therefore deletes only the partial lamp mesh package,
recreates the same object path with `FbxFactory`, validates
`FbxStaticMeshImportData`, bounds, and both boxes, and only then loads or saves
the map. Future Blender assets with authored collision should follow that lane.

## Third Asset: Road Signpost

`SM_EmbermereRoadSignpost_01` turns the waystone and lamp material language into
a practical navigation prop. Its broad timber post and directional arms use
stone/moss footing, dark iron straps, and three warm ember diamonds so the
silhouette reads as a road sign before its fantasy detailing is inspected.

The reviewed build script is
`Scripts/blender/build_embermere_road_signpost.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Environment/RoadSignpost`. The Unreal import and placement
lane is `Scripts/import_embermere_road_signpost_unreal.py`.

Verified result:

- dimensions: approximately `187.5 x 58.0 x 264.0` cm with a ground pivot;
- Blender mesh: 1,828 triangles, one UV channel, no non-manifold edges, and
  applied scale;
- materials: shared waystone stone/moss and lamp iron/ember, plus the new
  project-owned `M_EmbermereTimber`;
- collision: two retained `UBX_` boxes cover only the footing and post, leaving
  the overhead sign arms non-colliding;
- saved actor: `Embermere_RoadSignpost_01` at `(20, -170, 20)`, yaw `22`,
  tagged `EmbermereOriginalArt` beside the village road;
- verification: preview, imported thumbnail, and level framing were inspected;
  exact mesh/material/import/collision/transform assertions pass in the saved
  map validator.

The import exposed another persistence edge case. A generated material can
exist in the editor object registry and render correctly during the current
session while its package is still absent from disk. The signpost importer now
explicitly saves `M_EmbermereTimber`, and validation checks both its object path
and durable asset package before accepting the map.

The road signpost completed the first small Embermere roadside trio and set the
material language for a larger traversable boundary piece.

## Fourth Asset: Road Gate

`SM_EmbermereRoadGate_01` turns the roadside family into a traversable world
boundary. Two moss-stone footings support timber posts and a broad overhead
beam, with iron bands and an ember crest carrying the waystone palette into a
clear village-to-wilderness threshold.

The reviewed build script is `Scripts/blender/build_embermere_road_gate.py`;
editable source, FBX, preview, and deterministic metrics live under
`ArtSource/Blender/Environment/RoadGate`. The Unreal import and placement lane
is `Scripts/import_embermere_road_gate_unreal.py`.

Verified result:

- dimensions: approximately `96.0 x 432.0 x 373.7` cm with a ground pivot;
- Blender and Unreal render mesh: 3,296 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: shared waystone stone/moss/ember, lamp iron, and signpost timber;
- collision: four retained `UBX_` boxes cover the two footings and posts while
  the overhead span remains non-colliding;
- saved actor: `Embermere_RoadGate_01` at `(1080, 540, 20)`, yaw `20`, tagged
  `EmbermereOriginalArt` on the road into the wilderness pocket;
- traversal: a native trace through the 250 cm center opening returns no hit,
  while a support trace hits the authored collider;
- verification: preview and route framing were inspected through MCP, the Mac
  editor build succeeded, 21/21 automation tests passed, and the saved-map
  validator passed exact bounds, materials, import provenance, collision,
  transform, and five original-art placements.

The gate now has matching low fence modules and rune-topped end stones on both
sides. The accepted threshold is ready to sit beside a future larger modular
building kit.

## Fifth Asset: Boundary Fence

`SM_EmbermereBoundaryFence_01` turns the gate into a repeatable roadside
boundary family. Two stone-and-moss supports hold crossed timber rails with
iron bands and three ember diamonds, preserving the gate silhouette at a lower
height without creating a second traversable opening.

The reviewed build script is
`Scripts/blender/build_embermere_boundary_fence.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Environment/BoundaryFence`. The Unreal import and placement
lane is `Scripts/import_embermere_boundary_fence_unreal.py`.

Verified result:

- dimensions: `78.0 x 328.0 x 180.0` cm with a ground pivot;
- Blender and Unreal render mesh: 2,632 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: the same five stone, moss, timber, iron, and ember assets used by
  the road gate;
- collision: three retained `UBX_` boxes cover both supports and the center
  crossed-rail body;
- saved actors: `Embermere_BoundaryFence_GateSouth_01` at
  `(1206.55, 192.35, 20)` and `Embermere_BoundaryFence_GateNorth_01` at
  `(953.45, 887.65, 20)`, both yaw `20` and tagged
  `EmbermereOriginalArt`;
- traversal: native traces keep three lanes through the gate clear, prove one
  gate support solid, and prove both fence centers solid;
- verification: the Blender preview and Unreal road approach were inspected,
  the saved-map validator passed exact import/material/bounds/collision/tag/
  transform assertions, and the map now contains 62 local Fab placements plus
  seven original-art placements.

The north fence reads clearly from the current road approach. The original
south-side capture exposed two vendor trees masking the second fence; their
accepted foliage-only relocation is now locked by the saved-map validator so
the collision-validated boundary geometry stays untouched.

## Sixth Asset: Boundary Stone

`SM_EmbermereBoundaryStone_01` terminates each fence with a compact rune-topped
marker. Its stepped stone and moss base, timber core, iron bands, and two ember
diamonds reuse the complete threshold material language while remaining taller
than the fence and lower than the gate crest.

The reviewed build script is
`Scripts/blender/build_embermere_boundary_stone.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Environment/BoundaryStone`. The classic-FBX import,
placement, and foliage-composition lane is
`Scripts/import_embermere_boundary_stone_unreal.py`.

Verified result:

- dimensions: `96.0 x 92.0 x 253.731` cm with a ground pivot;
- Blender and Unreal render mesh: 1,872 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: the same five stone, moss, timber, iron, and ember assets used by
  the gate and fence;
- collision: two retained `UBX_` boxes cover the stepped footing and marker
  core;
- saved actors: `Embermere_BoundaryStone_GateSouth_01` at approximately
  `(1274.951, 4.375, 20)` and `Embermere_BoundaryStone_GateNorth_01` at
  `(885.049, 1075.625, 20)`, both yaw `20` and tagged
  `EmbermereOriginalArt`;
- composition: only `FabPass_Road_Pine_05` and
  `FabPass_Wild_Tree_South_01` moved within the south foliage band, revealing
  the fence and end marker without changing the 62-actor Fab count;
- verification: the Blender preview and fresh Unreal road capture were
  inspected, fresh-process map validation passed exact import/material/bounds/
  collision/tag/transform assertions for nine original placements, and native
  traces proved both stone cores solid while all three gate lanes remained
  clear.

## Seventh Asset: Supply Chest

`SM_EmbermereSupplyChest_01` replaces a temporary vendor crate stack with a
compact village prop in the established stone, moss, timber, iron, and ember
language. A low plank body, domed half-elliptical lid, iron bands and corners,
stone feet, ember lock, and three irregular moss patches give it a readable
front without introducing a one-off material family.

The reviewed build script is
`Scripts/blender/build_embermere_supply_chest.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Environment/SupplyChest`. The explicit classic-FBX import,
replacement, placement, and validation lane is
`Scripts/import_embermere_supply_chest_unreal.py`.

Verified result:

- dimensions: `180.0 x 119.0 x 123.1` cm with a ground pivot;
- Blender and Unreal render mesh: 2,364 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: the same five timber, stone, iron, ember, and moss assets used by
  the road-boundary family;
- collision: two retained `UBX_` boxes cover the chest body and lid;
- saved actor: `Embermere_SupplyChest_Vendor_01` at
  `(-1740, -1180, 0)`, yaw `108`, tagged `EmbermereOriginalArt`;
- replacement: removes `FabPass_Village_Crates_A`; the later grounding,
  Prowler, reed, and Fenwatch passes leave 57 local Fab actors and 15
  project-owned placements in the current map;
- verification: the Blender preview, Unreal asset thumbnail, and PlayerStart
  approach were inspected; saved-map validation locks classic-FBX provenance,
  exact bounds/materials/collision/tag/transform plus at least `225` cm of
  spawn-corridor clearance. Native traces in the initialized live editor proved
  both the lid solid and the old player-height autorun line clear.

The first technically valid placement presented the chest poorly from the
normal route. Yaw was therefore treated as an acceptance property, not merely
a valid transform: `108` exposes the lock and plank face to the PlayerStart
approach. A later clean-PIE autorun probe found that the original
`(-1545, -920, 0)` position still intersected the direct spawn route. The
accepted `(-1740, -1180, 0)` transform preserves the village silhouette and
front-facing read while satisfying saved geometric clearance and live traversal
checks.

## What To Build First

Good early Blender assets:

- waystones, shrines, signposts, lamps, fences, gates, barrels, crates, chests;
- modular timber-and-stone walls, doors, windows, roof pieces, and beams;
- ruin arches, pillars, broken walls, stairs, and landmark sculptures;
- simple weapons and shields after the static environment workflow is stable.

Defer until the pipeline is mature:

- playable race bodies;
- faces and hair systems;
- rigged creatures;
- production deformation topology;
- animation sets, skin weights, cloth, and facial rigs.

Those tasks need stronger art direction, anatomy, topology, rigging, and
animation review than a render-only loop can provide.

The first deliberate exception is the Marsh Prowler vertical slice documented
in `Docs/MARSH_PROWLER_ART_BRIEF.md`. It is intentionally bounded to one
starter enemy, retains the existing character capsule and gameplay class as
authorities, and keeps the placeholder visual as fallback until model, rig,
locomotion, import persistence, and PIE behavior pass independently.

## Eighth Asset: Marsh Prowler

`SK_EmbermereMarshProwler_01` proves that the deterministic pipeline can cross
from static props into one bounded animated creature without handing gameplay
authority to the art asset.

The reviewed build, preview, and inspection scripts are:

- `Scripts/blender/build_embermere_marsh_prowler.py`;
- `Scripts/blender/render_marsh_prowler_action_previews.py`;
- `Scripts/blender/inspect_marsh_prowler_scene.py`.

The saved Blender source contains 7,464 triangles, 3,878 vertices, five
materials, 26 authored bones, two physics proxies, and six actions: Idle, Walk,
Run, Attack, Hit, and Death. Unreal imports a skeletal mesh, skeleton, physics
asset, five materials, and six animation sequences into
`/Game/Art/Embermere/Characters/Enemies/MarshProwler`.

`AEmbermereEnemyCharacter` owns six soft animation roles and routes them from
generic movement, attack, damage, death, and respawn state. Combat rules remain
asset-agnostic, the capsule remains authoritative, and the placeholder visual
remains a fallback. The Blueprint CDO and all three saved map instances are
validated independently because updating a class default does not rewrite a
placed actor's serialized component override.

## Ninth Asset: Marsh Reed Cluster

`SM_EmbermereMarshReedCluster_01` is a compact visual-only environment asset
built by `Scripts/blender/build_embermere_marsh_reed_cluster.py` and imported
by `Scripts/import_embermere_marsh_reeds_unreal.py`.

Verified result:

- dimensions: `168 x 131.04 x 187.8` cm with a ground pivot;
- 1,012 triangles, 596 vertices, one UV channel, applied scale, and no
  non-manifold geometry;
- four existing project materials, including `M_EmbermereGround` on the low
  footprint so the cluster does not read as a pale planter;
- no authored collision and four exact saved `NoCollision` placements;
- explicit mesh package saving plus saved-map bounds, material, transform, tag,
  and collision validation.

The first render exposed a bright geometric base that technically passed but
did not belong on the terrain. Reusing the ground material and lowering the
moss footprint made the reeds read as growth rather than a prop. This is the
same acceptance rule as the larger assets: metrics make the model eligible;
the normal gameplay camera decides whether it belongs.

## Tenth Asset: Fenwatch Shelter

`SM_EmbermereFenwatchShelter_01` is the first full village-scale module built
from the established roadside language. Four stone-and-moss footings carry
timber supports, iron details, a broad gabled roof, and a restrained ember
crest. The center stays open so the shelter can frame Mara without becoming a
new gameplay wall.

The reviewed build script is
`Scripts/blender/build_embermere_fenwatch_shelter.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Environment/FenwatchShelter`. The classic-FBX import and
placement lane is `Scripts/import_embermere_fenwatch_shelter_unreal.py`.

Verified result:

- dimensions: `438.0 x 296.782 x 369.5` cm with a ground pivot;
- Blender and Unreal render mesh: 4,348 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: the same five stone, moss, timber, iron, and ember assets used by
  the road-boundary family;
- collision: four retained `UBX_` boxes cover only the supports, leaving the
  open center and overhead roof non-colliding;
- saved actor: `Embermere_FenwatchShelter_Mara_01` at
  `(-1740, -700, 0)`, yaw `-64`, tagged `EmbermereOriginalArt`;
- replacement: removes `FabPass_Mara_Stone_Backdrop`,
  `FabPass_Village_Market_Cover`, `Vendor_Placeholder`, and
  `Trainer_Placeholder`, leaving 57 Fab actors and 15 original placements;
- verification: fresh-process map validation locks classic-FBX provenance,
  exact dimensions, triangle count, shared materials, four colliders, tag,
  transform, and removed labels. Initialized-world traces prove all four
  supports solid, the center clear, and the PlayerStart autorun corridor open.

The first technically valid transform at `(-2180, -1060)`, yaw `160`, passed
import, package, and collision checks but hid Mara from the gameplay camera.
Clean PIE rejected it. The accepted transform places the shelter behind Mara,
preserves her name and quest-marker read, and passed a transform-measured `Q`
autorun route with independent `W` cancellation. Architecture is accepted as
part of a composition and a route, not as an isolated valid mesh.

## Eleventh Asset: Fenwatch Keeper

`SM_EmbermereFenwatchKeeper_Mara_01` replaces Mara's stretched engine cube
without moving quest, dialogue, interaction, or reward ownership into the art
asset. The static first-pass keeper uses a moss cowl, timber-brown bodice, pale
skirt, iron and ember details, staff, and satchel to read as part of the same
Fenwatch roadside family.

The reviewed build script is
`Scripts/blender/build_embermere_fenwatch_keeper.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Characters/NPCs/FenwatchKeeper`. The classic-FBX import,
Blueprint-template reconciliation, placed-instance update, and package-saving
lane is `Scripts/import_embermere_fenwatch_keeper_unreal.py`.

Verified result:

- dimensions: `107.45 x 71.0 x 207.5` cm with a ground pivot;
- Blender and Unreal render mesh: 3,280 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: five existing stone, moss, timber, iron, and ember assets plus the
  saved project-owned `M_FenwatchKeeperSkin` material;
- collision: no simple collision and an explicit `NoCollision` component
  contract, leaving the existing quest actor and interactable component as the
  authorities;
- saved presentation: Mara remains at `(-2050, -850, 140)`, yaw `35`; her
  visual component uses local location `(0, 0, -140)`, local yaw `100`, and
  unit scale so the feet meet the support surface and the keeper faces the
  PlayerStart approach;
- verification: fresh-process map validation locks the mesh metrics, materials,
  project-owned tag, exact transforms, and no-collision state on both the saved
  `BP_QuestGiver` SCS template and placed Mara actor. The focused native test
  loads that real SCS template. Clean PIE accepted grounded feet, shelter
  composition, staff/satchel silhouette, and unobstructed name and quest marker.

Two persistence details mattered. A Blueprint's inherited component template is
not reliably exposed by constructing or querying a transient native actor;
editor automation must use `SubobjectDataSubsystem`, and native automation must
inspect the saved SCS node template. The placed map actor also retained the old
cube's local offset, scale, facing, and collision overrides after the mesh swap.
Replacing art therefore required reconciling and saving both the class template
and the serialized instance before fresh-process validation could accept it.

## Twelfth Asset: Fenwatch Quartermaster

`SM_EmbermereFenwatchQuartermaster_01` establishes the first reusable
standalone service-NPC presentation lane. The static figure uses a moss cap and
shoulder wrap, timber clothing, pale apron, beard, ledger, pouches, keys, iron,
and ember details to read as a stocky quartermaster beside the supply chest
without owning vendor behavior.

The reviewed build script is
`Scripts/blender/build_embermere_fenwatch_quartermaster.py`; editable source,
FBX, preview, and deterministic metrics live under
`ArtSource/Blender/Characters/NPCs/FenwatchQuartermaster`. The classic-FBX
import, material creation, wrapper placement, and package-saving lane is
`Scripts/import_embermere_fenwatch_quartermaster_unreal.py`.

Verified result:

- dimensions: `120.842 x 93.0 x 217.0` cm with a ground pivot;
- Blender and Unreal render mesh: 3,632 triangles, one UV channel, no
  non-manifold edges, and applied scale;
- materials: five existing stone, moss, timber, iron, and ember assets plus the
  saved project-owned `M_FenwatchQuartermasterSkin` material;
- collision: no simple collision and explicit `NoCollision`, no overlap, and
  no navigation contribution on both wrapper visual lanes;
- saved presentation: `Embermere_FenwatchQuartermaster_Vendor_01` at
  `(-1530, -1190, 0)`, yaw `100`, unit scale, static-preferred mode, and the
  `EmbermereOriginalArt` tag;
- verification: fresh-process validation locks mesh metrics, materials,
  provenance, wrapper class, transform, lane, tag, and absence of interaction;
  native tests prove both static and skeletal resolution through one shared
  transform; clean PIE accepted grounded chest-side composition, Mara
  readability, and the open PlayerStart route.

The companion contract is documented in
`Docs/NPC_PRESENTATION_CONTRACT.md`. The later
`Embermere_FenwatchQuartermaster_Service_01` and
`DA_FenwatchQuartermasterStock` add interaction, stock, prices, and
transactions as a separate art-free gameplay lane documented in
`Docs/VENDOR_SERVICE_CONTRACT.md`; none of those responsibilities became
properties of this Blender asset or its presentation actor.

### Rigged Idle Upgrade

The quartermaster later became Embermere's second persisted production NPC on
the shared skeletal lane. The reviewed rigged builder is
`Scripts/blender/build_embermere_fenwatch_quartermaster_rigged.py`; editable
source, mesh/animation FBXs, preview, and metrics live under
`ArtSource/Blender/Characters/NPCs/FenwatchQuartermaster/Rigged`.

The rig retains the exact static source's grounded
`120.842 x 93.0 x 217.0` cm bounds, 3,632 triangles, six materials, clean UVs,
and manifold topology. It adds nine authored semantic bones, complete rigid
one-bone weights, and a restrained 121-frame Idle at 30 fps for an exact
4.0-second clip. The merchant's feet stay planted while the spine, head,
ledger hand, and free hand move subtly. Classic FBX adds one imported Armature
root in Unreal, so the accepted reference skeleton has ten bones while
retaining all nine authored names.

`Scripts/import_embermere_fenwatch_quartermaster_rigged_unreal.py` uses classic
`FbxFactory`, assigns exact skeletal-material structs, saves the SkeletalMesh,
Skeleton, and Idle, and switches only the art wrapper to skeletal-preferred
mode. The original `SM_EmbermereFenwatchQuartermaster_01` remains the exact
static fallback, and the separate vendor service is not modified.

This pass tightened the skeletal import lifecycle. UE 5.8 can route replacement
of an existing skeletal package through Interchange even when the requested
factory is classic FBX. Routine reruns therefore load and validate eligible
classic packages without importing. An intentional source rebuild uses
`Scripts/cleanup_embermere_fenwatch_quartermaster_rig_unreal.py` in one clean
Unreal process, creates the packages with classic FBX in a second, then runs
`Scripts/validate_fenwatch_quartermaster_rig_unreal.py` in a third. Cleanup,
creation, and authoritative validation are deliberately separate lifecycles.

Fresh-process validation and native automation lock provenance, bounds,
materials, bone hierarchy, Skeleton compatibility, 4.0-second duration,
wrapper lane, static fallback, collision, transform, and service separation.
Clean PIE then proved `playing=true` while the clock advanced from `0.853735`
to `2.195707` seconds. Package eligibility and live motion remain independent
acceptance gates.

## Thirteenth Asset: Fenwatch Armsmaster

`SM_EmbermereFenwatchArmsmaster_01` gives the first trainer service a distinct
project-owned identity without putting offerings or progression into the
model. A moss hood, timber tunic, pale guard tabard, ember crest, round shield,
and ember-tipped staff keep the figure in the Fenwatch family while separating
its silhouette from Mara and the quartermaster.

The reviewed build script is
`Scripts/blender/build_embermere_fenwatch_armsmaster.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Characters/NPCs/FenwatchArmsmaster`. Classic-FBX import is
handled by `Scripts/import_embermere_fenwatch_armsmaster_unreal.py`; data and
service placement use `Scripts/configure_fenwatch_trainer_unreal.py`.

Verified result:

- dimensions: `154.5 x 87.0 x 228.0` cm with a ground pivot;
- Blender source: 2,824 triangles, one UV channel, no non-manifold edges, and
  applied unit scale;
- Unreal package: 2,800 triangles after classic FBX removes 24 degenerate
  triangles during import;
- materials: five existing stone, moss, timber, iron, and ember assets plus
  the saved project-owned `M_FenwatchArmsmasterSkin` material;
- collision: no simple collision and explicit `NoCollision` on the art-only
  wrapper;
- saved presentation: `Embermere_FenwatchArmsmaster_Trainer_01` at
  `(-1320, -920, 0)`, yaw `100`, unit scale, static mode, and the
  `EmbermereOriginalArt` tag;
- saved service: a separate co-located
  `Embermere_FenwatchArmsmaster_Service_01` owns interaction and the soft
  reference to `DA_FenwatchArmsmasterOfferings` without owning art.

The first preview clipped the staff's ember tip even though the model metrics
passed. The authored camera was corrected and the preview rerendered before the
asset was accepted. Import then exposed a second useful distinction: Blender's
source triangle count and Unreal's post-import triangle count describe
different eligible artifacts. Recording and testing both avoids either tool
silently becoming the only truth.

### Rigged Idle Upgrade

The accepted static armsmaster later became the first persisted production NPC
on `AEmbermereNpcPresentationActor`'s skeletal lane. The rigged build reuses the
reviewed source geometry through
`Scripts/blender/build_embermere_fenwatch_armsmaster_rigged.py` and stores its
editable source, FBXs, preview, and metrics under
`ArtSource/Blender/Characters/NPCs/FenwatchArmsmaster/Rigged`.

Verified rig contract:

- the same `154.5 x 87.0 x 228.0` cm grounded bounds and 2,824 source
  triangles;
- six existing project-owned materials and one UV channel;
- nine authored bones: `root`, `pelvis`, `spine`, `neck`, `head`, both upper
  arms, and both forearms;
- rigid one-bone weights for every vertex, with no unweighted or multiply
  weighted vertices;
- a restrained 97-frame Idle at 30 fps, producing an exact 3.2-second clip;
- classic FBX's additional Armature root in Unreal, giving the accepted
  reference skeleton ten bones while preserving all nine authored names.

`Scripts/import_embermere_fenwatch_armsmaster_rigged_unreal.py` imports through
the classic skeletal `FbxFactory`, explicitly assigns the six material structs,
saves the SkeletalMesh, Skeleton, and Idle packages, and moves the existing
wrapper to the skeletal-preferred lane without changing its transform or the
co-located service. The wrapper retains
`SM_EmbermereFenwatchArmsmaster_01` as its reversible static fallback.

Two import details became durable rules. `USkeletalMesh.set_material()` is not
available to Unreal Python, so material interfaces must be assigned through the
skeletal-material struct array and written back. Also, deleting a SkeletalMesh
and Skeleton before reimporting them in the same commandlet can leave stale
UObject references and produce a mesh with no valid Skeleton. The first repair
replaced packages in place, reused the valid saved Skeleton, and rejected an
existing skeletal mesh whose Skeleton was missing. The later quartermaster
pass refined this further: normal reruns validate and preserve eligible classic
packages, while intentional rebuilds use separate cleanup, fresh creation, and
fresh validation processes. Replacing an existing package is not a reliable
way to force classic-FBX provenance in UE 5.8.

Fresh-process validation locks asset paths, bounds, material order, Skeleton,
Idle duration, wrapper lane, static fallback, collision, transform, and trainer
ownership. Native automation locks the imported-root hierarchy as well as the
nine authored bone names. Finally, clean PIE proved the active animation clock
advancing from `0.193888` to `1.670905` seconds while remaining `playing=true`
and `NoCollision`. Package eligibility and live motion are separate gates.

The full art/service/progression boundary is documented in
`Docs/TRAINER_SERVICE_CONTRACT.md`.

## Fourteenth Asset: Fenwatch Practice Dummy

`SM_EmbermereFenwatchPracticeDummy_01` gives the armsmaster a readable
training-yard focal point without putting interaction or progression into world
art. Its broad timber target, ember bullseye, stone-and-moss footing, iron
bands, and outstretched sparring arms extend the existing Fenwatch material and
silhouette language.

The reviewed build script is
`Scripts/blender/build_embermere_fenwatch_practice_dummy.py`; editable source,
FBX, preview, and deterministic metrics live under
`ArtSource/Blender/Environment/FenwatchPracticeDummy`. Classic-FBX import and
idempotent map placement are handled by
`Scripts/import_embermere_fenwatch_practice_dummy_unreal.py`.

Verified result:

- dimensions: `252.0 x 100.879 x 245.0` cm with a ground pivot;
- topology: 2,572 triangles, one UV channel, zero non-manifold edges, and
  applied unit scale;
- materials: the existing `M_Waystone`, `M_WaystoneMoss`,
  `M_EmbermereTimber`, `M_EmberLampIron`, and `M_WaystoneEmber` assets;
- collision: exactly two authored UBX boxes make the base and torso/core solid
  while both outstretched arms remain visual-only and traversable;
- saved placement: `Embermere_FenwatchPracticeDummy_TrainingYard_01` at
  `(-1120, -1120, 0)`, yaw `45`, unit scale, `BlockAll`, and the
  `EmbermereOriginalArt` tag;
- replacement: the generic `FabPass_Village_Crate_C` is removed, leaving 56
  Fab placements plus 19 project-owned placements.

The live Blender bridge correctly refused the build script because the script
resets the active scene. Rather than weaken Safe Mode or disturb the artist's
open file, the same reviewed script ran in a factory-clean headless Blender
process. That refusal is part of the pipeline working: live-scene safety and
deterministic asset generation use different execution contexts.

The first Unreal placement passed its numeric contract but aimed the target
away from the armsmaster. Human viewport review rejected the composition, and
the deterministic import/placement script was corrected and rerun instead of
leaving an untracked in-memory transform. Fresh-process asset and map
validators then proved exact metrics, materials, package persistence, tag, and
transform; initialized-world native traces proved the base/core solid and both
arms clear.

Lesson: a safe tool refusal, deterministic technical eligibility, and human
composition acceptance are complementary gates. Passing one does not waive the
others.

## Fifteenth Asset: Rigged Fenwatch Keeper

The 2026-08-13 pass upgraded Mara's accepted keeper silhouette without moving
her quest behavior into art. `build_embermere_fenwatch_keeper.py` now exposes
the exact reviewed parts behind an import-only guard, and
`build_embermere_fenwatch_keeper_rigged.py` reuses them in a deterministic
skeletal build.

The accepted Blender contract is:

- `107.45 x 71.0 x 207.5` cm grounded dimensions;
- 3,280 triangles, six existing project-owned materials, one UV channel, and
  no non-manifold edges;
- nine authored bones with applied armature scale, complete rigid one-bone
  weights, and no unweighted or multiply weighted vertices;
- one 109-frame action at 30 fps, producing an exact 3.6-second restrained Idle
  with planted feet and staff.

The classic-FBX lane explicitly saves the SkeletalMesh, imported Skeleton, and
AnimationSequence. Unreal retains one importer-owned Armature root in addition
to the nine authored bones. The saved
`Embermere_FenwatchKeeper_Mara_Presentation_01` wrapper prefers those assets,
remains `NoCollision`, and keeps the static keeper as a reversible fallback.

Mara exposed an ownership case the service NPCs did not. The existing
`BP_QuestGiver` could not simply be replaced because its interactable component
already owns the marker, display name, dialogue, quest, and reward flow. The
accepted map therefore keeps that gameplay actor unchanged, clears only its
dormant SCS render mesh, and composes the former local art transform into a
colocated wrapper. Fresh validation locks both actors and rejects quest or
interaction authority on the wrapper.

The no-hot-reload build and both headless and fresh-editor MCP suites passed all
51 tests. A clean PIE review kept Mara grounded beneath the shelter with her
marker and name clear, while a fresh-module two-sample probe advanced the Idle
from `0.333814` to `1.525603` seconds. On 2026-08-14, a real `F` press accepted
the Blueprint-owned quest, real Prowler combat advanced it to `3/3`, and the
same original interactable completed the return for XP, copper, and the Recruit
Pack. The rigged wrapper remained presentation-only throughout.

## Sixteenth Asset: Fenwatch Vendor Stall

The quartermaster had production character art and a real vendor service, but
the remaining generic fence behind him still read as borrowed scenery. The
2026-08-14 pass replaced it with a deterministic project-owned stall that uses
the same stone, moss, timber, iron, and ember language as the rest of Fenwatch.

The accepted Blender contract is:

- `394 x 242 x 306` cm grounded dimensions;
- 5,476 triangles, five existing project-owned materials, one UV channel, no
  non-manifold edges, and applied unit scale;
- four authored support collision boxes plus one counter collision box;
- an open service approach and side bypass rather than a closed building shell.

The reviewed script ran in a factory-clean Blender process with Safe Mode left
enabled. Classic `FbxFactory` import, explicit package saving, exact shared-
material reassignment, and fresh-package validation made the asset technically
eligible before it entered the map.

World validation caught the detail the isolated preview could not: classic FBX
mirrored Blender local Y, leaving a numerically valid first placement with its
counter facing away from the quartermaster. Native collision traces exposed
the backwards counter. The deterministic placement was corrected to yaw `180`,
saved, and revalidated. Four supports and the counter now block; the customer
approach and east bypass remain clear.

The stall replaces `FabPass_Village_Fence_01` at
`(-1530, -1430, 0)`. Focused PIE opened Fenwatch Supplies through normal `F`
interaction at the unchanged art-free vendor service, proving the architecture
did not acquire commerce authority. The accepted map now contains 55 Fab
actors and 20 original-art placements.

Lesson: topology and transform validation can prove that a model exists where
the recipe says it should. Directional collision and normal-route play prove
that its authored front actually belongs there.

## Seventeenth Asset: Fenwatch Cottage

The service cluster had readable shelters and stalls, but Fenwatch still did
not look inhabited. `SM_EmbermereFenwatchCottage_01` is the first closed
project-owned dwelling: a compact stone-and-timber home with a moss roof,
chimney, closed door, glowing windows, and ember crest. It reuses the exact
road-family material set so the new silhouette extends the village language
instead of starting a second one.

The reviewed source is
`Scripts/blender/build_embermere_fenwatch_cottage.py`; editable source, FBX,
preview, and deterministic metrics live under
`ArtSource/Blender/Environment/FenwatchCottage`. The accepted Blender contract
is:

- grounded `580 x 422 x 503` cm bounds and applied unit scale;
- 6,616 triangles, one UV channel, and zero non-manifold edges;
- the existing stone, moss, timber, iron, and ember material family;
- two authored UBX boxes for the closed body and doorstep;
- visual-only roof, chimney, trim, windows, and crest geometry.

The first preview clipped the upper roof even though the model metrics were
valid. Raising the deterministic camera's `clip_end` to `5000` made preview
framing part of the repeatable build rather than a one-off Blender tweak.
Original-resolution inspection then caught and fixed visible door-jamb gaps
before Unreal import.

`Scripts/import_embermere_fenwatch_cottage_unreal.py` uses classic
`FbxFactory`, explicitly saves the package, replaces only
`FabPass_Village_Fence_02`, and places
`Embermere_FenwatchCottage_West_01` at `(-2480, -260, 0)`, yaw `38`.
Fresh-package validation locks provenance, bounds, topology, materials,
collision count, tag, transform, `730.1` cm route clearance, and `730.1` cm
separation from Mara. Initialized-world traces independently prove the house
body and step solid, decorative roof/chimney clear, the direct
PlayerStart-to-Mara lane open, and a west-side bypass available.

Normal-route PIE kept Mara's shelter, marker, service cluster, and contextual
quest presentation readable. Physical `F` still reached the original
Blueprint-owned dialogue and accepted the quest. The saved map now contains
54 Fab actors and 21 original-art placements.

Lesson: a building is accepted at three scales. Its isolated preview must show
the whole authored object, its collision must match how the player uses it,
and its world placement must improve settlement composition without consuming
the routes or authority around it.

## Fenwatch Training Workshop

The cottage proved Embermere could author a closed dwelling. The next bounded
architecture pass tested the opposite spatial contract: an open-front module
that should frame gameplay without enclosing it. The reviewed source is
`Scripts/blender/build_embermere_fenwatch_training_workshop.py`; editable
source, FBX, preview, and metrics live under
`ArtSource/Blender/Environment/FenwatchTrainingWorkshop`.

The accepted Blender contract is:

- grounded `460 x 270.403 x 369` cm bounds and applied unit scale;
- 5,624 triangles, one UV channel, and zero non-manifold edges;
- the shared stone, moss, timber, iron, and ember material family;
- four UBX boxes for the two front posts, rear wall, and workbench;
- visual-only roof, trim, ember crest, weapon silhouettes, and small tools.

`Scripts/import_embermere_fenwatch_training_workshop_unreal.py` uses classic
`FbxFactory`, saves the mesh and level explicitly, replaces only
`FabPass_Village_Fence_03`, and places
`Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
`(-690, -1030, 0)`, yaw `-100`. Fresh validation locks exact classic-FBX
provenance, materials, bounds, triangles, four colliders, project-owned tag,
transform, 439.3 cm dummy spacing, 639.5 cm armsmaster spacing, and 401.6 cm
road-pine spacing.

Initialized-world traces prove both posts, the rear wall, and the workbench
solid while leaving the open-front center, player-height bay,
armsmaster-to-dummy approach, roof/tools, and road-side east bypass clear. The
first bypass assertion began exactly inside the existing road lamp and failed.
The world placement was valid; the test endpoint was not. Moving the trace
start into the actual bypass converted the intent into a trustworthy contract
without moving accepted art to satisfy a false negative.

Clean PIE accepted the workshop and practice dummy as one readable training
yard. The map now contains 53 Fab actors and 22 original-art placements.

Lesson: open architecture needs positive and negative collision evidence. It
must prove the surfaces the player expects are solid, the space the player
expects to use is clear, and every route assertion starts outside unrelated
collision.

## Fenwatch Notice Board

`SM_EmbermereFenwatchNoticeBoard_01` adds a compact civic roadside silhouette
without turning architecture into quest logic. The stone-footed timber frame,
moss roof, iron trim, layered notice cards, pins, and ember crest reuse the
existing Fenwatch language while remaining readable as ordinary world art.

The reviewed source is
`Scripts/blender/build_embermere_fenwatch_notice_board.py`; editable source,
FBX, preview, and metrics live under
`ArtSource/Blender/Environment/FenwatchNoticeBoard`. The accepted Blender
contract is:

- grounded `286 x 93.927 x 277` cm bounds with applied unit scale;
- 3,684 triangles, one UV channel, and zero non-manifold edges;
- five existing project-owned stone, moss, timber, iron, and ember materials;
- three UBX boxes for the two supports and central panel;
- visual-only roof, notices, pins, trim, and crest geometry.

`Scripts/import_embermere_fenwatch_notice_board_unreal.py` uses classic
`FbxFactory`, reassigns the exact shared materials, explicitly saves the static
mesh and level, and places `Embermere_FenwatchNoticeBoard_Road_01` at
`(-1560, -260, 0)`, yaw `-35`. The placement is tagged as project-owned art and
has no quest, marker, dialogue, interaction, reward, vendor, trainer, or
persistence authority.

Fresh package validation locks provenance, dimensions, topology, materials,
three colliders, tag, and transform. Initialized-world traces prove both
supports and the panel solid, every decorative layer clear, and the
PlayerStart-to-Mara, village-to-road, quartermaster, and armsmaster routes
open. Normal-route PIE accepted the grounded road-side composition. The map
now contains 53 Fab actors and 23 original-art placements.

Lesson: a civic prop can imply a larger settlement without acquiring the
systems its silhouette suggests. Let art say "notice board" while a future
service, if one is ever needed, owns interaction and content separately.

## Fenwatch Communal Well

`SM_EmbermereFenwatchCommunalWell_01` gives the southern village commons a
recognizable civic anchor without turning its silhouette into gameplay
authority. The open twelve-sided stone shaft, moss curb, timber uprights, pale
roof, iron axle and crank, hanging rope and bucket, and ember crest reuse the
established Fenwatch material language.

The reviewed source is
`Scripts/blender/build_embermere_fenwatch_communal_well.py`; editable source,
FBX, preview, and metrics live under
`ArtSource/Blender/Environment/FenwatchCommunalWell`. A factory-clean headless
Blender process produced the accepted contract:

- grounded `348 x 220 x 322` cm bounds with applied unit scale;
- 6,760 triangles, one UV channel, and zero non-manifold edges;
- five existing project-owned stone, moss, timber, iron, and ember materials;
- six UBX boxes for the four curb sections and two timber uprights;
- visual-only roof, axle, rope, bucket, crank, trim, and ember crest.

`Scripts/import_embermere_fenwatch_communal_well_unreal.py` uses classic
`FbxFactory`, reassigns the exact shared materials, rejects any collider-count
drift, explicitly saves both mesh and level, and places
`Embermere_FenwatchCommunalWell_SouthCommons_01` at
`(-950, -1600, 0)`, yaw `-135`. The saved actor is tagged project-owned art and
owns no quest, marker, dialogue, interaction, recovery, reward, vendor,
trainer, or persistence behavior.

The focused package validator locks import provenance, dimensions, triangle
count, materials, six colliders, transform, tag, collision profile, and spacing
from Mara, PlayerStart, both service NPCs, the practice dummy, workshop, and
vendor stall. Initialized-world traces prove each curb/support solid while
leaving the shaft, roof, axle, rope, bucket, crank, trim, and crest clear. They
also protect PlayerStart-to-Mara, village-to-road, quartermaster, vendor east
bypass, armsmaster, and workshop circulation. Clean PIE accepted the grounded
south-commons composition and retained Q movement plus W autorun cancellation.
The map now contains 53 Fab actors and 24 original-art placements.

The later recovery slice preserved this boundary: the colocated
`Embermere_FenwatchCommunalWell_RestService_01` is a native art-free actor with
its own data, interaction, channel, cooldown, and atomic Health/Mana contract.
The saved static well gained no components or gameplay tags. Replacing the FBX
or moving to another well mesh therefore cannot change recovery rules, and the
service actor does not increase the original-art count.

The later presentation slice preserves it again. The colocated
`Embermere_FenwatchCommunalWell_RestPresentation_01` observes immutable rest
outcomes and creates only transient cyan channel and mint completion geometry.
It has no mesh package, interaction, collision, recovery, or persistence
authority and does not enter the 24-piece original-art count. Art, service, and
observer can each be replaced independently.

Lesson: choose civic architecture from measured world space, not an apparently
empty screenshot. Lock the neighbors and routes first, then let the asset earn
its place through both positive collision and negative traversal evidence.

## Fenwatch Journal UI Emblem

The September 4 Astra evaluation exercised a small UI-art lane, not another
world-mesh import. `Scripts/blender/build_fenwatch_journal_emblem.py` creates
a closed moss journal, brass edging, iron clasp, pale pages, and ember crest
in a factory-clean headless process. It exports editable `.blend`, FBX,
metrics, and a transparent 512x512 render under
`ArtSource/Blender/UI/FenwatchJournal`.

The source has 2,580 triangles, five material roles, one UV channel, applied
scale, and no non-manifold edges. The reviewed live-bridge
`inspect_fenwatch_journal_emblem.py` checks alpha and framing without replacing
the scene: occupied bounds `(45,58)-(466,461)`, 166,690 transparent pixels,
and the original three scene objects retained. Safe Mode stays enabled.

The MCP script namespace does not supply `__file__`; pass `project_root`
explicitly using the established arguments contract. Inspect nested error
fields and stdout even when the outer response says `isError: false`.

Unreal imports the render through `TextureFactory`, explicitly saves
`/Game/UI/Icons/T_FenwatchJournalEmblem`, and validates 512x512, UI group,
editor-icon compression, sRGB, and no mips. The optional 48x48 image decorates
the native quest-update observer. The FBX was not imported into Unreal and
does not establish a collision or animation contract. No map placement or
original-world-art count changes.

## Sources

- Article/post: https://x.com/explosss1ve/status/2075654835597164769
- Selected MCP bridge: https://github.com/djeada/blender-mcp-server
- Popular raw-code bridge and security report:
  https://github.com/ahujasid/blender-mcp and
  https://github.com/ahujasid/blender-mcp/issues/201
- Alternative production-shaped guided server:
  https://github.com/PatrykIti/blender-ai-mcp
- Codex MCP configuration:
  https://learn.chatgpt.com/docs/extend/mcp.md
- Unreal FBX static mesh pipeline:
  https://dev.epicgames.com/documentation/en-us/unreal-engine/fbx-static-mesh-pipeline-in-unreal-engine
