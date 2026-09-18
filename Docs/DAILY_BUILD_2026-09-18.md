# September 18 Daily Build

## Covered-Porch Fenwatch North Cottage

Today's bounded milestone fills a real composition gap on Fenwatch's open
northwest edge instead of crowding the service core. A factory-clean Blender
MCP process ran the reviewed
`Scripts/blender/build_embermere_fenwatch_north_cottage.py` source and produced
editable `.blend`, classic FBX, preview, and deterministic metrics under
`ArtSource/Blender/Environment/FenwatchNorthCottage/`.

The accepted `SM_EmbermereFenwatchNorthCottage_01` contract is:

- grounded `550 x 497.403 x 472` cm bounds and applied unit scale;
- 7,132 triangles, one UV channel, and zero non-manifold edges;
- five shared Fenwatch stone, moss, timber, iron, and ember materials;
- six authored UBX boxes for the closed body, porch deck, two steps, and two
  structural posts;
- visual-only roof, chimney, hanging herbs, trim, windows, braces, and ember
  crest.

Classic `FbxFactory` import explicitly saved the mesh and map, then placed
`Embermere_FenwatchCottage_North_01` at `(-2500, 680, 0)`, yaw `28`. It removes
no Fab actor and remains a presentation-only `StaticMeshActor` with no quest,
interaction, service, recovery, reward, vendor, trainer, or persistence
authority. Normal-route, porch, rear, and cottage-pair viewport captures
accepted its grounded silhouette, readable herbs and entry, distinction from
the west cottage, and open lane.

The focused package validator locks classic import provenance, exact geometry,
materials, collision count, transform, tag, spacing, and ownership. It measured
1,594.8 cm PlayerStart/Mara-route clearance, 1,594.8 cm Mara separation, 940.2
cm between cottage centers, and 1,688.7 cm from the handcart. The exact native
non-complex trace validator proves all six purposeful surfaces solid, excludes
the roof from authored collision, and preserves the PlayerStart-to-Mara,
cottage-pair, and village-road lanes. The current map baseline is **53 grounded
Fab actors plus 27 original-art placements**.

## Verification

The editor and MCP listener were closed before isolated work.

- The only installed Xcode remains `/Applications/Xcode.app`, exposing Xcode
  and SDK `27.0`. UE 5.8 declares `26.9` as its maximum, so no compatible
  side-by-side toolchain is present and the known-invalid C++ build was not
  replayed.
- The import and focused saved-package validator emitted their exact success
  markers with no `LogPython: Error`.
- All **nine initialized-world collision/route suites** passed sequentially:
  road boundary, vendor stall, west cottage, training workshop, notice board,
  communal well, handcart, firewood rack, and north cottage. Every script
  emitted its exact success marker with no Python errors.
- All **26 fresh package validators** passed sequentially, including the new
  cottage and exact 53-Fab/27-original full-zone contract, with no Python
  errors. Evidence: `/tmp/embermere-sep18-north-cottage-packages.log`.
- All **100/100 isolated Embermere automation tests** passed with zero
  failures, warnings, skipped, not-run, or in-process records. Evidence:
  `/tmp/embermere-sep18-north-cottage-tests/index.json` and adjacent log.
- Blender source validation reports the exact accepted dimensions, topology,
  UV, material, and six-collider contract.
- A clean relaunched editor loaded `/Game/Maps/L_Embermere_Prototype`, forced
  fresh discovery of exactly 100 tests, started and stopped clean PIE, and
  reported both the map and north-cottage packages saved.

No Save/Load action occurred. The protected save and keeper-material hashes
remain unchanged. Existing Config, keeper-material, and Field Notes working
changes remain excluded from this milestone. Desktop control stayed disabled;
all visual work used Blender MCP or Unreal viewport-only capture.

## Next Start

Check once for a compatible side-by-side Xcode. If one exists, use only its
`Contents/Developer` through process-local `DEVELOPER_DIR`, prove the real
`-NoHotReloadFromIDE` build, then review and compile the pending Ledger focus
regressions before production repair. If compatibility remains blocked, choose
one different bounded MCP-backed milestone from TODO and fresh viewport
evidence rather than repeating readiness probes.
