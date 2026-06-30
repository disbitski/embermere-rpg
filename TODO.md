# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

## Start Here

- Restart Unreal before manual PIE. The 2026-06-30 work changed enemy components and C++ automation tests; the already-running editor can keep stale hot-reload registrations.
- After restart, run these automation tests:
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
  - `Tab` shows the selected enemy's overhead `TARGET` marker, name, HP, and ground target ring;
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
- first-pass inventory HUD panel showing empty state, reward item stacks, and `I` show/hide toggle;
- first-pass selected-target overhead marker/name/HP presentation plus a prototype ground target ring;
- automation coverage for the race/class matrix, quest completion rewards, selected-target presentation, and inventory toggle.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The biggest presentation gaps are now visual quality and asset replacement: the HUD, inventory panel, target marker, ground ring, and nameplate are functional programmer-art. The next meaningful step is a first Fab/Marketplace import pass, then replacing temporary/debug visuals with proper fantasy UI/billboard/decal treatment.

## Next Work

- Replace temporary selected-target text with better world readability:
  - replace the debug-drawn selected target ring with a real decal/mesh/material;
  - cleaner enemy nameplate/health readout treatment;
  - final clear target lost/dead behavior after restart verification.
- Improve inventory presentation:
  - item details for reward inspection;
  - cleaner empty/reward states.
- Start the first asset import pass from [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md), beginning with free stylized environment/UI packs.
- Tune starter enemy aggro, movement speed, attack range, damage, and respawn timing after in-editor playtesting.
- Tune player respawn and recovery rules after in-editor playtesting.
- Keep automation coverage growing around death/respawn, targeting, and hotbar behavior.

## Last Completed

- Verified the fresh editor test registry no longer included stale `TargetClearsOnDeath`.
- Ran the current pre-change Embermere automation tests: 3 passed, 0 failed.
- Added a prototype selected-target ground ring and HP-aware nameplate color for selected Marsh Prowlers.
- Added `I` inventory show/hide support without touching `DefaultInput.ini`.
- Added `Embermere.UI.InventoryToggle` automation coverage and hardened the target-presentation test to avoid transient actor registration under hot reload.
- Built successfully after the 2026-06-30 changes.
- In the already-open editor after hot reload, tests were 3 passed and 1 failed; the failure was the known hot-reload actor registration path for `Embermere.Combat.TargetSelectionPresentation`. Restart Unreal before treating that result as authoritative.
- Ran PIE HUD smoke through MCP and verified `Level 1`, `Inventory (I)`, and `Alt+R`/full hotbar text were visible.

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
