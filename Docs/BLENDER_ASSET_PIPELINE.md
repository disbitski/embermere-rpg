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
Scripts/blender/
  build_embermere_waystone.py
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
