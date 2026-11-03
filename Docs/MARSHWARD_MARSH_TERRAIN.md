# Marshward Marsh Terrain

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
