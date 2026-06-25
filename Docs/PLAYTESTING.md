# Playtesting

This is the current smoke test for the Embermere prototype inside Unreal Editor.

## Before Pressing Play

1. Open `/Game/Maps/L_Embermere_Prototype`.
2. If Codex has just changed C++ while the editor is open, restart Unreal so the editor loads the newest module.
3. Confirm the map shows the greybox village, road markers, wilderness pocket, ruin, quest giver, and three starter enemies.

## Current Play Loop

1. Press Play.
2. Use `WASD` to move.
3. Hold right mouse to rotate the camera and character facing together.
4. Hold left mouse to rotate the camera only.
5. Hold both mouse buttons to move forward.
6. Press `Q` to toggle autorun.
7. Walk near Mara Ashwick in the village and press `F`.
8. Watch for the temporary quest/dialogue message.
9. Move toward the ruin and enemy pocket.
10. Press `Tab` to target a nearby hostile.
11. Press `1` to use the first starter ability.
12. Expect nearby enemies to chase and melee you when you enter their aggro radius.
13. Defeat three starter enemies.
14. Return to Mara and press `F` to complete the quest.

## Expected Temporary Feedback

- Interacting with Mara shows a dialogue line.
- Accepting the quest shows an on-screen quest accepted message.
- Ability use shows an on-screen hit message.
- Enemies show red on-screen hit messages when they melee the player.
- Defeating starter enemies advances `StarterEnemyDefeated`.
- Defeated enemies hide and respawn after a short prototype delay.
- Completing the quest shows on-screen completion, XP, and reward item messages.

## Known Prototype Gaps

- The UI is still mostly temporary on-screen debug text.
- Enemy movement and attacks are deliberately simple prototype behavior.
- Loot popup and inventory presentation are not real widgets yet.
- Nameplates, target frame, quest tracker, and hotbar widgets still need visual UMG child widgets.
- Art is still greybox until Fab/Marketplace assets are imported.
