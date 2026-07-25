# Playtesting

This is the current smoke test for the Embermere prototype inside Unreal Editor.

## Before Pressing Play

1. Open `/Game/Maps/L_Embermere_Prototype`.
2. If Codex has just changed C++ while the editor is open, restart Unreal so the editor loads the newest module.
3. Confirm the map shows a blue atmospheric sky, readable ambient light, muted moss ground instead of a white foundation, 62 upright `FabPass_` actors plus nine original-art placements from the waystone, ember-lamp, road-signpost, road-gate, boundary-fence, and boundary-stone family, an unobstructed PlayerStart/Mara route, the dressed road, wilderness pocket, upgraded ruin, quest giver, and three starter enemies.
4. Inspect `Embermere_EmberLamp_Mara_01` and `Embermere_EmberLamp_Road_01`: pale stone/moss base, dark iron cage, warm ember crystal, terrain contact, clear route placement, and simple collision that does not trap the player.
5. Inspect `Embermere_RoadSignpost_01`: warm timber, stone/moss footing,
   iron bands, three ember route marks, terrain contact, clear route placement,
   and base/post collision without blocking beneath the two sign arms.
6. Inspect `Embermere_RoadGate_01`: stone/moss footings, timber span, iron
   bands, ember crest, terrain contact, clear road framing, four solid support
   boxes, and an unobstructed 250 cm center opening beneath the overhead beam.
7. Inspect `Embermere_BoundaryFence_GateSouth_01` and
   `Embermere_BoundaryFence_GateNorth_01`: low stone/moss supports, crossed
   timber rails, iron bands, ember diamonds, terrain contact, solid boundary
   collision, and no intrusion into the gate's three clear center lanes.
8. Inspect `Embermere_BoundaryStone_GateSouth_01` and
   `Embermere_BoundaryStone_GateNorth_01`: rune-topped timber/stone silhouettes,
   moss and iron accents, two solid authored boxes each, terrain contact, and
   clean fence termination. Confirm the relocated south-side trees preserve a
   foliage backdrop without masking the fence or gate opening.

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
10. Inspect the icon pass before and after earning items: all ten empty
    equipment slots show distinct fixed symbols; Recruit Pack and Marsh Tonic
    show project-owned art in their `18x18` rows, selected `42x42` detail header,
    and occupied slot where applicable. Confirm the exact `Recruit Pack` label
    fits its row and detail heading without clipping or crowding `Equipment`,
    labels/tooltips remain readable, no panel geometry shifts, and a test item
    without direct art uses a category fallback rather than an empty box.
11. After earning Mara's Recruit Pack reward, select it and confirm it identifies as level-1 Back armor with `+5 HP, +1 Armor`. Confirm its detail pane compares it with the empty Back slot, then hover its row and verify the tooltip includes quantity, armor/slot/level, effects, comparison, and description.
12. Drag Recruit Pack onto Back; confirm the matching slot highlights gold, the item leaves the bag, aggregate bonuses update, HP becomes `105/105`, and chat reports the action. Dragging over a wrong slot should show a restrained red state and dropping there must change nothing.
13. Confirm the drag visual is a fixed fantasy token rather than a plain text
    label: a fixed `44x44` Recruit Pack illustration, warm item name,
    `Back | Level 1` context, and no overlap with the inventory footer or
    hotbar. Repeat with Marsh Tonic and confirm its illustration/effect context
    remains readable. An item without resolved art should retain the compact
    category-sigil fallback in the same cell.
14. Drag the occupied Back slot onto the bag list; confirm the item returns once and stats/slot state reset without a fake loot popup. Repeat the equip/unequip flow with row click/action button and slot click to confirm the non-drag fallback remains intact.
15. With every bag slot occupied, confirm clicking or dragging an equipped slot toward the bag refuses to unequip and posts an inventory-full message without losing or duplicating the item.
16. Walk near Mara Fenwatch in the village and press `F`.
17. Watch for the temporary quest/dialogue message.
18. Move toward the ruin and enemy pocket, passing cleanly through the road gate between the two matching boundary fences and their rune-topped end stones.
19. Press `Tab` to target a nearby hostile.
20. Watch for the selected enemy's UMG nameplate, selected marker, HP text, HP bar, flat animated 24-segment emissive gold ground ring, and the HUD target panel range state.
21. Inspect the starter hotbar before combat. The first four slots should show
    fixed `32x32` Warrior illustrations inside the unchanged `92x64` cells,
    with readable key/name text and no movement of empty or Interact slots.
    Hover each active slot and confirm its tooltip includes description, power,
    mana, meter-based range or Self, and cooldown. Repeat after selecting each
    starter class so all sixteen project-owned illustrations receive a viewport
    check.
22. Press `1` to use the first starter ability.
23. Press `1` again before the cooldown finishes and confirm the bottom-left log reports the ability ready time.
24. Confirm the cooling hotbar icon and text dim together while the live countdown remains in the fixed two-line label area.
25. As Warrior, press `4` for Battle Shout and confirm chat reports `+8 Attack
    Power` for 10 seconds. After it expires, damage should return to baseline.
26. As Cleric, press `3` for Ward and confirm chat reports `+10 Armor` for 10
    seconds; incoming damage should be reduced during the ward.
27. As Ranger, press `2` for Snare and confirm it deals light damage, halves
    the target's movement for 6 seconds, and then restores normal movement.
28. As Ranger, press `4` for Nature's Focus and confirm the same shared timed
    Attack Power contract as Battle Shout.
29. As Wizard, press `2` for Frost Root and confirm it deals light damage,
    stops target movement for 4 seconds, and then releases the target.
30. Spend mana, press Wizard `4` for Meditate, and confirm exactly 18 missing
    mana is restored without exceeding maximum mana.
31. Confirm combat, target, quest, XP, inventory, mouse, cooldown, utility
    effect, and death/recovery messages remain clipped as single-line rows
    inside the bottom-left chat/combat log.
32. Expect one nearby enemy to chase and melee you when you enter its 525 cm aggro radius. The other two Prowlers should remain at their separated homes.
33. Confirm the selected enemy can cross the visual-only combat-pocket band and visual cone marker without sticking. Pull it farther away and confirm it eventually leashes back toward its spawn instead of chasing indefinitely into the village.
34. Confirm an enemy respawn clears any old Snare or Frost Root state instead
    of preserving reduced movement into its next life.
35. Defeat a Marsh Prowler and confirm the chat reports one Marsh Tonic looted and inventory gains a stack. Repeated drops should increase the same stack up to its limit.
36. After taking damage, select Marsh Tonic and click `Use`; confirm it restores up to 25 health and 10 mana and consumes one tonic. At full health/mana, `Use` must be disabled and preserve the stack.
37. Defeat three starter enemies.
38. Return to Mara and press `F` to complete the quest. Confirm the temporary
    reward popup shows the Recruit Pack art in a fixed `32x32` cell beside
    readable reward text without changing the popup bounds.
39. With both Recruit Pack and Marsh Tonic in the bag, select one and click `Sort`; confirm armor appears before consumables, names sort alphabetically within a category, the same item stays selected, and chat reports `Inventory sorted`.
40. Begin dragging a bag row and confirm the Sort control becomes unavailable and the bag order stays fixed until the drag ends. Recheck one valid and one invalid equipment drop afterward.
41. Inspect the equipment column with empty slots and again with Recruit Pack
    equipped. Confirm the project-owned armored-adventurer backdrop stays
    centered behind the unchanged ten-slot grid, remains decorative and
    noninteractive, does not obscure slot icons or labels, and does not move
    the Equipment title, bonus text, footer, or hotbar.

## Expected Temporary Feedback

- A styled first-pass HUD overlay shows player HP, mana, XP, current target, target HP, range state, quest progress, and all hotbar slots.
- A 700x330 structured inventory/equipment window appears in the top-right with `Slots X / 24`, an explicit category/name Sort control, clickable/draggable highlighted item rows, fixed project-owned item/slot icons and fantasy drag token, selected-item effects and net equipment comparison, row/occupied-slot tooltips, ten stable clickable/drop-target paper-doll slots layered over a restrained illustrated adventurer, gold/red drag feedback, equipment-to-bag return, aggregate bonuses, description, empty/reward state, `[`/`]` cycling, `I` close/show behavior, and Equip or Use actions when supported.
- Opening the inventory shows the cursor and permits UI clicks; closing it hides the cursor and restores classic game-only mouse input.
- Interacting with Mara shows a temporary bottom-screen dialogue panel.
- Mara has a temporary gold quest marker above her in PIE.
- `Q` toggles autorun, and manual `W`/`S` forward/back input cancels it.
- `Ctrl+M` toggles mouse Y inversion and posts a bottom-left chat/combat log message.
- Accepting the quest posts a bottom-left quest accepted message.
- Tab targeting shows the selected target name, HP, first-ability range state, a temporary screen-space UMG nameplate, HP-aware accent/health bar coloring, and a flat animated 24-segment gold ground ring.
- Ability use posts a bottom-left hit message clipped inside the shaded chat panel.
- Reusing an ability before its cooldown finishes posts a bottom-left ready-time message; the hotbar slot itself dims and counts down while cooling.
- Battle Shout and Nature's Focus post timed Attack Power feedback; Ward posts
  timed Armor feedback; Snare and Frost Root post movement-control feedback;
  Meditate reports actual mana restored.
- Enemies show red bottom-left combat log messages clipped inside the shaded chat panel when they melee the player.
- Enemies leash and return home if pulled too far from their spawn area.
- If the player dies, autorun turns off, a bottom-left death message appears, and the player respawns after a short delay with a brief damage-protection message.
- Defeating starter enemies advances `StarterEnemyDefeated`.
- Defeating a Marsh Prowler grants one stackable Marsh Tonic and reports it in chat.
- Defeated enemies hide and respawn after a short prototype delay.
- Completing the quest shows bottom-left completion/XP messages, a temporary
  loot/reward popup with fixed item art, and the reward item in the inventory
  panel.

## Known Prototype Gaps

- The UI has native first-pass HUD panels, but styling is still programmer-art and needs a proper fantasy skin.
- The hotbar has sixteen project-owned starter-ability illustrations, fixed
  art cells, synchronized cooldown dimming, accessible data-driven tooltips,
  and functional first-pass timed root, snare, buff, and mana-recovery rules.
  It does not yet show persistent buff/debuff status icons or class-specific
  VFX/audio.
- Enemy movement and attacks are deliberately simple prototype behavior. The
  first solo-pull pass now uses three WorldStatic-cleared homes, a 525 cm aggro
  radius, visual-only pocket/marker collision, 6 damage, and a 2 second attack
  cadence; leash and respawn feel still need normal-route playtesting.
- Player respawn is a simple prototype reset to the spawn point with short damage protection, not a full corpse run or revive system.
- Inventory presentation now has clickable and draggable rows, fixed
  project-owned item/equipment icons and fantasy drag token, category/missing-art
  fallbacks, stable identity-preserving category/name sorting, keyboard
  selection, tooltips, net equipment comparison, clickable/drop-target slots,
  atomic transfers, rollback-safe replacement, stat application, armor
  mitigation, safe consumable depletion, and a real Marsh Tonic loot source. It
  now reuses resolved item art in drag and reward feedback and layers a
  project-owned paper-doll illustration beneath the unchanged slot controls.
  That new decorative layer still needs clean-PIE visual acceptance; the next
  phases are documented in `Docs/INVENTORY_INTERACTION_PLAN.md`.
- Nameplates use a native UMG widget component and target highlighting uses a dedicated emissive material on a flat animated 24-segment ring; a future decal/texture treatment can add runes and softer edges.
- The bottom-left chat/combat log is clipped inside its shaded panel and currently uses single-line rows; final chat history, scrolling, and fantasy styling still need a proper UI pass.
- If Codex changed C++ during the same editor session, restart Unreal before validating interface-heavy tests or new target-presentation behavior.
- The first local Fab/Epic environment pass is upright and spawn-safe after removing three oversized sci-fi shells. Its first atmosphere/ambient-light correction is in place, but collision, scale, route readability, final lighting balance, and style cohesion still need manual PIE review.
- The original gate/fence/end-stone family is collision-validated and keeps the
  road opening clear. The two former masking trees now sit farther into the
  south foliage band, and exact validator assertions preserve that accepted
  composition without moving the boundary geometry.
- Raw Fab/Epic asset folders are local-only and ignored by Git, so missing local imports will make the art layer show missing references.
