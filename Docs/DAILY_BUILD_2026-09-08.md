# September 8 Daily Build

## Scope And Setup

Acceptance and readiness pass over September 7's bounded quest-tracker module.
No gameplay, UI, art, package, or save-schema changes were justified by this
run. The existing task `019efc2b-b36c-71f0-8892-8e4f80829fc9` reports cwd
`/Users/wizard/Documents/Unreal Game`. Its daily heartbeat is active at 8 AM
America/New_York. The prompt names this folder and reads TODO Start Here first.

Project config disables desktop plugins/bridges and defaults desktop access
to deny. Unreal MCP uses `http://127.0.0.1:8123/mcp`. The configured Blender
server executable and working directory exist; its live bridge was not queried
because no original-art work was planned. Shell builds, commandlets, Unreal
Slate inspection/input, and Git were the only execution paths used today.
No desktop control, OS screenshots, permission changes, or unlock request.

## Real Mara Route

A fresh Human Warrior accepted First Signs at the Ruin through Mara's original
F interaction. The run stayed at time dilation 1.0 throughout, with no position,
health, objective, reward, or level injection and no save/load operation.

| Check | Observed State |
| --- | --- |
| Start / accept | 100 health, 50 mana, 0 XP, 40 copper, empty bag, Mara 0/3 |
| First kill | Saved Prowler 01, Strike damage 36/36/28, Mara 1/3, one tonic |
| Second credited kill | Respawned Prowler 01, 36/36/28, Mara 2/3, two tonics |
| Third credited kill | Respawned Prowler 01, Shield Slam/Strike 48/36/16, Mara 3/3, three tonics, player 70 health |
| Return before F | 0 XP, 40 copper, 70 health, 50 mana, no premature reward |
| Original Mara F | Completed 3/3, 125 XP, 60 copper, three tonics and one Recruit Pack |
| Derived progression | Level 2, 80/110 health, 53/53 mana; 30 missing health preserved |
| Repeated Mara F | Same XP, copper, items, resources, and completed state |

These are three real kills of the same saved actor after its natural respawn,
not an all-three-Prowler kill sweep. Earlier approaches to the northern pocket
and pauses between tool calls caused real deaths; village recovery retained
quest progress and inventory. Do not describe the whole run as a clean solo-pull
or physical combat-feel approval. The third credited fight included retaliation;
all three Prowlers were later at their saved homes with 100 health, and the
practice target remained at 150. Target selection cleared on each credited kill.

The ready tracker and contextual greeting appeared before turn-in. At reward
commit, Slate exposed LEVEL 2, the quest update, the icon-bearing Recruit Pack
reward, and Mara's separate speaker/body presentation. Later snapshots showed
expired transient notifications, exact durable chat, and the unchanged 260x68
tracker. Ledger showed COMPLETED 3/3 and the exact reward description.

J -> M -> I -> I reached the completed Ledger, read-only Chronicle, Inventory,
and game input. Chronicle showed Current Journey: Human Warrior, Level 2,
with the separately rejected unstamped saved journey and disabled Load Saved.
Real F later opened Fenwatch Supplies at 60 copper with finite pack stock still
1, then Fenwatch Training with both Combat Drills tiers READY at 125 XP. No
purchase or training transaction was performed in this replay. Closing panels
left cursor hidden and move/look unsuppressed. Temporary callbacks were empty
and all held keys released before PIE teardown.

## Input Evidence

UE 5.8's `Input.+Key` is continuous injection, not a single held-key transition.
`EnhancedInputSubsystemInterface.cpp` injects `IE_Pressed` on each forced-input
tick. A duration-based Tab can therefore cycle repeatedly, especially when the
editor changes from slow background ticks to normal frame rate. This explained
inconsistent synthetic selection without requiring a game targeting change.

The final temporary probe released discrete actions on the next Slate post-tick,
then checked committed state on a later tick. Movement retained bounded holds;
polled panel keys retained enough time for the controller poll. Every key was
preceded by a fresh Unreal-owned viewport snapshot and click. It used only the
same `SlateInspectorToolset` CDO methods through reflected `call_method`, not
desktop automation. Source-location snapshots force a fresh tree; cached
snapshots are not sufficient evidence of a just-completed transition.

The helper had a 90-second wall-clock bound, original-world checks, release and
unregister cleanup, and optional predefined W retreat after a real target defeat.
Scripted control yaw aided headings; it did not teleport or relocate actors.
This is normal-time engine-input evidence, not physical held-input approval.
The probe remains temporary under `/tmp`, not production gameplay or a committed
automation script. Early API exploration produced Python errors for unavailable
bindings and one attempted sequence after player death. A final diagnostic also
queried the editor world during PIE, where it returned None; the corrected
post-PIE query confirmed the exact map and zero dirty packages. Those are
distinct from the validator suites; do not claim the entire GUI log was
error-free. Use the game world during PIE and the editor world after teardown.

## Protected State

The original `Saved/SaveGames/EmbermerePrototype.sav` was only inspected read-only
and retains SHA256
`c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
No version inference, recovery, migration, overwrite, or load. Existing Config
normalization, the keeper material resave, and unpublished `Docs/FieldNotes/`
remain outside this change. World baseline stays 53 Fab / 24 original-art
placements, confirmed by the fresh full-zone package validator.

## Verification

- No-hot-reload Editor build: passed September 8.
- Isolated automation: 95 succeeded, zero failed/warnings/not-run/in-process.
- Fresh sequential package aggregate: all 21 validators and the aggregate
  success marker passed, including exact 53 Fab / 24 original-art counts.
- Initialized-editor collision suites: well, notice board, workshop, cottage,
  stall, and road all passed with explicit markers.
- No LogPython: Error in either commandlet log or the six live trace runs.
- Fresh module: 95-test discovery; Human Warrior creation at exact 100 health,
  50 mana, 0 XP, 40 copper, empty quests/bag; Inventory close ended cursor-hidden
  with move/look unsuppressed and no active temporary input callback.
- UnrealEditor PID 96218 owns localhost 8123. Editor is out of PIE on
  `/Game/Maps/L_Embermere_Prototype.L_Embermere_Prototype`, with zero dirty map
  or content packages. PID is session evidence, not a future launch requirement.
- Evidence locations: `/tmp/embermere-sep8-tests/index.json`,
  `/tmp/embermere-sep8-tests.log`, `/tmp/embermere-sep8-packages.log`, and the
  editor's `SEP8_*` route logs. Temporary evidence is not durable test output.

## Remaining Gates

User review still owns final dialogue/tracker/reward pixels, physical W/S
autorun cancellation, held right-mouse camera, and held Ctrl+M. Preserve the
accepted September 7 Still Waters proof. A single-pass all-three-Prowler sweep
and normal-camera feel remain separate from today's exactly-once Mara route.
Do not add another system or alter accepted combat rules to conceal input-tool
limitations. Continue only with a bounded issue reproduced through actual play.
