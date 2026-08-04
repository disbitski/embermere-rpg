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
  item stacks and capacity.
- `UEmbermerePlayerHudWidget` presents stock and sends selected buy requests;
  it does not decide price, stock, affordability, or capacity.

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
resolved item icons, detail copy, remaining quantity, Buy, close, and chat
feedback. Opening it hides Inventory to prevent overlapping interactive panels;
closing restores the normal game-only input path.

## Transaction Order

Every purchase preflights the complete request before mutation:

1. Valid vendor, stock row, item, quantity, and non-negative price.
2. Available finite or unlimited stock.
3. Exact player affordability.
4. Inventory capacity for the complete quantity.

Commit then spends copper, adds the complete item quantity, and decrements
finite stock only after both mutations succeed. An unexpected inventory add
failure refunds the exact charge. Rejected purchases preserve copper, stock,
and inventory without partial state or duplication.

## Persistence And Validation

`Scripts/configure_fenwatch_vendor_unreal.py` creates or reconciles the stock
asset and saved service actor without touching the presentation mesh.
`Scripts/validate_fenwatch_vendor_unreal.py` reloads the packages and rejects
incorrect stock, transforms, tags, component ownership, art on the service, or
service behavior on the presentation.

Focused automation:

- `Embermere.Vendor.TransactionRules`
- `Embermere.Vendor.ServiceContract`
- `Embermere.Vendor.FenwatchStockData`
- `Embermere.UI.VendorPanel`

Clean PIE acceptance on 2026-08-04 proved normal `F` opening, a tonic purchase
from `40` to `32` copper, a finite Recruit Pack purchase from `32` to `2`,
sold-out state, insufficient-funds state, inventory mutation, chat feedback,
non-overlapping fixed panel copy, and close/input restoration.

## Current Limits

- Copper and runtime stock are not persisted between sessions.
- Copper is prototype starting state; quests and loot do not yet award it.
- Selling, stack quantities, buyback, confirmation, and reputation pricing are
  intentionally absent.
- The service uses the existing generic interactable marker; final merchant
  prompt and audio presentation remain future work.

The next useful economy slice is earning copper through the playable loop and
adding rollback-safe selling or buyback without weakening this ownership
boundary.
