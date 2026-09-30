# September 30 Daily Build

## Small-Viewport Inventory

Clean floating PIE after a transient Human Warrior confirmation exposed the
native Inventory's fixed `700x330` panel with no responsive parent. The Slate
tree reported its footer beyond the preview's right edge, but Unreal reported
a `1280x730` render viewport at UI scale `0.675`. Slate's text width and
window position are not directly comparable at that scale, so this did **not**
prove painted clipping in the current window. The new
`Embermere.UI.InventorySmallViewport` native test first failed because
`InventoryResponsivePanel` was absent. The narrow production change keeps
the same authored panel and inventory behavior inside a top-right `ScaleToFit`,
`DownOnly` ScaleBox inset 24 pixels from the viewport. The wrapper is
`SelfHitTestInvisible`, so only actual Inventory controls can take mouse
input. No inventory, equipment, character, save, or controller authority
changed.

The read-only Chronicle panel was opened in floating PIE without loading or
saving. Current Human Warrior identity, the existing versionless-slot warning,
disabled Load, Close, and Save Current were present within its fixed panel.
The native HUD handoff and Inventory controls were present after the change.
Slate's reported text geometry still extended beyond the preview window but
does not establish final painted clipping. An attempted PIE-only `r.SetRes`
did not change the floating viewport size. A user visual and physical-pointer
check remains open; synthetic Slate was not described as physical input.

## Verification

- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked. System Xcode 27 was unchanged.
- The focused Inventory test failed first for the missing parent. Final fresh
  discovery found **105** tests; the full run passed **105/105**, with zero
  failures, warnings, or skips.
- The final GUI-down `-ExecutePythonScript` aggregate exited zero and emitted
  the skeletal-material success, **53 Fab / 32 original-art** zone, and
  `validators=31` markers with no `LogPython: Error`. Full log:
  `/tmp/embermere-sep30-final-31-package-aggregate.log`.
- All **16** initialized-world collision/route suites passed in a clean editor
  session with no `LogPython: Error`. Clean floating PIE started and stopped;
  `/Game/Maps/L_Embermere_Prototype` remained clean outside PIE on real MCP
  port 8123.
- `Saved/SaveGames/EmbermerePrototype.sav` remained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  The unrelated keeper material remained
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
  Neither was modified or staged.

The voluntary real Mara/Still Waters two-quest Ledger playtest and physical
small-window pointer acceptance remain open. Do not manufacture quest state
or load the protected Chronicle slot for either review.
