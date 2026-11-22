# Marshward Causeway Render And Collision Baseline

Measured in the saved `/Game/Maps/L_MarshwardCauseway_Greybox` on November 22,
2026, before any still-water work. No map, asset, collision, or settings change
was made. The inventory is reproducible; this editor session did not provide
usable frame draw counts or a gameplay performance baseline.

## Placed Inventory

| Quantity | Unique installed meshes | All placed copies |
| --- | ---: | ---: |
| Meshes | 19 (18 imported, one Engine Cube) | 51 (22 greybox roles, 29 dressing) |
| Unreal LOD0 triangles | 3,840 | 9,370 |
| Unreal LOD0 render sections | 39 | 89 |
| Retained source triangles, excluding Engine Cube | 3,792 | 8,890 |

Each installed mesh has one render LOD and Nanite disabled. The retained original
greybox marker is audited separately: 36 LOD0 triangles, one section, one authored
box, zero placements. It is one of the 19 validated imported mesh packages but
does not enter the placed totals. The Engine Cube contributes 48 Unreal LOD0
triangles per copy across ten placements and has no project source row.

The 89 effective material slot assignments resolve to 11 distinct Unreal
`Material` assets, zero `MaterialInstance` assets. The assignments include
component overrides, such as one wet-peat assignment on the basin catch. Moss
recurs in 20 slots and greybox stone in 14. Six short reed clumps contribute
3,696 placed triangles; two tall clumps contribute 528; the low scrub contributes
540. Those three meshes account for 4,764 placed triangles (about 51%). This is
an inventory concentration, not evidence that foliage is a frame bottleneck.
Sections and slot assignments do not predict actual draws: visibility,
batching, render passes, shadows, and editor drawing can all change frame totals.

| Collision profile | Actors | Placed simple shapes on enabled components |
| --- | ---: | --- |
| `BlockAll` | 37 | 20 boxes and 32 convex hulls |
| `NoCollision` | 14 | 0 |

The boxes come from ten cubes, seven spans, and three carved markers. The
largest group of hulls is the four three-stone clusters with 12 hulls in total.
These counts come from each placed component's mesh body setup and collision
policy. They do not measure cooked triangle geometry, query cost, broadphase
activity, or continuous player-volume clearance.

## Editor Frame Probe

Unreal 5.8.0 Development editor, Apple M4 Max, Metal SM6. With PIE stopped, set
the actual editor camera, read back the transform, and wait at least four seconds
before running `csvprofile frames=60` through the Unreal Output Log. Each file
had all 60 frame rows and the terminal header and metadata. The table excludes
the first five samples from the timing summaries. Every pose uses pitch -5,
roll 0; coordinates are centimeters.

| Pose | Position | Yaw | Editor FrameTime median (range), ms | Reported GPUTime median, ms |
| --- | --- | ---: | --- | ---: |
| Arrival | (0, -450, 260) | 90 | 340.9 (333.3–372.5) | 0.981 |
| Crossing | (0, 3300, 360) | 90 | 340.7 (333.3–370.8) | 0.985 |
| Ruin approach | (0, 5700, 360) | 90 | 343.6 (333.3–364.7) | 0.985 |
| East return | (2700, 3600, 260) | 180 | 347.1 (333.3–370.2) | 0.982 |

All frame rows reported zero for `RHI/DrawCalls`, `RHI/PrimitivesDrawn` and
`DrawCall/SlateUI`. A separate arrival control requested Unreal's viewport-only
capture during profiling: it returned a 3051 × 1616 PNG and the capture's
reported `GPUTime` peaked at 78.829 ms, but all three counters stayed zero.
Treat draw calls and submitted primitives as **unavailable from this setup**,
not zero content cost. The control PNG is an output size, not the internal
render resolution during the timed runs. The CSV's 1280 × 720 system resolution
metadata does not establish the editor render resolution either. The unattended
editor frame pacing and reported GPU times include editor behavior and are not
gameplay FPS or map-only GPU costs.

To repeat the inventory, run `Scripts/audit_marshward_budget_unreal.py` with
`runpy` in the Unreal Output Log's cleared Python mode on the settled, clean map
outside PIE. Check `EMBERMERE_MARSHWARD_BUDGET_SUCCESS`, the per-mesh JSON rows,
material classes, and totals. For frame sampling, use the
[Unreal CSV Profiler](https://dev.epicgames.com/documentation/en-us/unreal-engine/csv-profiler?application_version=4.27)
and inspect the local, untracked captures under `Saved/Profiling/CSV`.
Before optimizing any asset based on frame cost, get a controlled playable run
with a known render resolution and verify that the chosen draw counters respond
to rendered content. This probe does not justify merging, reducing, or removing
any of the current art.

## Verification

65 focused host checks passed, including three new inventory accounting
regressions. The initialized Causeway suite passed 579 route-support and 1,104
clearance rays, the positive marker control, and all preceding asset, arrival,
and return controls, without Python errors. Floating PIE reached the Human
Warrior picker and stopped; synthetic confirmation did not reach the HUD.
The editor returned to Causeway, All Saved outside PIE. Both map hashes and
the protected save and keeper hashes were unchanged. No package import,
engine build, full native run, or Fenwatch rerun was needed for this audit.
The collision checks are sampled; physical walking, moving-camera clearance,
and first-visit wayfinding remain unverified.
