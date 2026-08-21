# Character Creation Contract

## Purpose

Embermere's first pre-play character picker turns the existing race/class
scaffold into a deliberate player choice without creating a second rules
engine. The panel presents identity and requests confirmation; authoritative
rules data decides whether the combination is legal and which starter state
the character receives.

## Ownership

- `UEmbermereRulesData` owns the eight race definitions, four class
  definitions, allowed race/class matrix, class starting attributes, and
  starter ability IDs.
- `UEmbermereCharacterCreationWidget` owns only pending selection and its
  fixed presentation. It may show disabled classes and request one confirmed
  choice, but it cannot silently replace an invalid choice or invent a
  fallback combination.
- `AEmbermereCharacter` validates the requested pair against rules data and
  atomically applies identity, starting attributes, full starting vitals, and
  the first four hotbar abilities.
- `AEmbermerePlayerController` owns the pre-play lifecycle and input handoff.
  It shows the modal once for an unconfirmed character, blocks gameplay input,
  then removes the modal and restores the normal HUD/input path after a
  successful confirmation.
- Existing HUD, combat, inventory, equipment, quest, vendor, trainer, and
  persistence systems remain consumers of the resulting character state. They
  do not own creation rules.

## First-Slice Lifecycle

1. A new character still constructs as Human Warrior with the established
   `100` health, `50` mana, and `10` Attack Power baseline. This is the
   reversible fallback if the creation surface is disabled or cannot be
   created.
2. The controller presents a fixed modal before gameplay and keeps the normal
   HUD hidden behind it.
3. All eight races and all four classes remain visible. Classes disallowed for
   the selected race are disabled. Changing race never silently changes the
   pending class, so an existing invalid pair remains visibly unresolved and
   cannot be confirmed.
4. Mouse buttons and keyboard navigation update only pending selection.
5. Confirmation revalidates against `UEmbermereRulesData`, applies the class's
   data-driven starting attributes and abilities once, broadcasts the accepted
   pair, and restores game-only input.
6. A second confirmation is rejected without changing stats, identity, or the
   hotbar.

The first slice does not include character naming, appearance editing,
multiple characters, deletion, autosave, or a return-to-creation flow.

## Restrictions

The UI must expose the complete data-driven matrix rather than carrying local
special cases. In particular:

- Dwarf Ranger is visible but disabled and cannot be confirmed.
- Bullywug Wizard is visible but disabled and cannot be confirmed.

Any future race/class change must update rules data and automatically flow to
the picker. The widget must not gain its own allow-list.

## Starting Attributes

Class definitions own the first bounded stat profiles. Human Warrior preserves
the existing gameplay baseline. Cleric, Ranger, and Wizard receive distinct
health, mana, and attribute profiles through the same `StartingAttributes`
field already present on class data. Runtime stats consume Max Health, Max
Mana, and Strength as base Attack Power while retaining Spirit, Agility, and
Intellect for later systems.

Equipment bonuses remain additive and idempotent. Character creation rebuilds
base values once and includes any already-authoritative equipment bonus rather
than double-applying it.

## Persistence Boundary

Save version 1 does not store race, class, creation completion, appearance, or
name. This milestone must not change the schema, migrate old saves, or infer a
saved choice from hotbar contents. A deliberate versioned persistence design
is required before creation state becomes durable.

For the prototype, each fresh play world asks for a deliberate choice before
gameplay. Loading version 1 progression then restores only the existing copper,
XP, inventory, equipment, quest, and finite vendor-stock owners onto the
currently selected character.

## Acceptance

- The panel has stable fixed bounds and does not overlap the normal HUD because
  the HUD is hidden during pre-play selection.
- Eight race controls and four class controls are present.
- Invalid classes remain visible and disabled.
- Dwarf Ranger and Bullywug Wizard fail both UI and character-side validation.
- A valid confirmation applies the exact selected race/class, class starting
  attributes, and four data-driven starter abilities.
- Confirmation is one-shot and leaves no duplicate stat or hotbar mutation.
- Gameplay input is blocked while the modal is open and returns to game-only
  mode after confirmation.
- The Human Warrior fallback remains valid when no deliberate selection is
  made.
- Save version 1 and every existing service/transaction authority remain
  unchanged.
