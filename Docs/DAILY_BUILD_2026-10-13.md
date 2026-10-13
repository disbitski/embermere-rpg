# October 13 Daily Build

## Dead-State Consumable Recovery

No voluntary real Prowler, death-panel, vendor walk-away, or two-quest
Ledger state was available. A fresh normal-route PIE start reached the Human
Warrior HUD and closed Inventory without a new visual defect. Reviewing the
first Marsh Tonic use path then exposed a gameplay mismatch: `Heal` returned
zero on a dead character, but `RestoreMana` still added mana. If mana was
missing during the death hold, a tonic's mana effect made the whole use count
as successful, consuming the item and posting `Used Marsh Tonic` even though
health stayed at zero.

`Embermere.Inventory.DeadConsumableRecovery` first failed on direct dead
mana restoration, item depletion, and false success chat. Stats now rejects
dead mana restoration. The inventory Use action and button both read the
existing Stats-owned `NeedsVitalRecovery` preflight instead of inferring
eligibility from one missing resource. Dead use preserves health, mana,
tonic quantity, and chat; the same retained tonic restores mana after
revival. Inventory remains the item/stack mutation owner. This changed no
equipment, combat result, quest, loot, save, art, or map authority.

## Verification

- Process-local Xcode 26.1.1 compiled and linked EmbermereEditor Mac
  Development with `-NoHotReloadFromIDE`; global Xcode 27 stayed unchanged.
- The focused regression was red on old production code and green after the
  repair. Fresh discovery and the final full native run passed **116/116**
  with no failures, warnings, or skips.
- A GUI-down `-ExecutePythonScript` commandlet exited zero. Dedicated
  `/tmp/embermere-oct13-31-package-aggregate.log` contains exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers without `LogPython: Error`.
- All **16** initialized-world collision/route suites passed in the clean
  editor session with exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` and no
  Python errors. Clean floating PIE entered the default Human Warrior level-1
  HUD and stopped without forcing combat, quests, commerce, or saved-slot
  access.
- Unreal is on `/Game/Maps/L_Embermere_Prototype`, outside PIE and All
  Saved, with the real editor owning MCP 8123. No content package changed.
  The protected save and keeper material retained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The native fixture invokes dead-state resource and item actions directly;
it does not prove physical tonic use during a real death hold or final
painted button state. Those remain user checks alongside the first Prowler
defeat/tonic, vendor walk-away, and two-quest Ledger gates. Four known raw-
vendor physics-resave warnings remain confined to FieldGrass, River_Rock,
Medium_Boulder, and HillTree; no raw vendor asset was resaved.
