# Vendor Service Contract

Embermere keeps merchant art, service rules, player resources, and UI in
separate replaceable layers. The Fenwatch quartermaster is the first complete
vertical slice of that contract.

## Ownership

- `AEmbermereNpcPresentationActor` owns only interchangeable static/skeletal
  visuals and their shared authored transform. It has no interaction or vendor
  component.
- `AEmbermereVendorServiceActor` owns the nearby interaction marker plus a
  `UEmbermereVendorComponent`. It has no static or skeletal mesh, collision, or
  navigation contribution.
- `UEmbermereVendorStockData` owns the merchant name, item references, prices,
  and initial finite/unlimited quantities.
- `UEmbermereWalletComponent` owns player copper. The existing inventory owns
  item identity, stacks, and capacity. Item data owns sell value; quest data
  owns copper reward value.
- `UEmbermerePlayerHudWidget` presents stock, selected inventory, and buyback
  state and sends buy, sell, and buyback requests. It does not decide price,
  stock, sellability, affordability, ownership, or capacity.

Changing the quartermaster mesh cannot change prices. Changing stock cannot
move the merchant. Replacing the HUD cannot bypass transaction rules.

## Fenwatch Slice

The saved map contains two co-located but independent actors at
`(-1530, -1190, 0)`, yaw `100`:

- `Embermere_FenwatchQuartermaster_Vendor_01`: art-only NPC presentation;
- `Embermere_FenwatchQuartermaster_Service_01`: invisible gameplay service,
  tagged `EmbermereGameplayService` and `EmbermereVendor`.

`/Game/Data/Vendors/DA_FenwatchQuartermasterStock` currently defines:

- Marsh Tonic: `8` copper, unlimited prototype stock;
- Recruit Pack: `30` copper, one per service runtime.

Players currently start with `40` prototype copper. Pressing `F` in range opens
the fixed native `Fenwatch Supplies` panel with purse balance, stock rows,
resolved item icons, detail copy, remaining quantity, Buy, Sell selected, the
most recent Buyback entry, close, and chat feedback. `[` and `]` can move the
selected bag identity while the panel is open. Opening it hides Inventory to
prevent overlapping interactive panels; closing restores the normal game-only
input path.

Saved economy values currently define:

- Marsh Tonic: `3` copper sell value;
- Recruit Pack: `12` copper sell value;
- First Signs at the Ruin: `20` copper completion reward, paid exactly once.

## Purchase Transaction Order

Every purchase preflights the complete request before mutation:

1. Valid vendor, stock row, item, quantity, and non-negative price.
2. Available finite or unlimited stock.
3. Exact player affordability.
4. Inventory capacity for the complete quantity.

Commit then spends copper, adds the complete item quantity, and decrements
finite stock only after both mutations succeed. An unexpected inventory add
failure refunds the exact charge. Rejected purchases preserve copper, stock,
and inventory without partial state or duplication.

## Sell Transaction Order

Every sale preflights before mutation:

1. Valid item, positive quantity, inventory, and wallet.
2. A positive data-driven sell value and a non-quest item category.
3. Exact ownership of the requested quantity.
4. Safe total-value arithmetic and enough wallet headroom.
5. Safe aggregation into the bounded buyback history.

Commit credits the exact sale value, removes the exact item identity, then
records buyback only after both mutations succeed. An unexpected remove failure
retracts the credited copper. Rejected sales do not create buyback entries.

## Buyback Transaction Order

Buyback is vendor runtime state, not global inventory state. The current panel
offers the newest entry first and records the item identity, quantity, and exact
sale-unit price.

1. Validate the entry and requested quantity.
2. Preflight exact affordability and complete inventory capacity.
3. Spend the exact recorded price.
4. Restore the exact item identity without a fake loot notification.
5. Decrement or remove the entry only after successful delivery.

An unexpected add failure refunds the exact charge. Full inventory,
insufficient funds, missing entries, and exhausted quantities preserve all
owners unchanged.

## Persistence And Validation

`Scripts/configure_fenwatch_vendor_unreal.py` creates or reconciles the stock
asset and saved service actor without touching the presentation mesh.
`Scripts/validate_fenwatch_vendor_unreal.py` reloads the packages and rejects
incorrect stock, transforms, tags, component ownership, art on the service, or
service behavior on the presentation.

Focused automation:

- `Embermere.Economy.FenwatchRewardsAndValues`
- `Embermere.Vendor.TransactionRules`
- `Embermere.Vendor.SellBuybackTransactions`
- `Embermere.Vendor.ServiceContract`
- `Embermere.Vendor.FenwatchStockData`
- `Embermere.UI.VendorPanel`

Clean PIE acceptance on 2026-08-05 proved the complete runtime sequence:

- tonic purchase `40 -> 32`;
- selected-identity tonic sale `32 -> 35`;
- tonic buyback `35 -> 32` with the same item restored;
- finite Recruit Pack purchase `32 -> 2` and stock `1 -> 0`;
- rejected tonic purchase holding the purse at `2`;
- Mara quest completion `2 -> 22`, `125` XP, and Recruit Pack reward;
- repeated completion rejected with the purse still at `22`.

The fixed panel, bottom-left chat, inventory stack count, sold-out row, latest
buyback row, and result copy all updated without footer or hotbar overlap. All
`38/38` automation tests and fresh-process economy validation passed.

## Current Limits

- Copper, runtime stock, and buyback are not persisted between sessions.
- Selling and buyback are intentionally one item at a time; quantity entry,
  confirmation, and reputation pricing remain absent.
- Buyback history is bounded and vendor-local. Its future persistence policy is
  deliberately undecided rather than accidentally serialized.
- The service uses the existing generic interactable marker; final merchant
  prompt and audio presentation remain future work.

The next useful economy slice is a versioned save-game contract for wallet,
inventory/equipment identity, quest completion, and finite stock, with an
explicit decision about whether buyback survives a loaded session.
