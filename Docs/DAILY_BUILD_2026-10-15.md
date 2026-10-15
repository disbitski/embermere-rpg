# October 15 Daily Build

## Dead-State F Interaction

No voluntary trainer, death-hold, first-Prowler, or two-quest playtest state
was supplied. Clean floating PIE showed normal Human Warrior PlayerStart F
feedback. A source review found that the controller's F route required a pawn
but did not reject a dead one. Closing panels on death was insufficient: a
new press could still search candidates and interact.

`Embermere.Input.DeadInteractionSafety` first failed on two old behaviors:
dead F posted the empty-range chat line, and dead F near a service reopened
the vendor panel and dialogue. The controller now rejects F before candidate
search when there is no possessed character, no Stats component, or dead
Stats. The green isolated test covers both dead routes, zero wallet/XP/bag/
quest mutation, and normal nearby F after respawn. Quest, service, HUD,
inventory, and save authority did not change.

## Verification

- Process-local Xcode 26.1.1 built and linked EmbermereEditor Mac Development
  with `-NoHotReloadFromIDE`; system Xcode 27 stayed unchanged.
- Fresh discovery found **118** tests; focused regression and the full native
  suite passed **118/118**, zero failures, warnings, or skips.
- GUI-down `-ExecutePythonScript` aggregate exited zero. Dedicated
  `/tmp/embermere-oct15-31-package-aggregate.log` contains exact skeletal-
  material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers with no `LogPython: Error`.
- All **16** initialized-world suites passed with exact success marker and no
  Python error in the final editor session. Clean floating PIE entered Human
  Warrior, closed Inventory, showed the unchanged living F empty-range line,
  then stopped. Map remained All Saved outside PIE with real MCP 8123.
- The protected save and keeper material remained at SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The isolated fixture explicitly calls the death handler because its unbegun
world does not dispatch the live death delegate. It does not prove a physical
F press during an actual death hold or painted panel behavior. That remains
a user playtest gate. No content packages changed. Four known raw-vendor
physics-resave warnings remain confined to FieldGrass, River_Rock,
Medium_Boulder, and HillTree; no vendor asset was resaved.
