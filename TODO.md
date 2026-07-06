# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

## Start Here

- Restart Unreal before manual PIE if the editor has been open since the 2026-07-05 C++ build or the 2026-07-05 Fab/Epic environment placement pass. The current source builds and headless automation passes, but a clean editor restart is still the reliable path for visual UI and map-art checks after C++ HUD/inventory work or large asset imports.
- Confirm the local Fab/Epic folders are present but ignored by Git:
  - `Content/KiteDemo/`
  - `Content/SoulCave/`
  - `Content/ParagonProps/`
  - `Content/Scifi_desert_city/`
- Run these automation tests:
  - `Embermere.Combat.TargetSelectionPresentation`
  - `Embermere.Quests.CompletionRewards`
  - `Embermere.Rules.RaceClassMatrix`
  - `Embermere.UI.ChatLog`
  - `Embermere.UI.EnemyNameplateWidget`
  - `Embermere.UI.InventoryToggle`
- Manually verify the styled first-pass HUD in PIE:
  - player HP, mana, XP, health bar, and mana bar are visible;
  - target panel appears after `Tab` and shows target HP plus range state;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - all hotbar slots show stable labels for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`;
  - Mara interaction shows the dialogue panel;
  - inventory panel starts as `Empty` and shows the `I` toggle hint;
  - pressing `I` hides/shows the inventory panel;
  - pressing `[` and `]` cycles the inspected inventory stack after multiple stacks exist;
  - quest completion/reward shows the loot popup and the inventory panel lists the reward item;
  - combat, target, quest, XP, inventory, mouse, and death/recovery messages appear clipped inside the bottom-left chat/combat log instead of overlapping the top-left player status panel or spilling beyond the chat panel border.
- Manually verify selected-target world readability in PIE:
  - `Tab` shows the selected enemy's UMG nameplate, selected marker, HP text, HP bar, and gold segmented mesh target ring;
  - the nameplate accent/HP bar changes toward red/orange as enemy HP falls;
  - switching targets clears the old enemy marker;
  - killing the selected enemy clears the target and marker.
- Manually verify the live control fix in PIE:
  - `W`/`S` should cancel autorun.
  - `Ctrl+M` should toggle mouse Y inversion and show a temporary message.
  - The fix now runs through the pawn/controller path known to receive live movement input, and the project builds successfully.
- Manually verify the new Mara marker in PIE:
  - Mara should show a temporary gold `!` and name marker above her.
- Manually verify the first `FabPass_` environment layer in PIE:
  - village props leave Mara, PlayerStart, and the quest interaction route readable;
  - the road dressing points naturally from the village toward the wilderness pocket;
  - foliage and rocks frame the enemy pocket without hiding enemies, target rings, or nameplates;
  - the upgraded ruin reads as a landmark and does not trap the player;
  - any sci-fi-kit props that read too futuristic are noted for replacement.
- Run the editor smoke test in [Docs/PLAYTESTING.md](Docs/PLAYTESTING.md).
- If Codex needs editor access, start MCP inside Unreal:

```text
ModelContextProtocol.StartServer 8123
```

## Current Prototype Status

Embermere has a working first-pass starter slice:

- player spawn in `L_Embermere_Prototype`;
- classic mouse camera behavior;
- `WASD`, `Q` autorun, tab target, and hotbar ability input;
- first quest giver, quest data, and reward item data;
- hostile starter enemies that aggro, chase, attack, die, and respawn;
- bottom-left chat/combat log feedback for targeting, combat, death, respawn, quest progress, XP, inventory, and rewards;
- temporary in-world interactable markers, including a gold quest marker for Mara;
- styled native HUD panels for player status, target, range state, quest progress, dialogue, loot, and hotbar labels;
- first-pass inventory HUD panel showing empty state, reward item stacks, richer item inspection text, `[`/`]` inspection cycling, and `I` show/hide toggle;
- first-pass selected-target UMG nameplate widget plus a gold segmented mesh target ring;
- first local Fab/Epic environment pass over the village, road, wilderness pocket, and ruin landmark;
- automation coverage for the race/class matrix, quest completion rewards, selected-target presentation, enemy nameplate widget, chat log, and inventory toggle.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The first local environment art pass is in place, but it needs manual PIE review for collision, scale, route readability, and style cohesion. The HUD, inventory panel, target ring, and nameplate are still functional first-pass programmer art.

## Next Work

- Replace temporary selected-target text with better world readability:
  - manually verify the new UMG nameplate widget, selected marker, HP bar, and HP-aware accent color in PIE;
  - replace the segmented engine-cube target ring with a real decal/mesh/material asset;
  - tune screen-space widget size/height against camera distance.
- Improve inventory presentation:
  - verify the richer item details for reward inspection in PIE;
  - manually verify bracket-key item cycling once inventory has multiple reward stacks;
  - cleaner empty/reward states.
- Clean up WIP HUD layout issues:
  - manually verify the 2026-07-04 chat clipping fix in PIE after a clean editor restart;
  - continue tuning chat panel height/line count against the hotbar and common desktop viewport sizes.
- Manually review the first Fab/Epic environment pass in PIE:
  - confirm the `FabPass_` village shells and props do not block Mara or the player spawn;
  - confirm the dressed road still leads clearly from spawn to Mara and the enemy pocket;
  - confirm the wilderness foliage/rocks do not hide starter enemies or target nameplates;
  - confirm the upgraded ruin reads better than the old greybox and does not trap the player;
  - identify any sci-fi props that look too off-theme for the starter village.
- Replace placeholder sci-fi village shells with a better stylized fantasy village kit once a suitable UE-compatible pack is found.
- Tune starter enemy aggro, movement speed, attack range, damage, and respawn timing after in-editor playtesting.
- Tune player respawn and recovery rules after in-editor playtesting.
- Keep automation coverage growing around death/respawn, targeting, and hotbar behavior.

## Last Completed

- 2026-07-02: built successfully after target/inventory polish.
- Replaced the loud overhead `TARGET` text with a smaller selected marker and routed target nameplate text/color through reusable C++ helpers.
- Swapped the target ring off the engine debug material and onto a gold-tinted basic material path.
- Added automation assertions for selected-target presentation text and low-health color behavior.
- Expanded inventory reward inspection text to show the inspected item name and stack count.
- Ran the four-test suite after build through MCP: 4 passed, 0 failed. The already-open editor still emitted hot-reload no-world warnings from the transient target test; restart Unreal before treating warning absence as authoritative.
- Started and stopped PIE through MCP after the build. PIE started successfully, but full visual verification should be done after a clean editor restart because this run followed a live C++ build.
- 2026-07-03: added a native `UEmbermereEnemyNameplateWidget` and mounted it on enemies through a screen-space `UWidgetComponent`.
- The legacy text-render nameplate/marker remains as fallback, but selected enemies now use UMG for name, selected marker, HP text, and HP bar.
- Added `Embermere.UI.EnemyNameplateWidget`; MCP automation suite now runs 5 tests, all passing with 0 warnings.
- Built successfully after the nameplate widget work and booted/stopped PIE through MCP with no current Embermere gameplay or Blueprint errors.
- Direct unauthenticated Fab search/API access is Cloudflare-gated from terminal automation; first asset import should happen through the signed-in Unreal Fab window or Epic Launcher.
- 2026-07-03: moved gameplay feedback out of Unreal's top-left debug overlay and into a native bottom-left chat/combat log.
- Routed player, target, combat, quest, XP, inventory reward, dialogue, enemy attack, death, and respawn messages through the HUD log.
- Added `Embermere.UI.ChatLog`; headless automation now runs 6 tests, all passing with 0 warnings.
- 2026-07-04: tightened the bottom-left chat/combat log so its text is clipped inside the shaded panel.
- Reduced visible chat history to the configured message limit, set a fixed wrap width, and added explicit widget clipping to the panel, stack, and text rows.
- Built successfully and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- No local `/Game/ThirdParty/Fab` import was present yet, so the first asset pass still needs the signed-in Unreal Fab/Epic Launcher import step before Codex can wire assets into the level.
- 2026-07-05: added bracket-key inventory inspection cycling for multi-stack inventories.
- The top-right inventory panel now marks the selected stack, shows `Inspecting X/Y`, and uses `[`/`]` to wrap between stacks.
- Expanded `Embermere.UI.InventoryToggle` coverage to verify inventory selection advances and wraps in both directions.
- Built successfully and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- 2026-07-05: added the first local Fab/Epic environment pass for the starter zone.
- Ignored raw local marketplace asset folders in Git so the public repo does not redistribute vendor content.
- Added reusable placement and validation scripts for the tagged `FabPass_` art layer.
- Saved `L_Embermere_Prototype` with 68 created `FabPass_` actors after removing the old visual-only greybox village, road, and ruin markers.
- Built successfully, passed the headless FabPass map validator, and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- Manual PIE verification is still needed after restarting Unreal because MCP dropped during the first heavy asset compile pass.

## Asset Hunt

Start with free Unreal/Fab assets:

- stylized modular fantasy village kit;
- stylized forest/foliage pack;
- ruins/stone props;
- fantasy UI/icons;
- simple fantasy enemy or creature pack;
- weapon pack for sword, shield, staff, and bow.

Track candidates and install notes in [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md).

## Daily Wrap

Before stopping each day:

- build the project;
- run available automation tests;
- note anything requiring manual PIE verification;
- commit and push intentional changes;
- update this file with the next starting point.
