# September 14 Daily Build

## Scope And Readiness

Repaired a reproduced focused-service navigation gap without changing service
rules, stock, prices, XP, save version 3, art, map, layout, or mouse handlers.
The project task's cwd is `/Users/wizard/Documents/Unreal Game`, and its existing
ACTIVE heartbeat remains scheduled for 8 AM America/New_York. Setup checks
passed the project, UE 5.8, full Xcode, Metal compiler, MCP/AllToolsets/Python
plugins, and localhost MCP URL. Desktop control remains denied and disabled.
The configured Blender executable exists; its live bridge was not queried
because this was not an art task.

## Focus Regression And Fix

Before the fix, clean Human Warrior PIE reached the real trainer with W/F.
Explicit in-engine focus on TrainerActionButton was confirmed true. Native
Slate Down left lesson 0 selected, including a later-frame query; native
Enter correctly committed 40 -> 30 copper and 0 -> 25 XP. This reproduced
focus-route navigation failure, not a transaction bug or pointer diagnosis.

Two new tests, `Embermere.UI.ServiceKeyboard.TrainerFocus` and `.VendorFocus`,
attach the actual HUD to a registered `SVirtualWindow`, confirm native button
focus, and route key-down/up through Slate. Both failed before the fix;
September 13's two controller tests still passed. All four passed afterward.
The virtual window is not an OS window. Focus and window registration are
cleaned up before world destruction; no desktop automation is involved.

The HUD now previews only unmodified Up/Down/Escape while a service is visible.
It calls existing selection/close methods, ignores repeat navigation, and
handles the event before it can also reach the controller. Enter and Space
retain native focused-button meaning, including Close rather than an unwanted
Buy/Train. Other panels, modifiers, brackets, I/J/M, and controller primary
actions keep their existing paths. See
[SERVICE_KEYBOARD_INPUT_CONTRACT.md](SERVICE_KEYBOARD_INPUT_CONTRACT.md).

Coverage includes unavailable rows, wrapping, repeat navigation, native
press/release and repeat safety, exact copper/XP/item commits, native Space,
arrows from the close button, Enter on Close, Escape, and no stale transaction.
Existing service tests retain funds, full-bag, stock, malformed-data, and
same-frame controller handoff/rejection coverage.

## Clean PIE Evidence

The fresh compiled editor discovered 100 tests. A fresh Human Warrior accepted
Mara at approximately `(-2153,-976,90.15)` and Still Waters near
`(-1703,-233,90.15)` through their original F owners, then reached the trainer
at `(-1414,-769,90.15)`. Both quest records remained independent at zero.

Focused native Down now selected Advanced and displayed the exact level-2
rejection copy without spending. Up from the close button restored Combat
Drills; focused native Enter committed 40 -> 30 copper and 0 -> 25 XP. Focused
Escape closed with cursor hidden and move/look unsuppressed.

At the vendor, approximately `(-1434,-1303,90.15)`, focused Down/Up selected
pack/tonic. Native Buy Enter spent 30 -> 22 for one tonic; Sell Space returned
22 -> 25 and removed it; Buyback Enter spent 25 -> 22 and restored it. Native
Close Enter closed without another transaction. All six button focus checks
were true. No gameplay delegate or transaction method was directly invoked.

Every measured key refreshed/right-clicked the Unreal viewport first. Focused
diagnostics then explicitly focused an engine button; these are qualified
Slate-route proofs, not organic focus traversal or hardware-input acceptance.
Walking uses bounded forced W keys and scripted headings, not actor teleports.
Creation used the established explicit engine widget focus plus Slate Enter.

The initial well visit correctly rejected full resources. Battle Shout grants
Attack Power and costs no mana; it cannot manufacture recovery eligibility.
Three subsequent shortcut route probes timed out at the gate/fence boundary
without damaging the player or mutating quests/rewards. No scenery or collision
was changed. An approach from the measured endpoint through `(1320,628)` to
`(1500,1100)` then produced real Prowler-02 retaliation: health 100 -> 82,
neighbors untouched. Retreat crossed the opening through `(1320,628)` and
`(800,438)`. The pause at that last point allowed further retaliation before
the next movement run; the final village return had 22 health. This is not
a clean continuous-retreat claim. Prowler 02 returned to exact home, unharmed.
The cross-well waypoint stopped at the purposeful curb, already within service
range at `(-812,-1538,90.15)`. Real F there restored 78 health to 100, retained
50 mana, and advanced only Still Waters to 1/1. Mara stayed 0/3; wallet 22,
XP 25, and the single tonic were unchanged. No partial recovery, artificial
health/mana spending, or gameplay-owner fixture was used.

The return via `(-200,-1530)`, `(-200,-350)`, and `(-1370,-250)` reached the
notice board at approximately `(-1468,-199,90.15)`. Original F committed the
exact Still Waters reward: XP 25 -> 75, copper 22 -> 32, no new item, completed
1/1. A second F paid nothing. Mara remained active at 0/3 and all three Prowlers
were full-health at home. This was not a new full Mara combat/animation sweep.

J showed active Mara 0/3 and completed Still Waters 1/1. M replaced it with
Chronicle's live Human Warrior Level 1 and the protected slot's exact missing-
version warning with Load disabled. No Save or Load action occurred. I/I
returned to cursor-hidden, unsuppressed game input with all owner state intact.
The route callback and forced key were both None before StopPIE.

## Verification

- Development build succeeded with `-NoHotReloadFromIDE`.
- All 100 isolated automation tests passed, with zero failures, test warnings,
  skipped, or in-process tests. Fresh GUI discovery also returned 100.
- All 23 sequential fresh NullRHI package validators passed their explicit
  markers, including materials-first, journal emblem, placed Prowlers,
  progression, and the 53 Fab / 24 original-art zone baseline.
- All six initialized-editor trace suites passed outside PIE: communal well,
  notice board, workshop, cottage, vendor stall, and road boundaries.
- The real GUI/MCP process was confirmed stopped before commandlets. Existing
  vendor physics-resave warnings remain outside test warnings and do not
  authorize vendor edits or establish cooked-build readiness.
- No `LogPython: Error` occurred in the test/package validation or either
  live editor evidence interval. Deprecated Python API warnings are not
  zero-warning editor claims.
- Final real UnrealEditor PID 78532 is outside PIE on
  `/Game/Maps/L_Embermere_Prototype`, with zero dirty content/map packages and
  localhost MCP on 8123. Both protected hashes were rechecked after live play.

Evidence lives in `/tmp/embermere-sep14-focus-before/index.json`,
`/tmp/embermere-sep14-focus-after/index.json`,
`/tmp/embermere-sep14-tests/index.json`, `/tmp/embermere-sep14-packages.log`,
`/tmp/embermere-sep14-focus-before-live.log`, and the fresh editor's `SEP14_`
log markers. Temporary probes reuse the reviewed September 13 route helper;
its outer log prefix remains `SEP13_STATE` with explicit `SEP14_` run labels.

## Preservation And Next Start

No save/load operation is authorized on the protected unstamped original.
Its SHA256 remains
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
The existing keeper working resave remains
`38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
Config, that resave, and unpublished Field Notes remain excluded from commits.

Keep physical Train/Buy, arrow/Enter/Escape, bracket, I handoff, held camera,
and HUD review as user gates. The focused-navigation repair does not resolve
September 12's MCP pointer issue or claim complete Tab navigation. Do not
repeat unbounded pointer probes or require desktop access. Continue unaffected
quest/service play and repair only concrete reproduced issues.
The Ledger's similar controller-only navigation can receive a bounded focused-
control inspection next; do not assume it is broken without a reproduction.
