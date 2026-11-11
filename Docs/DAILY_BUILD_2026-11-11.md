# Build Evidence: 2026-11-11

## Sparse Reed Variant

Fresh settled eastern-return views informed a written brief before Blender
MCP built `SM_MarshwardTallReeds_01`. The independent source is 220 x 180 x
300 cm, 264 triangles, one UV, zero non-manifold edges, three stems/six leaves
and no collision. It reuses the saved reed-green/seed materials without edits.
The original seven-stem, 616-triangle clump remains unchanged. Source, FBX,
preview, metrics and builder are retained; see `MARSHWARD_TALL_REEDS.md`.

Only `Marshward_ReedsEastNear` and `Marshward_ReedsEastFar` changed mesh
references. Numeric preservation checks proved every other actor property
unchanged before saving only the map. No actors were added: Causeway remains
44 static placements, with eleven kit meshes, six original reed clumps and
two sparse variants. All transforms/profiles, walking support, PlayerStart,
full buffers, markers, ruins, masonry, foundations, lighting and basin-only
wet peat are unchanged. No C++, gameplay, water, travel or save changes.

## Verification

- All 27 focused host checks and new script syntax checks passed. All rotated
  visible bounds retain the original full-width corridors plus 75 cm.
- Fresh classic-FBX import passed exact 220x180x300 / 264-triangle / one-UV /
  zero-hull / two-reused-material checks, true exit zero/no Python errors:
  `/tmp/embermere-nov11-tall-reeds-import.log`.
- Separate fresh Fenwatch skeletal-material-first 31-package validation
  passed every explicit marker, including Prowler loot, 53 Fab/32 original-art
  and validators=31, true exit zero/no Python errors:
  `/tmp/embermere-nov11-fenwatch-31-packages.log`.
- Fresh complete Causeway package validation passed eleven kit meshes,
  original 22 plus 22 dressing placements, fixed buffers and unchanged
  basin-only wet peat, true exit zero/no Python/material errors:
  `/tmp/embermere-nov11-causeway-packages.log`.
- The final clean editor passed all 16 Fenwatch suites, full Causeway 579
  support/1104 clearance and every preceding marker/bank/rock/apron/foundation/
  masonry/arrival control, plus six unfiltered reed-base-to-basin probes.
  NoCollision policy is retained; rays do not certify visual occlusion.
  Snapshot: `/tmp/embermere-nov11-tall-reeds-final-editor.log`, with exact
  markers, `TALL_REEDS_FINAL_DIRTY []` and the correct Causeway world.
- Settled near/far eastern views and three arrival offsets were inspected.
  Marker and pillar/lintel silhouettes remain separate in those stills;
  physical moving-camera/contrast acceptance remains open.
- Normal 1280x754 floating PIE used the picker and reached Human Warrior
  HP100/100/Mana50/50 via synthetic Slate, then stopped cleanly. No physical
  input, painted player-camera or complete walk is claimed.
- Fresh discovery lists 133 native tests, not another full run. November 9's
  133/133 zero-failure/warning/skip evidence remains authoritative; no C++
  build was needed. No raw vendor package was resaved; the four accepted
  vendor physics-resave warnings remain confined to their known packs.

The final editor owns MCP8123 on Causeway, All Saved outside PIE, with no
dirty packages or Python errors. Fenwatch and protected save/keeper hashes
stayed exact. Accepted Causeway SHA256:
`17994a6c2e011c30b57198aca9d293191c057f009163f8f8e7ce3eed00a4e8ed`.
Physical two-direction walking, first-visit south-turn wayfinding and moving-
camera clearance remain user gates; no additional density was introduced.
