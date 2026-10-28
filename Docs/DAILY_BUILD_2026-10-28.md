# Daily Build: 2026-10-28

## Milestone

Close Inventory and Chronicle with Escape through the game's existing input
owners. Ledger, vendor, and trainer already had this behavior; the two
remaining interactive panels did not. The controller handles ordinary game
input, while the HUD preview path handles Escape before a focused native
button consumes it. Up/Down and Enter/Space retain their existing owners.
The close path does not save, load, sort, consume, reward, or spend.

## Verification

- In clean floating PIE, the initial Human Warrior Inventory was visible.
  An unfocused synthetic Escape stopped the PIE window through the editor
  shortcut. This exposed the gap but is not proof of physical game input.
- `Embermere.UI.InventoryChronicleEscape` first failed on four assertions:
  controller and focused-Slate Escape left both Inventory and Chronicle
  visible. It then passed after the controller/HUD repair. The test uses a
  disposable Chronicle slot and checks item order, copper, XP, and no save.
- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked. Fresh discovery found 130 tests;
  full automation passed **130/130** with zero errors, warnings, or skips.
- GUI-down UnrealEditor `-ExecutePythonScript` passed all **31** sequential
  saved-package validators with skeletal materials first, saved Prowler loot,
  **53 Fab / 32 original-art**, and exact `validators=31` marker. The full
  `/tmp/embermere-oct28-31-package-aggregate.log` has no `LogPython: Error`.
- The clean final editor session passed all **16** initialized-world suites
  with `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no Python error.
  Floating PIE reached the normal Human Warrior HUD and stopped. Fenwatch is
  All Saved outside PIE with real UnrealEditor owning MCP 8123. The protected
  Chronicle save and unrelated keeper material hashes remained unchanged.

## Open Gate

The native controller and virtual-Slate paths are green. A direct player
check should establish whether Escape closes Inventory/Chronicle when a
panel child has focus, or in a standalone game. An unfocused floating-PIE
Escape can be consumed by Unreal Editor's Stop Play shortcut, so do not
describe that route as accepted or alter editor preferences to manufacture
the result. Do not save or load the protected Chronicle slot for this check.
