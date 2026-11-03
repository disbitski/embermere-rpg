# Marshward Peat Shelf

## Ownership And Brief

The longer shelf breaks the repeated round berm silhouette visible in the
Causeway overview. It is an original terrain variant, not new gameplay or a
new placement. Replace only `Marshward_BermWestFar` and `Marshward_BermEast`
mesh references; preserve their locations, rotations, scales, labels, folders,
tags and BlockAll profiles. Keep the original 22 greybox placements, route,
PlayerStart, eight reeds and four other berms unchanged. The map remains 36
static placements. Fenwatch, startup maps and protected saves are untouched.

`SM_MarshwardPeatShelf_01` is 2600 x 900 x 240 cm, with a base-center pivot,
an offset crest, asymmetric shoulders and tapered ends. Author closed low-poly
geometry with one UV channel and one simplified convex hull. Reuse
`M_MarshwardPeat`, `M_MarshwardMoss` and `M_MarshwardEarth`; add no material
or texture dependencies. Source, FBX, metrics and preview live beside the
original marsh kit under `ArtSource/Blender/Environment/MarshwardMarsh`.

## Gates

Require fresh classic-FBX import, exact saved mesh/material/collision checks,
and exact replacement ownership. Both new visible bounds must still clear
all full-width corridors plus 75 cm. Keep support controls on all six terrain
placements, including the two shelves, and the unchanged complete loop rays.
Inspect fresh overview/arrival views and normal PIE. Physical traversal and
camera feel remain user checks; this asset adds no water, travel, quests,
services, rewards or persistence.

## Verification: 2026-11-03

Blender MCP built the closed **66-triangle** shelf with one UV channel and
zero non-manifold edges. Fresh classic-FBX import retained its exact size,
one convex hull and three reused materials, with its exact success marker,
exit zero and no Python error. Source and preview were inspected.

The initialized editor replaced only two mesh references and saved the map.
Ten host tests include exact shelf/role contracts and prevent corridor widths
or the 75 cm buffer from shrinking. All **133/133** native tests passed without
failures, warnings or skips. Fresh Fenwatch **31-package** and Causeway
package gates passed independently. The clean final session passed all **16**
Fenwatch suites, **579 support / 1,104 clearance** Causeway rays and six terrain
controls including both shelves. Normal picker/Human Warrior PIE started and
stopped; physical walking/camera feel is not proven. Fenwatch and protected
hashes stayed unchanged.

Evidence: `/tmp/embermere-nov03-shelf-import.log`,
`/tmp/embermere-nov03-fenwatch-31-packages.log`, and
`/tmp/embermere-nov03-causeway-packages.log`.
