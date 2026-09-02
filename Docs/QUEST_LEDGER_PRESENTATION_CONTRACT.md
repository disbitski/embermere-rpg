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
- Quest data owns stable IDs, title, objective requirement, copy, and rewards.
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
