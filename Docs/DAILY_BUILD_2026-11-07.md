# Daily Build: 2026-11-07

## Arrival Composition Review

Fresh viewport-only editor captures at X=0,-500,+500, Y=-450, Z=260,
pitch-5/yaw90/roll0 and horizontal FOV90 showed a distinct pale crossing and
forward broken-ruin silhouette. The near marker remains separate from the
ruin; in the positive-X view it is mostly beyond the right edge. No concrete
framing or grounding defect justified adding density or moving accepted art.
All 41 placements/eight meshes, PlayerStart, materials, lighting, collision,
Blender sources, gameplay, startup maps and save behavior remain unchanged.

## Read-Only Regression

`Scripts/validate_marshward_arrival_vista_unreal.py` requires native Visibility
rays from the three inspected origins to first hit each intended saved west
pillar, east pillar and lintel: nine positive sightlines, no ignored actors.
Three downward rays require saved arrival ground at Z=0. At X=500, the
original arrival island and southern connector are coplanar; either exact
saved support is valid there. The guard runs with the complete Causeway
live suite and never saves, spawns or mutates content.

This is collision coverage plus static editor-view review, not proof of
NoCollision-art visibility, physical input, painted screen contrast or
camera/movement feel. See `Docs/MARSHWARD_ARRIVAL_REVIEW.md`.

## Verification

- Sixteen host corridor/asset tests and Python syntax checks passed.
- Fresh GUI-down Causeway validation passed exact eight-mesh/41-placement,
  full-width buffer and basin-only wet-peat markers, true exit zero and no
  `LogPython: Error`. Evidence: `/tmp/embermere-nov07-causeway-packages.log`.
- In the final clean editor, the new nine sightlines/three ground controls,
  original 579 support / 1104 clearance rays, marker, six banks, twelve
  stone/twelve underlying-ground and nine basin/two shelf-join apron controls
  passed. No Python errors appeared in that final session. Snapshot:
  `/tmp/embermere-nov07-arrival-editor.log`.
- Two earlier attempts were rejected: the initial support fixture omitted
  the coplanar connector, and an immediate map-load/trace submission saw
  ground before the ramp support settled. Exact owner expectations and a
  separate load/probe submission resolved them without geometry changes or
  relaxed support tolerances. No failed run is counted as acceptance.
- Normal floating PIE confirmed Human Warrior at HP100/100/Mana50/50 and
  stopped cleanly. Fresh discovery listed 133 native tests; full native and
  Fenwatch package/live runs were not repeated for this read-only slice.
  Their November 6 evidence remains authoritative. No C++ rebuild was needed.
- Both map files and protected save/keeper hashes stayed exact. Causeway's
  SHA256 is `f781f2922a2749b824cbcdc27d371947ef1a63ad7c9e20da45da40e7d820dab6`.
  Blender MCP inspection was healthy, with no model changed. Causeway is
  All Saved outside PIE on real MCP8123. No Chronicle action was used.

Physical walking/camera acceptance and the integrated two-direction loop
review remain open. No connected travel, water gameplay or cross-level
persistence is implied by this review.
