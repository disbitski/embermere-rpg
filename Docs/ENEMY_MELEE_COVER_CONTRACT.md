# Enemy Melee Cover Contract

## Authority

The enemy AI owns whether its own melee attack can connect. Being inside
`AttackRange` is necessary but not sufficient: immediately before an attack,
the enemy must have a clear simple `Visibility` ray between its origin plus
90 cm and the player's origin plus 90 cm. Ignore only the attacking enemy and
intended player. A blocking world hit prevents animation, damage, attack
cooldown, and hit chat for that attempt. It does not erase aggro, teleport the
enemy, alter leash or movement, or change the player's combat preflight.

When cover clears, the same live engagement can attack normally without
waiting through a cooldown from the blocked attempt. Decorative
`NoCollision` meshes must not become melee blockers. Dead or invalid targets
retain the existing rejection path. No quest, loot, inventory, save, or UI
authority moves into this rule.

## Acceptance

`Embermere.Enemy.MeleeCoverSafety` begins an isolated world with a possessed
living player, an in-range enemy, and a registered blocking box. A positive
native `Visibility` trace must hit the box. The enemy's real AI tick must
leave player health unchanged through cover; after the box moves clear, a
second tick must apply one ordinary hit. An unobstructed real Prowler fight
and final camera/animation read remain voluntary physical playtest gates.
