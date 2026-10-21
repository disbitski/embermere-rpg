# October 21 Daily Build

## Targeted Abilities Respect Solid Cover

No voluntary physical Mara, Prowler, Meditate, or small-window pointer state
was supplied. Clean normal-route PIE retained the Human Warrior HUD and target
selection. Code review then found a player-facing combat gap: targeting and
range checks did not stop a selected enemy from taking a targeted ability
through solid ruin collision. The saved ruin wall already had positive
`Visibility` traces in the initialized-world validation lane.

The test-first `Embermere.Combat.ObstructedAbility` created a registered solid
box in a started isolated world. It failed on the old code: the blocked cast
returned success, dealt 30 damage, spent 8 mana, and published one result.
Combat now preflights a simple `Visibility` ray at a stable 90 cm origin
offset, ignoring caster and target. It rejects a blocking world hit before
any resource spend or effect, while preserving existing no-target, range,
defeated-target, and self-ability behavior. Selection stays in place. The
controller renders the exact rejection as `Target is out of sight.`; it owns
no ray or combat mutation. The existing hotbar test now protects that chat
copy, zero mana/damage/cooldown mutation behind a wall, and a valid cast once
the wall moves clear. No saved content, save schema, quest, loot, or target
selection owner changed. See `Docs/COMBAT_SIGHTLINE_CONTRACT.md`.

## Verification

- The focused started-world regression was red before the production change
  and green after it. Both focused sightline and hotbar feedback tests passed.
  The process-local Xcode 26.1.1 EmbermereEditor Mac Development
  `-NoHotReloadFromIDE` build compiled and linked. Sandbox UBT log rotation
  was denied; the identical host-permitted build passed.
- Fresh discovery returned **123** Embermere tests. Final full automation
  passed **123/123** with zero failures, warnings, or skips.
- The sandboxed GUI-down validator stalled in AppKit before creating a log
  and was stopped. The identical host-permitted UnrealEditor
  `-ExecutePythonScript` run exited zero. Dedicated
  `/tmp/embermere-oct21-31-package-aggregate.log` contains explicit
  skeletal-material, placed-Prowler loot, **53 Fab / 32 original-art**, and
  `validators=31` success markers with no `LogPython: Error`.
- All **16** initialized-world suites passed in the final editor session
  with their exact success marker and no Python errors. Clean floating PIE
  confirmed Human Warrior, closed Inventory, and selected the Fenwatch
  Practice Target with its expected out-of-range HUD state from PlayerStart;
  PIE then stopped. The map is All Saved outside PIE on
  `/Game/Maps/L_Embermere_Prototype` with the real editor on MCP 8123.
- The protected save and unrelated keeper material retained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

The normal-route PIE smoke did not press a targeted ability through the
actual ruin wall or physically play a Prowler fight. That aiming/camera feel
and final painted chat remain user checks. Four known raw-vendor
physics-resave warnings remain confined to FieldGrass, River_Rock,
Medium_Boulder, and HillTree; none was resaved.
