# Incoming Damage Cue Contract

## Authority

Stats alone owns committed, post-mitigation health loss. A live damage event
reports only a positive amount actually removed by `ApplyDamage`; invalid,
immune, already-dead, heal, forced-death, identity rebuild, and save-load paths
do not publish it. This event grants no damage, threat, loot, quest credit,
currency, or persistence state.

The HUD may observe that event to briefly brighten the existing fixed-size
health bar. It owns neither eligibility nor resource mutation. The cue lasts
0.5 seconds, renews on another hit, then returns to the original red. It must
not move adjacent HUD content, intercept input, accumulate a message queue,
replay on load, or outlive a health rebuild, character unbinding, death, or
widget teardown.
The current health numbers and exact chat remain durable fallbacks.

## Acceptance

`Embermere.UI.IncomingDamageCue` first proves the existing health bar does not
respond transiently to a committed hit. It then protects fixed geometry,
post-mitigation damage, rejection silence, renewal, expiry, and teardown.
Clean normal-route PIE must retain the existing top-left status layout; final
painted contrast and physical combat feel remain a user playtest gate.
