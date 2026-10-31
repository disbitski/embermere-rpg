# Daily Build: 2026-10-31

## Milestone

Tab targeting could select a nearer hostile through solid cover even though
targeted abilities and initial Prowler aggro now require a clear sightline.
Target candidate filtering now rejects a blocking Visibility ray at the
existing 90 cm combat/interaction offset. A clear alternate can be selected;
moving the obstruction clear returns the original hostile to normal cycling.
Target order, combat execution, quest, loot, and save authority are unchanged.

## Verification

- Fresh normal-route floating PIE reached the Human Warrior HUD. In an
  isolated started world, the new
  `Embermere.Input.CoveredTargetCycle` first failed because the covered nearer
  target won. The fixture positively proved the solid wall blocked that
  target while the alternate ray was clear. After the fix, Tab selected the
  clear hostile and selected the nearer one when the wall moved away, with
  no copper, XP, or quest drift. The first fixture had a too-tight alternate
  ray and was corrected before accepting the genuine red result.
- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked. Final fresh discovery and full
  native automation passed **133/133** with zero failures, warnings, or skips.
- The GUI-down **31-package** aggregate exited zero with exact skeletal-
  material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers, with no `LogPython: Error`. Evidence:
  `/tmp/embermere-oct31-31-package-aggregate.log`.
- All **16** initialized-world suites passed with the exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` marker and no Python error.
  Clean floating PIE reached the Human Warrior HUD and stopped. Fenwatch is
  All Saved outside PIE on `/Game/Maps/L_Embermere_Prototype`, with real
  UnrealEditor owning MCP 8123 and 133 discoverable tests.
- The protected Chronicle save and unrelated keeper material retained their
  exact recorded SHA256 hashes. No content package was intentionally changed.

## Open Gate

A voluntary physical Tab check near the solid ruin pillar should skip a
covered hostile and pick a clear alternate, then restore the covered hostile
to normal cycling when the line clears. The isolated native test does not
prove physical keyboard routing or painted target-ring/camera behavior.
Earlier voluntary death-hold, Inventory, first-Prowler, service, and Ledger
checks remain open; do not force gameplay or load the protected save to
replace them.
