# October 6 Daily Build

## First-Combat Rejection Feedback

A clean floating PIE start showed the normal Human Warrior hotbar at PlayerStart.
Code review of that player path found that rejected hotbar presses all reached
the same `Unable to use` line, even though combat already distinguished no
target, out-of-range, defeated target, dead caster, and insufficient mana.
The improvement is limited to explaining those preflight failures in chat.

`UEmbermereCombatComponent::GetAbilityRejection` is a read-only query that
shares the exact ordered preflight used by `ExecuteAbility`. The controller
turns a failed hotbar action into a specific line: `Select a target for
Strike.`, `Strike is out of range.`, or `Not enough mana for Strike.` in the
focused cases. The existing cooldown line still takes precedence. Rejections
publish no combat result and spend no mana, deal no damage, advance no quest,
or grant any item. A valid Strike and immediate cooldown rejection retain
their original behavior. No saved data, assets, map actors, or input bindings
changed.

The first draft regression failed because the detached test input did not
route a simulated `1` key. After targeting the controller's hotbar action
directly, it failed on the old generic copy for all three cases. A later
assertion correction read the second and third chat rows instead of rereading
the first. The final test passed; it is native controller/HUD proof, not a
physical-key acceptance claim.

## Verification

- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked.
- Fresh discovery found **110** Embermere tests. The focused rejection test
  and final full run passed **110/110** with zero failures, warnings, or skips.
- The final GUI-down `-ExecutePythonScript` commandlet exited zero. Its
  dedicated log `/tmp/embermere-oct06-final-31-package-aggregate.log` contains
  exact skeletal-material, **53 Fab / 32 original-art**, and `validators=31`
  success markers with no `LogPython: Error`.
- The final live editor emitted
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` with no
  `LogPython: Error`. Floating PIE created a Human Warrior through the normal
  picker, showed the HUD, and stopped cleanly. The editor remains on
  `/Game/Maps/L_Embermere_Prototype`, outside PIE and All Saved, with real
  UnrealEditor owning MCP 8123.
- The protected save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper material remains SHA256
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

Four known raw-vendor physics-resave warnings remain confined to FieldGrass,
River_Rock, Medium_Boulder, and HillTree. Physical hotbar-key and final painted
chat checks remain user gates under the desktop-control restriction. Tomorrow,
inspect the first real Prowler defeat and Marsh Tonic pickup from the normal
route before choosing a distinct bounded content or feedback milestone.
