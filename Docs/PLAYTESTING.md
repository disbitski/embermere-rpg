# Playtesting

This is the current smoke test for the Embermere prototype inside Unreal Editor.

## Before Pressing Play

1. Open `/Game/Maps/L_Embermere_Prototype`.
2. If Codex has just changed C++ while the editor is open, restart Unreal so the editor loads the newest module.
3. Confirm the map shows a blue atmospheric sky, readable ambient light, muted moss ground instead of a white foundation, 64 upright `FabPass_` actors plus `Embermere_Waystone_Road_01`, an unobstructed PlayerStart/Mara route, the dressed road, wilderness pocket, upgraded ruin, quest giver, and three starter enemies.

## Finding Mara

Mara is the quest giver actor named `Quest_Giver_Mara_Fenwatch`.

From the village spawn, move mostly straight ahead and slightly right toward the center of the starter village. In world coordinates, the player starts around `(-2400, -1200)` and Mara is nearby at `(-2050, -850)`, near the village props and stone backdrop.

In Play In Editor, quest givers show a temporary gold `!` and name marker above them.

## Current Play Loop

1. Press Play.
2. Use `WASD` to move.
3. Hold right mouse to rotate the camera and character facing together.
4. Hold left mouse to rotate the camera only.
5. Hold both mouse buttons to move forward.
6. Mouse Y starts inverted by default; press `Ctrl+M` to toggle normal/inverted.
7. Press `Q` to toggle autorun.
8. Press `W` or `S` while autorunning to stop autorun.
9. Confirm the visible inventory opens in cursor-aware game/UI mode, then press `I` to hide/show it; closing the inventory should restore classic game-only mouse control.
10. After earning Mara's Recruit Pack reward, select it and confirm it identifies as level-1 Back armor with `+5 HP, +1 Armor`. Confirm its detail pane compares it with the empty Back slot, then hover its row and verify the tooltip includes quantity, armor/slot/level, effects, comparison, and description.
11. Click `Equip`; confirm the item leaves the bag, the gold Back-slot control shows Recruit Pack, aggregate bonuses update, HP becomes `105/105`, and chat reports the action. Hover the occupied Back slot for item details, then click it; confirm the item returns to the bag and stats/slot state return cleanly without a fake loot/reward popup.
12. With every bag slot occupied, confirm clicking an equipped slot refuses to unequip and posts an inventory-full message without losing or duplicating the item.
13. Walk near Mara Fenwatch in the village and press `F`.
14. Watch for the temporary quest/dialogue message.
15. Move toward the ruin and enemy pocket.
16. Press `Tab` to target a nearby hostile.
17. Watch for the selected enemy's UMG nameplate, selected marker, HP text, HP bar, flat animated 24-segment emissive gold ground ring, and the HUD target panel range state.
18. Press `1` to use the first starter ability.
19. Press `1` again before the cooldown finishes and confirm the bottom-left log reports the ability ready time.
20. Confirm the cooling hotbar slot dims and shows its live countdown.
21. Confirm combat, target, quest, XP, inventory, mouse, cooldown, and death/recovery messages appear clipped as single-line rows inside the bottom-left chat/combat log rather than overlapping the player status panel or spilling beyond the chat panel border.
22. Expect nearby enemies to chase and melee you when you enter their aggro radius.
23. Pull an enemy away from its wilderness pocket and confirm it eventually leashes back toward its spawn instead of chasing indefinitely into the village.
24. Defeat a Marsh Prowler and confirm the chat reports one Marsh Tonic looted and inventory gains a stack. Repeated drops should increase the same stack up to its limit.
25. After taking damage, select Marsh Tonic and click `Use`; confirm it restores up to 25 health and 10 mana and consumes one tonic. At full health/mana, `Use` must be disabled and preserve the stack.
26. Defeat three starter enemies.
27. Return to Mara and press `F` to complete the quest.

## Expected Temporary Feedback

- A styled first-pass HUD overlay shows player HP, mana, XP, current target, target HP, range state, quest progress, and all hotbar slots.
- A 700x330 structured inventory/equipment window appears in the top-right with `Slots X / 24`, clickable highlighted item rows, selected-item effects and net equipment comparison, row/occupied-slot tooltips, ten stable clickable paper-doll slots, aggregate bonuses, description, empty/reward state, `[`/`]` cycling, `I` close/show behavior, and Equip or Use actions when supported.
- Opening the inventory shows the cursor and permits UI clicks; closing it hides the cursor and restores classic game-only mouse input.
- Interacting with Mara shows a temporary bottom-screen dialogue panel.
- Mara has a temporary gold quest marker above her in PIE.
- `Q` toggles autorun, and manual `W`/`S` forward/back input cancels it.
- `Ctrl+M` toggles mouse Y inversion and posts a bottom-left chat/combat log message.
- Accepting the quest posts a bottom-left quest accepted message.
- Tab targeting shows the selected target name, HP, first-ability range state, a temporary screen-space UMG nameplate, HP-aware accent/health bar coloring, and a flat animated 24-segment gold ground ring.
- Ability use posts a bottom-left hit message clipped inside the shaded chat panel.
- Reusing an ability before its cooldown finishes posts a bottom-left ready-time message; the hotbar slot itself dims and counts down while cooling.
- Enemies show red bottom-left combat log messages clipped inside the shaded chat panel when they melee the player.
- Enemies leash and return home if pulled too far from their spawn area.
- If the player dies, autorun turns off, a bottom-left death message appears, and the player respawns after a short delay with a brief damage-protection message.
- Defeating starter enemies advances `StarterEnemyDefeated`.
- Defeating a Marsh Prowler grants one stackable Marsh Tonic and reports it in chat.
- Defeated enemies hide and respawn after a short prototype delay.
- Completing the quest shows bottom-left completion/XP messages, a temporary loot/reward popup, and the reward item in the inventory panel.

## Known Prototype Gaps

- The UI has native first-pass HUD panels, but styling is still programmer-art and needs a proper fantasy skin.
- Enemy movement and attacks are deliberately simple prototype behavior.
- Enemy leash/return-home behavior is first-pass and still needs tuning against the dressed FabPass terrain.
- Player respawn is a simple prototype reset to the spawn point with short damage protection, not a full corpse run or revive system.
- Inventory presentation now has clickable rows, keyboard selection, row and occupied-slot tooltips, net equipment comparison, clickable paper-doll slots, atomic bag/equipment transfers, rollback-safe replacement, equipment stat application, armor mitigation, safe consumable depletion, and a real Marsh Tonic enemy-loot source. It does not yet have illustrated body art, drag/drop gestures, or sorting; the interaction contract is documented in `Docs/INVENTORY_INTERACTION_PLAN.md`.
- Nameplates use a native UMG widget component and target highlighting uses a dedicated emissive material on a flat animated 24-segment ring; a future decal/texture treatment can add runes and softer edges.
- The bottom-left chat/combat log is clipped inside its shaded panel and currently uses single-line rows; final chat history, scrolling, and fantasy styling still need a proper UI pass.
- If Codex changed C++ during the same editor session, restart Unreal before validating interface-heavy tests or new target-presentation behavior.
- The first local Fab/Epic environment pass is upright and spawn-safe after removing three oversized sci-fi shells. Its first atmosphere/ambient-light correction is in place, but collision, scale, route readability, final lighting balance, and style cohesion still need manual PIE review.
- Raw Fab/Epic asset folders are local-only and ignored by Git, so missing local imports will make the art layer show missing references.
