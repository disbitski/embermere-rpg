# October 17 Daily Build

## Combat Credit And Two Quest Owners

No voluntary physical first-Prowler, death-hold, service walk-away, or
two-quest Ledger state was supplied. Clean floating PIE reached the normal
creation picker, and a Human Warrior confirmation showed the level-1 HUD;
neither step was used to manufacture quest or combat state.

The prior started-world Prowler regression protected death-event loot but
never executed a player ability or read Mara's objective. The new
`Embermere.Quests.SpawnedCombatCredit` test starts an isolated world with the
saved `BP_StarterEnemy` and both saved quest assets. A committed lethal
ability first defeats the art-free practice target without credit. Defeating
the saved Prowler through that ability advances only Mara from 0/3 to 1/3;
Still Waters remains 0/1. A rejected repeat cannot duplicate progress, and
neither quest pays its completion reward early. This guards exact quest
ownership without changing production gameplay, Blueprint, map, or assets.

## Verification

- The process-local Xcode 26.1.1 EmbermereEditor Mac Development
  `-NoHotReloadFromIDE` build compiled and linked. The first sandboxed UBT
  attempt could not rotate its log; the identical host-permitted build passed.
- Fresh discovery found **120** Embermere tests. The focused new test and
  final full automation passed **120/120**, zero failures, warnings, or skips.
- A sandboxed Unreal commandlet stalled in AppKit before creating a log and
  was stopped. The identical GUI-down host-permitted `UnrealEditor
  -ExecutePythonScript` run exited zero. Its dedicated
  `/tmp/embermere-oct17-31-package-aggregate.log` contains explicit
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers, with no `LogPython: Error`.
- All **16** initialized-world collision/route suites passed through Unreal
  MCP with the exact aggregate marker and no Python error. Clean floating
  PIE reached the Human Warrior HUD and stopped. The editor is on
  `/Game/Maps/L_Embermere_Prototype`, All Saved, outside PIE, with real MCP
  on port 8123.
- The protected save and unrelated keeper material retained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

This is an isolated native combat/quest proof, not a physical hotbar fight,
camera check, painted quest/loot copy, or tonic use. Those remain voluntary
player gates. Four known raw-vendor physics-resave warnings remain confined
to FieldGrass, River_Rock, Medium_Boulder, and HillTree; no vendor asset was
resaved.
