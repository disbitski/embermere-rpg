# October 1 Daily Build

## Six-Row Chat Scrollback

The bottom-left chat panel previously discarded messages after six lines. A
short quest/combat sequence could erase its opening context. The new native
`Embermere.UI.ChatHistoryNavigation` test was compiled and failed first:
the HUD retained six events where the test expected 30.

The HUD now keeps 30 transient messages while constructing and displaying
only its existing six fixed, clipped rows. PageUp/PageDown move one six-row
page toward older/newer events and clamp at either end. A new event preserves
an older reading window instead of snapping it to the live tail; returning to
the newest page reveals that event. Extreme scroll requests clamp without
integer overflow. The panel stays hit-test-invisible and in its original
`520x156` bottom-left slot. No chat record enters the save, and no combat,
quest, reward, inventory, or input-mode authority moved into presentation.
The old chat test now fills the larger bounded history; the new test covers
newest/older/oldest pages, incoming-message anchoring, empty events, and both
extreme scroll directions.

## Verification

- Process-local Xcode 26.1.1 compiled and linked `EmbermereEditor Mac
  Development -NoHotReloadFromIDE`; system Xcode 27 remained unchanged.
- Fresh discovery found **106** tests. The final complete run passed
  **106/106**, with zero failures, warnings, or skips.
- The final GUI-down `-ExecutePythonScript` aggregate exited zero with exact
  skeletal-material, **53 Fab / 32 original-art**, and `validators=31`
  markers and no `LogPython: Error` in
  `/tmp/embermere-oct01-final-31-package-aggregate.log`.
- A clean final editor session ran all **16** initialized-world collision/route
  suites with `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no
  `LogPython: Error`. Floating PIE started and stopped. The correct map stayed
  clean outside PIE; real UnrealEditor owned localhost MCP 8123.
- The protected `EmbermerePrototype.sav` hash stayed
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper-material hash stayed
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The first live-validator attempt in an earlier editor session inherited a
stale `imp` Output Log prefix and produced a contained syntax error before a
successful retry. That session is **not** counted as clean. A fresh relaunch
used the direct `py /absolute/script.py` form and produced the clean final
16-suite result above. No content or gameplay package was changed by that
console error.

Physical PageUp/PageDown and final painted chat readability remain user
checks under the no-desktop-control rule. The small-window Inventory pointer
review and voluntary real Mara/Still Waters two-quest Ledger route also remain
open; neither was manufactured with forced quest state or the protected save.
