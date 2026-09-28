# September 28 Daily Build

## Saved-Quest Ledger Gate

Normal floating PIE exposed the empty Quest Ledger through Unreal-owned Slate:
`Tracked Quests 0 / 8`, `No quests tracked.`, a disabled `Focus Quest`, and
fixed-size detail cells. The small floating preview did not provide pixel
acceptance for the full HUD. Q autorun reached Mara's greeting and W canceled
it, but the subsequent F request was rejected because it could mutate quest
state merely to probe Ledger focus. No quest, reward, save, or package was
changed to manufacture a two-record runtime view. Physical keyboard/pointer
and the real two-quest interaction route remain user playtest gates.

The bounded follow-up is a new native Slate integration regression,
`Embermere.UI.QuestLedgerKeyboard.SavedQuests`. Unlike the existing synthetic
keyboard fixtures, it loads both saved quest data assets. In a disposable
automation world it establishes active Mara and completed Still Waters before
opening the Ledger. Focused-button Up/Down wraps selection and changes exact
saved objective detail without changing the compact tracker; native Enter and
Space change focus explicitly; native Close and post-close Enter cannot
re-focus a stale row. The test checks unchanged quest progress/completion,
wallet, XP, and inventory after inspection. No production quest, HUD, save,
or package behavior changed.

## Verification

- Real UnrealEditor was outside PIE with zero dirty content and map packages
  before a Slate-owned close. MCP 8123 was free. With process-local
  `DEVELOPER_DIR=/Applications/Xcode_26.1.1.app/Contents/Developer`, the
  `EmbermereEditor Mac Development -NoHotReloadFromIDE` build compiled and
  linked against SDK 26.1. System Xcode 27 was not changed.
- Fresh discovery returned **103** tests. The focused new test passed. The
  first complete run passed 102/103; an unrelated hotbar test captured a
  transient MCP `resources/templates/list` log error. That test passed on a
  focused rerun, and the final complete run passed **103/103** with zero
  failures, warnings, or skips.
- The fresh GUI-down package pass emitted skeletal-material and ancient-ruin
  success markers, the exact **53 Fab / 32 original-art** zone marker, and
  `validators=31`, with no `LogPython: Error`. UE 5.8's
  `-run=pythonscript -script=...` returned zero but hid the validators'
  captured `unreal.log` output; it was not accepted as a gate. The accepted
  pass used `-ExecutePythonScript` and read Unreal's full log at
  `~/Library/Logs/Unreal Engine/EmbermereEditor/Embermere.log`, copied to
  `/tmp/embermere-sep28-31-package-aggregate.log`. The only four warnings
  were the known raw-vendor physics-resave notices; those assets were not
  resaved.
- Clean editor relaunch on `/Game/Maps/L_Embermere_Prototype` owned MCP 8123.
  All **16** sequential initialized-world collision/route suites emitted
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` with no Python error. Clean
  in-viewport PIE started and stopped; final dirty content and map lists were
  both empty.
- The protected Chronicle save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  The unrelated local keeper material remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
  Config, FieldNotes, vendor assets, and the user's save were not staged.

The old `TODO.md` Next Work paragraph still requested the quest-update
observer already delivered September 4. It now points to the real two-quest
playtest and a distinct evidence-backed next milestone rather than repeating
completed work.
