# October 12 Daily Build

## Vendor Range Transactions

No voluntary real Prowler/death or two-quest Ledger state was available.
Review of the Fenwatch quartermaster found that F used the controller's
350 cm radius only to open its panel. The service's buy, sell, and buyback
preflights did not check distance again. A new isolated test spawned the
real art-free service and possessed a player pawn: after a nearby tonic
purchase, moving the pawn 700 cm from the service still allowed all three
transaction types. The initial fixture used `SetPawn`, which left
`GetController()` unset in an unbegun world; it was corrected to real
`Possess` before interpreting the red result.

`UEmbermereVendorComponent` now checks the world service, possessed customer,
matching inventory/wallet owners, and the controller's existing 3D F radius
in each `Can*` path. `Try*` already calls that preflight before mutation.
Remote requests return `OutOfRange` and `Move closer to the vendor.`; nearby
purchase, sale, and buyback retain the 40 -> 32 -> 35 -> 32 copper loop. The
regression protects copper, tonic quantity, buyback history, XP, and quests.
Standalone rule fixtures without a separate service remain valid. Art, HUD,
map, quest, persistence, and trainer authority did not move.

## Verification

- Process-local Xcode 26.1.1 compiled and linked EmbermereEditor Mac
  Development with `-NoHotReloadFromIDE`; global Xcode 27 stayed unchanged.
- The corrected focused test was red on distant purchase/sale/buyback against
  old production code, then green after the service repair. Final fresh
  discovery and full automation passed **115/115**, with zero failures,
  warnings, or skips.
- GUI-down `-ExecutePythonScript` commandlet exited zero. Dedicated
  `/tmp/embermere-oct12-final-31-package-aggregate.log` contains exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers without `LogPython: Error`.
- All **16** initialized-world collision/route suites passed in the final
  clean editor session with
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` and no
  Python errors. Floating PIE started on the normal character picker and
  stopped without quests, commerce, or saved-slot access.
- Unreal remains on `/Game/Maps/L_Embermere_Prototype`, outside PIE and All
  Saved, with the real editor owning MCP 8123. No content package changed.
  The protected save and keeper material stayed at SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The isolated test moves a pawn directly; it does not prove a physical
walk-away with the panel open or final painted feedback. Those remain user
checks, as do real Prowler/death and Ledger gates. Four known raw-vendor
physics-resave warnings remain confined to FieldGrass, River_Rock,
Medium_Boulder, and HillTree; no vendor pack was resaved.
