# Astra In The Embermere Workflow

Date: 2026-09-04

## Scope And Baseline

User-selected setup: GPT-6 Astra, extra-high reasoning. This is one practical
build session, not a controlled comparison with another model. The starting
commit is `ca73911`, with 88 discovered Embermere tests and 20 saved-package
validators. Existing Config edits and the keeper skin material are excluded.

The challenge is the planned quest-update observer plus a small original
Blender-rendered journal emblem. Acceptance still requires authority boundaries,
focused regressions, a no-hot-reload build, full automation, fresh commandlet
package validation, and visual PIE inspection. A larger diff or a green tool
response is not evidence of better model performance.

## Evidence Log

- Unreal first-class MCP discovery responded and listed all 88 expected tests.
- Blender first-class MCP reported a three-object scene. Do not reset it.
- Unreal application capture succeeded. No direct HTTP fallback was needed.
- Source inspection found the established state-change delegate fires on
  restore; a separate live event is needed to prevent notification replay.
- One initial source lookup used incorrect directory names; `rg --files`
  corrected discovery. Tool friction and mistakes belong in the record too.
- First Blender build failed because the MCP script namespace does not define
  `__file__`. Its outer response still said `isError: false`; the nested error
  was authoritative. Corrected the source to accept the established explicit
  `project_root` argument. No scene or configuration change was needed.
- The corrected factory-clean build produced a 2,580-triangle journal emblem,
  five material roles, one UV channel, and zero non-manifold edges. The
  transparent 512x512 render was inspected visually. A live-bridge pixel
  inspection found alpha from 0 to 1, occupied bounds `(45,58)-(466,461)`,
  and 166,690 transparent pixels; the original three-object scene was intact.
- Unreal imported the PNG with `TextureFactory` and explicitly saved its UI
  texture package. The FBX is an editable-source export, not a newly tested
  classic-FBX Unreal mesh import. No rig, animation, world actor, or collision
  was added in this experiment.
- The first C++ build succeeded, and the initial complete run passed 91/91.
  All 21 saved-package validators then passed in a fresh commandlet with
  their success markers and no `LogPython: Error`. Existing vendor dependency
  warnings remain; this does not claim a warning-free engine log.
- An expanded test subsequently failed because a bare `NewObject` widget was
  not initialized before inspecting its actual widget tree. The fixture was
  corrected to call `Initialize`; a second failed run then exposed the need
  to retain the returned Slate shared reference so destruction did not unbind
  the widget before the test events. Neither failure was hidden by weakening
  the geometry assertions. Full persistence apply is also tested
  twice, beyond merely broadcasting a synthetic restore signal.
- The final fresh-process run passed **91/91**, zero failed, zero warnings,
  and zero unrun tests. The final sequential **21-package** aggregate passed
  every marker with no Python errors. After restart, first-class Unreal MCP
  rediscovered exactly 91 tests and all six initialized-world trace validators
  passed: well, notice board, workshop, cottage, stall, and road boundary.
- A clean PIE run confirmed Human Warrior, closed Inventory, synthetically
  relocated the character into Mara's interaction radius, and used real `F`
  dispatch to accept the existing quest. The captured frame showed the new
  top-right journal notification with exact `0 / 3` instructions, clear of
  the left HUD and bottom hotbar. This is not normal-route traversal proof.
- Live capture instrumentation initially assumed an unavailable
  `WidgetBlueprintLibrary` Python binding. Its callback errors were a probe
  failure, not a passing validation. A corrected local candidate attempted
  reflected controller/HUD access, but it was not fully exercised. The
  temporary helper was discarded rather
  than shipping an unvalidated capture script.
- The Mac was locked during the later interaction pass. CUA reported that
  state explicitly and could not provide the physical desktop-control path.
  Unreal and Blender MCP continued working. Separately, some Slate clicks/keys
  reported dispatch success without producing the intended control action;
  this run does not isolate the lock as their cause. `Alt+4` was not a valid Unreal key
  name, and atomic modifier chords did not substitute for held physical
  input. PIE was stopped and the editor's `Lit` mode verified afterward.
- After the user logged in locally, CUA access returned. A genuinely fresh
  Human Warrior run closed Inventory, used Q, and canceled with W. Measured
  position changed from approximately `(-2400,-1200)` to `(-1793,-831)`.
  Real F accepted Mara's quest and showed the journal notification. Unlike
  the earlier fixture, this acceptance used the PlayerStart approach.
- Later Q attempts did not move the pawn. Native bounds inspection found the
  existing `FabPass_Village_Table` at that contact position. It is not evidence
  of successful traversal or a reason to move accepted art automatically.
  Atomic Ctrl+M opened Chronicle without producing inversion feedback, and
  atomic Shift+F1 changed the view mode; neither counts as held-modifier proof.
- The resumed console probes exposed further mistakes: `PlayerHudWidget` is
  a non-reflected controller member, an object iterator can return objects
  without a class, and Stats exposes `CurrentExperience`, not `Experience`.
  Those probes logged Python errors. Runtime API inspection identified
  `unreal.WidgetLibrary.get_all_widgets_of_class`, which resolved the actual
  HUD and observer. This does not change the clean commandlet-validator result.
- A labeled temporary PIE objective fixture then committed 1/3 and 3/3 through
  the quest log. Paused-world captures showed exact Progress and Ready copy
  in fixed bounds. Real F performed Mara's turn-in, and a short-lived callback
  paused on the observed completion before unregistering. Exact live state was
  **125 XP, 60 copper, completed Mara quest, one Recruit Pack**. Completion,
  reward, and level-up panels had clear mutual separation. Inventory cleared
  the notice, and closing it did not replay it. This is not a real Prowler-kill
  or Still Waters route replay; no user save was written or loaded.
- Read-only Chronicle inspection rejected the existing v3 slot for
  contradictory legacy quest state. The old bottom dialogue also overlapped
  the hotbar in the completion capture. Both are recorded follow-ups, not
  repairs made by this observer milestone.

## What This Run Demonstrates

The useful outcome is a connected implementation: quest authority publishes
copied live facts, a bounded native observer consumes them, original Blender
art supplies its emblem, and persistence remains silent. The observer has
one fixed 420x108 surface, latest-wins replacement, a three-second lifetime,
and explicit modal/death/load/unbind clearing. It owns no quest or reward
mutation. Save version 3 and the 53-Fab/24-original world baseline are intact.

The most encouraging qualitative result is that the existing contracts were
carried through C++, Blender, saved packages, and regression checks in one
session. It is not evidence that Astra invented the architecture or that its
code is categorically better. Embermere's accumulated notes and tests supplied
the constraints. The expanded tests and failed probe also show why the model
must remain inside that verification loop.

## Remaining Acceptance

The real Prowler and Still Waters routes, remaining peer panels, and held
right-mouse/Ctrl+M sweep remain required. Normal approach/F acceptance and
fixture-backed progress/ready/completion/Inventory coexistence are now inspected,
but are not the entire playable loop. The prior compact Still Waters tracker
can clip its long instruction; full ledger detail remains the fallback. The
bottom dialogue/hotbar overlap and rejected existing Chronicle slot are also
separate follow-ups. No unrelated UI, save, or map repair was made here.

The report and companion field-note draft are review material. No public
note, social post, hero, or podcast was published during this experiment.

## A Fair Next Comparison

Use the same starting commit, bounded brief, tool versions, unlocked machine,
test suite, and acceptance rubric for each model in separate worktrees. Record
wall time, billed usage, human interventions, compiler/test failures, tool
retries, regressions, and blind normal-camera visual ratings. Repeat more
than one task and distinguish warm caches from model time. This run is a
candidate task/rubric, not a numerical result for that comparison.

## Comparison Limits

Historical Embermere notes establish prior failure modes and acceptance rules,
not same-task timing or a randomized baseline. Do not infer percentage speed,
cost savings, fewer errors, or general superiority from this run. The inherited
contracts, working bridges, existing assets, and regression suite all contribute
to the outcome. Record actual build attempts, tests, visual corrections, tool
fallbacks, and remaining limitations before forming a qualitative impression.

## Reproducible Evidence

- Final full automation report: `/tmp/embermere-astra-accepted-tests/index.json`
  at `2026-09-04 20:16:41 UTC`: 91 success, 0 failed/warnings/not-run.
- Final package log: `/tmp/embermere-astra-final-packages.log`, ending in
  `Embermere saved prototype package validation passed: validators=21`.
- Final journal marker: `FENWATCH_JOURNAL_VALIDATION_OK` in that same log.
- Live trace markers: editor log at `20:21:53-20:21:54 UTC`, all six passed.
- Unlocked PIE markers: `ASTRA_VISUAL_PROGRESS` at `20:58:26 UTC`,
  `ASTRA_VISUAL_READY` at `20:59:40 UTC`, `ASTRA_COMPLETION_CAPTURED` at
  `21:00:37 UTC`, and `ASTRA_REWARD_STATE` at `21:01:45 UTC`.
- Actual application captures: `/tmp/embermere-quest-progress.png` and
  `/tmp/embermere-quest-completion.png`; local draft copies are under
  `Docs/FieldNotes/assets/`. They explicitly show the fixture-backed states,
  including the inherited bottom dialogue overlap. They are not staged as
  publication assets or used to claim a full combat replay.
- The two intermediate expanded-fixture reports are retained under
  `/tmp/embermere-astra-final-tests` and `/tmp/embermere-astra-verified-tests`:
  each was 90 success and 1 failed. All four C++ builds in this implementation
  sequence compiled successfully; the failures were runtime test assertions.
- Blender source and render:
  `ArtSource/Blender/UI/FenwatchJournal/`; deterministic scripts and the
  texture validator are tracked. Temporary logs are local evidence and are
  not substitutes for the committed source/tests/contracts.

Official model reference, kept separate from this experiment's findings:
[OpenAI GPT-6 Astra model documentation](https://developers.openai.com/api/docs/models/gpt-6-astra).
