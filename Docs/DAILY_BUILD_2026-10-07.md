# October 7 Daily Build

## Saved First-Loot Contract

The requested normal-route first Prowler defeat could not be claimed from
synthetic Slate input. A fresh Human Warrior PIE start reached the authored
HUD, but no reliable physical-style combat key or traversal was available
through Unreal MCP. The user's protected Chronicle slot and quest state were
not used to manufacture that proof.

First-class Unreal object inspection confirmed the saved `BP_StarterEnemy`
defaults and all three placed actors point to `DI_MarshTonic`, with loot
enabled, quantity one, drop chance 1.0, and defeat credit enabled. The item
still reports stable `MarshTonic` identity, a five-item stack cap, and 25 HP /
10 mana recovery. The existing placed-Prowler validator had guarded only
visual mesh and six animation roles. It now guards this saved gameplay-data
link too, and emits `loot=one_guaranteed_marsh_tonic` on success. No C++,
Blueprint, item, map, quest, or save data changed.

## Verification

- Focused initialized-editor validator passed with the new loot marker.
- GUI-down `-ExecutePythonScript` commandlet exited zero; dedicated log
  `/tmp/embermere-oct07-final-31-package-aggregate.log` contains the exact
  skeletal-material, placed-Prowler loot, 53 Fab / 32 original-art, and
  `validators=31` markers, with no `LogPython: Error`.
- Fresh discovery found 110 Embermere tests. Full automation passed 110/110
  with zero failed, skipped, or warned tests.
- All 16 initialized-world collision/route suites passed in a clean editor
  session with no `LogPython: Error`. Floating PIE started and stopped.
- The editor remains on `/Game/Maps/L_Embermere_Prototype`, outside PIE and
  All Saved, with the real UnrealEditor owning MCP port 8123.
- Protected save SHA256 remains
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  unrelated keeper material SHA256 remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The first real normal-route Prowler defeat, Marsh Tonic pickup/use, and final
painted chat remain physical player checks. Four accepted raw-vendor physics
warnings remain confined to FieldGrass, River_Rock, Medium_Boulder, and
HillTree; no vendor packages were resaved.
