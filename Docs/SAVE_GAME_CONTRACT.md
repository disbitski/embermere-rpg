# Embermere Save-Game Contract

## Purpose

Embermere's first persistence slice saves durable progression without turning
runtime object pointers into a file format. The contract is intentionally
small, versioned, and independent of character, NPC, or environment art.

`UEmbermereSaveGame` currently writes format version `1` to the prototype slot
`EmbermerePrototype` for user index `0`.

## Persisted State

- wallet copper;
- current experience;
- inventory stack item identity, asset path, and quantity;
- equipped item identity, asset path, and body slot;
- active quest identity, objective progress, and completion state;
- every persistent vendor's stable ID, stock-data asset, and remaining finite
  or unlimited quantities.

Item, quest, and stock records carry both a stable data ID and a soft object
path. Load accepts a record only when the asset resolves and its authored ID
still matches. Vendor records use a world-stable `PersistenceId`; the Fenwatch
quartermaster owns `FenwatchQuartermaster` independently of its art actor.

## Session-Only State

The following state is deliberately not serialized:

- vendor buyback history;
- world position and rotation;
- current target, aggro, and combat state;
- current health and mana;
- cooldown timers and temporary buffs or control effects;
- UI selection, open panels, chat history, and cursor mode.

Buyback is a bounded convenience history for the current merchant session, not
durable ownership. A successful load clears it. Loading also clears the combat
target and temporary effects, then restores full health and mana against the
loaded equipment bonuses. Position remains at the current session's safe spawn.

## Atomic Load

Load is a two-phase operation.

1. Resolve every referenced asset and persistent vendor.
2. Validate the complete candidate against current item, equipment, quest,
   bag-capacity, stock, version, and ownership contracts.
3. Reject the candidate without mutating live state if any record fails.
4. Commit all already-resolved records, reapply equipment bonuses
   idempotently, reset transient combat state, and broadcast owner changes.

Rejected saves preserve the existing wallet, bag, equipment, quest, stock, and
buyback state. Current focused coverage rejects:

- unsupported format versions;
- missing assets and mismatched stable IDs;
- invalid or over-stack quantities;
- bags larger than current capacity;
- duplicate, invalid, or level-ineligible equipment slots;
- contradictory or out-of-range quest progress;
- missing, duplicate, or unknown persistent vendor IDs;
- changed stock assets and finite quantities above their authored maximum.

Applying the same valid save repeatedly is safe: items, XP, copper, quest
rewards, and equipment bonuses do not duplicate.

## Prototype Commands

While PIE is running, use the Unreal console commands:

```text
EmbermereSave
EmbermereLoad
```

Both commands post success or rejection feedback to the existing HUD/chat
channel. They are explicit prototype triggers, not an autosave policy.

The live two-session validator is
`Scripts/validate_persistence_live_unreal.py`:

1. run `prepare_and_save()` in a fresh PIE session;
2. stop PIE and start a second fresh PIE session;
3. run `load_and_validate()`.

The accepted 2026-08-06 run created the real commerce and quest state, wrote
`Saved/SaveGames/EmbermerePrototype.sav`, rebuilt a fresh world, restored exact
identity and progression, and loaded the file a second time without
duplication.

## Verification

- `Embermere.Persistence.RoundTrip` serializes through Unreal's SaveGame
  archive and proves exact inventory, equipment, stats, quest, wallet, stock,
  buyback-reset, and repeated-load behavior.
- `Embermere.Persistence.ValidationRollback` proves malformed candidates do
  not partially mutate live state.
- `Embermere.Vendor.ServiceContract` and the Fenwatch package validator lock
  the stable vendor ID to the art-free service boundary.
- Full automation, saved-package validators, initialized-world route traces,
  and the two-session PIE validator remain separate acceptance layers.

Future format changes must increment `EmbermereSaveGameVersion::Current` and
either provide an explicit migration or reject the older record clearly. Do
not silently reinterpret a prior version.
