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
- HUD and Chronicle are read-only consumers of the derived Stats result.
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

- Player status continues to show `Level` and total `XP` from Stats.
- Trainer requirements continue to read the same derived Stats level.
- Chronicle adds the derived level to its read-only identity summary.
- No progression widget owns thresholds, growth, identity legality, XP, or stats.

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

## Deferred

The first slice does not add a saved Level field, new save version, ability
unlock table, talent points, level rewards, death penalties, rested XP,
prestige, autosave, multiple profiles, or implicit migration.
