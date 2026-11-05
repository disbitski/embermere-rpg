# Marshward Wet Peat

## Ownership And Brief

The arrival view still shows a uniform pale basin beneath the raised route.
Create `M_MarshwardWetPeat` as a project-owned opaque, non-metallic surface
study for **only** `Marshward_BasinCatch`. Use broad world-space peat/mud
mottling and restrained wet/dry roughness. Keep the pale walking surfaces
distinct. No water, transparency, normal animation, displacement, physical
material, friction, collision or gameplay changes are authorized.

Use one level of computational value noise at scale 0.003 per centimeter,
clamped to 0..1, shared by base-color and roughness interpolation. Linear
wet/dry colors are (0.035,0.045,0.027) and (0.11,0.095,0.058); roughness is
0.42..0.82. This is a bounded first material study, not a performance claim
or a finished water surface. It adds no texture assets or mesh changes.

Preserve every actor transform, mesh, profile, support shape and PlayerStart;
the map remains 40 static actors/seven meshes. Only the basin component's
slot-zero override may change. Do not edit the shared greybox basin material
or any Blender mesh/material. Fenwatch and protected user files stay intact.

## Gates

Validate the persisted opaque graph, exact constants and links, absence of
displacement/opacity/emissive/physical-material ownership, and basin-only
assignment in a fresh process. Retain the separate Causeway package gate,
full corridor/bounds and initialized collision controls. Inspect fresh
arrival/overview views and clean normal PIE. Painted contrast under physical
camera movement and the complete walking loop remain user checks.

## Verification: 2026-11-05

Fresh material creation and independent Causeway package validation exited
zero with explicit success markers and no `LogPython: Error`. Five isolated
negative controls rejected wrong roughness, a missing color-mask link,
enabled tessellation, assignment to the arrival route, and missing basin
assignment. Each restored its transient mutation; no package was saved by
that test. The separate Fenwatch 31-package aggregate retained its exact
skeletal-material, placed-Prowler and 53 Fab / 32 original-art markers.

Thirteen host checks and all 133 native tests passed without failures,
warnings or skips. After clearing only disposable native-test worlds by
relaunching, the clean session passed all 16 Fenwatch suites and Causeway's
579 support / 1104 clearance rays, marker control, six bank controls and
twelve stone/twelve underlying-ground controls. Arrival, overview and close
editor-camera viewport evidence showed basin mottling beneath the distinct
pale route. Normal picker/Human Warrior floating PIE started/stopped; a
viewport capture requested during PIE was unavailable, so no painted player-
camera acceptance is claimed. Physical traversal/contrast remain open.

Causeway remains All Saved outside PIE with 133 freshly discoverable tests.
Fenwatch and protected user hashes stayed exact. No C++ or mesh rebuild was
needed. Logs: `/tmp/embermere-nov05-wet-peat-create-accepted.log`,
`/tmp/embermere-nov05-wet-peat-negative.log`,
`/tmp/embermere-nov05-causeway-packages.log`, and
`/tmp/embermere-nov05-fenwatch-31-packages.log`.
