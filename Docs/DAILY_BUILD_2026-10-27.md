# Daily Build: 2026-10-27

## Milestone

Prevent a Marsh Prowler from first acquiring a player through solid ruin
cover. The old prototype AI checked only its 525 cm radius. A fresh read-only
scan of the saved map found a grounded in-range stance at `(2300,500)` whose
90 cm-offset ray from `Starter_Enemy_01` hits the solid
`FabPass_Ruin_Agora_Pillar`. The change is limited to initial acquisition:
established pursuit, leash, melee cover, movement animation, rewards, and
save state remain with their existing owners.

## Verification

- `Embermere.Enemy.ObstructedAggroAcquisition` first failed because the
  started-world Prowler engaged and moved toward the covered player. It then
  passed with a positive registered-wall trace, no covered engagement,
  movement, or damage, clear-sight acquisition/chase, and engagement retained
  when cover returned. The existing BlockedPursuitVisual and
  MeleeCoverSafety tests also passed unchanged.
- The live Marsh Prowler pull-lane validator now requires clear 90 cm-offset
  aggro sightlines at all three accepted solo-pull stances and a positive
  pillar hit from the grounded in-range covered stance. Its focused run and
  the full **16** sequential initialized-world suites passed with exact
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no final-session
  `LogPython: Error`.
- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked. Fresh discovery found 129 tests;
  the full native run passed **129/129** with zero failures, warnings, or
  skips.
- The isolated GUI-down UnrealEditor `-ExecutePythonScript` aggregate passed
  **31** saved-package validators, with skeletal materials first, placed
  Prowler loot, **53 Fab / 32 original-art**, and exact `validators=31`
  markers. `/tmp/embermere-oct27-31-package-aggregate.log` contains no
  `LogPython: Error`. The sandboxed first attempt stalled in AppKit before
  log creation; its identical host-permission rerun is authoritative.
- Clean floating PIE reached the normal Human Warrior HUD and stopped.
  Fenwatch is All Saved outside PIE with real UnrealEditor owning MCP 8123.
  The protected Chronicle save and unrelated keeper material retained their
  original SHA256 hashes. No content package changed.

## Open Gate

The started-world test and saved-map traces do not prove physical movement,
animation, or camera feel. During a voluntary normal-route encounter, verify
the first Prowler stays unaware behind the pillar, acquires on a clear line,
and keeps normal pursuit after the player returns behind cover. Do not force
combat, quest progress, or the protected Chronicle slot merely to stage it.
