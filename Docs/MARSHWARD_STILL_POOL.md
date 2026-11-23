# Marshward West Basin Still Pool

## Brief

The settled arrival view keeps the pale crossing aligned with the broken
ruin gate. Looking west from the crossing reveals a quiet stretch of wet
peat between the two banks and the reed line. Use that pocket for one small
pool without making the whole basin read as flooded.

Build an independent `SM_MarshwardStillPool_01` in Blender: an uneven,
rounded 840 x 520 cm outline, a flat waterline, and a 3 cm closed thickness
that buries the underside. Use one UV channel and an authored vertex-color
falloff toward the shore. Keep the water an opaque, muted blue-green
surface with restrained gloss, no waves, transparent layers or Water
plugin dependency. The pool should read as off-route dressing, not a
walkable shortcut or a new destination.

Place at `(-1320,3340,-118)`, yaw 0, unit scale: waterline Z=-118 is 2 cm above
the solid basin and 238 cm below the adjacent raised crossing. The visible
footprint is X=-1740..-900 and Y=3080..3600. Require separation from the
surrounding dressing, the full walking corridors plus 75 cm and all existing
landmarks. Do not cast shadows, generate overlaps, influence navigation, or
add collision; the original wet-peat basin remains the floor. Probe the
center and four points inside the pool for exact basin first hits. Compare
settled before/after views for surface readability and route occlusion.

## Adopted Study

The 96-triangle, closed Blender mesh has one UV channel, one material slot
and no collision hulls. The outer ring and inner ring carry a vertex mask
that blends both tint and roughness from muddy edge to darker water.
The classic FBX retained vertex colors and the single UV. The companion
`M_MarshwardStillPool` is opaque, non-metallic and uses no textures or
displacement. Only the pool uses it; wet peat still belongs only to the
original basin.

Before/after editor views from arrival `(0,-450,260)`, west-facing crossing
`(0,3300,360)`, and east return `(2700,3600,260)` held the same camera
poses. From the crossing the blue-green surface reads separately from the
peat. At arrival and return it stays low in the background, leaving the
crossing and ruin markers clear. A short dark line remains visible at the
near pool edge in the crossing view; any shoreline dressing should address
that transition without moving the pool or changing the basin floor.

The original 51 static placements were compared before and after staging.
Only the pool was added. It uses `NoCollision` with overlaps, shadow casting
and navigation influence disabled. It does not add swimming, hazards,
physics, interaction or a connection to another map.

## Verification

- 69 focused host checks passed. Fresh classic-FBX import and an independent
  Causeway package run exited zero with exact success markers and no Python
  errors. The saved map has 52 static placements, 19 installed imported mesh
  types and 20 validated mesh packages, including the retained greybox marker.
- The initialized route suite passed 579 support rays, 1,104 clearance rays
  and all existing controls. Five unfiltered first hits inside the pool
  found the exact original basin at Z=-120, not the visible surface.
- Normal floating Play-in-Editor reached the Human Warrior HUD through
  synthetic Slate input and stopped. No protected save was loaded.
- Compared with the [pre-pool audit](MARSHWARD_BUDGET_AUDIT.md), the loaded map
  now has 9,466 placed LOD0 triangles, 90 render sections, 12 effective
  materials and 15 NoCollision placements. The 37 solid actors and their
  20 boxes and 32 convex hulls are unchanged. These are inventory counts,
  not measured frame draw calls.

The Fenwatch map, protected save and keeper material stayed byte-identical.
No C++ build, full native run or new Fenwatch package/live gate was performed
for this isolated art addition. Still-pose comparisons do not establish
physical traversal, moving-camera clearance or first-visit wayfinding.
