# Next Checklist

## Build The First Play Loop

- Flesh out `DA_EmbermereRules` with the starting races/classes and starter abilities.
- Create first item and quest data assets.
- Create Blueprint or UMG child widgets for:
  - character creation
  - target frame
  - hotbar
  - dialogue
  - quest tracker
  - loot popup
- Wire the starter quest into the village quest NPC placeholder.
- Make the three starter enemies attack, die, respawn, and grant quest progress.

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
