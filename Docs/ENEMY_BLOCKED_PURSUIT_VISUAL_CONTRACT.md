# Blocked Pursuit Visual Contract

## Ownership

The enemy AI owns pursuit, sweep collision, leash, and attack decisions.
Its visual animation selector may observe the displacement that the sweep
actually achieved, but must not infer movement from an attempted step.
When a solid obstacle halts a pursuing Marsh Prowler, the existing Idle
animation should replace Walk after the current animation lock, without
teleporting, changing aggro, spending an attack cooldown, or damaging the
player. When the path clears, the same pursuit should resume Walk.

## Acceptance

`Embermere.Enemy.BlockedPursuitVisual` starts an isolated world with the
saved Prowler class and animation assets. It lets the capsule settle against
a registered solid wall, proves a subsequent tick has no meaningful
displacement, then checks Idle. Moving the wall away must restore movement
and Walk. The focused test must be red for the stale Walk animation before
production repair, and green afterward. Physical cover composition and
animation feel remain a player playtest gate.
