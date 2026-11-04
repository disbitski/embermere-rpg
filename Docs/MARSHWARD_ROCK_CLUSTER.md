# Marshward Rock Cluster

## Ownership And Brief

`SM_MarshwardRockCluster_01` is an original three-stone terrain prop for the
standalone Causeway map. Author a 460 x 340 x 260 cm low-poly cluster with a
base-center pivot, three closed stone volumes, one UV channel, and exactly
three convex hulls (one per stone). All three bases lie on local Z=0.
Use one new project-owned gray stone material and the existing marsh moss
material. Moss is a face assignment, not extra collision or alpha foliage.

Four placements at west-near, inner-south, outer-northwest and outer-east
sites sit on the basin catch ground at Z=-120. Preserve the existing 36
placements, original 22 greybox actors, route support/collision and PlayerStart.
The map becomes 40 static placements; no enemy, quest, service, travel,
water or persistence authority is added. Fenwatch and protected user files
remain untouched. Keep every new visible bound outside full walking corridors
plus the accepted 75 cm buffer.

## Gates

Retain source, FBX, preview and deterministic metrics. Require fresh classic-
FBX import and saved mesh/material/hull checks, exact placement/profile and
corridor-bounds checks, then initialized live support probes at each of the
three authored stone centers in all four placements. Retain the original
579 support / 1104 clearance rays, marker control and six bank controls.
Inspect normal-camera arrival and overview views and run clean PIE.
Synthetic input and traces do not prove physical walking or camera feel.

## Verification: 2026-11-04

Blender MCP produced the closed **144-triangle** mesh at the briefed size,
one UV channel and zero non-manifold edges. Fresh classic-FBX import retained
three hulls, one new stone material and the reused moss material; its exact
marker, exit zero and absence of Python errors were required.

Four initialized-editor placements passed visible-bounds and exact package
checks. **13** host regressions and **133/133** native tests passed without
failures, warnings or skips. Fresh Fenwatch **31-package** and separate
seven-mesh Causeway gates passed. The clean final session passed all **16**
Fenwatch suites, the original **579 support / 1104 clearance** rays, marker
control, six bank controls and **12 stone / 12 ground** controls. Only the
underlying-ground diagnostics ignore their own cluster; route and stone
collision rays ignore nothing. All ground probes hit the basin catch floor
at the authored base height. Arrival/overview views and normal picker/Human
Warrior PIE startup/stop were checked; physical traversal remains unverified.

Logs: `/tmp/embermere-nov04-rock-import.log`,
`/tmp/embermere-nov04-fenwatch-31-packages.log`, and
`/tmp/embermere-nov04-causeway-packages.log`.
