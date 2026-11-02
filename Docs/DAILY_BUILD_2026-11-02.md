# Daily Build: 2026-11-02

## Milestone

Added the first original marsh terrain/art pass to the standalone Causeway:
six low colliding peat berms and eight decorative reed clumps. Blender MCP
built a 1800 x 1000 x 180 cm berm (96 triangles, three materials, one convex
hull) and 260 x 220 x 210 cm reeds (616 triangles, two materials, no collision).
Both have one UV channel and closed topology. The original 22 greybox
placements, route and PlayerStart are intact; total static placements are
now 36. Fenwatch, startup maps, gameplay, quest/service authority, save schemas
and protected user state are unchanged. No C++ build was needed.

## Verification

- Seven host corridor regressions passed. The new gate excludes visible
  bounds from full walking corridors plus 75 cm, including non-colliding art.
- Fresh classic-FBX import passed its exact marker, exit zero, no Python error.
- All 133/133 native tests passed with zero failures, warnings or skips.
- Fresh skeletal-material-first Fenwatch 31-package validation retained exact
  53 Fab / 32 original-art and validators=31 markers. Separate Causeway
  validation covered all five meshes, unchanged base placements, new
  transforms/materials/collision and corridor bounds. Both exited zero with
  exact success markers and no `LogPython: Error`.
- The clean post-relaunch session passed all 16 Fenwatch live suites and
  Causeway's 579 support / 1,104 clearance rays, positive marker collision
  and six solid berm controls. Arrival/overview viewport captures were
  inspected. Normal picker/Human Warrior floating PIE started and stopped.
- Protected save and keeper material retained their recorded SHA256 values.
  Fenwatch map hash remained
  `25c40e85c9c41b7b7cd4b95f0ed60cb4e2bf6e5b258bf44b9fcb754bf4a25b44`.

Evidence: `/tmp/embermere-nov02-marsh-import.log`,
`/tmp/embermere-nov02-fenwatch-31-packages.log`, and
`/tmp/embermere-nov02-causeway-packages.log`. The four accepted raw-vendor
physics-resave warnings remain unchanged; no vendor resaves were made.

## Remaining Gates

Physical traversal and camera feel are not proven by synthetic Slate or
geometry traces. The basin remains dry catch geometry, not water gameplay.
The perimeter, connected travel and cross-level respawn/save contracts remain
unfinished. See `Docs/MARSHWARD_MARSH_TERRAIN.md` for ownership and next checks.
