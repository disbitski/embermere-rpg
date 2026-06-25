# Tomorrow Checklist

## Finish Unreal Setup

- Confirm Unreal Engine 5.8 finished downloading.
- Open `/Users/wizard/Documents/Unreal Game/Embermere.uproject`.
- Allow Unreal to rebuild project modules if prompted.
- Confirm plugins are enabled:
  - Unreal MCP / `ModelContextProtocol`
  - `AllToolsets`
  - `PythonScriptPlugin`
  - `EnhancedInput`
- Restart Unreal if prompted.

## Start MCP

In the Unreal console:

```text
ModelContextProtocol.StartServer 8123
ModelContextProtocol.GenerateClientConfig Codex
```

Then run:

```bash
zsh Scripts/check_unreal_setup.sh
```

## First Editor Pass

- Create `/Game/Maps/L_Embermere_Prototype`.
- Create Blueprint children for:
  - `AEmbermereCharacter`
  - `AEmbermereEnemyCharacter`
  - `AEmbermereGameMode`
- Create `/Game/Data/DA_EmbermereRules`.
- Create first item and quest data assets.
- Create a tiny greybox village, wilderness pocket, and ruin.

## GitHub Decision

- Pick repo visibility:
  - private while learning and experimenting
  - public if we want to share the journey from day one
- Recommended repo name: `embermere-rpg`.
- First commit should include the current scaffold, docs, and setup scripts.

## Asset Hunt

Start with free Unreal/Fab assets:

- stylized modular fantasy village kit
- stylized forest/foliage pack
- ruins/stone props
- fantasy UI/icons
- simple fantasy enemy or creature pack
- weapon pack for sword, shield, staff, and bow
