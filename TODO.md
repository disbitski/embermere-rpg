# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

## Start Here

- Restart Unreal before manual PIE if the editor has been open since the 2026-07-02 C++ build. The current tests pass under MCP, but the already-open editor still emits hot-reload no-world warnings from the transient target-presentation test path.
- Run these automation tests:
  - `Embermere.Combat.TargetSelectionPresentation`
  - `Embermere.Quests.CompletionRewards`
  - `Embermere.Rules.RaceClassMatrix`
  - `Embermere.UI.InventoryToggle`
- Manually verify the styled first-pass HUD in PIE:
  - player HP, mana, XP, health bar, and mana bar are visible;
  - target panel appears after `Tab` and shows target HP plus range state;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - all hotbar slots show stable labels for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`;
  - Mara interaction shows the dialogue panel;
  - inventory panel starts as `Empty` and shows the `I` toggle hint;
  - pressing `I` hides/shows the inventory panel;
  - quest completion/reward shows the loot popup and the inventory panel lists the reward item.
- Manually verify selected-target world readability in PIE:
  - `Tab` shows the selected enemy's overhead selected marker, name, HP, and gold segmented mesh target ring;
  - the nameplate changes toward red/orange as enemy HP falls;
  - switching targets clears the old enemy marker;
  - killing the selected enemy clears the target and marker.
- Manually verify the live control fix in PIE:
  - `W`/`S` should cancel autorun.
  - `Ctrl+M` should toggle mouse Y inversion and show a temporary message.
  - The fix now runs through the pawn/controller path known to receive live movement input, and the project builds successfully.
- Manually verify the new Mara marker in PIE:
  - Mara should show a temporary gold `!` and name marker above her.
- Run the editor smoke test in [Docs/PLAYTESTING.md](Docs/PLAYTESTING.md).
- If Codex needs editor access, start MCP inside Unreal:

```text
ModelContextProtocol.StartServer 8123
```

## Current Prototype Status

Embermere has a working greybox starter slice:

- player spawn in `L_Embermere_Prototype`;
- classic mouse camera behavior;
- `WASD`, `Q` autorun, tab target, and hotbar ability input;
- first quest giver, quest data, and reward item data;
- hostile starter enemies that aggro, chase, attack, die, and respawn;
- temporary on-screen feedback for targeting, combat, death, respawn, quest progress, XP, and rewards;
- temporary in-world interactable markers, including a gold quest marker for Mara;
- styled native HUD panels for player status, target, range state, quest progress, dialogue, loot, and hotbar labels;
- first-pass inventory HUD panel showing empty state, reward item stacks, richer first-item inspection text, and `I` show/hide toggle;
- first-pass selected-target overhead marker/name/HP presentation plus a gold segmented mesh target ring;
- automation coverage for the race/class matrix, quest completion rewards, selected-target presentation, and inventory toggle.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The biggest presentation gaps are now visual quality and asset replacement: the HUD, inventory panel, target marker, target ring, and nameplate are functional first-pass programmer art. The next meaningful step is a first Fab/Marketplace import pass, then replacing temporary mesh/text visuals with proper fantasy UI/billboard/decal treatment.

## Next Work

- Replace temporary selected-target text with better world readability:
  - verify the new selected marker, gold ring material, and HP-aware nameplate color in a clean editor restart;
  - replace the segmented engine-cube target ring with a real decal/mesh/material asset;
  - cleaner enemy nameplate/health readout treatment using a widget/billboard instead of text render.
- Improve inventory presentation:
  - verify the richer item details for reward inspection in PIE;
  - add selected item cycling once inventory has multiple reward stacks;
  - cleaner empty/reward states.
- Start the first asset import pass from [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md), beginning with free stylized environment/UI packs.
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
