# September 9 Daily Build

## Scope And Readiness

One bounded gameplay correction: enemy return-home movement now slides along
blocking collision through Unreal's existing movement API. Chase, damage,
leash distances, rewards, map art, and save version 3 remain unchanged.

The existing daily heartbeat targets task
`019efc2b-b36c-71f0-8892-8e4f80829fc9` in
`/Users/wizard/Documents/Unreal Game`, at 8 AM America/New_York. Project config
points Unreal MCP at localhost 8123, disables desktop-control bridges/plugins,
and defaults desktop access to deny. The configured Blender executable exists;
its live bridge was not queried because no art was planned. No desktop control,
OS screenshots, security changes, or unlock request was used.

## Distinct-Prowler Route

A fresh Human Warrior accepted Mara through her original F interaction. Normal
hotbar combat defeated saved Prowlers 02, 01, and 03, one distinct actor each.
Time dilation stayed 1.0. No position, health, objective, or reward fixture was
injected in PIE, and no save/load operation was performed.

- Prowler 02: one tonic, Mara 1/3, player 76 health; neighbors untouched.
- Prowler 01: second tonic and 2/3. The approach also aggroed respawned 02.
  The initial retreat stopped too close to that pursuer, and the player died
  during the inter-call interval. Village recovery retained both tonics and
  2/3 progress. This is not a clean three-solo-pull or no-death run.
- Prowler 03: northern approach, third tonic and 3/3, player 76 health;
  Prowler 01 remained home and undamaged. Prowler 02 was already stalled
  on its return near the gate, as investigated below.
- Return to Mara: original F granted exactly 125 XP, 20 copper (40 -> 60),
  and one Recruit Pack. Repeat F changed nothing. Derived level 2 preserved
  24 missing health at 86/110, with 53/53 mana, three tonics, and completed 3/3.

Input used temporary bounded Unreal-only callbacks with fresh viewport
snapshot/click before each action, one-tick discrete input, and measured W
waypoints. Scripted headings are not physical camera proof. Waypoint tolerance
and slow background ticks can overshoot a planned endpoint; the failed recovery
waypoint and the two-enemy pull remain part of the evidence. The temporary
probe is not committed.

## Gate Return Correction

After the two-enemy pull, Prowler 02 remained around
`(1095.201,641.290,90.150)` for several minutes. Its center-to-home line trace
was clear, but a 34 cm radius / 88 cm half-height capsule sweep hit
`Embermere_RoadGate_01` after only about 0.71 cm. The footing, not missing
ground texture or a broken target rule, blocked the straight swept move.

`UpdateReturnHome` now uses `SafeMoveUpdatedComponent` and the public
`UMovementComponent` collision-slide interface for the remaining blocked step.
It still sweeps collision and retains the existing speed/control-effect rules.
Run-animation movement reporting now depends on actual displacement. This is
local collision sliding, not general obstacle pathfinding or teleport recovery.

`Embermere.Enemy.ReturnPastGate` loads the actual saved gate and enemy Blueprint
into an isolated started physics world, reproduces the contact, proves that
the center ray misses while the full capsule hits, and checks zero-delta,
disabled-AI, rooted, health-preservation, and exact-home outcomes. The corrected
baseline fixture failed at the recorded contact after ten simulated seconds;
the fix reached `(1700,1100,90.150)` in the same test.

Fresh-module normal-time PIE then pulled and selected Prowler 02, took one
6-damage retaliation, retreated, and proved its exact home and full health on
later observations. Other enemies stayed home and undamaged; XP, wallet,
quests, and bag stayed at fresh values. This live route did not prove identical
footing contact; the isolated exact-contact regression supplies that evidence.
J -> M -> I -> I returned cursor-hidden, unsuppressed move/look input. All
temporary callbacks and forced keys were clear before teardown.

## Newly Exposed Animation Gap

Do not interpret today's green suite as six-role runtime animation approval.
In fresh PIE, all three placed `BP_StarterEnemy` instances reported
`VisualSkeletalMesh=None`, `IdleAnimation=None`, and
`HasCompleteVisualAnimationSet=false`. The post-PIE editor instances reported
the same, while the Blueprint CDO reported a complete animation set. All
three retained 48 non-colliding target-ring segments, and the selected enemy's
ring was visible. The practice target correctly has no creature animation set.

Existing package validation checks the placed skeletal component and its default
Idle, while the native presentation test checks the Blueprint defaults. That
does not prove the placed actor's six soft animation references. First work
tomorrow: inspect these exact placed properties in a fresh commandlet, extend
validation to cover them, repair only verified instance/template drift, and
prove real Idle/Walk/Run/Attack/Hit/Death routing in PIE. Do not silently copy
defaults or claim the gap is repaired today.

## Verification And Protected State

- Final no-hot-reload Editor build passed.
- Full isolated suite: 96 succeeded, zero failed/warnings/not-run/in-process.
- All 21 fresh-process package validators and their aggregate marker passed.
- All six initialized-editor trace suites passed: well, notice board, workshop,
  cottage, stall, and road. Exact world counts remain 53 Fab / 24 original art.
- No Python errors in the final test/package logs or restarted editor session.
  Earlier exploration used an unavailable Python capsule getter; the corrected
  component query succeeded. Fixture development also corrected protected C++
  entry points and explicitly started actors before accepting regression results.
- Fresh GUI discovery found 96 tests. UnrealEditor PID 19118 owns localhost
  8123 and is left out of PIE on `L_Embermere_Prototype`, with zero dirty packages.
- Evidence: `/tmp/embermere-sep9-return-before2`, `...-return-after`,
  `/tmp/embermere-sep9-tests`, `/tmp/embermere-sep9-packages.log`, and editor
  `SEP9_*` logs. Temporary evidence is not a committed artifact.
- Original `Saved/SaveGames/EmbermerePrototype.sav` remains byte-identical:
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  Its omitted version stamp remains unresolved, not inferred or migrated.
- Existing Config changes, keeper material resave, and unpublished
  `Docs/FieldNotes/` are excluded. No packages were intentionally saved today.

Final HUD pixels, physical W/S cancellation, held right-mouse, Ctrl+M, and
normal-camera animation/feel review remain user gates. Preserve the previously
accepted Still Waters route and all existing ownership boundaries.
