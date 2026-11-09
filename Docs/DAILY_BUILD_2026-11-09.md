# Build Evidence: 2026-11-09

## Foundation Source

Fresh ruin-backdrop viewport evidence informed a written brief for one
original three-block foundation course. Blender MCP produced the 600 x 260 x
150 cm asset with 180 triangles, one UV, zero non-manifold edges, three hulls
and two reused materials. The preview was inspected. Editable source, FBX,
metrics and two proposed off-route placements are retained separately from
the installed map manifest. See `MARSHWARD_FOUNDATION_STONES.md`.

The first checkpoint retained source only. After the user dismissed the
rolled-back editor prompt, independent persisted verification and placement
adopted two courses. Causeway now has 43 placements/nine meshes, retaining
all preceding 41 actors, route, PlayerStart, materials, lights, full-width
buffers and arrival guard. The new mesh and intentional map edit are accepted.

## Verified

- Fresh classic-FBX import exited zero with the exact 600x260x150 / 180-triangle /
  one-UV / three-hull / two-reused-material marker and no Python error:
  `/tmp/embermere-nov09-foundation-import.log`.
- Sixteen existing host corridor checks and three independent source/planned-
  bounds checks passed. Syntax checks passed for the new builder/importer.
- Fresh native discovery/run passed 133/133 with no failures, warnings or skips.
  Only disposable `/Temp/Untitled_*` worlds became dirty and were discarded
  by a graceful restart, without saving. No C++ changed or rebuild was needed.
- Fenwatch's independent skeletal-material-first 31-package gate exited zero
  with exact placed-Prowler loot, 53 Fab / 32 original-art and validators=31
  markers, no Python error. All 16 initialized Fenwatch suites passed before
  the later failed placement session. Logs:
  `/tmp/embermere-nov09-fenwatch-31-packages.log` and
  `/tmp/embermere-nov09-native-editor.log`.
- Fenwatch and protected save/keeper hashes stayed exact. Causeway remained
  unchanged through the rejected attempt, then changed only for the two
  verified foundation placements.

## Earlier Rejected Attempts

An unsaved placement preflight compared Transform reprs, which contain
temporary struct addresses, and falsely reported preceding-actor drift.
Its two new actors were rolled back; the map was never saved. A subsequent
trace submission was also rejected because the rolled-back actors were
absent. Neither result is acceptance. Preserve numeric comparisons in the
eventual placement fixture; do not relax content or route assertions.

Closing that dirty-but-rolled-back editor stalled on its save-changes prompt,
blocking further MCP responses. The user selected Don't Save for Causeway,
and shell process/port checks confirmed full shutdown. No desktop control
or process kill was used. The rejected session snapshot is
`/tmp/embermere-nov09-rejected-placement-editor.log`.

## Verified Adoption

- Independent fresh saved-mesh validation passed exact dimensions, 180
  triangles, one UV, three hulls, both reused materials, no dirty packages
  or saves. True exit zero/no Python errors:
  `/tmp/embermere-nov09-foundation-persisted.log`.
- Numeric location/rotation/scale plus stable paths/profiles/tags/folder
  replaced temporary struct reprs. Signature stability is checked before
  spawning; every preceding actor was unchanged before saving only the map.
  Two host regressions cover stable repeated reads and actual drift. All
  **21** focused host checks passed. A broad host discovery that included
  an engine-only wet-peat module was rejected; use the focused host patterns.
- Fresh complete Causeway package validation passed nine meshes, original22
  plus21 dressing placements, fixed corridors+75 cm and basin-only wet peat,
  true exit zero/no Python errors:
  `/tmp/embermere-nov09-causeway-foundation-packages.log`.
- The final clean session passed all16 Fenwatch suites, full Causeway579
  support/1104 clearance and every earlier marker/bank/rock/apron/arrival
  control, plus six solid blocks and six adjacent ruin-island supports at
  Z120. No new ray ignores actors. Adjacent rays do not claim hidden contact
  beneath the collider. Snapshot:
  `/tmp/embermere-nov09-foundation-final-editor.log`.
- Ruin-front, raised ruin-side, arrival and east-ramp viewport-only views
  retained clear approaches and the pillar/lintel silhouette. Exact poses
  are in the foundation contract. No lighting or other actor was retuned.
- Floating PIE reached the Human Warrior picker and stopped cleanly.
  Synthetic confirmation did not advance the small floating picker; no new
  character-HUD or physical walking/camera acceptance is claimed. The map
  remains All Saved outside PIE, dirty packages empty, no final-session
  Python errors, real UnrealEditor on MCP8123. Fresh discovery lists133;
  the earlier same-day133/133 native run remains the full-run evidence.

No C++, gameplay, startup-map, water, travel or save schema changed. Fenwatch
and protected hashes stayed exact. Accepted Causeway SHA256 is
`6c13740b2519476c5002310466506613b72d6da4d6afad76a5d4885db9275234`.
Physical two-direction walking, first-visit wayfinding and moving-camera
checks remain open; the earlier blocked prompt is resolved.
