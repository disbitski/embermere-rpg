# Embermere Combat Feedback Contract

Floating combat feedback is a short-lived view of an outcome that combat has
already committed. It is never a second hit resolver and never a replacement
for the target frame or bottom-left combat log.

## Authority Boundary

`UEmbermereCombatComponent` remains the only owner of ability validation and
application. After an effect succeeds, it publishes an immutable
`FEmbermereCombatResult` containing the source, target, ability identifier,
result kind, exact applied amount, and defeated state. The result reports the
post-armor damage or actual recovered resource, not the requested power.

The combat-result event does not:

- apply health, mana, buffs, control, or death;
- choose or clear targets;
- drive AI, retaliation, leash, loot, XP, quests, rewards, or persistence;
- invent a miss when the rules have not resolved one;
- replace the existing `OnAbilityUsed` cooldown notification.

`Miss` is a supported result kind so a future hit-resolution rule can publish
one through the same contract. The current deterministic ability rules do not
generate misses.

## Presentation Contract

`UEmbermereCombatFeedbackWidget` is a hit-test-invisible, full-screen native
UMG observer. It accepts only damage and miss results for floating display.
Other result kinds remain available to future presentation consumers without
creating redundant text beside the existing status rows and resource bars.

The widget keeps exactly three fixed `112 x 32` slots. New outcomes appear
first, older outcomes move down, and a fourth rapid result evicts the oldest.
Each entry has a deterministic 1.25-second lifetime, restrained upward drift,
and a short terminal fade. Damage uses the exact rounded applied amount; misses
use `MISS`. Target projection is derived from the targetable presentation
anchor, with a validated native fallback for older saved Blueprints, so neither
the Prowler nor the practice target owns floating-text placement. Screen-space
nameplates are deliberately excluded from that anchor calculation because they
can inflate generic actor bounds far beyond the visible creature.

Entries clear when their target becomes invalid, the selected target changes,
the target dies and selection clears, the practice target enters reset, the
widget is destructed, or the world tears down. The target frame, native
nameplate, cyan target circle, status auras, and clipped chat log remain the
durable readability fallbacks.

## Acceptance Gates

1. Combat automation proves one exact result per committed primary outcome,
   post-mitigation amounts, source/target identity, and lethal state without
   changing the established ability-used event.
2. UI automation proves damage and future-miss formatting, a three-entry cap,
   newest-first ordering, fixed dimensions, deterministic expiry, target
   clearing, and no gameplay components or mutation authority.
3. Clean PIE proves rapid real hotbar strikes are readable from the normal
   camera on both Marsh Prowlers and the art-free practice target without
   obscuring nameplates, the cyan circle, status auras, target HP, or chat.

The rule is simple: combat decides what happened once; presentation may only
show that fact briefly.
