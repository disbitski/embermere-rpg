# September 10 Daily Build

## Scope And Readiness

One bounded saved-data repair: restore native mesh and six animation-role
references on the three actual placed Prowlers. No C++ gameplay, model,
material, collision, transform, or save-schema change was needed.

The existing ACTIVE heartbeat targets task
`019efc2b-b36c-71f0-8892-8e4f80829fc9`, whose verified working directory is
`/Users/wizard/Documents/Unreal Game`, at 8 AM America/New_York. The project
config points Unreal MCP to localhost 8123, disables desktop-control plugins
and bridges, and sets default desktop access to deny. The configured local
Blender server executable exists; its live bridge was not queried because
no original art was planned. No desktop control, broad editor capture,
security change, or unlock request was used.

## Reproduced Data Gap

The new validator failed in a fresh commandlet before repair: each placed
`Starter_Enemy_01/02/03` had empty `VisualSkeletalMesh`, `IdleAnimation`,
`WalkAnimation`, `RunAnimation`, `AttackAnimation`, `HitAnimation`, and
`DeathAnimation`. All three returned false for a complete native role set.
The Blueprint CDO was complete; placed component mesh, Idle, and transforms
were already correct.

`apply_marsh_prowler_presentation_mcp.py` originally assigned native role
references only to the CDO, then configured only placed skeletal components.
It now configures and verifies both. A separate guarded map-only mode requires
the exact three labels/class/map, canonical CDO and component state, and empty
or already-canonical native references before mutation. Only the seven
reviewed references per instance were written today; only the map was saved.
Unexpected custom references cause preflight rejection. Loaded and unloaded
MCP soft-reference representations are normalized for comparison.

The new `validate_marsh_prowler_instances_unreal.py` checks all three instances
plus CDO, exact mesh/role assets, shared Skeleton, positive sequence duration,
matched native/component offset, scale and rotation, default Idle, and the
unchanged CharacterMesh/QueryOnly collision profile. It is now part of the
22-module fresh-package aggregate, alongside all previous checks.

## Live Routing And Gameplay

A fresh Human Warrior accepted Mara through the original F owner, at time
dilation 1.0. Bounded Unreal-only key callbacks refreshed/clicked the current
Slate viewport before measured inputs. A separate read-only observer sampled
the actual SingleNode animation asset and playback position. No position,
health, quest, reward, speed, or time-dilation fixture was injected.

All six roles advanced on each of the three saved actors. Representative
observed clock intervals in seconds, not just default-asset assignments:

| Actor | Idle | Walk | Run | Attack | Hit | Death |
| --- | --- | --- | --- | --- | --- | --- |
| 01 | 0-1.568 | 0-1.001 | 0-0.667 | 0-0.668 | 0.333-0.533 | 0.333-1.333 |
| 02 | 0-1.564 | 0-1.001 | 0-0.762 | 0-0.667 | 0.334-0.533 | 0.333-1.333 |
| 03 | 0-1.588 | 0-1.001 | 0-0.667 | 0-0.667 | 0.333-0.533 | 0.333-1.333 |

The roles came from real chase, retaliation, hotbar damage, defeat, and leash
return. Each death cleared the target, completed the non-looping animation,
hid the body, and returned to full-health Idle at home after the existing
12-second respawn. Separate pull/retreat checks produced Run and exact home
on all three actors without another defeat/reward. All three retained exactly
48 non-colliding target-ring segments; the selected 03 ring alone was visible
at the final query.

The distinct kills were 02, 03, then 01. Each real hotbar sequence applied
36, 48, and 16 damage and yielded one tonic plus one Mara objective credit.
No early XP/copper was granted. After the second kill, real F at the well
restored 28 health to 100 while preserving 50 mana, two tonics, 2/3 progress,
0 XP, and 40 copper. After all kills and return checks, original Mara F paid
exactly 125 XP, 20 copper (40 -> 60), and one Recruit Pack. Repeat F paid
nothing. Derived level 2 preserved 30 missing health at 80/110 and full 53 mana,
with three tonics and completed 3/3.

Two short approaches toward 01 timed out at solid geometry; neither changed
enemy/player health or progress. A southern walking bypass completed its
pull/return/kill. The first 03 retreat stopped too close and allowed further
retaliation; a later continuous northward retreat proved Run/home. No art or
combat rule was moved to accommodate the probes. This is not physical camera
approval, a perfectly clean route, or general pathfinding acceptance.

J -> M -> I -> I retained the completed state and returned cursor-hidden,
unsuppressed move/look input. Chronicle displayed live Human Warrior Level 2
and the original slot's exact missing-version warning with Load disabled;
neither save nor load was clicked. Both bounded callbacks and forced keys were
clear before teardown. UnrealEditor PID 43777 is left out of PIE on the correct
map with zero dirty packages and the real MCP listener on 8123.

## Verification

- No-hot-reload Editor build succeeded.
- Isolated automation: 96 succeeded, zero warnings/failures/not-run/in-process.
- All 22 fresh-package validators and the aggregate success marker passed,
  with no LogPython errors. World counts remain 53 Fab and 24 original art.
- All six initialized-editor trace suites passed: well, notice board,
  workshop, cottage, stall, and road.
- The restarted real UnrealEditor owns localhost 8123; fresh discovery is 96.
- Evidence: `/tmp/embermere-sep10-prowler-before2.log`,
  `/tmp/embermere-sep10-packages.log`, `/tmp/embermere-sep10-tests/`, and
  editor `SEP10_ANIMATION` / route `SEP9_* sep10_*` logs.

Exploratory corrections are not hidden: the first standalone validator needed
its script directory on sys.path; its initial NoCollision assertion was an
incorrect assumption about the existing CharacterMesh profile, not a reason to
change collision. One MCP property probe used an invalid guessed subobject path;
the resolved CDO/component query corrected it. An unloaded soft reference came
back as a string and required normalization before repair. The first temporary
animation observer used a nonexistent Python is_hidden method, then switched
to the reflected hidden property. The final package/test runs have no Python
errors. Exit zero alone had not distinguished the earlier failed checks.

## Protected State

The original `Saved/SaveGames/EmbermerePrototype.sav` is preserved byte-for-byte:
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
No load, overwrite, inferred version, migration, or recovery was attempted.
Current save version remains 3; unstamped history remains rejected read-only.
Existing Config normalization, keeper-material resave, unpublished FieldNotes,
and temporary probes remain outside this change.

Physical W/S cancellation, held right-mouse, Ctrl+M, final HUD pixels, and
normal-camera animation quality remain user gates. Engine-owned input and
animation telemetry are not substitutes for those approvals.

## Next Start

Retain this placed-instance check and all six-role runtime evidence. Ask for
normal-camera animation/HUD and physical input review without requiring a
desktop unlock or weakening the MCP-only boundary. If no concrete play issue
is reported, a bounded next investigation is the existing startup warnings
about SkeletalMesh usage on project-owned shared Fenwatch materials. Inspect
their saved flags and actual rig dependencies in a fresh commandlet before
deciding whether an intentional package repair is warranted. Do not bulk
resave materials, touch raw vendor assets, or equate editor auto-compilation
with packaged-build readiness. Current tests have zero test warnings; existing
engine/vendor/material startup warnings are not claimed resolved.
