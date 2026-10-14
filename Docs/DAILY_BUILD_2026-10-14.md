# October 14 Daily Build

## Trainer Transaction Range

No voluntary physical trainer walk-away, death-hold tonic, first-Prowler,
vendor walk-away, or two-quest Ledger state was available. Clean floating
PIE reached the default Human Warrior HUD without a new visual defect. A
trainer ownership review then found a concrete transaction gap: initial F
used the controller's 350 cm radius, but an already-open panel could still
spend copper and grant XP after the customer moved away.

`Embermere.Trainer.RangeTransactions` used the saved Fenwatch offering asset
and a separate spawned art-free service. It first failed on a distant Combat
Drills action: the HUD reported success and changed 30 copper / 25 XP to 20 /
50. The trainer component now requires the Stats and Wallet to belong to the
same possessed customer within that controller's current interaction radius
for every placed-service `CanTrain`/`TryTrain` request. Detached data-only
fixtures retain their established contract. A distant request returns exact
`OutOfRange` and `Move closer to the trainer.` without mutation. The final
test also rejected a mismatched wallet and an unpossessed character, then
proved that returning nearby allows the next repeatable lesson at 20 copper /
50 XP. The HUD requests and displays the result; art, offerings, Stats XP,
Wallet copper, and save ownership did not move.

## Verification

- Process-local Xcode 26.1.1 compiled and linked EmbermereEditor Mac
  Development with `-NoHotReloadFromIDE`; global Xcode 27 stayed unchanged.
  The first sandboxed UBT attempt could not rotate its user-Library log; the
  identical host-permitted build succeeded.
- The focused regression was red on old transaction behavior and green after
  repair. Fresh discovery and the final full native run passed **117/117**
  with zero failures, warnings, or skips.
- A sandboxed commandlet stalled in AppKit before creating a log and was
  interrupted. The identical host-permitted GUI-down `-ExecutePythonScript`
  run exited zero. Dedicated
  `/tmp/embermere-oct14-final-31-package-aggregate.log` contains exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers without `LogPython: Error`.
- All **16** initialized-world collision/route suites passed in the clean
  final editor session with exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites` and no
  Python errors. Clean floating PIE entered the Human Warrior level-1 HUD
  and stopped without quest, combat, commerce, or saved-slot mutation.
- Unreal is on `/Game/Maps/L_Embermere_Prototype`, outside PIE and All Saved,
  with the real editor owning MCP 8123. No content package changed. The
  protected save and keeper material retained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The native fixture moves a pawn in an isolated world; it does not prove
physical movement while the panel is open or final painted rejection copy.
Those remain user checks. Four known raw-vendor physics-resave warnings stay
confined to FieldGrass, River_Rock, Medium_Boulder, and HillTree; no raw
vendor asset was resaved.
