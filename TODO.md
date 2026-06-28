# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

## Start Here

- Restart Unreal before manual PIE if the editor has been open since the C++ HUD build, so it loads the newest module without hot-reload noise.
- Manually verify the first-pass HUD in PIE:
  - player HP, mana, and XP are visible;
  - target text updates after `Tab`;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - hotbar slots `1`-`4` show the starter ability names.
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
- first-pass native HUD overlay for player status, target, quest progress, and hotbar labels;
- automation coverage for the race/class matrix and quest completion rewards.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The next meaningful gap is turning the first-pass HUD into proper styled widgets, then adding dialogue and loot/reward popups. After that, we start bringing in Fab/Marketplace art while keeping gameplay systems asset-agnostic.

## Next Work

- Replace the temporary text HUD with styled UMG sections for:
  - target frame;
  - hotbar;
  - quest tracker.
- Create first-pass UMG widgets for:
  - dialogue;
  - loot/reward popup.
- Tune starter enemy aggro, movement speed, attack range, damage, and respawn timing after in-editor playtesting.
- Tune player respawn and recovery rules after in-editor playtesting.
- Keep automation coverage growing around death/respawn, targeting, and hotbar behavior.

## Last Completed

- Added a native first-pass HUD overlay for player status, target, quest progress, and starter hotbar labels.
- Built successfully.
- Ran Embermere automation tests: 2 passed, 0 failed.
- Ran PIE boot smoke through MCP; no gameplay or Blueprint errors found.
- Manual visual/feel checks remain for HUD readability, marker readability, autorun cancel, and `Ctrl+M`.

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
