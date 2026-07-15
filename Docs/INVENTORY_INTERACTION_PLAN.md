# Embermere Inventory Interaction Plan

## Purpose

Build mouse-first item inspection and drag/drop on top of Embermere's existing
atomic bag/equipment transactions without creating a second set of inventory
rules. Click, keyboard, and future controller interaction remain supported.

## Current Foundation

- Inventory rows can be selected by click or with `[` and `]`.
- The detail pane exposes item action, slot, level, effects, description, and
  the net stat change against the currently equipped item.
- Inventory rows and occupied paper-doll slots have hover tooltips.
- Equipping, replacing, and unequipping already use rollback-safe component
  operations. Full-bag failures preserve both the bag and equipped state.
- The ten paper-doll slots remain readable buttons, so drag/drop will be an
  additional path rather than the only path.
- The first bounded drag/drop slice shipped on 2026-07-15: bag rows are drag
  sources, equipment slots are typed drop targets, and the bag list accepts
  equipped-item returns. Gold/red hover feedback and a compact item-name drag
  visual fit inside the existing window.

## Drag Payload

Add a small `UDragDropOperation` subclass that carries:

- the item data asset reference;
- whether the source is the bag or an equipment slot;
- the source equipment slot when applicable;
- display text or icon data for the drag visual.

Do not use a bag row index as item identity. Stack indexes can change after a
loot event, consumption, equipment replacement, or future sorting. Resolve the
item against the live inventory when the drop occurs and reject stale payloads.

## Interaction Rules

### Bag To Equipment

- A drag begins only after the pointer moves past Unreal's drag threshold, so a
  normal click still selects the row.
- Compatible paper-doll slots show a gold valid-drop state.
- Incompatible slots show a restrained red state and reject the drop without
  mutating inventory.
- A valid drop calls the same atomic equip-from-inventory operation used by the
  `Equip` button.
- Level requirements and replacement capacity rules remain authoritative in
  the equipment component.

### Equipment To Bag

- Dragging an occupied slot onto the bag list or a dedicated bag return target
  calls the existing atomic unequip-to-inventory operation.
- A full bag rejects the drop, keeps the item equipped, and posts the same clear
  chat feedback as click-to-unequip.
- Clicking an occupied slot remains the compact fallback behavior.

### Deferred Interactions

- Direct equipment-slot-to-equipment-slot moves are unnecessary while each
  item has one fixed slot.
- Splitting stacks, dropping items into the world, selling, and destroying
  items need explicit confirmation flows and are separate milestones.
- Controller drag emulation should wait until controller navigation is part of
  the prototype; all actions must remain reachable without dragging.

## Sorting Contract

- Sorting must preserve the selected item by item identity rather than by row
  number.
- Do not reorder stacks during an active drag.
- Start with stable category/name sorting and keep quest items predictable.
- Any future manual bag ordering needs persistent slot identities rather than
  the current compact stack array.

## Implementation Status

1. **Complete:** item-identity action helpers route click and drop through the
   same atomic equipment operations.
2. **Complete:** `UEmbermereItemDragDropOperation` carries live item identity,
   bag/equipment source, and source slot rather than a fragile row index.
3. **Complete:** equipment slots expose gold valid and restrained red invalid
   hover states, with matching-slot and level preflight.
4. **Complete:** the bag list accepts equipped-item returns and preserves the
   existing full-bag rejection/chat path.
5. **Next:** add stable sorting that restores selection by item identity and
   never reorders during an active drag.
6. **After PIE:** replace the compact text drag label with project-owned fantasy
   icon/frame art once the gesture survives clean-restart playtesting.

## Verification

- Valid bag-to-slot equip removes one item from the bag and updates stats once.
- Wrong-slot and under-level drops change nothing.
- Replacement returns the old item to the bag exactly once.
- A full bag rejects replacement or unequip without loss or duplication.
- A stale payload is rejected after its item leaves the bag.
- Tooltips and comparison text update after every successful transaction.
- Click, keyboard inspection, and action-button behavior remain unchanged.
- Drag hover and drop states fit inside the fixed inventory window at desktop
  and compact PIE resolutions.

Automation now covers the transaction and identity matrix in
`Embermere.UI.InventoryDragDrop`. A clean-restart PIE pass still owns pointer
threshold, cached-geometry hit testing, drag visual, hover feedback, cursor
mode, and compact-viewport verification.
