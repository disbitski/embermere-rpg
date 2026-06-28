# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

## Start Here

- Restart Unreal before manual PIE if the editor has been open since the C++ HUD build, so it loads the newest module without hot-reload noise.
- Manually verify the styled first-pass HUD in PIE:
  - player HP, mana, XP, health bar, and mana bar are visible;
  - target panel appears after `Tab` and shows target HP plus range state;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - all hotbar slots show stable labels for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`;
  - Mara interaction shows the dialogue panel;
  - quest completion/reward shows the loot popup.
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
- automation coverage for the race/class matrix and quest completion rewards.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The next meaningful presentation gap is selected-target/nameplate readability in the 3D world, followed by inventory presentation and the first Fab/Marketplace art import. The HUD is now good enough to playtest the loop, but it still needs a real fantasy skin later.

## Next Work

- Add selected-target world readability:
  - visible selected target marker or ring;
  - enemy nameplate/health readout treatment;
  - clear target lost/dead behavior.
- Add first-pass inventory panel or reward history so the starter item is inspectable after quest completion.
- Start the first asset import pass from [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md), beginning with free stylized environment/UI packs.
- Tune starter enemy aggro, movement speed, attack range, damage, and respawn timing after in-editor playtesting.
- Tune player respawn and recovery rules after in-editor playtesting.
- Keep automation coverage growing around death/respawn, targeting, and hotbar behavior.

## Last Completed

- Upgraded the native HUD into styled panels for player status, target HP/range, quest tracker, dialogue, loot popup, and all hotbar labels.
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
