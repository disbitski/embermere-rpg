# Targeted Ability Sightline Contract

## Authority

The combat component owns targeted-ability eligibility. Target cycling may
retain a selected enemy behind a wall, but an enemy- or ally-targeted ability
must preflight a simple `Visibility` ray before any mana spend, effect,
cooldown, immutable result, loot, or quest credit. A self-targeted ability
does not need a world ray. UI only renders the combat-owned rejection; it
does not infer geometry or mutate the target.

The ray runs between source and target at a stable 90 cm offset from each
actor origin. It ignores the caster and selected target, uses simple collision,
and treats the first blocking world hit as an obstruction. Decorative
`NoCollision` art remains non-authoritative. Existing range and living-target
preflights keep their current order; no selected target or too much distance
retain their existing messages. An obstructed attempt must keep selection,
health, mana, cooldown, result stream, rewards, and quest progress unchanged.
The controller renders the exact combat-owned rejection as
`Target is out of sight.` without calculating its own ray.

## Acceptance

`Embermere.Combat.ObstructedAbility` uses a registered solid box in an
isolated started world as a positive trace control. It must reject an in-range
targeted cast through the box without resource or reward mutation; the same
cast must work after the box moves clear. The existing
`Embermere.Input.HotbarRejectionFeedback` regression also checks the exact
chat copy and absence of mana, health, or cooldown mutation. The saved ruin
wall already has an
initialized-world positive `Visibility` control at three heights in
`validate_ancient_ruin_relief_traces_unreal.py`. Clean normal-route PIE must
retain unobstructed practice-target and Prowler combat. Physical combat feel,
the actual wall-blocked hotbar press, and final painted rejection copy remain
separate user checks.
