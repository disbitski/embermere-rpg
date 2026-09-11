# September 11 Daily Build

## Scope And Setup

Repair verified saved SkeletalMesh usage flags on project-owned Fenwatch NPC
materials. No C++ gameplay, mesh, animation, collision, transform, map, or save
schema changes. Preserve September 10's placed-Prowler reference repair.

The ACTIVE 8 AM America/New_York heartbeat targets existing task
`019efc2b-b36c-71f0-8892-8e4f80829fc9`, whose verified cwd is
`/Users/wizard/Documents/Unreal Game`. Project config denies desktop access,
disables computer-use plugins/bridges, and binds Unreal MCP to localhost 8123.
The configured Blender executable exists; its live bridge was not queried
because no original art was planned. No desktop control, broad editor capture,
macOS security changes, or unlock request was used.

## Saved-State Regression

The initial rendering editor reported all seven base flags true and zero dirty
packages despite startup warnings. A genuine PythonScriptCommandlet with
NullRHI, loading materials before rigs or map, instead found six false:

- `M_FenwatchArmsmasterSkin`
- `M_FenwatchQuartermasterSkin`
- `M_EmbermereTimber`
- `M_Waystone`
- `M_WaystoneEmber`
- `M_WaystoneMoss`

The local keeper skin already had the flag but was an existing uncommitted
resave. A temporary HEAD package copy proved its committed flag false too.
The iron slot uses `M_EmberLampIron`, a MaterialInstanceConstant whose
`/InterchangeAssets/Materials/FBXLegacyPhongSurfaceMaterial` parent supplies
valid usage. Neither the instance nor its engine/plugin parent was changed.

Engine source explains the discrepancy: SetMaterialUsage can automatically
enable editor usage and compile shaders, but MarkPackageDirty may fail during
loading. Runtime/non-editor paths cannot rely on that repair. The new
validator uses the read-only HasMaterialUsage query, requires NullRHI, reads
materials first, and checks all eighteen actual slot assignments on three rigs.
It is the first of 23 sequential package validators.

The first standalone regression/preparation calls failed on Python helper
imports and saved nothing. Explicit script-directory setup corrected those
entry points. The corrected regression then failed on exactly the six missing
local flags before preparation. The fresh post-repair regression passed.
An earlier exploratory GUI query also incorrectly tried a base-material
property on the iron instance; the corrected typed query was read-only.
These exploratory errors are not described as clean final validation runs.

## Deliberate Repair

The shared preparation helper preflights project ownership, supported types,
and instance compatibility, then enables and explicitly saves only missing
base usage flags. All three rig import entry points call it before mesh work.
The standalone preparation script avoids all reimport, map, and rig resaves.
A fresh second pass returned `saved=[]`; foreign/type rejection, duplicate
paths, and no-dirty-package checks also passed.

Six packages were corrected in the working tree. For the keeper, HEAD was
exported to an isolated temporary project with the same canonical `/Game`
path. Only its missing flag was prepared and saved. A second fresh process
confirmed the flag persisted and saved nothing. This generated baseline
correction was staged directly, preserving the existing working file:

- Unchanged working keeper SHA256:
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`
- Corrected HEAD-derived keeper SHA256:
  `541ecbc1cf0f960ec0067cb9e778521134d4e904a093b9224a62de853f6fe550`

The original journey remains protected, never loaded or overwritten:
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.

## Final Verification

No-hot-reload build succeeded (target up to date). All 96 isolated automation
tests passed with zero warnings, failures, skipped, or in-process tests. All
23 fresh package validators emitted their success markers, including the
journal emblem's `FENWATCH_JOURNAL_VALIDATION_OK`; the aggregate reported 23.
Fresh material regression, preparation idempotence/safety, isolated keeper
reload, and all six live trace suites passed with explicit markers and no
Python errors in those runs. Fresh MCP discovery also returned 96 tests.
The world remains 53 grounded Fab actors and 24 original-art placements.

After restart the real UnrealEditor owned 127.0.0.1:8123, and the targeted
SkeletalMesh usage warnings were absent. Other engine/vendor warnings are not
claimed fixed. This is saved-state and editor-runtime evidence, not a complete
cooked/packaged-game verification.

## PIE And Remaining Gates

A fresh Human Warrior confirmed creation, closed Inventory, and walked from
PlayerStart to Mara at normal time dilation. Original F accepted First Signs
at the Ruin, with zero progress/rewards. All three NPCs retained exact six-slot
materials and NoCollision; actual Idle playback positions changed:

| NPC | First Clock | Later Clock | Loop Duration |
| --- | --- | --- | --- |
| Quartermaster | 3.138898 | 1.876357 | 4.0 seconds |
| Armsmaster | 0.738897 | 2.676355 | 3.2 seconds |
| Keeper | 1.138899 | 3.076359 | 3.6 seconds |

These are samples across looping playback, not an elapsed-time measurement.
Viewport-only captures showed resolved skin, timber, stone, moss, ember and
iron on the three NPCs. Captures omit HUD and may render editor-world state
even while PIE runs; only runtime telemetry proves the active PIE animation
and material bindings. Physical pose/readability and final HUD review remain
user gates. One initial CaptureViewport call needed an explicit empty
annotation configuration despite its optional-looking schema.

Three short waypoint attempts near Mara timed out. At
(-2003.434,-710.575,90.150), non-overlapping capsule sweeps identified the
existing `FabPass_Village_Crate_B` at (-2060,-650,0) and
`Embermere_EmberLamp_Mara_01` at (-1970,-775,0), not an NPC collision change.
Cardinal probes hit within 0.094-1.273 cm. A diagonal exit toward
(-1830,-530), then (-1640,-200), (-1420,-450), (-1420,-700), succeeded and
normal F opened Fenwatch Training. Continuing toward (-1430,-1200) opened
Fenwatch Supplies. No scenery, gameplay positions, speed, health, or progress
were injected or changed. This is not approval of every possible shortcut.
An exploratory `GameplayStatics.break_hit_result` lookup was unavailable in
Python; the corrected read used the existing `HitResult.to_dict()` pattern.

Slate Train and Buy clicks returned true, but authoritative copper stayed 40,
XP stayed 0, and the bag stayed empty. An Enter probe also changed nothing.
Do not call these accepted transactions or a proven gameplay regression.
Next inspect Unreal-owned hit-test geometry/focus and input delivery. Existing
transaction automation still passed. A final I/I handoff restored hidden
cursor and unsuppressed move/look; all probe callbacks and forced keys were
clear. The three Prowlers retained complete native role sets and active Idle;
September 10 remains the full six-role/combat acceptance baseline.

The original save and existing keeper working-file hashes are unchanged.
Unreal is left out of PIE on `/Game/Maps/L_Embermere_Prototype`, with no dirty
content/map packages and the real editor MCP listener active. No test saves,
Config normalization, vendor packages, Field Notes drafts, or probes belong
in this commit. The next run should retain the material regression and
diagnose the service click mismatch without desktop-control workarounds.
