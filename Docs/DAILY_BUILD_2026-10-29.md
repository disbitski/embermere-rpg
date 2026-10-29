# Daily Build: 2026-10-29

## Milestone

The initially open Inventory had only keyboard close paths, while the other
interactive panels exposed an explicit X. A fixed 28x24 close button now sits
in the existing Inventory header. It only hides that panel and requests the
controller's existing input-mode refresh; item, quest, combat, commerce, and
save authority remain unchanged.

## Verification

- Clean floating PIE exposed the missing control in the normal Human Warrior
  Inventory. `Embermere.UI.InventoryCloseButton` first failed because no native
  close button existed. After the UI change, focused Enter and Space closed the
  panel. The detached test HUD has no local player context, so its cursor
  assertion explicitly refreshes the controller policy; normal-route PIE
  separately proved the live button click hides Inventory. Neither synthetic
  Slate path proves physical pointer feel.
- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked. Fresh discovery found 131 tests;
  the final full run passed **131/131**, with zero failures, warnings, or skips.
- The final GUI-down 31-package aggregate exited zero with exact skeletal-
  material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers, with no `LogPython: Error`. Evidence:
  `/tmp/embermere-oct29-final3-31-package-aggregate.log`. A prior run used
  `-log`, emitted all markers, then hung in macOS console teardown; it was
  terminated and not counted. Omitting `-log` kept the dedicated `-Abslog`
  and allowed the authoritative run to exit cleanly.
- All **16** initialized-world suites passed in the final editor session with
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no Python error. A clean
  floating PIE started, confirmed Human Warrior, clicked the Inventory X
  through Unreal-owned Slate, observed the panel disappear, and stopped.
  A cold relaunch left `/Game/Maps/L_Embermere_Prototype` All Saved outside
  PIE with real UnrealEditor owning MCP 8123 and 131 tests discoverable.
- The protected Chronicle save and unrelated keeper material retained their
  exact recorded SHA256 hashes. No content package was intentionally changed.

## Open Gate

The user should click the Inventory X in a small floating PIE or standalone
window, confirm the cursor and gameplay input return, and confirm the button
remains distinct from Sort. Native focus and synthetic Slate click are not a
physical pointer acceptance. The separate focused/standalone Escape check for
Inventory and Chronicle remains open; do not save or load the protected slot.
