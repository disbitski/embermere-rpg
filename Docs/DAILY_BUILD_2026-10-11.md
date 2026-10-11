# October 11 Daily Build

## Death-Panel And Input Handoff

No voluntary first-Prowler/tonic or selected-target death state was open.
The normal player lifecycle review found that the death callback cleared
autorun, movement, and the selected target but left interactive panels open.
`Embermere.Input.DeathPanelHandoff` first failed for Inventory and cursor/UI
click routing surviving death, and for an old vendor panel accepting Enter
after respawn. That stale purchase reduced copper from 40 to 32 and added an
item in the isolated test world.

The HUD now exposes one native close operation for Inventory, vendor,
trainer, Chronicle, and Ledger. It reuses the existing close methods so
service references, pending save/load confirmation, and transient Ledger
selection are retired. The controller calls the operation at death and
respawn entry, then refreshes input mode. The green regression covers an
Inventory opened before or during the death hold, vendor/trainer/Ledger
closure, stale Enter safety, and zero copper, XP, inventory, or quest drift.
No service transaction, quest, persistence, or combat authority moved.

## Verification

- Process-local Xcode 26.1.1 compiled and linked EmbermereEditor Mac
  Development with `-NoHotReloadFromIDE`; system Xcode 27 stayed unchanged.
- Fresh discovery and the final full native run passed **114/114** with zero
  failures, warnings, or skips. The focused regression was red on the old
  handler and green after the repair.
- The GUI-down `-ExecutePythonScript` commandlet exited zero. Dedicated log
  `/tmp/embermere-oct11-31-package-aggregate.log` contains the exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers with no `LogPython: Error`.
- All **16** initialized-world route/collision suites passed in a fresh final
  editor session with exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` and no
  Python errors. An earlier session had a `Cmd` autocomplete `py py` syntax
  error; it was rejected and replaced by the clean session, using Unreal's
  `Python` command mode plus `runpy.run_path`.
- Clean floating PIE reached the normal Human Warrior level-1 HUD with
  Inventory visible and stopped. The test fixture explicitly invokes the
  controller death handler; neither it nor synthetic Slate proves a physical
  death, vendor approach, or painted cursor handoff.
- The real editor remains on `/Game/Maps/L_Embermere_Prototype`, outside PIE
  and All Saved, owning MCP 8123. No saved content package changed. Both
  protected SHA256 values remain
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The physical death-panel/cursor handoff, first Prowler defeat/tonic, and
other direct-input gates remain user checks. Four known raw-vendor
physics-resave warnings remain confined to FieldGrass, River_Rock,
Medium_Boulder, and HillTree; no vendor pack was resaved.
