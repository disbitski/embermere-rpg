# Build Evidence: 2026-11-10

## Fallen Masonry

Fresh ruin-shoulder evidence informed a written brief before Blender MCP
authored one original two-fragment broken-beam scatter. It measures
500 x 300 x 140 cm, 88 triangles, one UV, zero non-manifold edges and two
convex hulls. It reuses saved stone/moss materials without edits. Editable
source, FBX, preview, metrics and builder are retained. See
`MARSHWARD_FALLEN_MASONRY.md` for the exact roles and acceptance limits.

One tagged BlockAll actor at (-1130,6700,120), yaw0, unit scale sits on the
west ruin shoulder. Numeric snapshots verified all preceding actors unchanged
before saving only the map. Causeway is **44 static placements/ten meshes**:
original22 plus22 dressing placements. Walking support, PlayerStart, full
buffers, markers, pillars, foundation courses and basin-only wet peat are
unchanged. No C++, gameplay, water, hazards, travel, save or startup-map changes.

## Verification

- Twenty-four focused host checks passed:16 corridor, five foundation and
  three masonry checks. New script syntax checks passed. The negative
  alternate-location fixture initially missed its segment endpoint; it was
  corrected to an actual horizontal-access intrusion, not a relaxed rule.
- Fresh GUI-down classic-FBX import passed exact size/88 triangles/one UV/
  two hulls/two reused materials, true exit zero, no Python errors:
  `/tmp/embermere-nov10-masonry-import.log`.
- Fenwatch's separate skeletal-material-first31-package aggregate passed
  all explicit markers, including saved Prowler loot,53 Fab/32 original-art
  and validators=31, true exit zero/no Python errors:
  `/tmp/embermere-nov10-fenwatch-31-packages.log`.
- Fresh complete Causeway package validation passed ten meshes, original22
  plus22 dressing, full-width corridors+75 cm and the unchanged wet-peat
  graph/assignment, true exit zero/no Python errors:
  `/tmp/embermere-nov10-causeway-packages.log`.
- The final clean editor passed all16 Fenwatch suites and full Causeway579
  support/1104 clearance, marker, six banks, twelve stone/twelve ground,
  nine basin/two shelf joins, six foundation/six adjacent-footing and nine
  landmark/three arrival-ground controls. Two new solid-fragment and two
  adjacent ruin-island support rays passed with no ignored actors. Adjacent
  support does not prove hidden contact under a collider. Snapshot:
  `/tmp/embermere-nov10-masonry-final-editor.log`, including exact success
  markers, `MASONRY_FINAL_DIRTY []` and the Causeway world path.
- Settled shoulder, arrival and east-ramp viewport-only views were inspected.
  An earlier camera-jump frame omitted existing geometry; setting the actual
  viewport, allowing a frame/tool boundary and recapturing restored the
  unchanged scene. Reject that incomplete frame, not valid content.
- Normal1280x754 floating PIE used the picker and confirmed Human Warrior
  HP100/100/Mana50/50 via synthetic Slate, then stopped. No physical control,
  painted player-camera or full walking acceptance is claimed.
- Fresh native discovery lists133; no new native run or C++ build was needed
  for this art-only change. November9's133/133 zero-warning/failure/skip run
  remains authoritative. Four accepted vendor physics-resave warnings remain
  confined to their known raw packs; no vendor package was resaved.

Fenwatch map and protected save/keeper hashes stayed exact. Causeway's
intentional accepted hash is
`b3ef2ca7fccfce69422b2e06938c04dabe6ac623ed5d350130ca6c42cf28a0af`.
The final real editor owns MCP8123, All Saved outside PIE, no dirty packages
or Python errors. Physical two-direction walking, moving-camera clearance
and first-visit south-turn wayfinding remain user checks, not completed gates.
