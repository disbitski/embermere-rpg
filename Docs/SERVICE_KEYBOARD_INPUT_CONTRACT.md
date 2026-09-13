# Service Keyboard Input

The controller supplies an explicit keyboard fallback for the existing native
vendor and trainer panels. This does not repair or certify MCP pointer clicks.

## Ownership

The controller reads fresh key presses only while a service panel is visible.
The HUD owns selection and sends the same requests as its existing buttons.
Vendor/trainer data and components still own stock, prices, level eligibility,
capacity, copper, XP, repeatability, and atomic rejection/rollback. Art owns none
of these. No new save records, input configuration, or panel geometry are needed.

## Input

- Up/Down wraps through vendor stock or trainer offerings, including unavailable
  rows so their exact rejection copy remains inspectable.
- Enter requests one Buy or Train for the selected row. Disabled mouse actions
  remain disabled; the service independently rejects an ineligible key request.
- Escape closes the service and restores the existing interactive input mode.
- Vendor brackets still select bag items, not stock. Trainer brackets still
  select offerings. Mouse buttons, Sell, Buyback, and I/J/M handoffs remain intact.
- Held-key repeat does not repeat transactions. A new press is a new request.
  Closing or switching panels prevents requests against the previous service.
- Selection/close takes precedence over an action arriving in the same frame.
  The existing Ledger input path is unchanged.

This is an explicit stock/lesson selection and primary-action fallback, not a
complete Tab-focus redesign or keyboard-only selling/buyback workflow. It does
not need probe-only UButton focus or direct delegate/transaction invocation.

## Acceptance

Routed automation must deliver InputKey events through PlayerInput/PlayerTick,
then assert selection, exact owner state, rejection, repeat safety, and peer
handoff. Directly calling a transaction is not an input test. Isolated input
tests do not prove OS input, Slate hit testing, or physical camera feel.

Clean PIE must reach the real services through movement/F, exercise the keys,
and prove exact wallet/XP/items and close/handoff state. Preserve the user's
original save and existing assets. Leave physical click/HUD review explicitly
unverified when editor MCP cannot establish it.
