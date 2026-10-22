# October 22 Daily Build

## Enemy Melee Respects Solid Cover

No voluntary physical ruin-wall fight or other pending playtest state was
supplied. Clean normal-route PIE reached the Human Warrior HUD without
changing quests, combat, commerce, or the protected save. Reviewing the
adjacent AI owner after yesterday's player ability preflight exposed a
different fairness gap: an in-range Prowler melee attack did not check
whether a solid wall separated it from the player.

The test-first `Embermere.Enemy.MeleeCoverSafety` starts an isolated world
with a possessed living player, an enemy inside attack range, and a registered
blocking box. A native `Visibility` trace proves the wall blocks the intended
90 cm sightline. The old AI hit through it for 6 damage (`100 -> 94`), giving
a genuine red regression. Enemy AI now rejects the blocked line before
animation, damage, and cooldown. The green test proves no health loss behind
cover, then moves the wall clear and proves the same enemy can hit immediately.
Aggro, movement, leash, player abilities, quest, loot, save, and UI authority
did not change. See `Docs/ENEMY_MELEE_COVER_CONTRACT.md`.

## Verification

- Process-local Xcode 26.1.1 `EmbermereEditor Mac Development
  -NoHotReloadFromIDE` compiled and linked after the final test and AI change.
  Fresh discovery returned **124** Embermere tests; the focused test and the
  full **124/124** run passed with zero failures, warnings, or skips.
- A GUI-down UnrealEditor `-ExecutePythonScript` run exited zero. Dedicated
  `/tmp/embermere-oct22-31-package-aggregate.log` contains explicit
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` success markers, with no `LogPython: Error`.
- All **16** initialized-world suites passed in the final real-editor session
  with exact `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites`
  and no Python error. Clean floating PIE confirmed the Human Warrior HUD,
  then stopped. `/Game/Maps/L_Embermere_Prototype` is All Saved outside PIE
  with the real UnrealEditor owning MCP 8123.
- The protected save and unrelated keeper material remained byte-for-byte
  at SHA256 `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

Physical Prowler cover, movement, animation, and camera acceptance are still
unverified. Four known raw-vendor physics-resave warnings remain confined to
FieldGrass, River_Rock, Medium_Boulder, and HillTree; none was resaved.
