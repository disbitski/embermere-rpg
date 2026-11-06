# Daily Build: 2026-11-06

## Decorative Bank Edge

Fresh arrival/return-side evidence supported a short apron at the east
shelf's straight basin join. Blender MCP built `SM_MarshwardBankEdge_01`:
1000 x 360 x 70 cm, 46 triangles, one UV channel, zero non-manifold edges,
three reused terrain materials and no collision. The narrow tip was refined
in source preview before import. One NoCollision actor at (3910,3100,-120),
yaw90, stays beyond full-width corridors plus 75 cm. The map now has 41
static actors/eight meshes, retaining every preceding actor/transform,
walking support/profile, PlayerStart and basin-only wet-peat assignment.
No C++, materials, startup map, gameplay, water, travel or save behavior changed.

## Verification

- Sixteen host corridor/asset tests passed, including exact brief, role and
  probe coordinates; all 19 dressing bounds retain the fixed route buffer.
- Fresh classic-FBX import passed its exact size/46-triangle/one-UV/
  zero-collision/three-reused-material marker, true exit zero, no Python error.
  No existing mesh or material was resaved.
- All 133/133 native tests passed with zero failures, warnings or skips.
  Only disposable `/Temp/Untitled_*` maps were dirty afterward; graceful
  shutdown cleared them without saving unrelated packages.
- Fresh Fenwatch skeletal-material-first 31-package validation retained all
  explicit markers, placed-Prowler loot, 53 Fab / 32 original-art and
  validators=31. Independent Causeway validation retained eight meshes,
  22 original plus 19 dressing placements and basin-only wet peat. Both
  processes exited zero without `LogPython: Error`.
- The final clean editor passed all 16 Fenwatch initialized-world suites,
  Causeway's original 579 support / 1104 clearance rays and marker control,
  six banks, twelve stone/twelve underlying-ground controls, plus nine new
  unfiltered apron-to-basin rays and two unfiltered existing-shelf joins.
  The apron has no collision or support authority. No Python errors appeared.
- Return-side, close, arrival and overview viewport captures were inspected.
  Normal floating PIE confirmed Human Warrior at HP100/100/Mana50/50 and
  stopped cleanly. This does not prove physical walking or camera feel.
- Causeway is All Saved outside PIE on real MCP8123 with 133 freshly
  discoverable tests. Fenwatch map and protected save/keeper hashes stayed
  exact. No Chronicle action was used. Four accepted raw-vendor physics-
  resave warnings remain; those packages were not resaved.

Logs: `/tmp/embermere-nov06-bank-edge-import.log`,
`/tmp/embermere-nov06-fenwatch-31-packages.log`, and
`/tmp/embermere-nov06-causeway-packages.log`.

## Remaining Review

See `Docs/MARSHWARD_BANK_EDGE.md` for the asset and ownership contract.
Review the arrival composition using the accepted kit and voluntary traversal
feedback. Physical camera/walking acceptance, finished boundaries/water,
connected travel and cross-level persistence remain unproven.
