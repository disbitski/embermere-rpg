# September 17 Daily Build

## Original Fenwatch Firewood Rack

Today's bounded world-art milestone adds a project-owned firewood rack beside
the west cottage. A factory-clean Blender MCP process ran the reviewed
`Scripts/blender/build_embermere_fenwatch_firewood_rack.py` source and produced
editable `.blend`, classic FBX, preview, and deterministic metrics under
`ArtSource/Blender/Environment/FenwatchFirewoodRack/`.

The accepted `SM_EmbermereFenwatchFirewoodRack_01` contract is:

- grounded `392.937 x 157.793 x 238` cm bounds and applied unit scale;
- 3,644 triangles, one UV channel, and zero non-manifold edges;
- five shared Fenwatch stone, moss, timber, iron, and ember materials;
- four authored UBX boxes for the broad log stack, two structural sides, and
  chopping block;
- visual-only individual logs, moss roof, axe, trim, and ember crest.

Classic `FbxFactory` import explicitly saved the mesh and map, then placed
`Embermere_FenwatchFirewoodRack_CottageWest_01` at `(-3050, -120, 0)`, yaw
`38`. It remains a presentation-only `StaticMeshActor` with no quest,
interaction, service, reward, vendor, trainer, recovery, or persistence
authority. Normal-route and opposite-side viewport inspection accepted the
grounded cottage-edge composition, readable logs/chopping block/axe, separation
from the provision handcart, and open work lane.

The focused package validator locks import provenance, geometry, topology,
materials, collider count, transform, project-owned tag, spacing, and ownership.
The exact native non-complex trace validator proves all four purposeful surfaces
solid; keeps roof, individual detail, axe, trim, and crest out of authored
collision; and protects PlayerStart-to-Mara, west-cottage bypass,
cottage-to-handcart, and village-to-road routes. The current map baseline is
**53 grounded Fab actors plus 26 original-art placements**.

## Verification

The editor and MCP listener were closed before isolated work.

- The full `-NoHotReloadFromIDE` build was genuinely rerun after the user's
  Xcode license acceptance. UnrealBuildTool reached platform validation but
  stopped before C++ compilation: installed Xcode/SDK `27.0` remains above
  UE 5.8's declared `26.9` maximum. License acceptance is no longer the gate;
  a compatible side-by-side Xcode is still required for a fresh module build.
- The exact firewood-rack native collision/route script emitted its success
  marker with no `LogPython: Error`.
- All **eight initialized-world collision/route suites** passed sequentially:
  road boundary, vendor stall, cottage, training workshop, notice board,
  communal well, handcart, and firewood rack. Every script emitted its exact
  success marker with no Python errors.
- All **25 fresh package validators** passed sequentially, including the new
  rack and exact 53-Fab/26-original full-zone contract, with no Python errors.
  Evidence: `/tmp/embermere-sep17-final-packages.log`.
- All **100/100 isolated Embermere automation tests** passed with zero
  failures, warnings, skipped, not-run, or in-process records. Evidence:
  `/tmp/embermere-sep17-final-tests/index.json` and adjacent log.
- Blender source validation reports the exact accepted dimensions, topology,
  UV, material, and four-collider contract.
- A clean relaunched editor loaded `/Game/Maps/L_Embermere_Prototype`, forced
  fresh discovery of exactly 100 Embermere tests, started and stopped clean PIE,
  and reported both the map and rack packages saved.

Unreal MCP's generic `trace_world` can report complex visible-geometry hits.
That is useful for world probing but is not interchangeable with the project's
`trace_complex=False` authored-collision assertion. Today retained both layers:
MCP proved the intended solids and protected routes, while the exact native
validator proved decorative geometry stays out of simple collision.

## Preservation And Next Start

No Save/Load action occurred. Existing Config, keeper-material, and Field Notes
working changes remain excluded from this milestone. The protected save and
keeper package hashes must remain unchanged at commit time.

Next: if a compatible side-by-side Xcode becomes available, select its
`Contents/Developer` through process-local `DEVELOPER_DIR`, prove the real
no-hot-reload build, then review and compile the pending Ledger focus
regressions before production C++ repair. If compiler compatibility remains
blocked, continue one bounded MCP-backed gameplay or original-art milestone
without weakening current acceptance gates or repeatedly treating the known
SDK mismatch as new work.
