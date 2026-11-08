# Daily Build: 2026-11-08

## Integrated Loop Review

Eight fresh normal-height viewport-only editor views covered both directions
of the crossing, ruin reverse/east-ramp approach, north/south eastern return,
and west/east southern connector. Exact poses and observations are retained
in `Docs/MARSHWARD_LOOP_REVIEW.md`. The central walking lane and landmark
references remain open in these stills; no concrete blocking/framing defect
justified changing accepted content.

The same-color south/east platforms do not supply a strong material turn
cue in a straight still. First-visit wayfinding there remains a focused user
playtest question, not a proven defect or permission to add another marker.
All 41 placements/eight meshes, PlayerStart, support/collision, full-width
buffers, basin-only wet peat, lighting and the arrival guard are unchanged.
This slice changes documentation only; no source or asset was rebuilt.

## Evidence

- Existing full Causeway initialized-world validation passed: 579 support /
  1104 clearance rays, marker, six banks, twelve stone/twelve ground controls,
  nine basin/two shelf joins and nine landmark/three arrival-ground controls.
  The map was already loaded and settled; no same-submission load/probe was
  used and no support tolerance was relaxed.
- Sixteen host corridor/asset checks passed. Fresh discovery listed 133
  native tests, without a new native run.
- Normal floating PIE confirmed Human Warrior at HP100/100/Mana50/50 and
  stopped cleanly. This is startup/teardown proof, not physical traversal.
- The existing real editor stayed healthy outside PIE with clean packages
  and no Python errors; no restart was needed. Snapshot:
  `/tmp/embermere-nov08-loop-editor.log`, including November 8 success markers
  and `LOOP_REVIEW_FINAL_DIRTY []`.
- Both map hashes and protected save/keeper hashes remained exact. Causeway
  retained `f781f2922a2749b824cbcdc27d371947ef1a63ad7c9e20da45da40e7d820dab6`.
  Blender MCP scene inspection was healthy; no model was changed.
- No C++ build, import, package commandlet or Fenwatch/full-native rerun was
  performed for this documentation-only review. November 7 Causeway package
  and November 6 full native/Fenwatch evidence remain authoritative. The four
  accepted raw-vendor physics-resave warnings remain unrelated and untouched.

## Remaining Acceptance

Physical two-direction walking, south-turn wayfinding, ramp feel and moving-
camera clearance remain open. Finished outer boundaries, water gameplay,
connected travel and cross-level persistence are not proven. Any further
ruin dressing must preserve the reviewed approach, return and access lanes
and begin with fresh view evidence plus a written asset brief.
