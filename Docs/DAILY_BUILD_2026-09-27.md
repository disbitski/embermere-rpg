# September 27 Daily Build

## Ledger Keyboard Focus

The Xcode 26.1.1 recovery unblocked a test-first repair of the existing Quest
Ledger focus gap. I reviewed the previously unapplied September 15 draft
against current source, compiled it, and ran both new tests before changing
production code. `Embermere.UI.QuestLedgerKeyboard.Controller` passed, while
`Embermere.UI.QuestLedgerKeyboard.FocusedButtons` failed with native Focus
Quest button focus: Down/Up left row selection and its detail unchanged, and
Escape did not close. This reproduced the known distinction between
controller-polled input and focused Slate input.

The HUD preview handler now routes only unmodified, non-repeating Up/Down and
Escape to the visible Ledger. Native Enter and Space continue to activate the
focused action or close button. The next test run exposed a second bug: an
old focused action could request a different quest after the panel closed.
`FocusSelectedQuest` now rejects hidden-panel requests. The final two focused
tests pass, including wrap, repeat suppression, detail versus tracker focus,
native Enter/Space, Escape, peer handoff, and unchanged quest, wallet, XP, and
inventory ownership. The pending draft was removed after acceptance so it
cannot be applied twice. No saved quest data or gameplay owner changed.

## Verification

- The real editor was closed through Unreal-owned Slate and MCP 8123 was free
  before each `EmbermereEditor Mac Development -NoHotReloadFromIDE` build.
  Process-local `DEVELOPER_DIR=/Applications/Xcode_26.1.1.app/Contents/Developer`
  selected SDK 26.1. The test build and both repair builds compiled and linked
  successfully. The unchanged system Xcode remains 27.
- The first focused run failed as described above. After navigation routing,
  only the stale post-close action assertion failed. After its guard, both
  focused tests passed. Fresh discovery returned **102** Embermere tests;
  the complete run passed **102/102**, zero failures, warnings, or skips.
- With the GUI down, a fresh sequential NullRHI aggregate emitted
  `EMBERMERE_FENWATCH_SKELETAL_MATERIAL_VALIDATION_SUCCESS`,
  `EMBERMERE_ANCIENT_RUIN_RELIEF_VALIDATION_SUCCESS`, the exact **53 Fab / 32
  original-art** full-zone marker, and `validators=31`. It exited zero with
  no `LogPython: Error`. Evidence:
  `/tmp/embermere-sep27-31-package-aggregate.log`.
- The relaunched initialized editor ran all **16** native route/collision
  suites sequentially and emitted
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` with no
  `LogPython: Error`. Clean PIE started/stopped. The editor is outside PIE on
  `/Game/Maps/L_Embermere_Prototype`, owns MCP 8123, and reports
  `EMBERMERE_DIRTY_PACKAGES=[]`.
- The user's Chronicle save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated local keeper material remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
  Neither was staged or reset. Config and FieldNotes changes remain excluded.

The in-engine virtual Slate path is accepted; a physical keyboard/pointer
walkthrough remains unverified because desktop control is revoked. The next
useful normal-PIE review is Ledger focus and peer-panel handoff with the real
Mara/Still Waters records, using Unreal-owned Slate only, followed by user
physical-input feedback when convenient. Do not overwrite the user's save.
