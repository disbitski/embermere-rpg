# Marshward Foundation Stones

![Original foundation-course source preview](../ArtSource/Blender/Environment/MarshwardMarsh/foundation_preview.png)

## Ownership And Brief

Fresh viewport-only views from (0,6100,360), pitch -5/yaw90, and
(1850,5850,750), pitch -18/yaw130, show the plain northern ruin shoulder
behind the existing pillars. Add one original low foundation-course asset,
used twice on that shoulder. This supplies a small worked-stone language,
not another destination, route, wall, marker or gameplay owner.

`SM_MarshwardFoundation_01` is 600 x 260 x 150 cm with a base-center pivot.
Three separate closed, chamfered blocks have flat bases at local Z=0,
narrow open joints and unequal heights. Use one UV channel, at most 200
triangles, zero non-manifold edges and three authored convex hulls, one
per block. Reuse only `M_MarshwardMarshStone` and `M_MarshwardMoss` without
editing either material. Keep independent Blender source, FBX, metrics and
preview beside the accepted marsh kit.

Saved placements: `Marshward_FoundationWest` at (-1020,7120,120) and
`Marshward_FoundationEast` at (1020,7120,120), yaw0, unit scale, BlockAll.
Use the existing marsh tag/folder. Bounds lie at X=-1320..-720 and
720..1320, Y=6990..7250: wholly on the original ruin island, beyond the
route's northern protected edge at Y=6875. The stones do not replace or
support the pillars. No other actor moves or changes mesh/material/profile.
The original island retains walking support; full-width corridors plus
75 cm, PlayerStart, arrival landmark rays and basin-only wet peat stay fixed.

The map has 43 static placements/nine meshes, retaining all preceding
41 actors and original 22 greybox placements. No C++, lighting, material,
water, boundary, combat, quest, service, travel or save change is authorized.
Fenwatch and protected user files remain untouched.

## Gates

Require Blender topology/dimension/UV/hull metrics and preview inspection;
fresh classic-FBX import with exact reused materials and three hulls;
saved-package checks for all assets and exact placements; and host full-width
bounds tests. In a settled initialized world, keep all route/arrival controls
and positively hit all six blocks. Six adjacent base-height rays, just beyond
each block's northern toe, must first hit the original ruin island at Z=120
without ignoring any actor. These prove nearby footing, not hidden contact
under the collider; flat source bases and exact placement retain that role.
Inspect ruin/arrival/return views and clean normal PIE. Geometric controls
and static editor views do not prove physical traversal or camera feel.

## Source Evidence: 2026-11-09

Blender MCP built the briefed 600 x 260 x 150 cm course at 180 triangles,
one UV channel, zero non-manifold edges and three authored hulls. The preview
was inspected. Both existing materials were reused without changes. Source,
FBX, metrics, preview and a separate foundation-role manifest are retained.
The installed manifest now includes the same two roles, and the host tests
require both manifests to agree.

Fresh classic-FBX import passed exact dimension/triangle/UV/hull/material
checks and exited zero without Python errors. The initial source checkpoint
excluded the local Unreal package until independent persisted verification.
Sixteen existing host corridor checks plus three source/bounds checks passed.
All 133 native
tests passed without failures/warnings/skips; Fenwatch's independent fresh
31-package gate and all 16 initialized-world suites also passed.

The first unsaved placement attempt was rejected by a faulty preservation
fixture: stringifying an Unreal Transform includes a temporary struct address.
It rolled back its two actors without saving. A following trace submission
was also rejected because those actors were absent; neither run is acceptance.
The saved Causeway map retained its exact pre-work hash. Future placement
must compare numeric transform/material/profile values, not struct reprs.

The editor close then stalled on a save-changes prompt; MCP requests stopped
responding. The user chose Don't Save for that rolled-back map, allowing a
verified clean shutdown. No desktop-control substitute or process kill was used.

Evidence: `/tmp/embermere-nov09-foundation-import.log`,
`/tmp/embermere-nov09-fenwatch-31-packages.log`,
`/tmp/embermere-nov09-native-editor.log`, and the explicitly rejected
`/tmp/embermere-nov09-rejected-placement-editor.log`.

## Adopted And Verified: 2026-11-09

Independent GUI-down mesh verification exited zero with its exact persisted
dimension/triangle/UV/hull/material marker and no dirty package or Python
error. The corrected placement helper compares numeric transforms and stable
paths/profiles/tags/folders, checks signature stability before spawning, and
verified every preceding actor unchanged before saving only the map. Two new
host regressions cover temporary-address exclusion and real property drift:
**21 host checks** now pass across the three focused test modules.

A second fresh GUI-down gate passed the complete **nine-mesh / 43-placement**
saved map, full-width corridors plus 75 cm and unchanged basin-only wet peat,
true exit zero, no Python errors. The clean final editor session passed all
**16 Fenwatch suites**, Causeway's **579 support / 1104 clearance** rays,
marker, six banks, twelve stone/twelve underlying-ground controls, nine basin/
two shelf joins, nine landmark/three arrival-ground controls, and the new
**six block / six adjacent ruin-island support** controls. No new ray ignores
actors; the adjacent supports do not claim hidden underside contact.

Viewport-only review at (0,5700,360), pitch-5/yaw90; (1850,5850,750),
pitch-18/yaw130; arrival (0,-450,260), pitch-5/yaw90; and east-ramp approach
(1100,6400,360), pitch-8/yaw0 retained open approaches and the pillar/lintel
silhouette. Roll0/FOV90 throughout. No other actor or lighting was retuned.
Floating PIE reached the Human Warrior picker and stopped cleanly. Synthetic
confirmation did not advance that picker in this small-window session, so
this is not character-HUD or physical walking/camera acceptance.

Causeway is All Saved outside PIE on real MCP8123, dirty packages empty,
133 tests freshly discoverable, no Python errors in the final session. The
earlier 133/133 native and separate Fenwatch 31-package evidence remains valid;
no C++ changed. Fenwatch and protected save/keeper hashes stay exact.
Accepted Causeway SHA256:
`6c13740b2519476c5002310466506613b72d6da4d6afad76a5d4885db9275234`.

Accepted additional logs: `/tmp/embermere-nov09-foundation-persisted.log`,
`/tmp/embermere-nov09-foundation-placement-editor.log`,
`/tmp/embermere-nov09-causeway-foundation-packages.log`, and
`/tmp/embermere-nov09-foundation-final-editor.log`.
