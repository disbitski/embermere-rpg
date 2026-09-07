# Embermere Quest Objective Presentation Contract

## Purpose

Every tracked quest needs a short instruction that tells the player what to do
without forcing UI code to infer prose from stable IDs, titles, narrative copy,
or numeric requirements. This contract adds descriptive metadata only. It does
not create a new quest, progress, reward, focus, or persistence owner.

## Data Authority

`UEmbermereQuestData::ObjectiveInstructions` owns the authored player-facing
instruction. The first accepted records are:

- `FirstSignsAtTheRuin`: `Defeat 3 Marsh Prowlers.`
- `FenwatchStillWaters`: `Complete a rest at the communal well.`

The field is deliberately distinct from `QuestId`, `ObjectiveId`, title,
description, contextual greetings, and `RequiredObjectiveCount`. Stable IDs and
the numeric requirement continue to own mutation eligibility. Changing display
copy cannot change acceptance, progress, completion, rewards, or save data.

## Read-Only Consumers

The compact tracker combines the focused authoritative record with the authored
instruction in a fixed `260x68` clipped region. The September 7 correction
removes the redundant heading and uses 13-point text with explicit 260-pixel
wrapping, leaving room for a two-line instruction without growing the panel.
Its column slot is left-aligned, not filled, so a wider adjacent target name
cannot stretch the fixed tracker cell:

```text
<title>
<current>/<required>   <objective instruction>
```

The Quest Ledger appends the same instruction to the selected record's objective
line. Its accepted `620x550` panel, eight `596x30` rows, and `596x120` detail
region do not change. Selection remains local; explicit stable-ID focus remains
the only action that changes the compact tracker.

Both consumers use `Objective details unavailable.` when the quest reference is
missing or the authored field is blank or whitespace-only. This fallback is a
presentation fact, not permission to synthesize identity or progress rules.
Long text clips inside the existing fixed cells and never grows neighboring UI.

## Persistence Boundary

Save version `3` continues to serialize stable quest records, objective counts,
and completion state. Objective copy is resolved from the referenced quest data
after load. It is not serialized, migrated, cached as player state, or used to
validate progress. Versions `1` and `2` retain their existing singular-record
adapters.

## Acceptance

Automation must prove exact Mara and Still Waters copy, the stable missing-copy
fallback, active/ready/completed continuity, fixed geometry, clipping, selection
versus focus independence, peer-panel handoff, and zero quest, reward, wallet,
XP, or inventory mutation. Saved-package validation must verify both authored
assets retain their stable IDs, requirements, and distinct instructions.

Clean PIE must create both records through their existing physical `F` owners.
The compact tracker must show the title and full starter instruction at normal
camera distance, including the wrapped Still Waters instruction. The
ledger must show each exact instruction while selecting Mara and tracking Still
Waters or vice versa, with no panel growth or state mutation.

## Accepted 2026-09-04 Implementation

Both quest packages were authored and saved explicitly through the reviewed
commandlet lane. `Embermere.UI.QuestObjectiveDisplayPresentation` brings the
suite to `88`; the no-hot-reload build and isolated run passed `88/88`. The new
focused validator and sequential `20`-package aggregate passed with explicit
success markers and no `LogPython: Error`, retaining 53 grounded Fab actors plus
24 original-art placements.

Clean PIE accepted Mara and Still Waters through physical `F`. The corrected
tracker showed each title, exact progress, and instruction inside `260x68`.
The fixed ledger displayed both rows and each objective line; physical Up
selected Mara while Still Waters remained the compact focus, visibly proving
selection and focus independence without changing either record.

## September 7 Text-Fit Correction

The historical September 4 check established exact payloads and declared
bounds, not full text fit. A new real-widget, saved-asset regression reproduced
Mara at 266.5x69 and Still Waters at 377.5x69. The corrected measurements are
235.5x42 and 213x63 inside the unchanged 260x68 cell. Explicit wrapping and
non-fill column alignment preserve width even beside the longer practice
target name. `Embermere.UI.QuestTrackerTextFit` also compares serialized owners
before and after rendering. Existing fallback, focus, completion, long-copy
clipping, and peer-panel tests remain in place.

A fresh Human Wizard accepted both quests through their existing F owners,
spent 10 mana with Spark Bolt on the practice target without objective credit,
walked to the well, and rested back to 110 mana. Only Still Waters advanced
to 1/1. Returning to the notice board granted exactly 50 XP and 10 copper,
no item, and no second reward on repeated F. Mara remained 0/3. Ledger and
read-only Chronicle handoffs preserved that state and the user's original
save. This proves the real quest/service route and Slate payloads, not final
HUD pixel approval or physical held-input behavior.
