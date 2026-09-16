# September 16 Daily Build

## Original Fenwatch Provision Handcart

Today's resumed run delivered a real bounded world-art milestone rather than
stopping at toolchain readiness. A factory-clean Blender MCP build created the
project-owned `SM_EmbermereFenwatchHandcart_01` family under
`ArtSource/Blender/Environment/FenwatchHandcart/`, including reviewed `.blend`,
FBX, preview, and deterministic metrics. The accepted source contract is:

- grounded `430.578 x 178 x 194.5` cm bounds and applied unit scale;
- 5,436 triangles, one UV channel, and zero non-manifold edges;
- the existing timber, iron, stone, moss, and ember Fenwatch materials;
- four UBX boxes for the loading bed, axle/wheel zone, and two grounded rests;
- visual-only handles, grips, cargo, wheel spokes/rims, and rear ember crest.

`Scripts/import_embermere_fenwatch_handcart_unreal.py` used classic
`FbxFactory`, explicitly saved the mesh and map, and placed
`Embermere_FenwatchHandcart_CottageWest_01` at `(-3040, -920, 0)`, yaw `38`.
The cart is static presentation art with no quest, interaction, service,
reward, vendor, trainer, or persistence authority. It reads as a provision cart
beside the west cottage while leaving the village center and service spaces
open. The full-zone baseline advances from 24 to **25 original-art placements**
while retaining 53 grounded upright Fab actors.

Focused saved-package validation locks classic-FBX provenance, exact metrics,
five shared materials, four authored colliders, transform, tag, and art-only
ownership. Initialized-world traces prove the bed, axle zone, and rests solid;
exclude handles, supplies, wheel detail, and crest from collision; and retain
clear PlayerStart-to-Mara, west-cottage-bypass, and village-to-road routes.

## Verification

The real editor was stopped through dedicated Unreal MCP and port 8123 was
confirmed down before isolated work.

- The requested `-NoHotReloadFromIDE` UBT command was genuinely rerun. It exits
  6 before C++ compilation because Xcode exposes SDK 27.0 while installed UE
  5.8 accepts 15.2 through 26.9 and prefers 26.1.1. License acceptance and the
  Metal component are no longer blockers; SDK compatibility is.
- All **100/100 isolated automation tests** passed, with zero failures,
  warnings, skipped, not-run, or in-process records. Evidence:
  `/tmp/embermere-sep16-handcart-tests/index.json` and adjacent log.
- All **24 fresh NullRHI package validators** passed with the exact aggregate
  marker and no `LogPython: Error`. The new handcart validator is part of the
  chain. Evidence: `/tmp/embermere-sep16-handcart-packages.log`.
- The relaunched editor loaded `/Game/Maps/L_Embermere_Prototype`, rediscovered
  all 100 Embermere tests, and reported both the map and handcart packages
  saved.
- Seven initialized-world trace suites passed: the new handcart, communal well,
  notice board, training workshop, cottage, vendor stall, and road boundary.
- Live full-zone validation passed at exactly 53 Fab actors plus 25 original
  placements. Blender MCP also answered successfully after the restart cycle.

The visual acceptance used only Unreal viewport capture and Blender-owned
rendering. No desktop control, OS input, accessibility automation, or broad
editor capture was used. The handcart reads grounded beside the cottage in the
normal village composition. Physical controller/camera feel was not needed to
change its validated static placement.

## Preservation And Next Start

The protected save SHA256 remains
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
the preserved local keeper resave remains
`38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
No Save/Load action occurred. Existing Config, keeper, and Field Notes changes
remain excluded from the handcart commit. The unapplied Ledger regression draft
remains uncompiled and is not part of the accepted 100-test suite.

Next: install or select a compatible side-by-side Xcode through process-local
`DEVELOPER_DIR`, prove a real UBT build, then review and execute the pending
Ledger focus regressions before touching production C++. If that prerequisite
is still unavailable, continue a different bounded MCP-backed milestone rather
than repeatedly probing the known-invalid Xcode 27 configuration.
