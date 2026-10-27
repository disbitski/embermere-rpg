# Enemy Aggro Sightline Contract

## Authority

The enemy AI owns initial aggro acquisition. A living player inside the saved
Prowler's 525 cm radius is eligible only when a simple `Visibility` ray is
clear between enemy and player origins plus 90 cm. Ignore only the two
endpoint actors. Solid world cover blocks a new acquisition; decorative
`NoCollision` art does not. This rule does not change the radius or add a
stealth, awareness, or persistence state.

Once the Prowler has acquired a living player, later cover does not discard
that engagement. Existing chase, blocked-movement animation, leash, melee
cover, death, quest credit, loot, and respawn rules remain authoritative.
No UI, vendor, trainer, inventory, or save ownership moves into aggro.

## Acceptance

`Embermere.Enemy.ObstructedAggroAcquisition` starts a world with the saved
Prowler class, a possessed player inside aggro range, and registered solid
cover. A positive native trace must hit the wall. A real AI tick cannot
engage, move, or damage through it; after the wall moves clear, the same
Prowler engages and chases. Restoring cover cannot erase that already-live
engagement. The existing blocked-pursuit and melee-cover regressions must
remain green.

The initialized Fenwatch validator protects three clear solo-pull sightlines
and a grounded in-range pillar-covered stance at `(2300,500)` for
`Starter_Enemy_01`. A voluntary physical Prowler encounter is still needed
to judge final acquisition, animation, and camera feel.
