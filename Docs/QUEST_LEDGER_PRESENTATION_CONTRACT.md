# Embermere Quest Ledger Presentation Contract

## Purpose

The quest ledger gives the accepted version-3 quest collection a deliberate
player-facing surface without creating a second quest owner. It lists every
tracked record, preserves completed history, and lets the player choose which
record the compact HUD tracker projects.

This is a presentation and input-lifecycle feature. It does not widen the save
schema or any quest mutation API.

## Authority Boundary

- `UEmbermereQuestLogComponent::QuestStates` remains the only mutable runtime
  quest authority.
- Quest data owns stable IDs, title, objective requirement, objective
  instructions, contextual copy, and rewards.
- Quest gameplay owners continue to own acceptance, progress, completion, and
  exactly-once reward commits.
- The ledger widget may read `QuestStates` in authored order and request one
  explicit `FocusQuest(QuestId)` operation.
- `FocusedQuestId` and the widget's selected row are transient. Neither is
  serialized, migrated, or inferred as durable player intent.
- Focusing may update only the established `ActiveQuest` compatibility
  projection. It may not alter progress, completion, rewards, wallet, XP,
  inventory, equipment, service state, or save bytes.

## First-Slice Surface

- One fixed native panel lists at most the accepted eight tracked records.
- Every occupied row shows the quest-owned title, one exact state label, and
  objective progress:
  - `ACTIVE` before the objective requirement is met;
  - `READY` at the requirement before turn-in;
  - `COMPLETED` after the authoritative reward commit.
- An empty ledger shows a fixed `No quests tracked.` state without resizing.
- One selected row is presentation state. Mouse click or Up/Down changes the
  selected row; `Focus Quest`, Enter, or an explicit row action requests focus.
- `J` toggles the ledger and Escape closes it. The command remains mouse
  accessible beside the existing bottom-right Chronicle command.
- Opening Inventory, Vendor, Trainer, or Chronicle closes the ledger; opening
  the ledger closes those peer panels. Controller-owned input mode and cursor
  state follow the same established lifecycle.
- The panel, all eight row bounds, footer, and action areas remain fixed. The
  panel must stay clear of chat, hotbar, target/nameplate presentation, and the
  compact focused-quest tracker.

## Explicit Non-Goals

This slice does not add quest acceptance, abandonment, sorting, sharing, map
tracking, reward claims, save/load controls, focus persistence, auto-focus
rules, or a new quest state. An available but unaccepted quest belongs to its
world owner and contextual presentation, not the tracked-record ledger.

## Selected-Quest Detail Slice

The next bounded surface extends the accepted ledger without changing its
authority. The panel grows once to a fixed `620x550`; all eight existing
`596x30` record rows remain unchanged. A fixed `596x120` detail region sits
between the record list and the existing selection/tracker footer.

The selected record reads these values directly from its authoritative quest
state and quest data:

- quest-owned title and description;
- the exact active, ready, or completed greeting for the current state;
- current objective progress, the quest-owned requirement, and the distinct
  authored objective instruction;
- exact XP and copper rewards;
- the optional reward item's display name when the soft reference resolves.

The detail region uses fixed title, wrapped description, wrapped state-copy,
objective, and reward cells. Long copy is clipped inside those cells rather
than increasing panel or row size. A quest with no reward item shows
`No item reward`; a non-null item reference that cannot resolve shows the
stable presentation fallback `Reward item unavailable`. Those phrases report
presentation availability only and do not change reward validation or commit.

Changing the selected row may refresh only these detail cells. It cannot
change `FocusedQuestId`, the compact tracker, progress, completion, rewards,
wallet, XP, inventory, equipment, save bytes, or any service. `Focus Quest`,
Enter, and explicit row focus remain the only focus requests. The detail
surface is destroyed with the widget and has no durable state.

## Acceptance

Focused automation must cover:

- empty, one-record, two-record, and full eight-record layouts;
- exact active, ready, and completed copy and objective counts;
- mouse-facing row selection plus keyboard-style wrapped selection;
- explicit focus switching and duplicate-focus safety;
- completed-history visibility;
- peer-panel handoff, close, and teardown-safe state;
- zero wallet, XP, inventory, progress, completion, or reward mutation;
- save capture and restore proving selected row and focused quest are absent
  from durable state.

Clean PIE must show active Mara and completed `Still Waters` together, switch
the compact HUD projection both ways, and prove both authoritative records and
their rewards remain unchanged.

## Accepted 2026-09-02 Implementation

The first slice is implemented as a fixed `620x430` native panel. It reserves
eight stable `596x30` row cells, reports the tracked count against the accepted
eight-record cap, and keeps selection/tracker status plus the focus action in
fixed footer bounds. The separate `140x38` Quest Ledger command sits at the
bottom-right, eight pixels above Chronicle, while `J` remains the direct
keyboard path.

`Embermere.UI.QuestLedgerPresentation` covers empty, one-record, two-record,
and full eight-record geometry; exact `ACTIVE`, `READY`, and `COMPLETED` copy;
fixed row bounds; mouse-facing row actions; completed history; and the empty
state. `Embermere.UI.QuestLedgerFocusLifecycle` covers wrapped keyboard
selection, explicit and duplicate focus, nonserialized focus, peer-panel
handoff, teardown, and zero quest/reward/currency/inventory mutation. The full
suite now passes `85/85` in both an isolated commandlet and the restarted
editor's MCP runner.

Clean PIE accepted Mara at `0/3` and Still Waters independently, then completed
Still Waters through one real damaged-player communal-well rest and physical
notice-board turn-in. The ledger displayed `First Signs at the Ruin | ACTIVE |
0/3` beside `Still Waters | COMPLETED | 1/1`. Up/Down changed only selection;
Enter switched the compact tracker in both directions; `I`, `J`, and `M`
performed exclusive peer-panel handoff. XP remained `50`, the Still Waters
reward remained exactly once, and neither quest record changed while focus was
switched.

## Accepted 2026-09-03 Selected-Quest Details

The accepted detail extension grows the panel once to a fixed `620x550` while
retaining all eight `596x30` record rows. The selected record now owns one fixed
`596x120` read-only region for quest title, clipped wrapped description, exact
active/ready/completed quest copy, objective progress, and rewards. Selection
refreshes those cells without moving compact-tracker focus; Enter or the focus
action remains the only way to change that transient projection.

Rewards come directly from quest data. Mara displays `125 XP | 20 copper |
Recruit Pack`; Still Waters displays `50 XP | 10 copper | No item reward`.
An authored but unusable reward item reports `Reward item unavailable` without
changing completion validation or reward ownership. The detail and its selected
row disappear with the widget and add no save field.

`Embermere.UI.QuestLedgerDetailsPresentation` covers fixed geometry, long-copy
clipping, active/ready/completed copy, exact item/no-item rewards, and the
unavailable-item fallback. `Embermere.UI.QuestLedgerDetailLifecycle` covers
mouse and wrapped keyboard selection, focus independence, peer handoff,
teardown, and zero quest, wallet, XP, inventory, or reward mutation. The
no-hot-reload build, isolated commandlet, and restarted-editor MCP runner each
passed the expanded `87/87` suite; all focused validators and the sequential
19-package aggregate retained their exact success markers.

Clean PIE produced the real two-record state through physical notice-board,
Mara, and communal-well `F` interactions. Completed Still Waters and active
Mara fit together at normal camera distance with all detail copy inside the
fixed panel and clear of chat, hotbar, tracker, and bottom-right commands.
Up/Down changed details only, Enter moved focus explicitly, and Inventory
replaced the ledger cleanly. Still Waters remained paid exactly once at `50`
XP and `10` additional copper while Mara remained active at `0/3`.

## Accepted 2026-09-04 Objective Instructions

`UEmbermereQuestData::ObjectiveInstructions` now owns one short descriptive
instruction independent of stable objective identity and numeric requirement.
Mara owns `Defeat 3 Marsh Prowlers.` and Still Waters owns `Complete a rest at
the communal well.` The selected detail objective line consumes that field
read-only. Blank, whitespace-only, or missing data reports `Objective details
unavailable.` without changing validation, progress, or rewards.

The compact tracker consumes the same field in a fixed `260x68` clipped region.
The ledger remains exactly `620x550`, with eight unchanged `596x30` rows and one
unchanged `596x120` detail region. Long source copy remains intact but clips at
the render boundary instead of growing either surface.

`Embermere.UI.QuestObjectiveDisplayPresentation` covers exact copy, fallback,
active/ready/completed continuity, fixed geometry, clipping, selection/focus
independence, peer handoff, and zero mutation. The no-hot-reload build and
isolated suite passed `88/88`; the focused validator and sequential 20-package
aggregate passed without Python errors. Clean PIE accepted both quests through
physical `F`, displayed both exact instructions, and selected Mara while Still
Waters remained tracked.
