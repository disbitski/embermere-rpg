# October 10 Daily Build

## Player-Death Target Cleanup

No voluntary first-Prowler/tonic playtest state was open. Clean floating PIE
showed a normal Human Warrior start, Inventory handoff, and reachable practice
target. Reviewing that targeting lifecycle found a bounded defect: the player
death handler stopped autorun and movement but retained the selected enemy,
so its cyan ring and target HUD could remain through village respawn.

`Embermere.Combat.PlayerDeathTargetCleanup` uses an isolated spawned world to
enter dead state and invoke the controller death handler. It failed against
the old handler specifically for selected target and ring persistence during
death and after respawn, while autorun cleanup worked. The controller now
calls `Combat->SetTarget(nullptr)` on death, letting the established target
change event clear observers. The final regression passed for target/ring
cleanup, no automatic retarget, and unchanged enemy health, XP, and copper.
The isolated world does not prove physical combat or a real `OnDied` dispatch;
those remain separate playtest gates.

## Verification

- The final EmbermereEditor Mac Development `-NoHotReloadFromIDE` build linked
  with process-local Xcode 26.1.1. System Xcode 27 stayed unchanged.
- Fresh discovery found **113** Embermere tests; focused and final full
  automation passed **113/113** with zero failures, warnings, or skips.
- The GUI-down `-ExecutePythonScript` commandlet exited zero. Dedicated log
  `/tmp/embermere-oct10-final-31-package-aggregate.log` contains exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers, with no `LogPython: Error`.
- All **16** initialized-world route/collision suites passed with exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` and no
  Python errors.
- Clean floating PIE confirmed Human Warrior at level 1, closed the initial
  Inventory, selected the Fenwatch Practice Target via Unreal-owned synthetic
  Slate, and stopped. It did not force a quest, Prowler kill, or protected save.
- The real editor remains on `/Game/Maps/L_Embermere_Prototype`, outside PIE
  and All Saved, owning MCP 8123. No saved content package changed.
- The protected save and unrelated keeper material SHA256 values remain
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

Physical selected-Prowler death, ring clearing, and respawn feel remain user
checks. The four known raw-vendor physics-resave warnings stay confined to
FieldGrass, River_Rock, Medium_Boulder, and HillTree; no vendor pack was
resaved.
