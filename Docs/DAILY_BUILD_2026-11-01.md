# Daily Build: 2026-11-01

## Scope

Documented a first expansion direction beyond Fenwatch. This is a
documentation-only slice; no C++, map, mesh, Blueprint, quest, service, save,
or gameplay state changed. `Docs/FUTURE_LEVELS_PLAN.md` is a proposal, not an
accepted level or asset.

## Verification

- The accepted production baseline remains `e5bc35c` from October 31.
- First-class Unreal MCP forced discovery and listed **133** Embermere tests.
  Unreal-owned Slate showed `L_Embermere_Prototype` and All Saved; PIE was
  stopped. This is discovery/read-only inspection, not a fresh full test run.
- Blender MCP returned scene information successfully. No Blender scene or
  asset was edited.
- Protected save SHA256: `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  Unrelated keeper material SHA256:
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
- The last full native, package, live-world, and PIE gates remain the October
  31 evidence in `Docs/DAILY_BUILD_2026-10-31.md`; they were not rerun for
  this documentation-only change.

## Next Slice

Use fresh viewport evidence to settle the Causeway arrival direction, then
make a separate greybox-map milestone or one briefed, validated Blender kit
piece. Keep Fenwatch's accepted map and all existing gameplay ownership
unchanged. Voluntary physical playtest gates remain open.

## Later: Causeway Greybox Implementation

The user approved beginning the second level with Blender-authored assets.
The separate `/Game/Maps/L_MarshwardCauseway_Greybox` now has 22 greybox mesh
placements and a normal PlayerStart. Three kit pieces (span, ramp, waymarker)
were built and rebuilt through Blender MCP with 12/12/36 triangles, one UV
channel each, and zero non-manifold edges. The saved map has no enemies,
quests, services, travel triggers, or new persistence behavior. Startup maps
and Fenwatch content remain unchanged; no C++ build was needed.

Fresh classic-FBX import and an independent GUI-down Causeway package gate
passed with their exact success markers, exit zero, and no `LogPython: Error`.
The existing skeletal-material-first Fenwatch 31-package gate also passed
with the exact 53 Fab / 32 original-art and validators=31 markers. Evidence:
`/tmp/embermere-marshward-kit-import.log`,
`/tmp/embermere-marshward-package-validation.log`, and
`/tmp/embermere-marshward-fenwatch-31-packages.log`.

All 133/133 native tests passed without failures, warnings, or skips. A clean
post-relaunch session passed all 16 Fenwatch live-world suites and Causeway's
579 support / 1,104 clearance rays plus positive solid control, with no Python
error. Initial map creation used established EditorLevelLibrary APIs and
emitted two deprecation warnings; the fresh validators had no Python errors.
Normal floating PIE reached the Human Warrior HUD on the new map and stopped.
Arrival and overview viewport captures were inspected. Physical traversal,
camera feel, boundaries, travel, and cross-level respawn/save behavior are
not accepted by these checks. Protected hashes and the Fenwatch map's hash
stayed exact. See `Docs/MARSHWARD_CAUSEWAY_GREYBOX.md`.
