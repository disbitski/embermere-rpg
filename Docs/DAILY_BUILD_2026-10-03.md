# October 3 Daily Build

## Dead-State Autorun Safety

Clean normal-route floating PIE exposed the saved Fenwatch Practice Target
through Tab, its 150/150 target frame and world nameplate, and the Warrior
hotbar's Battle Shout cooldown and ready-time chat. No concrete targeting or
cooldown presentation defect appeared, so those accepted surfaces were left
alone.

Review of the adjacent controller path found that `ToggleAutorun()` inverted
its flag without checking for a living possessed character. The death handler
cleared the flag, but a Q press during death hold could set it again before
respawn restored walking. `Embermere.Input.DeadAutorunSafety` was compiled
and failed first on unpossessed and dead-state toggles. The controller now
rejects those requests and clears the flag, and respawn clears any stale
flag before either its normal or no-pawn path. Living Q on/off behavior and
manual W/S cancellation retain their existing owners. The HUD cue remains
a read-only reflection of that flag; no save, quest, combat, or UI authority
moved.

## Verification

- Process-local Xcode 26.1.1 compiled and linked `EmbermereEditor Mac
  Development -NoHotReloadFromIDE`; system Xcode 27 was unchanged.
- The new focused regression passed after the fix. Fresh discovery found
  **108** tests; the full run passed **108/108** with zero failures, warnings,
  or skips.
- The GUI-down `-ExecutePythonScript` commandlet exited zero. Dedicated log
  `/tmp/embermere-oct03-final-31-package-aggregate.log` contains exact
  skeletal-material, **53 Fab / 32 original-art**, and `validators=31`
  markers, with no `LogPython: Error`. Four known raw-vendor physics-resave
  warnings remain; no vendor package was touched.
- A clean editor session emitted
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` with no
  `LogPython: Error`. Floating PIE started/stopped, and the saved map stayed
  clean outside PIE with real UnrealEditor owning MCP 8123.
- `Saved/SaveGames/EmbermerePrototype.sav` remained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper material remained SHA256
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The focused test proves controller state transitions, not a physical Q press
or painted HUD behavior during a real death. The user still needs to check
Q during death hold, held W/S cancellation and cue readability, physical
chat paging, small-window Inventory pointer behavior, and a voluntary
two-quest Ledger route. No quest was forced and the protected Chronicle
slot was neither loaded nor changed.

Tomorrow's best start is a distinct bounded gameplay or player-facing
milestone from fresh normal-route evidence. Do not retune the accepted
target/nameplate, hotbar, or autorun cue without a concrete issue.
