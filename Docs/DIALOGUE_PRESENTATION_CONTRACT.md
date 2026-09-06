# Dialogue Presentation Contract

## Boundary

Quest/interactable/service owners provide the speaker and complete dialogue.
The native HUD displays that payload only. It must not change interaction,
quest state, progress, rewards, services, focus, or save version 3. Existing
chat and Quest Ledger remain independent fallbacks.

## Fixed Surface

The inherited 620x72 panel allowed wrapped text to paint beyond its bounds,
with only eight pixels between the panel and the unchanged hotbar. Replace
that layout with one fixed 620x120 panel, bottom-centered 116 pixels above
the viewport edge. This leaves 16 pixels above the 64-pixel hotbar whose
bottom margin remains 36 pixels. Do not move or resize any hotbar slot.

Keep the existing 12x10 panel padding. The inner canvas is 596x100: a 596x28
single-line speaker region and a 596x72 wrapped body region below it. Speaker
font stays 16; body uses 14 with an explicit 596-pixel wrap width. Both text
regions and their panel clip to bounds, including long words and newlines.
The whole panel is hit-test-invisible so dialogue cannot capture camera/input.

The original seven-second world-time lifetime and latest-payload replacement
remain unchanged. There is no new queue, modal, dialogue choice, save state,
or dismiss command. This slice does not alter peer-panel or quest-update
lifecycle. Broader dialogue redesign is separate work.

## Verification

Test the real initialized native widget tree, exact canvas geometry and gap,
speaker/body payload separation, hit-test invisibility, clipping, authored
Mara/rest copy fit, extreme-copy containment, repeated replacement, and zero
durable owner mutation. Retain the existing hotbar 92x64 slots and 32x32 art.
Use clean PIE F interaction and Unreal-owned Slate bounds/text; final HUD
pixel and held-mouse acceptance remain user gates when viewport-only captures
exclude UMG. Do not use desktop or broad editor captures to fill that gap.
