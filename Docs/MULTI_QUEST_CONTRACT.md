# Embermere Multi-Quest Contract

## Purpose

Embermere save version `3` makes multiple durable quests possible without
erasing completed no-replay history or hiding progress in world art and
services. The runtime and save formats now share one bounded keyed-ledger
contract. Versions `1` and `2` remain readable through explicit singular-record
adapters.

The migration was accepted on 2026-08-31. The first real second-quest slice,
`Still Waters`, was accepted on 2026-09-01. This document is now the durable
foundation, content, and presentation boundary for both.

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
additional content slice is also accepted:

- quest ID: `FenwatchStillWaters`;
- title: `Still Waters`;
- objective ID: `FenwatchRestCompleted`;
- requirement: one committed successful communal-well rest;
- reward: `50` XP and `10` copper, with no item reward;
- offer and turn-in owner: the separate art-free
  `AEmbermereRestQuestServiceActor` saved as
  `Embermere_FenwatchNoticeBoard_StillWatersService_01` at the notice board's
  `(-1560, -260, 0)`, yaw `-35` transform;
- progress owner: a dedicated quest objective router that observes the
  immutable committed rest outcome and forwards one stable objective event to
  the quest ledger: `UEmbermereRestQuestObjectiveRouterComponent`.

The notice-board mesh remains presentation-only. The well mesh remains art and
collision only. The rest service continues to own recovery eligibility and
mutation; the rest observer continues to own only transient VFX. None of them
contains quest data, progress, completion, or reward logic.

Only a committed rest `Success` may advance `FenwatchRestCompleted`. Pending,
interrupted, rejected, duplicate, loaded, or replayed presentation state does
not advance it. Reaching the authored cap also blocks duplicate post-completion
progress before turn-in or after completion.

## Accepted Verification

The 2026-08-31 foundation and 2026-09-01 content slice are accepted. Focused
automation proves:

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
  free of quest authority;
- the saved Still Waters data and owner/router separation, exact IDs/copy,
  committed-success filtering, objective-cap duplicate safety, exact
  once-only rewards, and two-record version-3 round-trip.

The focused suite is:

- `Embermere.Quests.MultiQuestRuntime`;
- `Embermere.Persistence.MultiQuestRoundTrip`;
- `Embermere.Persistence.LegacyQuestCompatibility`;
- `Embermere.Persistence.MultiQuestValidationRollback`;
- `Embermere.Quests.StillWatersServiceContract`;
- `Embermere.Quests.StillWatersRestRouting`;
- `Embermere.Persistence.StillWatersRoundTrip`.

Both the isolated commandlet and restarted-editor MCP runner passed all `83`
Embermere tests. The no-hot-reload build, focused validators, sequential
19-package aggregate, full-zone validator, and initialized-world well,
notice-board, workshop, cottage, stall, and road traces also passed with no
`LogPython: Error` while retaining 53 grounded Fab actors plus 24 original-art
placements.

Clean PIE confirmed Human Warrior creation and Inventory close, then used the
real `F` path to accept `Still Waters` at the notice board and Mara's original
quest independently. Moving `75` cm during one pending rest produced an
interruption and left Still Waters at `0/1`; a second stationary channel
committed recovery and advanced it to `1/1`. Physical notice-board turn-in paid
exactly `50` XP and `10` copper once while Mara remained active at `0/3`; a
repeat `F` paid nothing.

An isolated `EmbermereStillWatersLiveProbe` slot then captured completed Still
Waters and active Mara, deliberately diverged live state, and restored the exact
two-record ledger, `50` XP, and `50` copper on two confirmed loads. The probe
deleted its slot and restored the controller's real `EmbermerePrototype` target,
so acceptance did not inspect, overwrite, or load the user's Chronicle save.

## Accepted Presentation Boundary

The bounded player-facing ledger is defined and accepted in
[QUEST_LEDGER_PRESENTATION_CONTRACT.md](QUEST_LEDGER_PRESENTATION_CONTRACT.md).
It may list and transiently focus the accepted records, but remains a read-only
consumer. `QuestStates` stays authoritative; `FocusedQuestId` stays
session-only; changing focus may only change the existing compatibility
projection. The UI must not accept, advance, complete, reward, serialize,
abandon, reorder, or rewrite a quest.

The fixed native `620x430` panel lists up to eight `596x30` rows with exact
active, ready, and completed state plus objective progress. Two focused tests
bring the full suite to `85`; isolated commandlet and restarted-editor MCP runs
both pass all tests. Clean PIE showed active Mara and completed Still Waters
together and switched the compact HUD projection both ways through physical
Up/Down and Enter input without state or reward mutation.
