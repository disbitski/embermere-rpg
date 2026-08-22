# Playtesting

This is the current smoke test for the Embermere prototype inside Unreal Editor.

## Before Pressing Play

1. Open `/Game/Maps/L_Embermere_Prototype`.
2. If Codex has just changed C++ while the editor is open, restart Unreal so the editor loads the newest module.
3. Confirm the map shows a blue atmospheric sky, readable ambient light, the
   varied moss/earth road surface, 53 grounded upright `FabPass_` actors plus
   23 original-art placements from the waystone, ember-lamp, road-signpost,
   road-gate, boundary-fence, boundary-stone, supply-chest, Fenwatch shelter,
   Mara's keeper, the Fenwatch quartermaster and armsmaster, the vendor stall,
   practice dummy, first closed cottage, open-front training workshop,
   roadside notice board, and four marsh-reed clusters, a
   navigable PlayerStart/Mara
   route, the dressed road, wilderness pocket, upgraded ruin, quest giver, and three original
   Marsh Prowlers.
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
9. Inspect `Embermere_SupplyChest_Vendor_01` beside the vendor table: the
   route-facing ember lock and timber planks should read from PlayerStart, all
   five shared materials should remain coherent, the feet should contact
   terrain, both authored body/lid boxes should be solid, and the chest should
   remain at its accepted `(-1740, -1180, 0)` transform. Straight `Q` autorun
   from spawn must pass the old contact point and leave the Mara route clear.
10. Inspect `Embermere_FenwatchShelter_Mara_01` behind Mara: all five shared
    materials should read as one family, four stone-footed timber supports
    should contact terrain and remain solid, and the open center and overhead
    roof should remain traversable. Mara's name and quest marker must read in
    front of it from PlayerStart, with the removed stone backdrop, market cover,
    and vendor/trainer cubes still absent.
11. Inspect Mara's rigged `Embermere_FenwatchKeeper_Mara_Presentation_01` from
    PlayerStart and at interaction distance. Her feet should contact the
    terrain; the moss cowl, timber-brown clothing, pale skirt, ember crest,
    staff, and satchel should form a readable Stylized Classic silhouette. The
    exact 3.6-second Idle should remain restrained with planted feet and staff.
    The wrapper should face the approach, own no collision or gameplay
    component, and leave the existing gold quest marker, name, `F` interaction,
    dialogue, and quest flow unobstructed on `Quest_Giver_Mara_Fenwatch`.
12. Inspect `Embermere_FenwatchQuartermaster_Vendor_01` beside the supply
    chest. The moss cap/wrap, beard, apron, ledger, pouches, keys, and ember
    accents should read as a stocky merchant silhouette distinct from Mara. Its
    feet must contact terrain; it must remain `NoCollision`, non-interactable,
    and clear of the spawn route, chest silhouette, Mara, and her marker/name.
    The saved actor uses `(-1530, -1190, 0)`, yaw `100`, unit scale, and the
    skeletal Idle lane of `AEmbermereNpcPresentationActor`; the reviewed
    static mesh remains its fallback.
13. Inspect `Embermere_FenwatchVendorStall_Quartermaster_01` behind the
    quartermaster at `(-1530, -1430, 0)`, yaw `180`. Its pale moss roof,
    timber frame, iron counter, stone feet, and ember crest should read as the
    same Fenwatch family. Confirm all four supports and the counter are solid,
    the customer-side service approach and east bypass remain clear, and the
    supply chest stays accessible. Press `F` at the unchanged quartermaster
    service and confirm Fenwatch Supplies opens. The replaced
    `FabPass_Village_Fence_01` must remain absent.
14. Inspect `Embermere_FenwatchArmsmaster_Trainer_01` near the quartermaster.
    The moss hood, timber tunic, pale guard tabard, ember crest, shield, and
    ember-tipped staff should form a distinct guard-trainer silhouette. Its
    feet must contact terrain; it must remain `NoCollision`, carry no
    interaction or progression component, and preserve the chest, Mara, and
    PlayerStart sightlines. The saved presentation and separate service use
    `(-1320, -920, 0)`, yaw `100`, and unit scale. The preferred visual lane
    uses `SK_EmbermereFenwatchArmsmaster_01` with the exact 3.2-second
    `A_EmbermereFenwatchArmsmaster_Idle`; the reviewed static mesh remains its
    fallback.
15. Inspect `Embermere_FenwatchPracticeDummy_TrainingYard_01` at
    `(-1120, -1120, 0)`, yaw `45`. Its timber target face should point toward
    the armsmaster, its stone/moss footing should contact terrain, and its iron
    bands plus ember bullseye should read from the training-yard approach. The
    authored base and torso/core must be solid, both outstretched arms must be
    clear, and the open village route must remain traversable. Confirm the
    generic `FabPass_Village_Crate_C` it replaced is absent. Confirm the
    colocated `Embermere_FenwatchPracticeTarget_Gameplay_01` is a separate
    native actor with no render mesh, collision, navigation, or trainer
    authority and remains fixed at the same transform.
16. Inspect `Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
    `(-690, -1030, 0)`, yaw `-100`. Its moss roof, stone-footed timber frame,
    iron braces, ember crest, rear tool wall, and workbench should read as a
    compact open-front training shed facing the practice dummy. Confirm both
    front supports, rear wall, and workbench are solid; the center and
    player-height bay remain clear; the roof, trim, crest, sword, spear, and
    tools add no collision; and both the armsmaster-to-dummy approach and
    road-side east bypass remain open. `FabPass_Village_Fence_03` must remain
    absent.
17. Inspect `Embermere_FenwatchCottage_West_01` at `(-2480, -260, 0)`, yaw
    `38`. The stone base, timber frame, moss roof, chimney, glowing windows,
    closed door, and ember crest should read as Fenwatch's first real dwelling
    without hiding Mara, her contextual greeting, the vendor approach, or the
    training yard. Confirm the full house body and doorstep are solid, roof and
    chimney details do not create invisible collision, the foundation is
    grounded, and both the direct PlayerStart-to-Mara route and west bypass
    remain clear. `FabPass_Village_Fence_02` must remain absent.
18. Inspect `Embermere_FenwatchNoticeBoard_Road_01` at
    `(-1560, -260, 0)`, yaw `-35`. Its grounded stone-footed timber frame,
    moss roof, iron trim, pinned notices, and ember crest should read from the
    normal village-road approach. Confirm both supports and the central panel
    are solid; roof, notices, pins, trim, and crest are clear; and the direct
    PlayerStart-to-Mara, village-to-road, quartermaster, and armsmaster routes
    remain open. The board must stay presentation-only with no quest marker,
    dialogue, interaction, reward, vendor, trainer, or persistence authority.
19. Inspect each marsh-reed cluster from the gameplay camera. Its low footprint
    should blend into the ground, reeds should add scale without hiding the
    route, and the whole cluster must remain `NoCollision`.
20. Confirm the suspended SoulCave canopy/pillar accents and three old enemy
    marker meshes have not returned. Foliage should use readable
    project-owned overrides rather than white/default rendering.

## Finding Mara

Mara is the quest giver actor named `Quest_Giver_Mara_Fenwatch`.

From the village spawn, move mostly straight ahead and slightly right toward the center of the starter village. In world coordinates, the player starts around `(-2400, -1200)` and Mara is nearby at `(-2050, -850)`, in front of the open-sided Fenwatch shelter.

In Play In Editor, Mara's project-owned rigged keeper is a colocated art-only
wrapper. The existing quest actor still owns the temporary gold `!`, name
marker, interaction, dialogue, quest state, and rewards.

The wrapper may also show a compact contextual greeting beneath that marker.
Verify it is hidden outside `420` cm, then shows the quest-owned gold `!`
available line, cyan `...` active line, green `?` ready line, and muted green
`+` completed line as the original quest changes state. The fixed `320x56`
panel must fit without hiding Mara's marker/name, shifting the HUD, or receiving
mouse input. Physical `F` must still open and mutate only the original
Blueprint-owned quest flow, and a second completed interaction must not replay
XP, copper, or the Recruit Pack.

## NPC Skeletal Idle Acceptance

Use this focused lane when accepting wrapper-based NPC art. The Fenwatch
armsmaster, quartermaster, and keeper are saved production fixtures with
distinct rigs, Idles, and reversible static fallbacks.

1. Start clean PIE on the current no-hot-reload module.
2. Confirm both skeletal lanes are visible, their static components are
   inactive, each exact mesh and Idle resolves, mode is `AnimationSingleNode`,
   and collision remains `NoCollision`.
3. Confirm runtime playback reports `playing=true` at the authored rate. Sample
   animation position twice far enough apart to prove it advances. Saved
   `AnimationData` by itself is not playback proof.
4. Inspect grounded feet, planted staff, ledger/merchant gestures, restrained
   torso/head/hand motion, material readability, service-marker clearance, and
   route spacing from the normal PlayerStart, vendor, and training-yard
   cameras.
5. Press `F` and complete vendor, trainer, and Mara dialogue/quest flows. The
   service and quest actors must still own their markers, data, transactions,
   dialogue, rewards, UI, and persistence handoffs; the wrappers must still own
   none of them.
6. Rerun all three fresh rig validators plus vendor/trainer and saved-map
   validators. Each wrapper must retain its exact rig/Idle and static soft-
   reference fallback.

The armsmaster acceptance measured `0.193888 -> 1.670905` seconds. The
quartermaster acceptance measured `0.853735 -> 2.195707` seconds. The fresh-
module keeper acceptance measured `0.333814 -> 1.525603` seconds. All remained
`playing=true` and `NoCollision`. All 52 tests, fresh package validators, and
initialized-world route traces passed. On 2026-08-14 a real `F` press opened
Mara's Blueprint-owned dialogue and accepted the quest, real Prowler combat
advanced it to `3/3`, and the same original interactable completed the return
for `125` XP, `20` copper, and one Recruit Pack while the rigged wrapper stayed
presentation-only.

On 2026-08-15 the contextual observer passed all four live states. The ready
and completed states were driven through the authoritative quest component,
physical `F` still completed the original interaction for the exact reward,
and a repeat `F` preserved `125` XP without reward replay.

## Character Creation Pre-Play Gate

1. Start clean PIE and confirm the normal HUD is hidden behind a centered
   `940x560` creation panel.
2. Confirm all eight races and all four starting classes remain visible.
3. Select Ranger, then Dwarf. Ranger must remain selected but visibly disabled;
   the exact invalid-combination message must appear and Confirm must stay
   disabled. Pressing Enter must not begin play.
4. Select Wizard, then Bullywug and repeat the same rejection proof. The UI
   must not silently replace Wizard with a legal class.
5. Select Elf Wizard and confirm with Enter. The modal must disappear, the HUD
   must return, and chat must report `Journey begun: Elf Wizard`.
6. Verify the accepted Elf Wizard starts at `80/80` health and `110/110` mana
   with Spark Bolt, Frost Root, Arcane Burst, and Meditate in the first four
   hotbar slots.
7. Confirm gameplay input and classic mouse capture return after acceptance.
   A second confirmation request must not reapply stats or reset the hotbar.
8. Repeat with at least one other legal pair when changing rules data. The
   picker must consume the rules asset rather than maintain a second allow-list.

Save version `2` persists only a deliberately confirmed race/class pair through
stable IDs. Each fresh prototype world still presents the picker first; a
confirmed Chronicle load may then atomically replace that temporary choice with
the saved identity, class stats, starter hotbar, equipment, and progression.
Version `1` slots remain loadable through the documented current-rules Human
Warrior fallback without rewriting the old file.

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
20. Watch for the selected enemy's UMG nameplate, selected marker, HP text, HP
    bar, complete cyan-blue emissive ground circle, and the HUD target panel
    range state. The 48-segment circle should size from the target's visual
    bounds, sit visually flush above the supporting surface, and leave the paws
    clear. Its effective `16` cm collision-hit clearance compensates for the
    prototype moss surface rendering above the traceable ground.
21. Inspect the Marsh Prowler at normal camera distance: peat/charcoal/moss/mud,
    bone, and amber accents should separate cleanly; all four paws should meet
    terrain; the capsule, target ring, and nameplate must remain readable.
22. Watch Idle before aggro, Walk or Run during movement, Attack during
    retaliation, Hit when damaged, and Death when defeated. The hidden/dead
    interval and respawn must return to a clean living locomotion state.
23. Inspect the starter hotbar before combat. The first four slots should show
    fixed `32x32` Warrior illustrations inside the unchanged `92x64` cells,
    with readable key/name text and no movement of empty or Interact slots.
    Hover each active slot and confirm its tooltip includes description, power,
    mana, meter-based range or Self, and cooldown. Repeat after selecting each
    starter class so all sixteen project-owned illustrations receive a viewport
    check.
24. Press `1` to use the first starter ability.
25. Press `1` again before the cooldown finishes and confirm the bottom-left log reports the ability ready time.
26. Confirm the cooling hotbar icon and text dim together while the live countdown remains in the fixed two-line label area.
27. As Warrior, press `4` for Battle Shout and confirm chat reports `+8 Attack
    Power` for 10 seconds. Confirm one beneficial status cell appears beneath
    player mana with the saved Battle Shout icon, name, live countdown, and
    hover description. Reapply it after cooldown and confirm the same cell
    refreshes instead of duplicating. After it expires, the cell should clear
    and damage should return to baseline. Eight restrained orange-gold world
    segments should appear around the player while the effect is active, then
    hide with the same authoritative status state.
28. As Cleric, press `3` for Ward and confirm chat reports `+10 Armor` for 10
    seconds; incoming damage should be reduced during the ward and its
    beneficial status cell plus blue-white world segments should clear when the
    effect expires.
29. As Ranger, press `2` for Snare and confirm it deals light damage, halves
    the target's movement for 6 seconds, and then restores normal movement.
    The selected-target panel should show the Snare icon, name, harmful color,
    live countdown, and hover description beneath target HP. Eight marsh-green
    inner segments should sit around the Prowler's grounded footprint without
    hiding its paws, nameplate, or outer cyan target circle. The accepted mesh
    and scale resolve to a `94.352` cm radius at `21` cm above the support hit.
30. As Ranger, press `4` for Nature's Focus and confirm the same shared timed
    Attack Power, player-status, and orange-gold world-aura contract as Battle
    Shout.
31. As Wizard, press `2` for Frost Root and confirm it deals light damage,
    stops target movement for 4 seconds, shows the same generic target-status
    treatment with Frost Root data plus a lighter ice-white cyan inner aura,
    clearly distinct from the calmer outer target circle, and then releases the
    target.
32. Spend mana, press Wizard `4` for Meditate, and confirm exactly 18 missing
    mana is restored without exceeding maximum mana. Because it is
    instantaneous, Meditate should not create a timed-status cell or world
    aura.
33. Confirm combat, target, quest, XP, inventory, mouse, cooldown, utility
    effect, and death/recovery messages remain clipped as single-line rows
    inside the bottom-left chat/combat log.
34. Expect one nearby enemy to chase and melee you when you enter its 525 cm aggro radius. The other two Prowlers should remain at their separated homes.
35. Confirm the selected enemy can cross the visual-only combat-pocket band
    without sticking. Pull it farther away and confirm it eventually leashes
    back toward its spawn instead of chasing indefinitely into the village.
36. Confirm an enemy respawn clears any old Snare or Frost Root state instead
    of preserving reduced movement into its next life.
37. Defeat a Marsh Prowler and confirm the chat reports one Marsh Tonic looted and inventory gains a stack. Repeated drops should increase the same stack up to its limit.
38. After taking damage, select Marsh Tonic and click `Use`; confirm it restores up to 25 health and 10 mana and consumes one tonic. At full health/mana, `Use` must be disabled and preserve the stack.
39. Defeat three starter enemies.
40. Return to Mara and press `F` to complete the quest. Confirm the temporary
    reward popup shows the Recruit Pack art in a fixed `32x32` cell beside
    readable reward text without changing the popup bounds.
41. With both Recruit Pack and Marsh Tonic in the bag, select one and click `Sort`; confirm armor appears before consumables, names sort alphabetically within a category, the same item stays selected, and chat reports `Inventory sorted`.
42. Begin dragging a bag row and confirm the Sort control becomes unavailable and the bag order stays fixed until the drag ends. Recheck one valid and one invalid equipment drop afterward.
43. Inspect the equipment column with empty slots and again with Recruit Pack
    equipped. Confirm the project-owned armored-adventurer backdrop stays
    centered behind the unchanged ten-slot grid, remains decorative and
    noninteractive, does not obscure slot icons or labels, and does not move
    the Equipment title, bonus text, footer, or hotbar.
44. For a bounded finite-world recovery check, move below the prototype
    foundation or use a PIE-only transform below `Z=-1000` while autorun is
    active. Confirm `You have fallen` appears, autorun stops, movement freezes,
    and the player returns to the village after five seconds at full health
    with walking restored, zero residual velocity, and three seconds of damage
    protection.

## Fenwatch Practice Target Loop

1. Start clean PIE, hide Inventory with `I`, and approach the visible practice
   dummy from the armsmaster side.
2. Press `Tab`. Confirm `Fenwatch Practice Target` appears in the HUD and
   native world nameplate with `150/150`, while the complete cyan target circle
   wraps the dummy without touching its supports or replacing its art.
3. Confirm the gameplay target remains at `(-1120, -1120, 0)` with zero
   gravity and `MOVE_None`. It must not move, aggro, retaliate, leash, block the
   player, or add navigation collision.
4. Use real hotbar abilities. Strike should update both health presentations
   and chat while retaining the ordinary cooldown contract. Root, Snare, and
   other class effects may be accepted for normal combat consistency, but they
   must not give the target AI or locomotion.
5. Defeat it. Confirm selection, nameplate, and cyan circle clear immediately;
   the separate visible dummy remains in place; and no loot, XP, copper,
   inventory stack, Mara objective progress, or trainer/service state changes.
6. Wait three seconds. Confirm the native target returns at `150/150` at the
   exact authored transform, then press `Tab` and reacquire it for another
   round.
7. Retain the accepted baseline proof: six Strikes dealt
   `28, 28, 28, 28, 28, 10`; player health stayed `100`, XP stayed `0`, bag
   stacks stayed `0`, and quest progress stayed `0` before and after reset.

The authority and reset contract is in
[PRACTICE_TARGET_CONTRACT.md](PRACTICE_TARGET_CONTRACT.md).

## Floating Combat Feedback Loop

1. In clean PIE, target the Fenwatch Practice Target and strike it from the
   normal training-yard camera. Confirm the exact applied damage appears in a
   fixed `112x32` cell beside, rather than over, the native nameplate.
2. Strike rapidly and confirm newest results remain readable without growing
   the layout beyond three cells; a fourth result evicts the oldest.
3. Confirm each result rises slightly, fades, and expires after `1.25` seconds
   while target HP and the bottom-left chat retain the durable combat facts.
4. Defeat the target. Confirm the number, nameplate, and cyan circle clear
   immediately, the visible dummy remains, and no stale number returns during
   the three-second reset or after reacquisition.
5. Repeat on a real Marsh Prowler from normal camera distance. Confirm the
   number coexists with the Prowler nameplate, cyan target circle, and status
   aura, then switch or clear targets and confirm immediate removal.
6. Retain the accepted reference geometry: the practice-target proof showed a
   `104x30` floating result with a `16` pixel nameplate gap; the Prowler proof
   showed the same fixed geometry while HP changed and chat remained intact.

The outcome/presentation authority boundary is in
[COMBAT_FEEDBACK_CONTRACT.md](COMBAT_FEEDBACK_CONTRACT.md).

## Fenwatch Vendor Loop

1. Start a fresh PIE session, hide Inventory with `I`, approach the
   quartermaster beside the supply chest, and press `F`.
2. Confirm `Fenwatch Supplies` opens with `Purse: 40 copper`, Marsh Tonic at
   `8 copper`, Recruit Pack at `30 copper | 1 left`, resolved icons, selected
   item details, Buy, Sell selected, latest Buyback, close, and no overlapping
   Inventory panel.
3. Buy Marsh Tonic. Confirm the purse becomes `32 copper`, Inventory gains one
   tonic, and identical readable success copy appears inside the fixed vendor
   result cell and bottom-left chat.
4. Sell the selected tonic. Confirm the purse becomes `35 copper`, the exact
   tonic leaves Inventory, and Buyback offers `Marsh Tonic x1 | 3 copper`.
5. Buy it back. Confirm the purse returns to `32 copper`, the same item identity
   returns to Inventory, and Buyback becomes empty.
6. Select and buy Recruit Pack. Confirm the purse becomes `2 copper`, Inventory
   gains the pack, its row reports `0 left`, and the row is disabled.
7. Select Marsh Tonic again. Confirm Buy is disabled and the fixed result cell
   reads `You do not have enough copper.` without crossing the footer.
8. Complete Mara's quest. Confirm the chat reports `Reward: 20 copper`, the
   purse becomes `22 copper`, XP and Recruit Pack rewards still arrive, and a
   repeated completion cannot pay again.
9. Click `X`. Confirm the panel disappears, normal game-only mouse input is
   restored, and movement/camera controls still work.
10. Retain the visual/service split: the visible quartermaster is non-colliding
   art; the co-located interaction marker and stock come from the invisible
   service actor. Neither should obstruct Mara, the chest, or traversal.

The full ownership and rollback contract is in
[VENDOR_SERVICE_CONTRACT.md](VENDOR_SERVICE_CONTRACT.md).

## Fenwatch Trainer Loop

1. Start a fresh PIE session, hide Inventory with `I`, approach the armsmaster
   near the quartermaster, and press `F`.
2. Confirm `Fenwatch Training` opens with `Purse: 40 copper`, one selected
   `Combat Drills` row, level 1 requirement, `10 copper`, `+25 XP`, description,
   Train, close, and no overlapping Inventory, Vendor, Chronicle, hotbar, or
   chat panel.
3. Train once. Confirm the purse becomes `30 copper`, player XP becomes `25`,
   and identical completion feedback appears in the fixed result cell and
   bottom-left chat.
4. Repeat until the purse reaches zero. Confirm the action disables, the panel
   reports insufficient copper, and no extra XP or negative currency appears.
5. Press `I` while the trainer is open. Confirm the trainer closes and Inventory
   opens in its place. Reopen the trainer and click Chronicle; confirm the same
   exclusive handoff.
6. Close the trainer with `X`. Confirm the panel disappears, game-only input
   returns, and movement/camera controls still work.
7. Retain the visual/service split: the armsmaster remains non-colliding art;
   the separate service actor owns the marker, `F` interaction, offering data,
   and transaction authority.
8. For the trainer persistence lane, begin again at `40` copper and `0` XP,
   train once to `30`/`25`, and save deliberately through Chronicle. Stop PIE,
   start a fresh world, confirm the normal `40`/`0` baseline, then load through
   Chronicle twice. Both loads must resolve to exactly `30`/`25` with no items,
   equipment, quest, vendor-stock, buyback, reward, or schema mutation.

The full ownership and rollback contract is in
[TRAINER_SERVICE_CONTRACT.md](TRAINER_SERVICE_CONTRACT.md).

## Prototype Save And Load

1. Complete the Fenwatch vendor sequence through the `22` copper quest state.
2. Equip one Recruit Pack in Back, leaving the quest reward pack and Marsh
   Tonic in the bag.
3. Press `M` or click `Chronicle`. Confirm the centered fixed panel opens with
   cursor input and shows the confirmed race/class, `22 copper | 125 XP`,
   `2 bag stacks | 1 equipped`, and `Quest complete` without covering the
   hotbar or chat.
4. Choose Save Journey. Because the acceptance slot already exists, confirm
   that `Confirm Overwrite` and Cancel appear; cancel once and verify no state
   changes, then confirm the save and its success feedback.
5. Close Chronicle with `M` or `X`. Confirm game-only input returns. Opening
   Inventory or Vendor must hide Chronicle, and opening Chronicle must hide
   either of those panels.
6. Stop PIE and start a second fresh PIE session. Confirm a different legal
   race/class and verify its class stats and hotbar, then confirm live objects
   begin at the normal 40-copper, empty-bag, stock-one baseline before loading.
7. Open Chronicle, choose Load Journey, and verify `Confirm Load` appears before
   any mutation. Confirm it and verify exact restoration: `22` copper, `125` XP,
   the saved race/class, its class base stats and starter hotbar, one tonic, one
   bagged Recruit Pack, one Recruit Pack equipped in Back, correctly derived
   equipped HP/Armor, completed quest progress, and Recruit Pack vendor stock
   at zero.
8. Confirm buyback history is empty after load and the completed quest cannot
   award XP, copper, or another item again.
9. Load through Chronicle a second time. Confirm identity, class stats, hotbar,
   quantities, XP, copper, and equipment bonuses remain unchanged rather than
   drifting or doubling.
10. Confirm an empty or rejected slot disables Load and displays readable
    missing, malformed, missing-asset, or unsupported-version feedback without
    partially changing the current session.
11. `EmbermereSave` and `EmbermereLoad` remain equivalent console fallbacks for
    debug and automated acceptance.
12. For the tracked two-session acceptance lane, run `prepare_and_save()` and
   `load_and_validate()` from `Scripts/validate_persistence_live_unreal.py` as
   documented in [SAVE_GAME_CONTRACT.md](SAVE_GAME_CONTRACT.md).
13. For the trainer-only lane, use
    `prepare_trainer_progression_for_chronicle()`, begin a fresh PIE world,
    verify it with `validate_fresh_trainer_session_before_chronicle_load()`,
    then validate each confirmed Chronicle load with
    `validate_trainer_chronicle_load()`.

## Expected Temporary Feedback

- A styled first-pass HUD overlay shows player HP, mana, XP, current target, target HP, range state, quest progress, and all hotbar slots.
- A 700x330 structured inventory/equipment window appears in the top-right with `Slots X / 24`, an explicit category/name Sort control, clickable/draggable highlighted item rows, fixed project-owned item/slot icons and fantasy drag token, selected-item effects and net equipment comparison, row/occupied-slot tooltips, ten stable clickable/drop-target paper-doll slots layered over a restrained illustrated adventurer, gold/red drag feedback, equipment-to-bag return, aggregate bonuses, description, empty/reward state, `[`/`]` cycling, `I` close/show behavior, and Equip or Use actions when supported.
- Opening the inventory shows the cursor and permits UI clicks; closing it hides the cursor and restores classic game-only mouse input.
- Interacting with Mara shows a temporary bottom-screen dialogue panel.
- Interacting with the Fenwatch quartermaster shows a fixed native stock panel
  with purse, wares, prices, finite stock, details, Buy, Sell selected, latest
  Buyback, transaction state, and chat feedback.
- Interacting with the Fenwatch armsmaster shows a fixed native training panel
  with purse, data-driven offerings, requirements, costs, XP rewards, Train,
  rejection state, and clean peer-panel handoff.
- Pressing `M` or the top-center Chronicle command opens the fixed one-slot
  Save/Load panel with read-only race/class and progression summary, deliberate
  confirmations, cancel/close controls, readable rejection feedback, and clean
  cursor/input handoff.
- Mara has a temporary gold quest marker above her in PIE.
- `Q` toggles autorun, and manual `W`/`S` forward/back input cancels it.
- `Ctrl+M` toggles mouse Y inversion and posts a bottom-left chat/combat log message.
- Accepting the quest posts a bottom-left quest accepted message.
- Tab targeting shows the selected target name, HP, first-ability range state,
  a temporary screen-space UMG nameplate, HP-aware accent/health bar coloring,
  and a restrained cyan-blue emissive ground circle around the target's
  footprint.
- Ability use posts a bottom-left hit message clipped inside the shaded chat panel.
- Successful damage also publishes one immutable post-commit result to a
  hit-test-invisible, fixed three-entry floating observer. Exact damage appears
  briefly beside the selected target nameplate, never replaces chat or target
  HP, and clears on expiry, target switch, defeat, reset, or teardown.
- Reusing an ability before its cooldown finishes posts a bottom-left ready-time message; the hotbar slot itself dims and counts down while cooling.
- Battle Shout and Nature's Focus post timed Attack Power feedback; Ward posts
  timed Armor feedback; Snare and Frost Root post movement-control feedback;
  Meditate reports actual mana restored.
- Timed buffs appear in fixed cells beneath player mana, while Snare and Frost
  Root appear in fixed cells beneath selected-target health. Each cell reuses
  saved ability art and data for its name, countdown, and hover description;
  duplicate applications refresh, and expiration/respawn/target clearing
  removes stale presentation without moving neighboring HUD panels.
- Timed effects also drive eight fixed non-colliding world segments from the
  same read-only snapshots: Attack Power is orange-gold, Armor blue-white,
  Snare marsh green, and Frost Root ice-white cyan. Harmful effects scale from
  the transformed creature footprint, trace the supporting surface, take visual
  priority, and hide on empty/dead/cleared state without changing gameplay.
- Enemies show red bottom-left combat log messages clipped inside the shaded chat panel when they melee the player.
- Marsh Prowlers use the project-owned skeletal mesh and route Idle, Walk, Run,
  Attack, Hit, and Death animations from generic enemy state. The same combat
  capsule, loot, leash, target, quest, and respawn rules remain authoritative.
- Enemies leash and return home if pulled too far from their spawn area.
- If the player dies or crosses the `Z=-1000` recovery plane, autorun turns
  off, a bottom-left death message appears, movement freezes, and the player
  respawns after a short delay with zero residual velocity and a brief
  damage-protection message.
- Defeating starter enemies advances `StarterEnemyDefeated`.
- Defeating a Marsh Prowler grants one stackable Marsh Tonic and reports it in chat.
- Defeated enemies hide and respawn after a short prototype delay.
- Completing the quest shows bottom-left completion/XP and `20` copper reward
  messages, a temporary loot/reward popup with fixed item art, and the reward
  item in the inventory panel. Repeated completion must not pay twice.

## Known Prototype Gaps

- The UI has native first-pass HUD panels, but styling is still programmer-art and needs a proper fantasy skin.
- The hotbar has sixteen project-owned starter-ability illustrations, fixed
  art cells, synchronized cooldown dimming, accessible data-driven tooltips,
  and functional first-pass timed root, snare, buff, and mana-recovery rules.
  Timed buffs and selected-target controls now have data-driven first-pass
  status cells with live countdowns and hover descriptions. The player and
  target rows passed clean-PIE visual acceptance on 2026-07-27, and the first
  presentation-only eight-segment world aura passed beneficial and harmful
  visual checks by 2026-07-31. Harmful auras now scale from transformed creature
  bounds and trace the support surface; the accepted Prowler result is a
  `94.352` cm inner radius at `21` cm above the hit, distinct from the outer
  target circle. Final fantasy styling, authored Niagara/class-specific effects,
  and audio remain open.
- Enemy movement and attacks are deliberately simple prototype behavior. The
  first solo-pull pass now uses three WorldStatic-cleared homes, a 525 cm aggro
  radius, visual-only pocket/marker collision, 6 damage, and a 2 second attack
  cadence; leash and respawn feel still need normal-route playtesting.
- Player respawn is a simple prototype reset to the spawn point with short
  damage protection, not a full corpse run or revive system. It now also owns
  the finite-world recovery contract: crossing below `Z=-1000` forces death,
  cancels autorun, and restores walking with cleared velocity.
- Inventory presentation now has clickable and draggable rows, fixed
  project-owned item/equipment icons and fantasy drag token, category/missing-art
  fallbacks, stable identity-preserving category/name sorting, keyboard
  selection, tooltips, net equipment comparison, clickable/drop-target slots,
  atomic transfers, rollback-safe replacement, stat application, armor
  mitigation, safe consumable depletion, and a real Marsh Tonic loot source. It
  now reuses resolved item art in drag and reward feedback and layers a
  project-owned paper-doll illustration beneath the unchanged slot controls.
  The empty-state composition passed clean PIE on 2026-07-26 and occupied
  Recruit Pack contrast passed on 2026-07-27; the populated drag token in
  motion remains a manual check. The next phases are documented in
  `Docs/INVENTORY_INTERACTION_PLAN.md`.
- The first vendor supports exact one-at-a-time buying, finite/unlimited stock,
  data-driven one-at-a-time selling, bounded latest-item buyback, full-bag/
  affordability/wallet rollback, and native feedback. Mara's first quest now
  awards copper. Versioned prototype persistence now keeps confirmed race/class,
  copper, XP, exact bag/equipment identity, quest progression, and finite stock
  across sessions; version `1` remains loadable as Human Warrior and buyback
  remains session-only by policy. Chronicle now supplies the deliberate
  one-slot lifecycle and confirmations; quantity entry, reputation pricing,
  autosave, deletion, profiles, and migrations remain open. See
  `Docs/VENDOR_SERVICE_CONTRACT.md` and `Docs/SAVE_GAME_CONTRACT.md`.
- Nameplates use a native UMG widget component and target highlighting uses a
  dedicated emissive material on a non-colliding 48-segment circle. Runtime
  bounds sizing and a downward surface trace keep it clear of both paws and
  raised ground; a future texture treatment can add restrained runes or softer
  edges without changing targeting rules.
- The bottom-left chat/combat log is clipped inside its shaded panel and currently uses single-line rows; final chat history, scrolling, and fantasy styling still need a proper UI pass.
- If Codex changed C++ during the same editor session, restart Unreal before validating interface-heavy tests or new target-presentation behavior.
- The first local Fab/Epic environment pass is grounded and spawn-safe after
  removing three oversized sci-fi shells, unsupported ruin accents, and
  redundant enemy markers. Its atmosphere, moss/earth path, project-owned
  foliage overrides, and sparse reeds are accepted as a prototype baseline,
  but style cohesion and real fantasy village architecture still need work.
- Some KiteDemo source meshes retain missing internal vendor
  material/texture references in fresh commandlet logs. The visible component
  overrides are intentional temporary presentation; they are not a repaired
  vendor dependency graph.
- The original gate/fence/end-stone family is collision-validated and keeps the
  road opening clear. The two former masking trees now sit farther into the
  south foliage band, and exact validator assertions preserve that accepted
  composition without moving the boundary geometry.
- The original supply chest replaces one temporary vendor crate stack and is
  saved with two authored collision boxes. Its route-facing presentation and
  clear Mara path passed the first editor inspection; retain that orientation
  when the village composition changes.
- Raw Fab/Epic asset folders are local-only and ignored by Git, so missing local imports will make the art layer show missing references.
