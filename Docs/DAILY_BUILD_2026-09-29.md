# September 29 Daily Build

## Small-Viewport Character Creation

Clean floating PIE opened a 640x394 preview, smaller than the authored
940x560 pre-play picker. Unreal-owned Slate initially exposed only its title
and subtitle, leaving the race/class controls and confirmation clipped.
`Embermere.UI.CharacterCreationSmallViewport` first failed because the fixed
panel lacked a responsive parent. The narrow fix wraps the same authored
panel in a centered `ScaleToFit`, `DownOnly` ScaleBox with 12-pixel outer
padding. Rules, pending choice, confirmation, controller input ownership,
and normal-size panel dimensions did not change.

After the fix, the same 640x394 PIE preview exposed all eight race controls,
four class controls, selected-path detail, and Enter Embermere. Unreal-owned
Slate selected Dwarf Warrior, showed Ranger and Wizard disabled, confirmed
the choice, and displayed `Journey begun: Dwarf Warrior` with the normal HUD.
This is a synthetic Slate interaction, not proof of physical mouse or keyboard
input. The existing real two-quest Ledger route was not forced or altered.

## Verification

- `EmbermereEditor Mac Development -NoHotReloadFromIDE` compiled and linked
  with process-local Xcode 26.1.1. System Xcode 27 remained unchanged.
- Fresh discovery found **104** tests; the focused test and full **104/104**
  run passed with zero failures, warnings, or skips.
- With the real editor down and MCP 8123 free, the fresh NullRHI
  `-ExecutePythonScript` aggregate exited zero and emitted
  `EMBERMERE_FENWATCH_SKELETAL_MATERIAL_VALIDATION_SUCCESS`, the exact
  **53 Fab / 32 original-art** zone marker, and `validators=31`; no
  `LogPython: Error`. Full log: `/tmp/embermere-sep29-31-package-aggregate.log`.
- After the clean editor relaunch, all **16** initialized-world collision
  and route suites emitted `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS`
  without a Python error. Clean PIE started and stopped; the saved map stayed
  clean outside PIE on real MCP 8123.
- `Saved/SaveGames/EmbermerePrototype.sav` remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  The unrelated keeper material remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
  Neither was loaded, modified, or staged. Unrelated Config and FieldNotes
  changes remain outside this milestone.

Physical input and the voluntary two-quest normal-route Ledger acceptance
remain open user gates; no quest state was manufactured for this fix.
