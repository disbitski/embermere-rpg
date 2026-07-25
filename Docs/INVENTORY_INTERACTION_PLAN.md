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
- The explicit Sort control shipped on 2026-07-16. It uses stable
  weapon/armor/consumable/quest/misc category priority, then item name,
  preserves the selected item occurrence, and disables while a pointer press
  or drag could make a row identity ambiguous.
- The first data-driven icon family shipped on 2026-07-21. Recruit Pack and
  Marsh Tonic own soft item references; ten equipment slots plus category and
  missing-art states resolve through `UEmbermereUiIconSet`. Fixed row, detail,
  and slot dimensions preserve layout while text and tooltips remain available.
- The first paper-doll illustration shipped on 2026-07-25 as a deterministic
  `128x160` project-owned texture. It resolves through the same shared data
  asset and sits in a hit-test-invisible overlay beneath the unchanged slot
  grid.

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
5. **Complete:** stable category/name sorting restores the selected item and
   duplicate-stack occurrence by identity and never runs during an active drag.
6. **Complete:** the compact text drag label is now a fixed project-owned
   fantasy token with category sigil, item/context text, and stable bounds.
7. **Complete:** project-owned item and equipment-slot art resolves through a
   shared data asset with direct, category, slot, and missing-art fallbacks.
8. **Complete:** the shared resolver now supplies a fixed `44x44` item image in
   the drag token and fixed `32x32` item art in reward feedback without changing
   payload or transaction identity. Category-sigil and missing-item fallbacks
   remain available when direct art does not resolve.
9. **Complete:** the equipment column now layers a restrained illustrated
   adventurer behind the existing ten-slot grid without changing slot bounds,
   labels, input, drag targets, or transaction rules.
10. **Next:** clean-restart visual approval of the paper-doll, icon-bearing
    drag/reward presentation, and populated-slot contrast; then continue with
    bounded status/VFX presentation or the next item-action milestone.

## Verification

- Valid bag-to-slot equip removes one item from the bag and updates stats once.
- Wrong-slot and under-level drops change nothing.
- Replacement returns the old item to the bag exactly once.
- A full bag rejects replacement or unequip without loss or duplication.
- A stale payload is rejected after its item leaves the bag.
- Tooltips and comparison text update after every successful transaction.
- Click, keyboard inspection, and action-button behavior remain unchanged.
- Sorting orders equipment before consumables and quest/misc items, preserves
  alphabetical and equal-key stability, and keeps the inspected item selected.
- The Sort control is unavailable while a bag press or drag is active.
- Drag hover and drop states fit inside the fixed inventory window at desktop
  and compact PIE resolutions.
- Item and slot icons stay inside fixed bounds, preserve text/tooltips, and use
  category or missing-art fallback without moving rows or the footer.

Automation covers the transaction and identity matrix in
`Embermere.UI.InventoryDragDrop` plus order and selection invariants in
`Embermere.Inventory.StableSorting`, while `Embermere.UI.IconPresentation`
covers icon resolution, source dimensions, fallback paths, and fixed UI bounds.
`Embermere.UI.PaperDollPresentation` covers the backdrop reference, persisted
source dimensions, HUD resolver, and fixed presentation bounds. A clean-restart
PIE pass still owns pointer threshold, cached-geometry hit testing, the visible
Sort control, icon-bearing drag visual, reward-popup art, backdrop contrast,
icon readability, hover feedback, cursor mode, and compact-viewport
verification.
