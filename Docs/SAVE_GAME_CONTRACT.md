# Embermere Save-Game Contract

## Purpose

Embermere's first persistence slice saves durable progression without turning
runtime object pointers into a file format. The contract is intentionally
small, versioned, and independent of character, NPC, or environment art.

`UEmbermereSaveGame` writes format version `3` to the prototype slot
`EmbermerePrototype` for user index `0`. Versions `1` and `2` remain readable
through the explicit compatibility rules below; loading either never rewrites
the slot or silently migrates its bytes.

## Persisted State

- wallet copper;
- current experience;
- character race and starting class as stable IDs;
- inventory stack item identity, asset path, and quantity;
- equipped item identity, asset path, and body slot;
- a bounded keyed array of quest identity, objective, progress, and completion
  records;
- every persistent vendor's stable ID, stock-data asset, and remaining finite
  or unlimited quantities.

Item, quest, and stock records carry both a stable data ID and a soft object
path. Load accepts a record only when the asset resolves and its authored ID
still matches. Vendor records use a world-stable `PersistenceId`; the Fenwatch
quartermaster owns `FenwatchQuartermaster` independently of its art actor.

Character identity uses the stable IDs `Human`, `Elf`, `Dwarf`, `Gnome`,
`DarkElf`, `Lizardman`, `Ogre`, and `Bullywug` for race, and `Warrior`,
`Cleric`, `Ranger`, and `Wizard` for class. Serialized IDs are not enum
ordinals, display labels, localized text, or inferred hotbar contents. Current
`UEmbermereRulesData` must resolve both IDs, allow the pair, expose the class
starting attributes, and resolve all four starter abilities before any live
owner may change.

Level is intentionally absent from this list. Current XP plus validated rules
derive it deterministically; serializing both values would create two competing
progression truths.

## Version 2 Character Identity

Version `2` makes a deliberately confirmed race/class pair durable while
leaving the rest of character creation out of scope.

- Capture rejects a character that has no deliberate, rules-valid identity.
- Preflight resolves stable IDs to current native identity types, then asks the
  same rules data used by character creation to validate legality, starting
  attributes, and starter abilities.
- Commit atomically replaces race, class, saved XP, derived level, identity
  base attributes, full vitals, and the first four hotbar slots before restoring
  saved equipment.
- Equipment remains additive: base class attributes are rebuilt once, then the
  saved equipment set replaces the live set and its total bonus is applied
  idempotently.
- Repeated loads replace the same identity and loadout; they cannot stack base
  stats, equipment bonuses, abilities, XP, copper, items, or quest rewards.
- Unknown, missing, malformed, or currently illegal identity IDs reject the
  entire save before wallet, inventory, equipment, quest, vendor, or character
  state mutates.

Version `1` contains no trustworthy identity record. Its explicit compatibility
interpretation is a deliberately confirmed Human Warrior using current rules
data. That fallback is validated and applied through the same atomic restore
path as a version `2` identity. A version `1` load may therefore replace the
current session's pending or confirmed identity with Human Warrior, but it does
not alter the version `1` file. This is backward compatibility, not an implicit
migration.

Appearance, character name, creation-panel selection, confirmation UI state,
and per-character slot/profile ownership are not serialized in version `3`.

## Version 3 Multi-Quest Ledger

Version `3` adds `QuestStates`, a bounded array of at most eight records. Each
record carries a stable quest ID, matching soft quest-data path, stable
objective ID, validated progress, and completed state. The live
`UEmbermereQuestLogComponent::QuestStates` ledger is the only mutable quest
authority.

Load resolves the entire candidate ledger before mutation. Assets and authored
IDs must match, quest IDs must be unique, progress must be in range, completed
records must be terminal, and the array must fit the bound. Missing,
duplicated, mismatched, invalid, contradictory, over-capacity, or mixed native
and legacy records reject the complete load. Only a fully valid candidate may
replace the live ledger atomically.

The singular version-1/version-2 `QuestState` field remains solely as a read
adapter. Zero or one legacy record is resolved through the same validator and
becomes an in-memory candidate ledger without rewriting the older slot.
`ActiveQuest` is a derived Blueprint/HUD compatibility projection;
`FocusedQuestId` is transient and never serialized.

The full runtime, compatibility, and first second-quest boundaries live in
[MULTI_QUEST_CONTRACT.md](MULTI_QUEST_CONTRACT.md).

## Derived Level And Growth

Save versions `1`, `2`, and `3` use the same current-rules progression resolver. The
first bounded cumulative curve is `0`, `100`, `250`, `450`, and `700` XP for
levels `1` through `5`. Thresholds must begin at zero and rise strictly;
race/class growth must remain finite and nonnegative.

For derived level `L`, identity-owned base attributes are class starting
attributes plus `(L - 1)` times the combined race and class growth. Equipment
is not part of this formula. Load resolves identity and candidate saved XP
before mutation, derives the candidate level/base result, validates each saved
item's level requirement against that candidate level, then applies the saved
equipment set exactly once.

Load does not publish XP or level-up feedback. Version `1` uses the same
derived-level path after selecting its explicit current-rules Human Warrior
fallback, and the old slot is not rewritten. The complete authority and growth
rules live in [LEVEL_PROGRESSION_CONTRACT.md](LEVEL_PROGRESSION_CONTRACT.md).

## Session-Only State

The following state is deliberately not serialized:

- vendor buyback history;
- world position and rotation;
- current target, aggro, and combat state;
- current health and mana;
- cooldown timers and temporary buffs or control effects;
- UI selection, open panels, chat history, and cursor mode.
- focused-quest selection and the derived `ActiveQuest` compatibility
  projection.

Buyback is a bounded convenience history for the current merchant session, not
durable ownership. A successful load clears it. Loading also clears the combat
target and temporary effects, then restores full health and mana against the
loaded equipment bonuses. Position remains at the current session's safe spawn.

## Atomic Load

Load is a two-phase operation.

1. Resolve every referenced asset, identity/progression rule, and persistent
   vendor.
2. Derive the candidate level from saved XP, then validate the complete
   candidate against current item, equipment, quest, bag-capacity, stock,
   version, and ownership contracts.
3. Reject the candidate without mutating live state if any record fails.
4. Commit all already-resolved records, reapply equipment bonuses
   idempotently, reset transient combat state, and broadcast owner changes.

Rejected saves preserve the existing race, class, base stats, hotbar, wallet,
bag, equipment, quest, stock, and buyback state. Current focused coverage
rejects:

- unsupported format versions;
- negative XP or malformed/non-monotonic progression rules;
- missing, unknown, or illegal version-2/version-3 race/class IDs;
- missing assets and mismatched stable IDs;
- invalid or over-stack quantities;
- bags larger than current capacity;
- duplicate, invalid, or level-ineligible equipment slots;
- duplicate, missing, mismatched, contradictory, out-of-range, mixed-format,
  or over-capacity quest records;
- missing, duplicate, or unknown persistent vendor IDs;
- changed stock assets and finite quantities above their authored maximum.

Applying the same valid save repeatedly is safe: items, XP, copper, quest
rewards, and equipment bonuses do not duplicate.

The Fenwatch trainer added no version-1 schema field. An accepted training
transaction mutates the already-owned copper and XP values, so those results
flow through the existing wallet/stats records. Trainer offerings, panel
selection, interaction range, and rejection copy remain data or transient
session state. The 2026-08-10 integrated proof exercised that exact boundary:
a normal `40` copper/`0` XP world trained once, saved at `30`/`25` through the
real Chronicle control, began a second world at the normal `40`/`0` baseline,
and restored `30`/`25` through two confirmed loads. Neither load created items,
equipment, quest state, vendor-stock changes, buyback history, repeated rewards,
or currency/XP drift. Save version 1 did not expand. Version `3` continues to
serialize the resulting copper and XP through those existing owners without
adding trainer state.

The 2026-08-25 level-gated trainer extension preserves that boundary. Both
repeatable offerings have stable data IDs, but only their successful copper and
XP mutations are durable. Required level, repeatability, current selection,
lock/ready presentation, and interaction state remain rules or session data.
Advanced Combat Drills therefore restores through the existing version-3
wallet/XP records and derived-level resolver without a trainer record, reward
replay, or schema expansion.

The accepted 2026-08-26 integration proof exercised that richer boundary
through the real Chronicle UI. An Elf Wizard completed Mara's authoritative
quest reward, ran one real Advanced transaction, equipped the resulting Recruit
Pack, and saved at exactly level `2`, `175` XP, and `40` copper with completed
quest state and untouched finite vendor stock. A fresh Dwarf Warrior world
proved level `1`, `0` XP, and `40` copper before two confirmed loads restored
the exact Elf Wizard identity, base and equipment stats, Wizard hotbar, quest,
stock, wallet, and XP. Neither load restored transient trainer selection or
panel state, replayed the quest reward, duplicated equipment bonuses, expanded
the schema, or drifted any owner.

The accepted 2026-08-31 foundation advances the format to version `3` without
changing any non-quest durable owner. Native capture sorts and writes the
bounded quest ledger as stable records, while versions `1` and `2` adapt their
singular quest field in memory. Focused coverage round-trips zero, one, and two
quests, loads the same ledger repeatedly, preserves exactly-once rewards, and
rejects malformed collections before wallet, XP, inventory, equipment, quest,
or vendor mutation. The old source save is never rewritten merely because it
was read through an adapter.

## Prototype Commands

The player-facing `Embermere Chronicle` opens with `M` or the bottom-right
Chronicle button. It is a fixed one-slot surface over `EmbermerePrototype`, user
index `0`; it does not own serialization or mutate state while inspecting the
slot.

- A valid slot shows the read-only race/class identity and derived level,
  copper, XP, bag-stack count, equipped-item count, and quest state before the
  player chooses an action. Version `1` slots identify their explicit Human Warrior legacy
  fallback.
- Saving to an occupied slot requires an explicit overwrite confirmation.
- Loading always requires confirmation because it replaces the current
  session's durable owners.
- Empty, unreadable, malformed, unsupported-version, missing-asset, and other
  rejected states use the persistence result and message rather than inventing
  UI-specific recovery behavior.
- Inventory, Vendor, and Chronicle are mutually exclusive. Closing Chronicle
  restores game-only input.
- There is no autosave, delete action, multiple-profile lifecycle, or implicit
  migration in version `3`.

`InspectSaveSlot` is deliberately read-only. It resolves enough slot metadata
to provide a summary and detect unavailable or unsupported files, while the
authoritative load path still performs complete asset, identity, capacity,
quest, equipment, and vendor preflight before any live mutation.

## Debug Commands

While PIE is running, use the Unreal console commands:

```text
EmbermereSave
EmbermereLoad
```

Both commands post success or rejection feedback to the existing HUD/chat
channel. They remain explicit test/debug fallbacks, not an autosave policy.

The live two-session validator is
`Scripts/validate_persistence_live_unreal.py`:

1. run `prepare_and_save()` in a fresh PIE session;
2. stop PIE and start a second fresh PIE session;
3. run `load_and_validate()`.

The accepted 2026-08-07 run created the real commerce and quest state, wrote
`Saved/SaveGames/EmbermerePrototype.sav`, rebuilt a fresh world, restored exact
identity and progression, and loaded the file a second time without
duplication. The same state was inspected and loaded through Chronicle: the
visible summary showed Human Warrior at derived level `2`, `22` copper,
`125` XP, two bag stacks, one equipped item, and a completed quest; overwrite
and load confirmations both preserved their cancel paths.

The accepted 2026-08-10 trainer lane uses the same validator and Chronicle
surface without replacing the established commerce fixture:

1. run `prepare_trainer_progression_for_chronicle()` in fresh PIE;
2. save through Chronicle at exactly `30` copper and `25` XP;
3. stop PIE, start a second world, and run
   `validate_fresh_trainer_session_before_chronicle_load()` to prove `40`/`0`;
4. confirm Chronicle Load and run `validate_trainer_chronicle_load()`;
5. confirm Load a second time and run the same validation again.

The trainer helper also proves one transient offering remains available while
inventory, equipment, quest, finite vendor stock, and buyback remain untouched.
This is an integration proof over existing durable owners, not a new serialized
trainer record.

The accepted 2026-08-26 Advanced lane deliberately uses a richer, different-
identity fixture:

1. confirm Elf Wizard and run
   `prepare_advanced_trainer_progression_for_chronicle()`;
2. use Chronicle `Save Current`, then run
   `validate_advanced_chronicle_slot_created()`;
3. stop PIE, confirm Dwarf Warrior in a genuinely fresh world, and run
   `validate_fresh_advanced_session_before_chronicle_load()`;
4. confirm `Load Saved`, then run `validate_advanced_chronicle_load()`;
5. confirm `Load Saved` a second time and run the same validation again.

This lane proves exact identity, derived progression, class base stats,
equipment, hotbar, completed quest, wallet, and finite stock together while
still proving that offerings, selection, interaction, panel state, and buyback
are transient.

The accepted 2026-08-22 identity lane adds a deliberately different-state
proof. Chronicle first inspected a preexisting malformed version-2 slot,
reported that its stable identity was missing, and kept Load disabled without
mutating the live Elf Wizard. After an explicit overwrite, the slot showed Elf
Wizard, `40` copper, `0` XP, no bag stacks, no equipment, and no active quest.
A fresh PIE world then confirmed Lizardman Ranger at `100/100` health,
`60/60` mana, and its Ranger hotbar before Chronicle Load replaced it with the
saved Elf Wizard at exact `80/80` health, `110/110` mana, Spark Bolt, Frost
Root, Arcane Burst, and Meditate. A second confirmed load produced the same
identity, vitals, and hotbar with no drift or duplication.

The accepted 2026-08-23 progression lane serialized no new save field. Clean
PIE used the real Trainer owner for `25` XP and Mara's original quest owner for
another `125`, producing Human Warrior level `2` at `150` XP with exact
`110/110` health, `53/53` mana, `12` Attack Power, and unchanged Warrior
hotbar. Chronicle read the same derived level. Focused persistence coverage
also proved that equipment eligibility uses the candidate level implied by the
save, repeated load applies growth/equipment once, version `1` derives level
through its fallback identity, and malformed progression rejects before any
live mutation.

## Verification

- `Embermere.Persistence.RoundTrip` serializes through Unreal's SaveGame
  archive and proves exact inventory, equipment, stats, quest, wallet, stock,
  buyback-reset, and repeated-load behavior.
- Focused character-identity persistence coverage proves version-2 ID
  round-trip, exact class stats and hotbar restoration, repeated-load
  idempotence, unknown and illegal ID rollback, malformed record rollback, and
  the version-1 Human Warrior fallback.
- `Embermere.Progression.LevelRules` covers exact thresholds, multi-level
  resolution, the first cap, distinct growth, and malformed-rule rejection.
- `Embermere.Progression.LiveExperienceAndEquipment` proves live base growth,
  missing-resource preservation, exact feedback, and one additive equipment
  layer.
- `Embermere.Progression.RewardOwners` proves Trainer and Mara grant XP without
  calculating level or duplicating progression authority.
- `Embermere.Progression.ValidationRollback` proves candidate-level equipment
  preflight, silent restore, version-1 fallback, repeated-load idempotence, and
  complete rollback.
- `Embermere.Persistence.ValidationRollback` proves malformed candidates do
  not partially mutate live state.
- `Embermere.Quests.MultiQuestRuntime` proves two simultaneous keyed quests,
  independent exact progress, wrong-giver rejection, reward-preflight rollback,
  explicit focus, and exactly-once completion.
- `Embermere.Persistence.MultiQuestRoundTrip` proves empty and multi-record
  serialization plus repeated-load idempotence without reward replay.
- `Embermere.Persistence.LegacyQuestCompatibility` proves version-1/version-2
  singular adapters without source-slot rewriting.
- `Embermere.Persistence.MultiQuestValidationRollback` proves duplicate,
  missing, mismatched, invalid-progress, contradictory, mixed-format, and
  over-capacity ledgers reject without partial owner mutation.
- `Embermere.Persistence.SlotInspection` proves empty, valid, and unsupported
  slot summaries without mutating gameplay owners.
- `Embermere.UI.SaveLoadPanel` locks the panel's fixed bounds, visibility,
  inventory handoff, and explicit close behavior.
- `Embermere.Trainer.LevelGatedProgression` locks the visible level-1 rejection,
  exact level-2 transaction, repeatability policy, malformed-data rejection,
  and complete wallet/XP rollback.
- `Embermere.Trainer.LevelGatedPersistence` proves the resulting copper and XP
  round-trip through existing save owners without trainer-specific state or
  repeated-load drift.
- `Embermere.Vendor.ServiceContract` and the Fenwatch package validator lock
  the stable vendor ID to the art-free service boundary.
- Full automation, saved-package validators, initialized-world route traces,
  and the two-session PIE validator remain separate acceptance layers. The
  2026-08-31 baseline is 80/80 tests plus the sequential 18-package aggregate.

Future format changes must increment `EmbermereSaveGameVersion::Current` and
either provide an explicit compatibility interpretation or reject the older
record clearly. Do not silently reinterpret or rewrite a prior version.
