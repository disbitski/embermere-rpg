# Marshward Arrival Review

## Scope And Ownership

Review the accepted arrival composition without changing content. Preserve
all 41 static placements/eight meshes, PlayerStart, walking support, full-
width buffers and basin-only wet peat. No new asset, material, lighting,
camera setting, gameplay, travel or save change is justified merely by this
review. Blender remains available for a separately briefed, evidenced asset.

Inspect viewport-only editor captures at X=0,-500,+500, Y=-450, Z=260,
pitch -5, yaw90, roll0, horizontal FOV90. These are repeatable normal-height
editor poses, not a physical player-camera traversal. Assess the pale
crossing, separate near marker and forward broken-ruin silhouette; retain
space around them rather than adding density to fill the basin.

## Read-Only Guard

`Scripts/validate_marshward_arrival_vista_unreal.py` belongs to initialized-
world review, not gameplay or asset authoring. At those three origins, nine
native Visibility rays must first hit the intended saved west pillar, east
pillar and lintel. Three downward rays must reach saved arrival ground at Z=0;
the positive-X pose lies on the original coplanar island/south-return join,
where either of those two saved supports is valid.
No actors are ignored and each landmark ray requires a positive intended
hit, not merely an empty trace. Preserve the exact landmark centers.

The guard runs with the existing complete Causeway collision suite. It
does not save, spawn, move, repair, select or mutate an actor/package. Native
collision rays do not detect NoCollision art occlusion, measure final screen
contrast, or prove physical camera/movement feel; viewport review and the
user's physical traversal remain separate gates.

Load the map in a separate editor submission before running live traces.
An immediate load-and-trace submission can precede settled collision; do
not relax support-height tolerances to accept that initialization artifact.

## Review: 2026-11-07

| Editor Origin | Observation |
| --- | --- |
| (0,-450,260) | Pale crossing is centered; the near marker stays separate from both pillars and broken lintel. |
| (-500,-450,260) | Crossing shifts left; the larger near marker remains to the side of the forward ruin silhouette. |
| (500,-450,260) | Crossing shifts right; the near marker is mostly off the right edge, while the forward ruin remains visible. |

All three captures used the specified rotation/FOV. No concrete arrival
framing or grounding defect justified moving art, changing lighting or adding
density. All accepted assets, placements and content packages remain intact.
Blender MCP scene inspection was healthy; no model was changed for this review.

The clean initialized session passed the new **nine positive landmark /
three arrival-ground** controls and the existing **579 support / 1104
clearance** rays, marker, six bank, twelve stone/twelve underlying-ground
and nine basin/two shelf-join apron controls. No Python error appeared in
that final session. Fresh Causeway package validation exited zero with exact
eight-mesh/41-placement, full-width buffer and basin-only wet-peat markers.
Sixteen host checks passed; normal Human Warrior floating PIE reached the
HUD and stopped. Physical movement, screen contrast and camera feel are not
proven. Fresh discovery listed 133 native tests; the full native/Fenwatch
package runs were not repeated for this read-only slice, and their November 6
evidence remains authoritative.

Two earlier attempts were rejected: a fixture initially required only the
island at the coplanar join, and a same-submission load/trace saw unsettled
ramp support. Correcting the owner expectation and separating loading from
probing retained exact support tolerances and required no content changes.

Causeway map SHA256 remained
`f781f2922a2749b824cbcdc27d371947ef1a63ad7c9e20da45da40e7d820dab6`;
Fenwatch and protected user hashes also stayed exact. Package evidence:
`/tmp/embermere-nov07-causeway-packages.log`. The accepted live marker is
`EMBERMERE_MARSHWARD_ARRIVAL_VISTA_SUCCESS`; the clean editor-log snapshot is
`/tmp/embermere-nov07-arrival-editor.log`.
