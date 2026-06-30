# Playtesting

This is the current smoke test for the Embermere prototype inside Unreal Editor.

## Before Pressing Play

1. Open `/Game/Maps/L_Embermere_Prototype`.
2. If Codex has just changed C++ while the editor is open, restart Unreal so the editor loads the newest module.
3. Confirm the map shows the greybox village, road markers, wilderness pocket, ruin, quest giver, and three starter enemies.

## Finding Mara

Mara is the quest giver actor named `Quest_Giver_Mara_Fenwatch`.

From the village spawn, move mostly straight ahead and slightly right toward the center of the circular village safe-area marker. In world coordinates, the player starts around `(-2400, -1200)` and Mara is nearby at `(-2050, -850)`, between the village blockout buildings.

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
10. Walk near Mara Ashwick in the village and press `F`.
11. Watch for the temporary quest/dialogue message.
12. Move toward the ruin and enemy pocket.
13. Press `Tab` to target a nearby hostile.
14. Watch for the selected enemy's overhead `TARGET` marker, name, HP, ground ring, and the HUD target panel range state.
15. Press `1` to use the first starter ability.
16. Expect nearby enemies to chase and melee you when you enter their aggro radius.
17. Defeat three starter enemies.
18. Return to Mara and press `F` to complete the quest.

## Expected Temporary Feedback

- A styled first-pass HUD overlay shows player HP, mana, XP, current target, target HP, range state, quest progress, and all hotbar slots.
- A first-pass inventory HUD panel appears in the top-right, starts as `Empty`, and toggles with `I`.
- Interacting with Mara shows a temporary bottom-screen dialogue panel.
- Mara has a temporary gold quest marker above her in PIE.
- `Q` toggles autorun, and manual `W`/`S` forward/back input cancels it.
- `Ctrl+M` toggles mouse Y inversion and shows a temporary message.
- Accepting the quest shows an on-screen quest accepted message.
- Tab targeting shows the selected target name, HP, first-ability range state, a temporary overhead target marker/nameplate, and a prototype ground ring.
- Ability use shows an on-screen hit message.
- Enemies show red on-screen hit messages when they melee the player.
- If the player dies, autorun turns off, a temporary death message appears, and the player respawns after a short delay.
- Defeating starter enemies advances `StarterEnemyDefeated`.
- Defeated enemies hide and respawn after a short prototype delay.
- Completing the quest shows on-screen completion, XP, a temporary loot/reward popup, and the reward item in the inventory panel.

## Known Prototype Gaps

- The UI has native first-pass HUD panels, but styling is still programmer-art and needs a proper fantasy skin.
- Enemy movement and attacks are deliberately simple prototype behavior.
- Player respawn is a simple prototype reset to the spawn point.
- Inventory presentation is a minimal HUD panel, not a full inventory screen yet.
- Nameplates and selected-target world highlighting are temporary text/debug render components and still need dedicated visual treatment.
- If Codex changed C++ during the same editor session, restart Unreal before validating interface-heavy tests or new target-presentation behavior.
- Art is still greybox until Fab/Marketplace assets are imported.
