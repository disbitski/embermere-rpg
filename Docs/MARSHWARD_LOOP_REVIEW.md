# Marshward Loop Review

## Scope

This is an integrated static-view review of the crossing, ruin island,
eastern return and southern connector, not another arrival-only check or
a physical walk. Counts below belong to their dated reviews; the current
November 28 scene has 53 static placements and 20 installed imported mesh
types. Keep PlayerStart, support/collision, full-width buffers, basin-only wet
peat and arrival sightline guard intact. No new content, lighting or gameplay change
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

## Integrated Review: 2026-11-15

Revisit the loop with the carved markers,foundation courses,fallen masonry,
taller reeds and low scrub now installed. No voluntary physical traversal
feedback was supplied. Preserve all45 static placements,12 installed mesh
types/13 validated packages,original22 roles,PlayerStart,walking supports,
full-width corridors plus75 cm,basin-only wet peat,materials and lighting.
This review changes documentation only,not content or regression behavior.

Eight fresh viewport-only captures used settled actual editor cameras with
the identical explicit capture transform,roll0 and confirmed FOV90. These
are static normal-height editor poses,not a continuous player-camera walk.

| View | Position | Pitch / Yaw | Observation |
| --- | --- | --- | --- |
| Crossing reverse | (0,2800,360) | -5 / -90 | Pale deck meets the arrival island; carved arrival marker remains separate from the center approach. |
| Crossing north | (0,3900,360) | -5 / 90 | Pillars and broken lintel frame the destination; existing cover and shoulder stonework leave the approach open. |
| Ruin reverse | (0,6200,360) | -5 / -90 | Raised crossing leads back toward the arrival marker; banks and reeds remain beside the deck. |
| Ruin toward ramp | (850,6400,360) | -8 / 0 | The ramp joins the lower return; warm return marker stays to the side of the connection. |
| Ramp reverse approach | (2400,6400,260) | -3 / 180 | Ramp rises toward the ruin; foundation courses and cover remain outside the clear approach. |
| Return marker approach | (2700,4700,260) | -5 / 90 | Close marker occupies part of the side view while the ramp remains visible; moving-camera comfort is not established by this still. |
| Connector toward arrival | (1700,0,260) | -5 / 180 | Same-color connected ground leads toward the arrival marker and crossing entrance; turn recognition remains a player question. |
| Arrival toward crossing | (250,500,260) | -5 / 90 | Offset approach retains the pale crossing and forward ruin silhouette,with banks/reeds beside rather than across it. |

No concrete blocking,grounding or framing defect in these stills warrants
moving art,adding density or changing support geometry. Retain the scene.
The shoulder marker can dominate a close static view; this is a targeted
camera-room question,not evidence to move its accepted bounds/collision.
The plain platform edges and same-color southern turn remain blockout
limitations,not permission for a speculative marker or apparently walkable
decorative surface across the120 cm basin drop.

### Refreshed Evidence

All38 focused host checks passed. The existing settled editor passed the
complete Causeway suite with fresh November15 markers:579 support/1104
clearance,positive marker,six banks,twelve stone/twelve underlying-ground,
nine basin/two apron joins,six foundation/six adjacent-footing,two masonry/
two adjacent-footing,nine landmark/three arrival-ground,six tall-reed/three
scrub,six marker-body/six decorative-clear,and15 platform/10 basin/24 sampled
return-clearance controls. No tolerance changed. Only the pre-existing rock
underlying-ground diagnostics ignore their own cluster. Neither collision
rays nor these stills certify NoCollision foliage visibility while moving.

Normal1280x754 floating PIE reached the Human Warrior picker and stopped.
A synthetic Slate click returned success but did not advance the observed
picker; do not claim a new HUD,character-confirmation or physical walk check.
The accepted November14 HUD evidence remains separate. No creation fix or
forced gameplay state was attempted. Fresh native discovery lists133 tests,
not another full native run. Blender MCP inspection was healthy on
MarshwardWaymarker; no model or source changed.

No restart,build,import,package commandlet or Fenwatch live rerun was needed.
November13 fresh Causeway/Fenwatch package and all16 Fenwatch live evidence
remain authoritative; November9 retains the full133/133 native run.
Snapshot `/tmp/embermere-nov15-integrated-review-editor.log` contains fresh
complete-suite markers,`INTEGRATED_REVIEW_FINAL_DIRTY []` and the correct
Causeway world,with no November15 review Python errors. Causeway remains
All Saved outside PIE on real MCP8123. Both maps and protected user files
retain their hashes; Causeway remains
`84b82b523129ccf31d089c57e4847e1451e7eb83aa70245c053ae134173d2edb`.

## Integrated Review: 2026-11-28

Revisit the accepted 53-placement scene with the pool/shore, timber, mooring
posts, western tree/tussock, rocky bank and detached ruin wall now present.
No voluntary physical walking, camera or first-visit turn feedback was supplied.
The review retains all actors, sources, mesh/material references, collision,
lighting, support geometry, PlayerStart and startup/save behavior unchanged.

Nine fresh viewport-only captures used the actual editor camera, numeric
readback and a settle boundary before the identical explicit capture pose.
All have roll 0 and confirmed FOV 90; coordinates are centimeters. These are
static editor views, not a continuous two-direction player-camera walk.
Editor sprites and grids are not gameplay HUD evidence.

| View | Position | Pitch / Yaw | Observation |
| --- | --- | --- | --- |
| Arrival north | (0,-450,260) | -5 / 90 | Pale crossing leads to the broken gateway; the near warm marker remains off the central approach. |
| Crossing north | (0,3300,360) | -5 / 90 | Two pillars and lintel frame an open landing; low shoulder stonework and reeds remain beside it. |
| Crossing south | (0,3300,360) | -5 / -90 | Arrival island and marker remain distinct return references; side rocks and posts do not cross the deck. |
| Ruin reverse | (0,6100,360) | -5 / -90 | The crossing remains open toward arrival; the off-route pool, banks and timber are visually separate in this still. |
| Ruin to east descent | (1100,6400,360) | -8 / 0 | Pale ramp joins the lower platform; the warm marker stays beside the connection. |
| East return north | (2700,3600,260) | -5 / 90 | Shoulder marker, ramp and ruin remain visible without an intervening prop in the central lane. |
| East return south | (2700,3600,260) | -5 / -90 | Broad lane remains open past outer scrub/reeds and inner rocks; the southern turn still lacks a distinct material break. |
| South connector west | (1700,0,260) | -5 / 180 | Connected ground leads toward the arrival marker and crossing ramp, with the western tree/bank beyond. |
| South connector east | (100,0,260) | -5 / 0 | Same-color ground joins the return platform; the still does not establish intuitive first-visit turn recognition. |

No concrete blocking or framing defect in these stills warrants moving art,
adding density, retuning lights or changing marker placement. The dark horizon
remains unfinished art, not a new destination or a diagnosed lighting fault.
Physical review should establish whether the plain southern junction is
understandable without coaching and whether the shoulder marker and ramp
joins leave comfortable camera room during movement. Do not infer acceptance
or add a speculative marker from the static review alone.

### Refreshed Full-Width Evidence

All 80 focused host checks passed without new tests or relaxed rules. The
existing complete validator passed in the initialized editor: 22 original
roles plus 31 dressing actors, 53 static placements and 21 validated imported
mesh packages including the retained marker. It rechecks original transforms,
mesh/profile ownership, all 31 dressing bounds against the nine route segments,
exact half-widths 600/250/250/400/400/250/300/600/600 cm and the fixed 75 cm
buffer. The pool/shore pair retains its exact existing overlap exception;
no generalized overlap waiver or corridor shrink was introduced.

Although the existing validator emits its package success markers, this was
an initialized-editor contract run, **not a new GUI-down persisted-package
gate**. Its map load completed in one submission; live collision checks ran
later after editor-frame/tool boundaries. The fresh November 24 Causeway
package evidence remains separate and authoritative.

The full live suite passed 579 support / 1,104 clearance rays, positive marker
control and every earlier asset guard, including the nine positive arrival
landmarks, sampled return shoulders, wall, pool and shore. Only the existing
rock underlying-ground diagnostics ignore their own cluster; other route and
art probes ignore no actors. These samples are not exhaustive player-volume,
moving-camera or NoCollision foliage-occlusion proof.

The refreshed inventory is unchanged: 9,558 placed LOD0 triangles, 91 sections,
13 effective materials, 37 BlockAll / 16 NoCollision actors, and 20 placed
boxes / 32 convex hulls. These counts do not establish frame cost. No restart,
build, import, source reauthoring, new native run or Fenwatch rerun was needed.
November 27's source-quality evidence remains separate, not a repeated audit.

Normal floating PIE reached the Human Warrior picker in the current
640 x 394 Slate preview window and stopped. No character confirmation,
HUD, painted player-camera or physical-walk check is claimed from that run.
Causeway returned All Saved outside PIE with no dirty packages or November 28
Python errors. Both maps and protected save/keeper files retained their hashes;
Causeway remains `f45b70257346b9dfae5989f8e52b2ff973ddc0d08f62a2c94902117793c12976`.
The session log snapshot is `/tmp/embermere-nov28-loop-review-editor.log`.

## Physical Gates

Walk the whole loop in both directions, including a first-visit approach to
the southern turn. Check whether the return direction is understandable
without coaching, whether the original shoulder marker leaves comfortable
camera room, and whether both ramp joins feel continuous during movement.
These stills and geometric rays do not prove moving-camera collision,
painted contrast, UI occlusion, movement feel or player navigation. The dry
catch floor and unfinished outer perimeter are not water gameplay or finished
boundaries. Connected travel and cross-level persistence remain separate work.
