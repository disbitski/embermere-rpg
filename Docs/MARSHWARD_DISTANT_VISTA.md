# Marshward Distant Vista Study

## Decision

Keep the current Causeway composition. Do not add an Oldstone Hollow
backdrop or another gate yet. The existing broken gateway remains the
primary arrival landmark; the northeast fragment supplies a separate
ruin shape when seen from the upper return. Oldstone Hollow is still
a future level concept, not a visible playable destination.

This is a review of the current scene and reuse options, not an adopted
backdrop. No candidate was spawned, scaled, moved or rendered in place.
No asset, actor, light, collision or map package changed.

## Inspected Views

Fresh viewport-only captures on November 25 used actual editor cameras,
read-back transforms and a settle boundary before capturing from the
same explicit pose. Roll is zero throughout. These are static editor
views, not a physical or continuous moving-camera walk.

| View | Position, cm | Pitch / Yaw | Observation |
| --- | --- | --- | --- |
| Arrival | (0,-450,260) | -5 / 90 | The pale crossing leads to the gateway; markers and low side dressing remain separate. |
| Crossing approach | (0,3900,360) | -5 / 90 | Pillars and broken lintel dominate the skyline; the opening is unobstructed. |
| Ruin forward | (0,6400,360) | -5 / 90 | Through the opening, the low north berm sits below an unfinished horizon, with no onward route. |
| Northeast shoulder | (800,6940,370) | -8 / 31 | Existing foundation stonework and detached wall read as nearby ruin fragments, not a distant site. |
| Upper return | (2700,5850,260) | -5 / 102 | The detached wall is a distinct stepped shape beside the raised island. |
| East descent | (1100,6400,360) | -8 / 0 | The pale ramp leads to the lower return; the marker stays beside the connection. |

The horizon is visibly unfinished, especially from within the gateway.
That is an art gap, not evidence that the level is visually finished or
that the north opening should become an exit. No voluntary first-visit
wayfinding or moving-camera feedback was supplied for this review.

## Existing-Art Reuse Options

- The 900 x 220 x 420 cm wall already has a grounded northeast role.
  Another unit-scale copy would repeat that nearby fragment language;
  making it a tall remote silhouette would require a separate scale,
  skyline and route-expectation study. Do not stack copies into a second gate.
- The 600 x 260 x 150 cm foundation and low fallen masonry suit shoulders
  and footing. Their modest height does not establish a distant landmark
  from the inspected arrival and crossing views.
- The 2200 x 400 x 650 cm rocky bank is already a western terrain backdrop.
  It is a possible future terrain framing element, not recognizable
  Oldstone architecture or a reason to relocate its accepted placement.

Retain the current primary gateway and off-axis fragment hierarchy.
Revisit an Oldstone silhouette only after an uncoached walk tests where
the player expects to go from the island and whether the east return
reads as the continuation. A later backdrop must not imply a traversable
bridge, marker, portal or third zone; connected travel needs its own design.

## Verification

All 72 existing focused host tests passed. No tests were relaxed or added
to manufacture a content change. The saved-map and physical acceptance
limits remain separate: the route probes check collision, not the visual
occlusion of NoCollision art or how a player interprets a skyline.

The initialized Causeway suite passed 579 route support and 1,104 clearance
rays plus all existing controls, including the nine positive arrival
landmark sightlines, wall, pool and shore probes. The loaded inventory
remained 53 placements, 9,558 placed LOD0 triangles, 91 sections and
13 materials, with 20 box and 32 convex collision shapes. No runtime
rendering performance claim follows from those inventory figures.

No restart, import, package commandlet, engine build, full native run or
Fenwatch rerun was needed for this documentation-only review. The fresh
November 24 Causeway package proof remains authoritative. Blender MCP
scene inspection was healthy; no source was changed or rebuilt.

Normal 1280 x 754 floating PIE reached the Human Warrior picker and stopped.
The synthetic confirmation click focused the button but did not reach the
HUD in this run; no new HUD or walking acceptance is claimed. The editor
returned to Causeway, All Saved outside PIE. Both maps and the protected
save and keeper material retained their recorded hashes.

For the next physical walk, approach the ruin without coaching and note
where you expect to go after reaching the island: through the north
opening, toward the detached fragment, or down the east return. That
feedback should guide a later distant landmark, not the empty horizon alone.
