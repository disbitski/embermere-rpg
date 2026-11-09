# Build Evidence: 2026-11-09

## Foundation Source

Fresh ruin-backdrop viewport evidence informed a written brief for one
original three-block foundation course. Blender MCP produced the 600 x 260 x
150 cm asset with 180 triangles, one UV, zero non-manifold edges, three hulls
and two reused materials. The preview was inspected. Editable source, FBX,
metrics and two proposed off-route placements are retained separately from
the installed map manifest. See `MARSHWARD_FOUNDATION_STONES.md`.

This is a source checkpoint, not accepted map dressing. The local imported
Unreal mesh is excluded pending fresh persisted-package and placement proof.
Causeway's saved map, all 41 placements/eight meshes, route, PlayerStart,
materials, lights, full-width buffers and arrival guard remain unchanged.

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
- Both map files and the protected save/keeper hashes stayed exact.

## Rejected And Pending

An unsaved placement preflight compared Transform reprs, which contain
temporary struct addresses, and falsely reported preceding-actor drift.
Its two new actors were rolled back; the map was never saved. A subsequent
trace submission was also rejected because the rolled-back actors were
absent. Neither result is acceptance. Preserve numeric comparisons in the
eventual placement fixture; do not relax content or route assertions.

Closing that dirty-but-rolled-back editor stalled on its save-changes prompt,
blocking further MCP responses. The user was asked to select Don't Save for
Causeway. No desktop control or process kill was used. No new Causeway fresh
package/live or PIE acceptance is claimed; November 7 package and November 8
live evidence remain the unchanged map's last accepted checks. The rejected
session snapshot is `/tmp/embermere-nov09-rejected-placement-editor.log`.

After the prompt is dismissed, verify process/port shutdown, relaunch using
the project launcher, independently validate the saved imported mesh, then
finish placement and deliberate package-contract integration. Require fresh
Causeway package/live, unchanged arrival guard, viewport review and clean PIE
before accepting map adoption. Physical walking/camera gates remain open.
