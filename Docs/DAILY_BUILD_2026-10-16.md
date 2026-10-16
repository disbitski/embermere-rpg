# October 16 Daily Build

## Spawned Prowler Death Loot

No voluntary first-kill, death-hold, trainer/vendor walk-away, or two-quest
Ledger state was supplied. A normal editor viewport showed coherent village
composition from above terrain; the lower initial editor camera sat beneath
the ground surface, so that first capture was not evidence of missing art.
The saved Prowler package gate already guaranteed a one-item Marsh Tonic
reference, while native loot tests called `GrantLootTo` directly. The missing
bridge was the spawned actor's death delegate in a running world.

`Embermere.Enemy.SpawnedDefeatLoot` starts an isolated world, spawns the saved
Blueprint Prowler and a possessed player, then applies lethal Stats damage.
The bound death event delivers exactly one Marsh Tonic; more damage while
dead cannot deliver another, and this loot path does not directly grant
copper or XP. The first fixture failed before reaching those assertions
because an Embermere controller tried to create a HUD without a local player.
Replacing only that fixture with a plain player controller made the focused
test pass. No gameplay code, Blueprint, map, or saved asset changed.

## Verification

- Process-local Xcode 26.1.1 compiled and linked EmbermereEditor Mac
  Development with `-NoHotReloadFromIDE`; system Xcode stayed unchanged.
- Fresh discovery found **119** Embermere tests. Focused and final full native
  automation passed **119/119**, zero failures, warnings, or skips.
- A sandboxed package run stalled before creating a log. The next attempted
  host command used `UnrealEditor-Cmd` with incompatible `-ExecutePythonScript`
  and was rejected; it is not acceptance evidence. The final GUI-down
  `UnrealEditor -ExecutePythonScript` run exited zero and its dedicated
  `/tmp/embermere-oct16-final-31-package-aggregate.log` contains exact
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` markers without `LogPython: Error`.
- The first GUI relaunch hung before MCP at Metal initialization; a clean
  process-local-Xcode relaunch recovered. The first focused test then exposed
  the fixture error above. After the correction, all **16** initialized-world
  suites passed with exact marker and no Python error. Clean floating PIE
  reached the Human Warrior HUD and stopped.
- The full native test run dirtied only two `/Temp/Untitled_*` test maps;
  no saved content package was dirty. The editor was cleanly relaunched for
  the final ready state. The protected save and keeper material retained
  SHA256 `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

This test proves the runtime death-to-loot hook, not a physical hotbar kill,
quest credit, final chat paint, or tonic use. Those remain direct player
checks. Four known raw-vendor physics-resave warnings remain confined to
FieldGrass, River_Rock, Medium_Boulder, and HillTree; no vendor asset was
resaved.
