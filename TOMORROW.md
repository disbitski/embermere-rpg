# Next Checklist

## Build The First Play Loop

- Restart Unreal after C++ changes, then run the editor smoke test in [Docs/PLAYTESTING.md](Docs/PLAYTESTING.md).
- Create Blueprint or UMG child widgets for:
  - character creation
  - target frame
  - hotbar
  - dialogue
  - quest tracker
  - loot popup
- Tune starter enemy aggro, movement speed, attack range, damage, and respawn timing after in-editor playtesting.
- Tune player respawn and recovery rules after in-editor playtesting.
- Add real loot popup and inventory reward widgets.
- Replace temporary debug text with first-pass UI widgets.

## Keep MCP Running

In the Unreal console:

```text
ModelContextProtocol.StartServer 8123
ModelContextProtocol.GenerateClientConfig Codex
```

Then run:

```bash
zsh Scripts/check_unreal_setup.sh
```

## Starter Map

- Open `/Game/Maps/L_Embermere_Prototype`.
- Rebuild the greybox if needed:

```text
py exec(open('/Users/wizard/Documents/Unreal Game/Scripts/setup_prototype_level.py').read())
```

- Replace blockout buildings, NPCs, enemies, and ruin pieces as Fab assets arrive.
- Keep the village, path, wilderness pocket, and ruin readable from the default camera.

## Asset Hunt

Start with free Unreal/Fab assets:

- stylized modular fantasy village kit
- stylized forest/foliage pack
- ruins/stone props
- fantasy UI/icons
- simple fantasy enemy or creature pack
- weapon pack for sword, shield, staff, and bow

Track candidates and install notes in [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md).
