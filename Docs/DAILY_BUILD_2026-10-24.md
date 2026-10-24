# October 24 Daily Build

## Blocked Prowler Pursuit

No voluntary physical Prowler or quest playtest state was supplied. Clean
floating PIE reached the normal Human Warrior HUD without advancing combat
or loading the protected Chronicle slot. Code review of the saved Prowler's
chase path found that `MoveTowardTarget` marked a movement frame after every
attempted swept step, even when solid collision kept the capsule still.
That flag selects Walk, so a blocked Prowler could walk in place against a
wall. This is separate from the accepted melee-cover damage preflight.

`Embermere.Enemy.BlockedPursuitVisual` starts an isolated world with the
saved Prowler and its real Idle/Walk assets. The fixture lets the capsule
settle against a registered wall, then proves the next sweep has no
meaningful displacement. With production unchanged, only the stale Walk
assertion failed. The AI now sets its visual movement flag from achieved
displacement, not the attempted step. The same fixture passes Idle while
blocked and movement plus Walk after the wall moves away. Aggro, sweep,
leash, attack, health, and reward authority did not change. See
`Docs/ENEMY_BLOCKED_PURSUIT_VISUAL_CONTRACT.md`.

## Verification

- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked after the final code change.
  Fresh discovery returned **126** tests; full automation passed **126/126**
  without failures, warnings, or skips.
- The GUI-down UnrealEditor `-ExecutePythonScript` run exited zero. Dedicated
  `/tmp/embermere-oct24-31-package-aggregate.log` contains exact
  skeletal-material, placed-Prowler-loot, **53 Fab / 32 original-art**, and
  `validators=31` success markers, without `LogPython: Error`.
- All **16** initialized-world suites passed with exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` and no
  Python error. Clean floating PIE reached the Human Warrior HUD, then
  stopped. `/Game/Maps/L_Embermere_Prototype` was All Saved outside PIE;
  native dirty-package inspection returned `[]` with real Unreal MCP 8123.
- The protected save and unrelated keeper material retained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The fixture proves the animation-selection rule, not physical Prowler
pathing or painted animation feel near the ruin. Four known raw-vendor
physics-resave warnings remain confined to FieldGrass, River_Rock,
Medium_Boulder, and HillTree; none was resaved.
