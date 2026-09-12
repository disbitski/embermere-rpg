# September 12 Daily Build

## Scope And Setup

Diagnose September 11's Unreal MCP Buy/Train click mismatch before changing
gameplay. No production C++, package, map, collision, art, or save-format
change was justified. This run records input evidence, retains all automated
gates, and corrects a misleading trainer keyboard note.

The ACTIVE daily heartbeat runs at 8 AM America/New_York in existing task
`019efc2b-b36c-71f0-8892-8e4f80829fc9`. App thread inspection confirms its cwd
is `/Users/wizard/Documents/Unreal Game`, also the saved Unreal Game project.
The local configuration binds Unreal MCP to `http://127.0.0.1:8123/mcp` and
disables desktop-control plugins/bridges with default desktop access denied.
No desktop control, broad capture, security change, or unlock request was used.

`Scripts/check_unreal_setup.sh` passed every check: project, Epic launcher,
full Xcode selection, working Metal compiler, UE 5.8, MCP/AllToolsets/Python
plugins, and local MCP URL. The configured project-local Blender executable
exists; its live bridge was deliberately not queried because no art was
planned. Do not claim a Blender live connection was verified today, or
regenerate the already-correct client config from the checker's generic hint.

## Pointer Versus Keyboard Evidence

Fresh discovery found 96 tests in the existing editor. Clean Human Warrior
creation's pointer confirmation did not close the modal. Explicit in-engine
keyboard focus followed by native Slate Enter did confirm it. No confirmation
method was invoked directly. Inventory closed and a normal-time walking route
reached the trainer through original F interaction:

`PlayerStart -> (-2020,-370) -> (-1640,-200) -> (-1420,-450) -> (-1420,-700)`.

The measured trainer endpoint was approximately `(-1411.98,-765.54,90.15)`.
Continuing toward `(-1430,-1200)` reached the vendor at approximately
`(-1434.09,-1298.53,90.15)`, and original F opened Fenwatch Supplies. No
position, health, XP, currency, or inventory fixtures were injected. These
were bounded W inputs with scripted headings, not physical camera approval.

Temporary read-only observers listened to native button pressed, released,
clicked, and hovered delegates. The probe never broadcast those delegates or
called service/HUD transaction methods. Both action buttons were enabled.
MCP pointer Click returned true but produced no observed button events and no
authoritative wallet/XP/item change. Explicitly focusing the same UButton in
Unreal and sending native Slate SpaceBar did emit pressed/released/clicked
events and commit the existing transaction:

| Native Keyboard Action | Copper | XP | Bag |
| --- | --- | --- | --- |
| Combat Drills | 40 -> 30 | 0 -> 25 | Empty |
| Buy Marsh Tonic | 30 -> 22 | 25 | One tonic |
| Sell selected tonic | 22 -> 25 | 25 | Empty |
| Buyback tonic | 25 -> 22 | 25 | One tonic |

Exact purchase, sale, buyback, and training copy appeared in native status/chat.
Player health remained 100, quests stayed empty, and enemies stayed at their
unchanged home/full-health state. This is successful native keyboard UI
routing with explicit focus, not a physical click or complete Tab-navigation
acceptance. It also does not newly prove finite-stock exhaustion or every
transaction rejection; those remain covered by the existing automation.

The engine implementation explains why tool success is insufficient:
`USlateInspectorToolset::SimulateClick` sends pointer events and returns true
without verifying the intended widget handled them. Its center calculation
does use transformed geometry. Snapshot positions are absolute while sizes
are local; adding half the reported size to the position is not a reliable
absolute-center calculation. The inspected UButton geometry reported zero
while the snapshot had nonzero Slate button bounds. That discrepancy is
diagnostic evidence, not a proven project layout defect or a complete native
hit-test-path diagnosis. Do not patch engine code or infer a Mac lock state.

After the required isolated checks and editor restart, window geometry changed
from 3840x1966 to 1920x983 in Slate. Pointer confirmation still failed. A fresh
confirmed Warrior walked the same route and repeated the Train comparison:
pointer Click gave no button events and left 40 copper/0 XP; explicitly
focused SpaceBar gave all three events and exactly 30 copper/25 XP. A restart
therefore did not resolve the observed issue.

## Limits And Probe Corrections

- Trainer source has bracket offering selection, but no implemented `T`
  action. The displayed X is a close button, not a proven X-key close binding.
  Correct the contract rather than claiming those keys were tested.
- Vendor stock rows intentionally call `InitIsFocusable(false)`. An attempted
  focus/Space selection of Recruit Pack left the tonic selected; no pack
  purchase, lock, or rejection proof came from that attempt. Do not enable
  focus only inside a probe and describe it as the shipped keyboard contract.
- Initial exploratory calls hit unavailable API/property names, a callback
  signature error, and a dependent uninitialized variable. A zero-argument
  callback factory fixed the delegate observer. These errors are not clean
  validation runs.
- One fresh-session console Type left an incomplete prefix, and the following
  command produced a SyntaxError. Short, individually confirmed commands
  recovered. No route callback started from those malformed commands. Inspect
  the Cmd subtree and require a unique log marker before assuming execution.
  Type appends characters; it does not replace existing text automatically.
- Both completed probe sessions removed every delegate observer, released all
  forced keys, and unregistered callbacks. I/I handoff ended with cursor false,
  move/look suppression false, callback/key None, and zero observer bindings.

## Verification And Preservation

- No-hot-reload Development editor build succeeded; target was up to date.
- All 96 isolated automation tests passed, zero test warnings, failed,
  skipped, or in-process tests. No Python errors in this run.
- All 23 sequential fresh NullRHI package validators emitted explicit success
  markers, followed by the aggregate's `validators=23` marker; no Python errors.
  Saved material usage, all eighteen rig slots, placed Prowler roles, UI art,
  progression, services, quests, and the journal emblem remain valid.
- Six initialized-editor collision suites passed: well, notice board,
  workshop, cottage, vendor stall, and road boundary. No Python errors in
  their validation interval. World baseline remains 53 Fab and 24 original art.
- Fresh post-restart MCP discovery again returned 96. The real UnrealEditor,
  PID 21774 at closeout, owns 127.0.0.1:8123. It is outside PIE on
  `/Game/Maps/L_Embermere_Prototype`, with no dirty content/map packages.

Local evidence: `/tmp/embermere-sep12-tests/index.json`,
`/tmp/embermere-sep12-tests.log`, `/tmp/embermere-sep12-packages.log`, and
`/tmp/embermere-sep12-input.log` (pre-restart probe). Restart evidence remains
in the current Unreal editor log. Temporary probes and test outputs are not
repository artifacts. This is not a new Prowler combat/animation sweep, a
cooked build, a physical camera/HUD review, or a completed pointer fix.

The protected original save was never loaded, rewritten, or migrated; SHA256:
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
The existing local keeper material resave also remains unchanged; SHA256:
`38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
Config normalization, keeper resave, and unpublished Field Notes stay excluded.

## Next Start

Ask for a physical Train/Buy check before treating the MCP pointer mismatch
as a shipped gameplay bug. In a fresh Warrior world, one real Train click
should produce 40 -> 30 copper and 0 -> 25 XP; a subsequent tonic Buy should
produce 30 -> 22 copper and one tonic. No save operation is needed. Preserve
all automated gates and continue unaffected work without desktop access.

The next bounded improvement is to review the vendor/trainer keyboard
selection-and-action contract, especially non-focusable vendor stock rows,
before implementing any explicit navigation fallback. Do not add unsupported
shortcut claims or bypass transaction authority. Existing keyboard focus plus
Space can provide qualified MCP action evidence, not mouse or full navigation
approval. Keep September 10's Prowler and September 11's material acceptance.
