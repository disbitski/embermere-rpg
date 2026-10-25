# Daily Build: 2026-10-25

## Milestone

Keep Tab targeting quiet throughout the player death hold. The death handler
already cleared the combat target, but the controller's still-bound Tab path
could immediately reacquire a nearby live target. The new
`Embermere.Input.DeadTargetCycleSafety` test failed on both reacquisition and
an extra chat line, then passed after `CycleTarget` required living possessed
Stats. Normal candidate order and post-respawn selection are unchanged; no
combat, quest, inventory, service, or save authority moved.

## Verification

- Before edits, first-class Unreal MCP found the real editor outside PIE on
  `/Game/Maps/L_Embermere_Prototype`, All Saved, with 126 discoverable tests.
  A clean floating PIE reached the Human Warrior HUD. No voluntary physical
  death/combat state was supplied.
- The new regression failed first: dead Tab reacquired the practice target and
  posted one extra target line. Process-local Xcode 26.1.1
  `EmbermereEditor Mac Development -NoHotReloadFromIDE` compiled and linked
  after the controller guard.
- Fresh discovery found 127 Embermere tests; the complete native run passed
  **127/127**, with zero failures, warnings, or skips.
- The isolated GUI-down UnrealEditor `-ExecutePythonScript` aggregate exited
  zero. `/tmp/embermere-oct25-31-package-aggregate.log` contains the exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers with no `LogPython: Error`. The first sandboxed
  commandlet stalled in AppKit before log creation; the identical host-permission
  rerun produced the authoritative result.
- All **16** initialized-world suites passed through first-class Unreal MCP
  with `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no Python errors.
  Final clean floating PIE confirmed living Tab selected the Fenwatch practice
  target, then stopped. Fenwatch is All Saved outside PIE with MCP 8123.
- The protected Chronicle save and unrelated keeper material retained their
  original SHA256 hashes. No content package was changed.

## Open Gate

During an organic physical death hold near a living target, press Tab and
confirm no target frame, cyan ring, or target chat returns before recovery;
after respawn, a new Tab should select normally. The isolated explicit death
callback and synthetic Slate are not physical keyboard or combat acceptance.
