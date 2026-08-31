# Embermere Multi-Quest Contract

## Purpose

Embermere save version `3` makes multiple durable quests possible without
erasing completed no-replay history or hiding progress in world art and
services. The runtime and save formats now share one bounded keyed-ledger
contract. Versions `1` and `2` remain readable through explicit singular-record
adapters.

The migration was accepted on 2026-08-31. This document is now both the durable
foundation contract and the implementation boundary for the first second-quest
slice, `Still Waters`.

## Accepted Version-3 Save Shape

Version `3` adds a bounded array of records. Each record must contain:

- stable quest ID;
- matching soft quest-data path;
- stable objective ID;
- validated objective progress;
- completed state.

The ledger is bounded to eight records. Presentation may expose one transient
focused quest without making focus durable authority. The old singular
`QuestState` field remains serialized only so versions `1` and `2` can be read;
a native version-3 save must not mix that legacy field with the new array.

Before any live mutation, load must resolve and validate the entire candidate:

- every quest asset resolves and its authored quest/objective IDs match;
- quest IDs are unique;
- progress is nonnegative and does not exceed the authored requirement;
- completed records contain a coherent terminal progress value;
- unknown, missing, duplicate, mismatched, or contradictory records reject the
  complete load;
- capacity conflicts reject without dropping an otherwise valid record.

Only after all records pass may the live ledger be replaced atomically. A
repeated load must replace the same ledger without duplicating progress or
replaying rewards.

## Legacy Version-1 And Version-2 Boundary

Versions `1` and `2` each contain zero or one singular quest record. That source
shape is preserved as history rather than promoted to a second runtime
authority:

- the adapter resolves the legacy asset and validates stable quest/objective
  IDs, progress, and completion through the same native version-3 path;
- a valid singular record becomes a one-record candidate ledger in memory;
- an empty record becomes an empty candidate ledger;
- an invalid record rejects the entire load before mutation;
- reading an older slot never rewrites, migrates, or drops its completed
  history.

## Runtime Authority

The runtime quest owner is `QuestStates`, a keyed ledger rather than one
implicit active slot. Its mutation APIs take stable quest and objective IDs
explicitly:

- offer or accept one quest by quest ID;
- add committed objective progress to one matching quest/objective pair;
- query available, active, ready, and completed state by quest ID;
- complete and grant rewards for one exact quest ID;
- reject duplicate acceptance and duplicate completion.

Quest data owns IDs, objective requirements, copy, and rewards. A dedicated
quest gameplay owner owns acceptance, progress, completion, and exactly-once
reward commit. UI and world presentation remain read-only consumers. Art,
rest, trainer, vendor, combat, and persistence services must not acquire quest
authority merely because a quest observes one of their committed outcomes.

`ActiveQuest` remains only a derived compatibility projection for established
Blueprint and HUD readers. `FocusedQuestId` selects that projection, follows
explicit focus or exact progress, and is transient. Save capture serializes the
ledger, never focus or projection state. Mara's contextual greeting queries
`FirstSignsAtTheRuin` directly, so focusing a second quest cannot hide or alter
her authoritative state.

## Second Fenwatch Slice: Still Waters

The version-3 ledger and compatibility suite are accepted. The first
additional content slice is now:

- quest ID: `FenwatchStillWaters`;
- title: `Still Waters`;
- objective ID: `FenwatchRestCompleted`;
- requirement: one committed successful communal-well rest;
- reward: `50` XP and `10` copper, with no item reward;
- offer owner: a separate art-free notice-board quest actor or component;
- progress owner: a dedicated quest objective router that observes the
  immutable committed rest outcome and forwards one stable objective event to
  the quest ledger.

The notice-board mesh remains presentation-only. The well mesh remains art and
collision only. The rest service continues to own recovery eligibility and
mutation; the rest observer continues to own only transient VFX. None of them
contains quest data, progress, completion, or reward logic.

Only a committed rest `Success` may advance `FenwatchRestCompleted`. Pending,
interrupted, rejected, duplicate, loaded, or replayed presentation state must
not advance it.

## Accepted Foundation Verification

The 2026-08-31 foundation is accepted. Focused automation proves:

- empty, one-record, and multiple-record round trips;
- version `1` and `2` singular-record compatibility without slot rewriting;
- repeated-load idempotence and no reward replay;
- duplicate quest IDs, missing assets, mismatched IDs, invalid progress,
  contradictory completion, and capacity overflow reject atomically;
- two simultaneous quests can progress independently;
- an unrelated quest giver cannot complete or replace another quest;
- exact quest/objective routing and exactly-once completion rewards;
- the established HUD reads the focused compatibility projection without
  owning or mutating the ledger;
- focused-quest selection remains transient unless a later contract says
  otherwise;
- the notice board, well art, rest service, rest presentation, and trainer stay
  free of quest authority.

The focused suite is:

- `Embermere.Quests.MultiQuestRuntime`;
- `Embermere.Persistence.MultiQuestRoundTrip`;
- `Embermere.Persistence.LegacyQuestCompatibility`;
- `Embermere.Persistence.MultiQuestValidationRollback`.

Both the isolated commandlet and restarted-editor MCP runner passed all `80`
Embermere tests. The no-hot-reload build, five focused validators, sequential
18-package aggregate, full-zone validator, and initialized-world well,
notice-board, workshop, cottage, stall, and road traces also passed with no
`LogPython: Error` while retaining 53 grounded Fab actors plus 24 original-art
placements.

Clean PIE confirmed Human Warrior creation, Inventory close, measured Q movement
and W cancellation, and physical `F` acceptance of Mara's original quest. Live
inspection showed one keyed `FirstSignsAtTheRuin` record, matching transient
focus, and the expected read-only compatibility projection.

## Still Waters Acceptance Gate

Clean PIE must then prove Mara's existing quest and `Still Waters` can coexist,
save, restore twice, complete independently, and never replay either reward.
