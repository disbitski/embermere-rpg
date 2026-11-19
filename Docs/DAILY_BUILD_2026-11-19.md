# Build Evidence: 2026-11-19

## Western Dead Tree

One original 260-triangle dead tree is set between the two western peat
banks at (-2850, 3350, -120). The asymmetrical crown is a single connected
mesh with one UV channel. The actor has no collision and uses three unchanged
materials. Existing dressing, support, route, lights, PlayerStart, markers
and basin-only wet peat were retained. See `MARSHWARD_DEAD_TREE.md`.

## Verification

- 54 host tests passed. Fresh classic-FBX import and saved Causeway package
  validation exited zero with no Python errors. Causeway has 49 static
  placements, 16 installed imported types and 17 validated mesh packages.
- The separate 31-package Fenwatch aggregate and all 16 initialized-world
  suites passed. The complete Causeway suite passed 579 support/1,104
  clearance rays, all earlier controls, and one unfiltered trunk-foot
  first hit on the original basin.
- Two settled crossing views, three arrival views and normal Human Warrior
  PIE passed. The editor was left All Saved outside PIE. Discovery listed
  133 native tests; no native rerun or C++ build. The Fenwatch map, protected
  files and reused materials stayed unchanged. Physical walk and camera
  acceptance are not claimed.
