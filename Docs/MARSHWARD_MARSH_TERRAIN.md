# Marshward Marsh Terrain

## Low Scrub Accent

One original 540-triangle broad-leaf scrub occupies the outer east shoulder,
NoCollision and beyond the fixed full-width walking buffer. All prior44
actors are retained; current Causeway is45 static placements/twelve kit
meshes,including six original reeds and two tall variants. Fresh source,
package and live checks passed,including three original-basin support rays.
No existing source/material or support geometry changed. Physical foliage
readability remains separate; see `Docs/MARSHWARD_LOW_SCRUB.md`.

## First Terrain Pass

Extend the standalone Causeway greybox without changing its original 22
static placements, PlayerStart, walking route, startup maps, or gameplay
systems. The new assets belong only to the Causeway map, not Fenwatch.

- `SM_MarshwardPeatBerm_01`: 1800 x 1000 x 180 cm closed low-poly terrain,
  base-center pivot, three peat/moss/earth material slots, one UV channel,
  and one authored convex collision hull. Six low berms flank the crossing,
  inner basin, east bank, and distant north edge.
- `SM_MarshwardReedClump_01`: 260 x 220 x 210 cm closed stalk/leaf/seed
  geometry, base-center pivot, two material slots, one UV channel, and no
  collision. Eight clumps sit outside the route, never over its deck.

Retain all source, exports, previews, and metrics. Shared project-owned
materials are explicit; do not import raw vendor content or modify the
existing greybox kit. Terrain is solid, dry geometry: this pass adds no
water simulation, hazards, enemies, services, rewards, travel, or persistence.

## Route Protection

Keep the original support/clearance checks and add visible-bounds exclusion
against each full-width walking corridor plus a 75 cm buffer. Corridor
half-widths follow the route's nine segments: 600, 250, 250, 400, 400, 250,
300, 600, 600 cm. This protects the broad eastern and southern return paths,
not only the player's centerline. Validate both geometry and collision;
non-colliding art can still conceal or visually obstruct a path.

Fresh package validation must check mesh dimensions, topology metrics,
materials, authored hull count, exact placements/profiles, and corridor
bounds. Initialized-world traces must retain the complete loop checks and
positively hit all six berms. Inspect fresh arrival/overview views and clean
PIE. Physical traversal and camera feel remain separate player checks.

## Verification: 2026-11-02

Blender MCP produced the berm at **96 triangles** and reeds at **616**, both
with one UV channel and zero non-manifold edges. Fresh classic-FBX import
retained their sizes, materials, one convex berm hull and zero reed collision.
Six berms and eight reeds were placed through the initialized editor. The
map now has **36** static placements: the original 22 plus these 14 additions.
The original layout manifest is unchanged.

Seven host regressions cover visible bounds, buffer contact, reversed routes,
wide return paths, endpoints, invalid segments and all authored placements.
All **133/133** native tests passed without failures, warnings or skips.
Fresh GUI-down Fenwatch **31-package** and separate Causeway package checks
passed with exact markers, exit zero and no Python error. The clean final
session passed all **16** Fenwatch live suites and Causeway's **579 support /
1,104 clearance** rays, positive marker collision and **six** berm controls.
Arrival/overview views were inspected; normal floating PIE reached the
Human Warrior HUD and stopped. Physical walking/camera feel remain open.

Logs: `/tmp/embermere-nov02-marsh-import.log`,
`/tmp/embermere-nov02-fenwatch-31-packages.log`, and
`/tmp/embermere-nov02-causeway-packages.log`.

## Asymmetric Outer Banks

The November 3 shelf replaces the west-far and east bank mesh references
without moving actors or adding placements. The current set is four round
berms, two 66-triangle asymmetric shelves and eight reeds. The shelf reuses
the terrain materials and one convex hull; full corridor widths and the
75 cm buffer remain fixed. See `Docs/MARSHWARD_PEAT_SHELF.md` for dimensions
and verification. Rebuilds must import both original marsh meshes and the
shelf before terrain placement; the helper preflights all mesh dependencies.

## Grounded Rock Accents

Four original rock-cluster actors were added outside the corridors without
altering the preceding 36 placements. That pass brought the map to 40 static
actors and seven unique meshes. Each 144-triangle cluster has three closed stones,
one UV channel and one convex hull per stone. Ground probes verify the basin
under all twelve authored stone centers, alongside unfiltered route rays.
See `Docs/MARSHWARD_ROCK_CLUSTER.md` for ownership and evidence. Import the
rock cluster before using the complete terrain placement helper in a rebuild.

## Basin Surface Study

The basin catch actor now uses a separate opaque wet-peat material with
world-space mottling and restrained roughness variation. Only its slot-zero
component override changes; all walking-surface materials, original Blender
materials, geometry, collision and placements stay intact. This is not water
or a friction change. See `Docs/MARSHWARD_WET_PEAT.md`. In a rebuild, create
the new material separately and apply it after the original map and dressing
are complete, using `Scripts/apply_marshward_wet_peat_unreal.py` in the editor.

## Decorative Bank Join

One original 46-triangle apron softens the east shelf/basin join outside the
walking buffer. It is closed geometry with one UV channel, three reused
terrain materials and no collision; the basin and shelf remain its only
underlying support. All prior 40 placements are unchanged. The map now has
41 static actors/eight meshes. Nine unfiltered basin probes and two existing-
shelf join controls supplement the complete route tests. See
`Docs/MARSHWARD_BANK_EDGE.md`. Import the apron before a complete dressing
rebuild; its dedicated placement script adds only that actor to an existing map.

## Ruin Foundation Courses

Two original three-block courses now sit on the northern ruin shoulders,
beyond the full-width protected approach. Each is 180 triangles, one UV and
three convex hulls, reusing existing stone/moss. All prior 41 actors stay
unchanged; the map has 43 static placements/nine meshes. Six solid-block
and six adjacent ruin-island supports supplement every prior live control.
Import the foundation mesh before a complete dressing rebuild; the dedicated
placement helper checks numeric preservation before saving only its two new
actors. See `Docs/MARSHWARD_FOUNDATION_STONES.md`.

## Fallen Masonry

One original two-fragment broken-beam scatter sits between the west marker
and foundation, outside all protected walking buffers. It has88 triangles,
one UV,two authored hulls and reused stone/moss; all preceding43 actors stay
unchanged. Current Causeway is44 static placements/ten meshes. Two fragment
hits and two adjacent island supports augment every earlier live control.
Import it before a full dressing rebuild. See `Docs/MARSHWARD_FALLEN_MASONRY.md`
for the brief, exact placement and separate physical acceptance limits.

## Sparse Eastern Reeds

The two eastern reed actors now use a taller, narrower three-stem variant
with six leaves and no collision. Six original clumps remain; total reed
actors stay eight and total static placements stay 44. The kit now has eleven
meshes. All transforms, profiles and support geometry are preserved; only
two mesh paths changed. Fresh import/package checks, six base-to-basin rays
and settled eastern/arrival review passed. See `Docs/MARSHWARD_TALL_REEDS.md`.
