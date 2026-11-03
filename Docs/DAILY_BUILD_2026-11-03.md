# Daily Build: 2026-11-03

## Milestone

Fresh Causeway overview evidence showed repeated round berm silhouettes.
Blender MCP built one longer asymmetric peat shelf with tapered ends:
2600 x 900 x 240 cm, 66 triangles, one UV channel, closed topology and one
authored convex hull. It reuses the three saved peat/moss/earth materials.
Only `Marshward_BermWestFar` and `Marshward_BermEast` mesh references changed.
The map remains 36 static placements: original 22, four berms, two shelves,
eight reeds. All transforms, route and PlayerStart remain unchanged.
No C++, startup-map, quest, service, water, travel or save behavior changed.

## Verification

- Ten host regressions passed, covering the new brief, allowed roles, visible
  bounds and fixed full-width corridor/buffer contract.
- Fresh classic-FBX import passed its marker, exit zero and no
  `LogPython: Error`. No materials or existing meshes were resaved.
- All 133/133 native tests passed with zero failures, warnings or skips.
- Fresh sequential Fenwatch 31-package and separate Causeway checks exited
  zero with exact skeletal-material, 53 Fab / 32 original-art, validators=31
  and six-mesh Causeway markers, without Python errors.
- The clean final session passed all 16 Fenwatch live suites, Causeway's
  579 support / 1,104 clearance rays, marker collision and six terrain controls
  including both shelves. Arrival/overview viewport captures were inspected.
  Normal floating PIE reached the Human Warrior HUD and stopped.
- Fenwatch map SHA256 remained
  `25c40e85c9c41b7b7cd4b95f0ed60cb4e2bf6e5b258bf44b9fcb754bf4a25b44`.
  Protected save and keeper material hashes retained their recorded values.

Evidence: `/tmp/embermere-nov03-shelf-import.log`,
`/tmp/embermere-nov03-fenwatch-31-packages.log`, and
`/tmp/embermere-nov03-causeway-packages.log`.
The four known raw-vendor physics-resave warnings remain unchanged.

## Open Gates

Physical walking/camera feel remain unverified. This is still a dry,
standalone study, not finished perimeter/water gameplay or linked travel/
persistence. See `Docs/MARSHWARD_PEAT_SHELF.md` for the asset contract.
