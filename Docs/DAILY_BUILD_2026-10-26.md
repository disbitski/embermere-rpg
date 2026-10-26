# Daily Build: 2026-10-26

## Milestone

Require a clear `Visibility` line for the controller's existing 350 cm F
candidate selection. The old distance-only path could open the quartermaster
through a registered solid wall. The controller now skips a blocked owner,
selects a clear alternate when one exists, and posts exact `Move around the
obstacle to interact.` when in-range candidates are blocked. The existing
one-second chat throttle applies. Empty-range feedback remains unchanged;
no service, quest, inventory, wallet, or persistence authority moved.

## Verification

- A read-only initialized-world probe compared sightline heights. The 90 cm
  character/owner offset cleared all five saved F approaches, while a 150 cm
  ray intersected the notice board's own decorative geometry. The saved
  approach validator now guards the accepted 90 cm lines.
- `Embermere.Input.BlockedInteraction` first failed because F opened the
  blocked vendor. Its final started-world fixture registers the wall and a
  rooted alternate owner, then passes blocked rejection, exact chat and
  throttle, zero copper/XP/bag/quest drift, clear-alternate selection, and
  same-service retry after the wall moves. An early fixture called `EndPlay`
  before shared cleanup and crashed the editor; the corrected fixture ends
  only a begun world during teardown. No production behavior was changed to
  accommodate that fixture.
- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked. Fresh discovery found 128 tests;
  full automation passed **128/128** with zero failures, warnings, or skips.
- The isolated GUI-down UnrealEditor `-ExecutePythonScript` aggregate passed
  all **31** saved-package validators, including skeletal materials first,
  placed-Prowler loot, and **53 Fab / 32 original-art** zone markers.
  `/tmp/embermere-oct26-31-package-aggregate.log` contains the exact
  `validators=31` marker and no `LogPython: Error`.
- A clean final editor session passed all **16** initialized-world suites,
  including five clear saved F sightlines and the registered fence control,
  with `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no Python errors.
  Floating PIE reached the normal Human Warrior HUD and stopped. The map is
  All Saved outside PIE with real UnrealEditor on MCP 8123.
- The protected Chronicle save and unrelated keeper material retained their
  original SHA256 hashes. No content package changed.

## Open Gate

Physical F through solid cover and the final painted explanation remain a
user check. Native started-world and saved-map traces do not prove physical
keyboard input or a live service handoff from behind a wall. Do not force
quest, commerce, or protected save state just to manufacture this check.
