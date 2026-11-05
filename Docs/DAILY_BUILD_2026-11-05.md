# Daily Build: 2026-11-05

## Basin Surface Study

Fresh arrival evidence showed a uniform greybox basin beneath the Causeway.
New project-owned `M_MarshwardWetPeat` adds opaque world-space peat/mud
mottling and modest roughness variation to **only** `Marshward_BasinCatch`.
The graph has ten nodes and one computational value-noise level. It adds no
texture assets, water, displacement, physical material or gameplay authority.
All 40 placements/seven meshes, route materials, support/collision geometry,
PlayerStart and Blender sources are unchanged. No C++ build was needed.

## Evidence

- Fresh material creation exited zero with its exact success marker and no
  Python/material compile error. Two earlier unsaved graph attempts failed
  pin-name assertions; they were rejected. Discovering the Noise and Saturate
  input names fixed authoring. An initial unsaved assignment validator used
  an unavailable displacement enum; the override was restored, then the
  validator was corrected to guard actual tessellation/WPO state.
- Independent fresh Causeway validation exited zero with exact wet-peat,
  seven-mesh/40-placement and full-width-corridor markers, no Python errors.
- Five fresh-process negative controls rejected wrong roughness, a broken
  mask link, enabled tessellation, route material leakage and missing basin
  assignment; every mutation was restored and no package was saved.
- Fenwatch's separate skeletal-material-first 31-package aggregate exited
  zero with exact skeletal-material, placed-Prowler, 53 Fab / 32 original-art
  and validators=31 markers, no Python errors. Four accepted raw-vendor
  physics-resave warnings remain; no vendor package was resaved.
- Thirteen host corridor/asset tests passed. Fresh native automation passed
  133/133 with zero failures, warnings or skips. Dirty packages afterward
  were exclusively `/Temp/Untitled_*`; a clean relaunch cleared them without
  saving any unrelated content.
- The final clean editor session passed all 16 Fenwatch suites, Causeway's
  579 support / 1104 clearance rays, positive marker, six bank controls,
  twelve stone and twelve underlying-ground controls, without Python errors.
- Arrival, overview and close editor-camera viewport captures were inspected.
  Normal floating PIE confirmed Human Warrior, HP100/100/Mana50/50, then
  stopped cleanly. A PIE viewport capture was unavailable; physical camera
  contrast and the complete walking loop remain unverified.
- Causeway is All Saved outside PIE on real MCP8123 with 133 freshly
  discoverable tests. Fenwatch, the protected save and keeper material retain
  their exact recorded hashes. No Chronicle action was used.

## Logs And Remaining Work

Accepted logs: `/tmp/embermere-nov05-wet-peat-create-accepted.log`,
`/tmp/embermere-nov05-wet-peat-negative.log`,
`/tmp/embermere-nov05-causeway-packages.log`, and
`/tmp/embermere-nov05-fenwatch-31-packages.log`.
See `Docs/MARSHWARD_WET_PEAT.md` for the material contract. Further art should
respond to observed off-route bank/basin seams while preserving full-width
buffers. Connected travel, finished boundaries/water and physical traversal
remain outside this study's proof.
