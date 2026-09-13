# September 13 Daily Build

## Scope And Readiness

Implemented a small controller-routed keyboard fallback for the existing
vendor and trainer panels. No service rules, prices, stock data, progression,
save schema, assets, map, layout, mouse handlers, or focusability changed.
See [SERVICE_KEYBOARD_INPUT_CONTRACT.md](SERVICE_KEYBOARD_INPUT_CONTRACT.md).

App inspection confirms the ACTIVE 8 AM America/New_York heartbeat targets
task `019efc2b-b36c-71f0-8892-8e4f80829fc9`, whose cwd is exactly
`/Users/wizard/Documents/Unreal Game`. The setup checker passed project,
Epic launcher, full Xcode selection, Metal compiler, UE 5.8, enabled Unreal
MCP/AllToolsets/Python plugins, and the localhost MCP URL. The configured
Blender bridge executable exists; its live bridge was not queried because
this was not an art task. Do not claim live Blender connectivity from that.

Desktop-control denial and disabled plugins/bridges remain intact. No desktop
control, broad screenshots, security changes, unlock requirement, or direct
HTTP fallback was used. Unreal MCP handled editor inspection and input.

## Implementation And Regression

Up/Down wraps vendor stock or trainer offerings, including unavailable rows.
Enter requests the selected Buy/Train through the same HUD/service methods
as the existing buttons. Escape closes the panel and refreshes input mode.
Vendor brackets still select bag identities; trainer brackets still select
offerings. Controller-visible peer handoffs happen before service dispatch.
Close/selection precedes Enter when those keys arrive in the same frame.

Two tests, `Embermere.UI.ServiceKeyboard.Vendor` and `.Trainer`, create an
isolated world and deliver simulated InputKey events through PlayerInput and
PlayerTick. They do not call the new handlers or transaction methods directly
to manufacture input success. HUD/service owners use actual saved stock and
offerings. Fixture-only malformed data and resource setup stay in automation.

Both tests failed before the controller change: arrows did not select,
Enter did not buy/train, and Escape did not close. Both passed afterward.
Final coverage includes wrapping, unavailable selection, exact delivery and
cost/XP, finite stock, insufficient funds, full bag, malformed prices/lessons,
repeat-event safety while affordable, bag versus stock selection, Escape,
same-frame close/Inventory handoff, and no stale action after closing.
The detached fixture does not prove Slate focus, hardware delivery, or a real
viewport's mouse capture; those require separate live/physical evidence.

## Clean PIE Evidence

Both worlds began as fresh Human Warriors at village PlayerStart with 40
copper, 0 XP, full health/mana, empty bags, and no accepted quests. Creation
used the existing qualified in-engine widget focus plus Slate Enter path.
No transaction delegates, owner methods, positions, health, XP, currency, or
quest progress were injected. Movement used bounded normal-time W input and
scripted headings through the accepted route waypoints.

The trainer endpoint was approximately `(-1411.94,-765.59,90.15)` and the
vendor endpoint `(-1434.10,-1298.53,90.15)`. Original F opened each service.
Every measured key refreshed and clicked the current Unreal viewport first.

An initial probe used left-click focus. Up selected row 0 in a single-step
probe, but the next Enter probe ended on row 1 with the locked rejection.
The viewport-center click overlaps the trainer rows at this window size.
Using right-click focus instead preserved selection and produced the expected
key results. This is a probe confound consistent with the focus click changing
selection, not proof of a transaction bug or the root cause of September 12's
specific button-click failure. No engine or production pointer patch followed.

With fresh viewport right-click focus and one-engine-tick discrete keys:

- Down selected locked Advanced Combat Drills; Enter kept copper/XP unchanged
  and showed exact `Advanced Combat Drills requires level 2.` feedback.
- Up selected Combat Drills; Enter committed exactly 40 -> 30 copper and
  0 -> 25 XP. Native chat retained the exact completion result.
- Escape closed the trainer, hid the cursor, and left move/look unsuppressed.
- At the vendor, Down selected Recruit Pack. Enter charged 30 -> 0 copper,
  delivered one pack, and exhausted finite stock. Another Enter produced
  `Recruit Pack is out of stock.` without mutation. Up then Enter on tonic
  produced `You do not have enough copper.` without delivery or spend.
- Escape, Enter with no service, and I/I preserved owners and restored the
  cursor-hidden unsuppressed state.
- In a genuinely fresh second world, Enter bought tonic for 40 -> 32 copper;
  Down/Enter bought pack for 32 -> 2. The bag contained exactly one of each.
  RightBracket changed bag index 0 -> 1 without changing stock index 1.
  Up selected tonic; Enter rejected insufficient funds without mutation.
  I/I handed off to Inventory and back to game-only input.

Health stayed 100, mana 50, quest arrays empty, enemies unchanged, and time
dilation 1.0. Every temporary callback/key was None before StopPIE. No save
operation was performed. Native text/state was inspected through Slate; no
claim is made for final HUD pixels, physical keyboard/mouse, held input,
full Tab-focus navigation, a new combat sweep, or keyboard selling/buyback.

## Verification And Preservation

- Final Development build succeeded with `-NoHotReloadFromIDE`.
- All 98 isolated tests passed; zero test warnings, failures, skipped, or
  in-process tests. The same fresh GUI discovered 98 tests.
- All 23 sequential fresh NullRHI package validators emitted their explicit
  markers, including materials-first, UI art, placed Prowlers, progression,
  journal emblem, and the full-zone 53 Fab / 24 original-art baseline.
- All six initialized-editor suites passed: well, notice board, workshop,
  cottage, vendor stall, and road boundaries. GUI was closed for commandlets;
  traces ran outside PIE in the initialized editor world.
- No `LogPython: Error` occurred in today's test, package, or live editor logs.
  Existing vendor physics-resave warnings remain outside test warnings; no
  vendor assets were resaved and this is not a cooked-build readiness claim.
- Final editor PID 47698 owns `127.0.0.1:8123`, outside PIE on
  `/Game/Maps/L_Embermere_Prototype`, with zero dirty content/map packages.

Evidence: `/tmp/embermere-sep13-keyboard-before/index.json`,
`/tmp/embermere-sep13-keyboard-after/index.json`,
`/tmp/embermere-sep13-tests/index.json`, `/tmp/embermere-sep13-packages.log`,
and the current Unreal editor log's `SEP13_` markers. Temporary probes and
test output are not repository artifacts.

Protected original save SHA256 remains
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
Its omitted version stamp was not inferred, loaded, repaired, or rewritten.
The existing keeper resave remains
`38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
Config changes, keeper resave, and unpublished Field Notes remain excluded.

## Next Start

Ask for physical service keyboard and mouse review: F, Up/Down, Enter,
Escape, brackets, and I handoff, without saving/loading the protected journey.
This fallback consumes controller-routed input, not every possible focused
Slate widget key. Check native button focus coexistence before expanding the
contract or claiming full keyboard accessibility. Keep September 12's pointer
issue open and avoid unbounded probes or speculative engine fixes.

Continue unaffected work with the real Still Waters/Mara and panel handoff
routes. Make the next small input/presentation repair only from a reproducible
issue, retaining all 98 tests, 23 package checks, six trace suites, authority
boundaries, protected files, exact project task, and desktop-control denial.
