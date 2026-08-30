# Embermere Multi-Quest Contract

## Purpose

Embermere save version `2` has one durable `QuestState`, and the live
`UEmbermereQuestLogComponent` has one `ActiveQuest`. That is enough for Mara's
first quest, including completed history that prevents reward replay. It is not
enough for a second quest: accepting another record would either erase Mara's
completion history or leave the new quest outside persistence.

The second Fenwatch quest therefore requires a deliberate save version `3`
multi-quest ledger. Do not ship another durable quest by replacing the version
`2` slot, hiding state in an art actor or service, or expanding the existing
schema without changing its version.

## Current Version-2 Boundary

Until version `3` is implemented:

- one valid quest may occupy the live and saved quest slot;
- the same quest giver may revisit that quest and complete it when ready;
- a different quest offer reports an explicit occupied-slot result;
- a different quest giver cannot complete, replace, or reward the tracked
  quest;
- a completed quest remains in the slot as durable no-replay history;
- capture and restore retain that exact record under save version `2`.

`Embermere.Quests.SingleSlotCompatibility` locks this boundary. It also guards
against the prior generic interaction fallback where any failed quest
acceptance could attempt to complete the unrelated active quest.

## Proposed Version-3 Save Shape

Version `3` replaces the singular saved quest record with a bounded array of
records. Each record must contain:

- stable quest ID;
- matching soft quest-data path;
- stable objective ID;
- validated objective progress;
- completed state.

The first implementation should remain deliberately small. A bounded ledger
is preferable to an unbounded archive, and presentation may expose one
transient focused quest without making focus durable authority.

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

## Backward Compatibility

Versions `1` and `2` remain readable. Their singular `QuestState` is interpreted
as zero or one version-3 ledger record during load. Reading an older slot does
not rewrite it, silently migrate its bytes, or discard its completed history.

The compatibility adapter must use the same stable-ID, asset, objective,
progress, and completion validation as a native version-3 record. An invalid
legacy record rejects the complete candidate through the existing rollback
path.

## Runtime Authority

The runtime quest owner becomes a keyed ledger rather than one implicit active
slot. Its mutation APIs take stable quest and objective IDs explicitly:

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

## Second Fenwatch Slice: Still Waters

After the version-3 ledger and compatibility suite are accepted, the first
additional content slice is:

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

## Verification

Version `3` is not accepted until focused automation proves:

- empty, one-record, and multiple-record round trips;
- version `1` and `2` singular-record compatibility without slot rewriting;
- repeated-load idempotence and no reward replay;
- duplicate quest IDs, missing assets, mismatched IDs, invalid progress,
  contradictory completion, and capacity overflow reject atomically;
- two simultaneous quests can progress independently;
- an unrelated quest giver cannot complete or replace another quest;
- exact quest/objective routing and exactly-once completion rewards;
- Chronicle and HUD read ledger state without owning it;
- focused-quest selection remains transient unless a later contract says
  otherwise;
- the notice board, well art, rest service, rest presentation, and trainer stay
  free of quest authority.

Clean PIE must then prove Mara's existing quest and `Still Waters` can coexist,
save, restore twice, complete independently, and never replay either reward.
