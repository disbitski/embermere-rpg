# Pending Ledger Regression Draft

`2026-09-15-ledger-keyboard-tests.patch` contains two proposed tests against
`EmbermereServiceKeyboardInputTests.cpp` at the September 14 `15325ab` C++
baseline. It is **not compiled, not executed, and not an accepted repair**.
Xcode 27 failed UE 5.8 SDK validation before compilation. Only
`git apply --check` has passed. The accepted suite remains 100 tests.

After installing a compatible toolchain, review the draft against current
source, apply it deliberately, build, and run these tests before production
changes:

- `Embermere.UI.QuestLedgerKeyboard.Controller`
- `Embermere.UI.QuestLedgerKeyboard.FocusedButtons`

They propose actual controller dispatch and registered virtual Slate-window
focus coverage. Review fixture assumptions, record order, completed history,
repeat/release semantics, and cleanup during compilation and execution. A
draft assertion is not proof of behavior. In particular, only focused Down
has a live failure reproduction; Up and Escape remain unproven.

Require a genuine focused-path failure before the narrow HUD preview repair,
then the complete suite, fresh packages, live traces, and clean PIE. Preserve
native Enter/Space button meaning, explicit tracker focus, quest authority,
fixed geometry, and all protected files. Remove this pending artifact once
its reviewed replacement has been accepted, to avoid duplicate test additions.
