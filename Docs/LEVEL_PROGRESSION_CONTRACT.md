# Embermere Level Progression Contract

## Purpose

Embermere derives character level from durable experience. Level is a runtime
result, not a second saved progression value. This keeps trainer rewards, quest
rewards, Chronicle, equipment requirements, character identity, and repeated
loads on one deterministic contract.

## Authority

- `UEmbermereStatsComponent` owns the live experience total and derived level.
- `UEmbermereRulesData` owns ordered cumulative XP thresholds, class starting
  attributes, and race/class per-level growth.
- `AEmbermereCharacter` resolves one legal race/class progression profile and
  applies it atomically to Stats and the existing starter hotbar.
- Equipment remains a separate additive layer. Progression rebuilds identity
  base stats first; equipment bonuses are then applied exactly once.
- Trainer and quest systems remain the only current gameplay owners of XP
  grants. They request a transaction from Stats and do not calculate level.
- Stats exposes one read-only progression-presentation snapshot containing the
  derived level, total XP, current and next rules-owned thresholds, cap state,
  and normalized within-level progress.
- HUD and Chronicle are read-only consumers of the derived Stats result. UMG
  never copies thresholds or derives a level.
- Save data stores XP and stable identity IDs. It does not store Level.

## First Progression Curve

The first bounded cap is level 5, using cumulative thresholds:

| Level | Required total XP |
| --- | ---: |
| 1 | 0 |
| 2 | 100 |
| 3 | 250 |
| 4 | 450 |
| 5 | 700 |

XP may continue to accumulate safely after 700, but level remains capped at 5
until the rules asset deliberately adds another strictly increasing threshold.

## Stat Resolution

For a legal race/class pair at derived level `L`, identity-owned attributes are:

`class starting attributes + (L - 1) * (race growth + class growth)`

All thresholds and growth values are validated before use. Thresholds must
start at zero and increase strictly. Growth values must be finite and
non-negative. Health must resolve to at least one; mana and attributes must
remain finite and non-negative.

Equipment bonuses are not part of this formula. They are added after the base
result and preserve one additive application across level-up and repeated load.

## Live XP Lifecycle

1. The XP owner requests a positive grant.
2. Stats rejects overflow before mutation.
3. Stats resolves the candidate level and attributes from its validated profile.
4. XP and any new level/base attributes commit together.
5. Existing missing health and mana are preserved while maximum values grow.
6. Stats publishes the existing exact XP message and one level-up message when
   the derived level increases. One large grant may cross multiple levels.
7. A separate live-only transition event is emitted after the authoritative
   level and attributes commit. Presentation may observe it but cannot request
   XP, calculate a threshold, or mutate progression.

Reaching a threshold grants no extra item, currency, quest credit, or ability.
Starter abilities remain class-owned in this first slice.

## Save And Load Lifecycle

1. Resolve stable race/class IDs, with the explicit current-rules Human Warrior
   fallback for version 1.
2. Reject negative XP, illegal identity, malformed progression rules, or an
   unresolvable candidate level before mutation.
3. Validate equipped-item level requirements against the candidate saved level,
   never the live pre-load level.
4. Commit identity, XP, derived level, and identity base stats atomically and
   silently.
5. Restore bag/equipment identity and apply equipment bonuses once.
6. Restore wallet, quest, and finite vendor state under their existing contracts.

Load and repeated load do not publish XP or level-up feedback, replay rewards,
or rewrite a version-1 slot. Any rejected record leaves the complete live state
unchanged.

## Presentation

- Player status shows `Level`, total `XP`, the next cumulative threshold, and a
  fixed `260x8` gold within-level progress bar from the Stats snapshot. At the
  first cap it uses explicit `CAP` copy and a full bar.
- A standalone hit-test-invisible observer shows a fixed `360x76` level-up
  panel for `2.75` seconds. It handles one-level and multi-level transitions,
  replaces any prior transient result, fades without changing layout, and
  tears down safely when unbound or the world ends.
- The observer listens only to the post-commit live level-transition event.
  Save restoration does not broadcast that event, so load cannot replay the
  celebration even when the restored identity implies a different level.
- Trainer requirements continue to read the same derived Stats level.
- Chronicle adds the derived level to its read-only identity summary.
- Exact XP and level-up chat remains the durable fallback.
- No progression widget owns thresholds, growth, identity legality, XP, stats,
  rewards, equipment, save data, or load behavior.

## Acceptance Gates

- Exact threshold edges and the explicit level-5 cap.
- A single grant that crosses more than one threshold.
- Rejection of malformed, non-monotonic, non-finite, or negative rules.
- Distinct race/class growth results from the same XP total.
- Equipment bonuses remain single and additive through level changes.
- Version-2 round-trip, repeated-load idempotence, and complete rollback.
- Version-1 Human Warrior fallback derives its level from legacy XP.
- Trainer XP and Mara quest XP produce the expected live level transitions.
- Chronicle and HUD report the authoritative derived level.
- Level-1 progress, threshold approach, exact-threshold rollover, within-level
  normalization, and explicit cap copy.
- Multi-level transient copy, fixed geometry, deterministic expiry, teardown,
  and silent save restoration.

## Deferred

The first slice does not add a saved Level field, new save version, ability
unlock table, talent points, level rewards, death penalties, rested XP,
prestige, autosave, multiple profiles, or implicit migration.
