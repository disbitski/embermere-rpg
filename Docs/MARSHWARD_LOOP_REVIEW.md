# Marshward Loop Review

## Scope

This is an integrated static-view review of the crossing, ruin island,
eastern return and southern connector, not another arrival-only check or
a physical walk. Keep the accepted 41 static placements/eight meshes,
PlayerStart, support/collision, full-width buffers, basin-only wet peat and
arrival sightline guard intact. No new content, lighting or gameplay change
is warranted merely to fill open space.

## Views: 2026-11-08

All captures used Unreal's viewport-only tool with roll0 and horizontal
FOV90. Coordinates are centimeters. Camera Z is 240 cm above the crossing/
ruin surface and 260 cm above the lower route. These are repeatable editor
poses, not sampled moving player-camera positions. Editor sprites/grids in
the captures are not game UI evidence.

| View | Position | Pitch / Yaw | Observation |
| --- | --- | --- | --- |
| Crossing north | (0,3300,360) | -5 / 90 | Open pale deck leads to the two pillars and broken lintel; reeds remain beside the deck. |
| Crossing south | (0,3300,360) | -5 / -90 | Arrival island and its marker remain visible as return references; deck edges are distinct from the basin. |
| Ruin reverse | (0,6100,360) | -5 / -90 | The crossing leads back toward the arrival marker; lower return terrain is visible to the side. |
| Ruin to east ramp | (1100,6400,360) | -8 / 0 | Ramp links the pale platforms without an intervening prop; descent depth and camera response still need physical review. |
| East return north | (2700,3600,260) | -5 / 90 | Original marker occupies a shoulder, leaving the central lane open; the connecting ramp and ruin remain visible. |
| East return south | (2700,3600,260) | -5 / -90 | Long return lane is open; no distinct material break announces the southern turn. |
| South connector west | (1700,0,260) | -5 / 180 | Connected ground leads back toward the arrival marker and crossing entrance. |
| South connector east | (100,0,260) | -5 / 0 | Same-color connected platforms dominate the still; this view alone does not establish an obvious turn north. |

No evidenced blocking or framing defect in these views justified moving art,
adding density, changing support geometry or retuning lighting. Retain the
accepted composition. The south/east junction's first-visit turn cue remains
a specific playtest question, not a proven navigation defect or acceptance.
Do not add another marker without that feedback and a bounded design review.

## Verification

The already-loaded, settled initialized editor passed the unchanged complete
Causeway suite: **579 support / 1104 clearance** rays, marker control, six
banks, twelve stone/twelve underlying-ground controls, nine basin/two shelf-
join apron controls and the **nine landmark / three arrival-ground** guard.
The November 8 markers are in `/tmp/embermere-nov08-loop-editor.log`, along
with `LOOP_REVIEW_FINAL_DIRTY []`; no `LogPython: Error` appeared.

Sixteen host corridor/asset tests passed. Fresh discovery listed **133**
native tests; they were not run again. Normal floating PIE confirmed Human
Warrior at HP100/100/Mana50/50 and stopped cleanly, with no Chronicle action.
Blender MCP inspection was healthy; no model was changed. No build, import,
package commandlet, full native run or Fenwatch suite was needed for this
documentation-only review. November 7 remains the fresh Causeway package
evidence; November 6 remains the full native/Fenwatch baseline.

Both maps and protected user files retained their recorded hashes. Causeway
remains `f781f2922a2749b824cbcdc27d371947ef1a63ad7c9e20da45da40e7d820dab6`,
All Saved outside PIE on real MCP8123. No source, asset, collision, material,
startup-map, quest, combat, inventory, service or save owner changed.

## Physical Gates

Walk the whole loop in both directions, including a first-visit approach to
the southern turn. Check whether the return direction is understandable
without coaching, whether the original shoulder marker leaves comfortable
camera room, and whether both ramp joins feel continuous during movement.
These stills and geometric rays do not prove moving-camera collision,
painted contrast, UI occlusion, movement feel or player navigation. The dry
catch floor and unfinished outer perimeter are not water gameplay or finished
boundaries. Connected travel and cross-level persistence remain separate work.
