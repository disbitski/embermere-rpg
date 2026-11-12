# Marshward Causeway Greybox

## Scope And Ownership

The separate `/Game/Maps/L_MarshwardCauseway_Greybox` is an exploratory second
level, not connected travel or finished art. Fenwatch, startup maps, protected
saves, quests, services, combat, and rewards remain unchanged. Use the normal
Embermere game mode and picker for walk/camera review; place no enemies,
interactables, persistence actors, or travel triggers in this first slice.

## Layout

All distances are centimeters. Arrival is `(0,0,0)`, facing north (+Y).
A 500-wide raised crossing runs to the ruin island at `(0,6400,120)`.
A 600-long ramp rises 120 at each end of the loop; the eastern return path
is at ground height, centered on X=2700. A southern connector closes the
loop. The shallow basin has solid catch ground rather than lethal water.
The entrance remains visible back along the crossing. Ruin blocks frame a
future destination but do not imply an exit, dungeon, or quest.

## Blender Kit Brief

- `SM_MarshwardSpan_Greybox`: 600 x 500 x 60 deck, top-center pivot at Z=0,
  one exact UBX deck box; no curbs or collision protrusions into the route.
- `SM_MarshwardRamp_Greybox`: 600 x 500 x 160 wedge, bottom at Z=-40,
  top from Z=0 at X=-300 to Z=120 at X=300; one convex UCX wedge.
- `SM_MarshwardWaymarker_Greybox`: 180 x 140 x 520 total, base pivot,
  one UBX main-body box. Cap and directional inset remain decorative.

Each mesh has one UV channel, triangulated closed geometry and zero
non-manifold edges. A single project-owned stone material serves the span
and ramp; the marker uses a contrasting warm material. These are blockout
assets, not final Fenwatch-family art. Source, FBX, metrics, and kit preview
are retained in `ArtSource/Blender/Environment/MarshwardGreybox`.

## Acceptance

Require exact saved mesh dimensions, triangles, materials, collision shapes,
actor transforms and map ownership. In the initialized world, check support
at center and player-width offsets along the entire loop, horizontal
clearance at 95/155 above each route point, and a positive solid-wall hit.
Then inspect an arrival and overview viewport and run clean normal PIE.
Geometric traces and synthetic Slate do not prove physical traversal or
camera feel. No claims of connected travel or save/load are permitted.

## First Slice Evidence (2026-11-01)

Blender MCP built and rebuilt the three-piece kit with 12/12/36 triangles,
one UV channel each, and zero non-manifold edges. Fresh classic-FBX import
passed with two boxes and one convex shape. A separate fresh process loaded
and validated the saved map and meshes with
`EMBERMERE_MARSHWARD_PACKAGE_SUCCESS`, exit zero, and no `LogPython: Error`.
Evidence: `/tmp/embermere-marshward-kit-import.log` and
`/tmp/embermere-marshward-package-validation.log`.

The initialized map passed **579** support and **1,104** clearance rays plus
a positive marker-collision control. Arrival and overview viewport captures
showed the route, ramps, marker hierarchy, and broken ruin silhouette.
Normal floating PIE reached the picker and Human Warrior HUD and stopped;
this is startup proof, not a completed physical walk around the loop.
All **133/133** existing native tests passed without failures, warnings, or
skips. Fenwatch's fresh **31-package** aggregate and **16** live-world suites
passed; its map file SHA256 stayed
`25c40e85c9c41b7b7cd4b95f0ed60cb4e2bf6e5b258bf44b9fcb754bf4a25b44`.

## Remaining Player Review

The November 2 terrain pass adds six low berms and eight reed clumps outside
full-width walking corridors plus 75 cm. The original 22 placements and route
are unchanged. Two outer banks use the November 3 asymmetric shelf; four
grounded rock clusters were added on November 4 without moving prior art.
The rock pass reached seven meshes and 40 static placements. See
`Docs/MARSHWARD_MARSH_TERRAIN.md`, `Docs/MARSHWARD_PEAT_SHELF.md` and
`Docs/MARSHWARD_ROCK_CLUSTER.md`.
The basin alone uses a wet-peat surface study; the route's pale materials
and solid catch-floor geometry remain unchanged. See
`Docs/MARSHWARD_WET_PEAT.md` for the saved-graph and assignment gates.
A single decorative bank-edge apron now brings the current gate to eight
meshes and 41 static placements. All prior actors stay unchanged; the apron
is NoCollision and outside the route. See `Docs/MARSHWARD_BANK_EDGE.md`.
Arrival review left content unchanged and added nine positive landmark
sightlines plus three arrival-ground controls to the initialized suite.
See `Docs/MARSHWARD_ARRIVAL_REVIEW.md` for the three inspected editor poses,
coverage limits and remaining physical acceptance.
The integrated eight-view review is in `Docs/MARSHWARD_LOOP_REVIEW.md`.
It retained the scene unchanged and leaves south-turn wayfinding, moving
camera clearance and both-direction physical walking explicitly unverified.
Two original foundation courses subsequently brought the map to nine meshes
and 43 static placements without moving any preceding actor. They occupy
the northern ruin shoulders, not the central route or ramp approach. Fresh
mesh/map gates and six solid-block/six adjacent-footing controls passed;
physical traversal remains separate. See `Docs/MARSHWARD_FOUNDATION_STONES.md`.
One sparse fallen-masonry scatter subsequently brings the installed gate to
ten meshes/44 static placements. It stays on the west shoulder, preserving
the prior43 actors, original22, all routes and landmarks. Fresh package/live
gates and normal picker/Human Warrior PIE passed; physical walking remains
open. See `Docs/MARSHWARD_FALLEN_MASONRY.md`.
Two eastern reed mesh substitutions then retained all 44 static placements
while bringing the kit to eleven meshes. The new variant is taller and
sparser, NoCollision, and outside the unchanged full-width buffers. See
`Docs/MARSHWARD_TALL_REEDS.md`; native traces do not prove visual occlusion.
One low NoCollision broad-leaf scrub then brings the current map to45 static
placements/twelve kit meshes while retaining all previous44 actors. It sits
outside the eastern return buffer and separate from the bank apron. Fresh
package/live checks and three original-basin supports passed; physical
walking/camera acceptance remains open. See `Docs/MARSHWARD_LOW_SCRUB.md`.
To recreate the map, build the original greybox kit/map first, then import
and place the separate marsh assets; keep commandlet import and initialized
editor placement separate.

Open the greybox explicitly through File > Open Level; startup maps are
unchanged. Start PIE, confirm a character, close Inventory, and walk the
crossing, ruin island, east ramp, and lower return path. Check ramp joins,
camera clearance, landmark readability and whether the return route feels
obvious. There are no enemies or transactions to test here yet. Do not use
Chronicle load/save: cross-level persistence and respawn integration are not
part of this slice. The basin is dry catch geometry, not water gameplay;
the outer map perimeter is not a finished playable boundary.
