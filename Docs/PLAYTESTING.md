# Playtesting

This is the current smoke test for the Embermere prototype inside Unreal Editor.

## Before Pressing Play

1. Open `/Game/Maps/L_Embermere_Prototype`.
2. If Codex has just changed C++ while the editor is open, restart Unreal so the editor loads the newest module.
3. Confirm the map shows the `FabPass_` village, dressed road, wilderness pocket, upgraded ruin, quest giver, and three starter enemies.

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
9. Press `I` to hide/show the inventory panel.
10. Press `[` and `]` after inventory has multiple stacks to cycle the inspected item.
11. Walk near Mara Ashwick in the village and press `F`.
12. Watch for the temporary quest/dialogue message.
13. Move toward the ruin and enemy pocket.
14. Press `Tab` to target a nearby hostile.
15. Watch for the selected enemy's UMG nameplate, selected marker, HP text, HP bar, gold segmented ground ring, and the HUD target panel range state.
16. Press `1` to use the first starter ability.
17. Press `1` again before the cooldown finishes and confirm the bottom-left log reports the ability ready time.
18. Confirm combat, target, quest, XP, inventory, mouse, cooldown, and death/recovery messages appear clipped as single-line rows inside the bottom-left chat/combat log rather than overlapping the player status panel or spilling beyond the chat panel border.
19. Expect nearby enemies to chase and melee you when you enter their aggro radius.
20. Defeat three starter enemies.
21. Return to Mara and press `F` to complete the quest.

## Expected Temporary Feedback

- A styled first-pass HUD overlay shows player HP, mana, XP, current target, target HP, range state, quest progress, and all hotbar slots.
- A first-pass inventory HUD panel appears in the top-right, starts as `Empty`, shows item detail text when an item is present, cycles inspected stacks with `[` and `]`, and toggles with `I`.
- Interacting with Mara shows a temporary bottom-screen dialogue panel.
- Mara has a temporary gold quest marker above her in PIE.
- `Q` toggles autorun, and manual `W`/`S` forward/back input cancels it.
- `Ctrl+M` toggles mouse Y inversion and posts a bottom-left chat/combat log message.
- Accepting the quest posts a bottom-left quest accepted message.
- Tab targeting shows the selected target name, HP, first-ability range state, a temporary screen-space UMG nameplate, HP-aware accent/health bar coloring, and a gold segmented mesh ground ring.
- Ability use posts a bottom-left hit message clipped inside the shaded chat panel.
- Reusing an ability before its cooldown finishes posts a bottom-left ready-time message.
- Enemies show red bottom-left combat log messages clipped inside the shaded chat panel when they melee the player.
- If the player dies, autorun turns off, a bottom-left death message appears, and the player respawns after a short delay.
- Defeating starter enemies advances `StarterEnemyDefeated`.
- Defeated enemies hide and respawn after a short prototype delay.
- Completing the quest shows bottom-left completion/XP messages, a temporary loot/reward popup, and the reward item in the inventory panel.

## Known Prototype Gaps

- The UI has native first-pass HUD panels, but styling is still programmer-art and needs a proper fantasy skin.
- Enemy movement and attacks are deliberately simple prototype behavior.
- Player respawn is a simple prototype reset to the spawn point.
- Inventory presentation is a minimal HUD panel with bracket-key stack inspection, not a full inventory screen yet.
- Nameplates now use a first native UMG widget component, but selected-target world highlighting still needs dedicated decal/material art.
- The bottom-left chat/combat log is clipped inside its shaded panel and currently uses single-line rows; final chat history, scrolling, and fantasy styling still need a proper UI pass.
- If Codex changed C++ during the same editor session, restart Unreal before validating interface-heavy tests or new target-presentation behavior.
- The first local Fab/Epic environment pass is in place, but collision, scale, route readability, and style cohesion still need manual PIE review.
- Raw Fab/Epic asset folders are local-only and ignored by Git, so missing local imports will make the art layer show missing references.
