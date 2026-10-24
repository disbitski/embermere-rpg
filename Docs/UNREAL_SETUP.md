# Unreal And MCP Setup

This repo is ready for Unreal. The local machine needs Epic Games Launcher, Unreal Engine 5.8, full Xcode, and an Unreal-generated MCP client config before Codex can drive the editor.

## Current Embermere Mac

Use the project launcher after quitting any existing Unreal Editor instance:

```bash
zsh Scripts/launch_unreal_editor.sh
```

It selects `/Applications/Xcode_26.1.1.app` only for the new editor process,
checks that Metal executes, refreshes a stale `xcrun` lookup once if needed,
and starts the project's MCP listener on port `8123`. It refuses to launch
over another editor process or an occupied MCP port. It does not change
system-wide `xcode-select`, install components, or dismiss editor dialogs.
The system default may remain Xcode 27; Unreal 5.8's declared Mac SDK range
ends at 26.9, so use Xcode 26.1.1 for project builds as well.

If a startup dialog says the Metal Toolchain is missing, do not immediately
download it: `xcodebuild -showComponent MetalToolchain -json` may show it is
already installed while `xcrun` still points to Xcode's stub. Quit Unreal,
run `DEVELOPER_DIR=/Applications/Xcode_26.1.1.app/Contents/Developer xcrun
--kill-cache`, then retry the launcher. Only investigate component
installation if `xcrun metal -v` still fails after the refreshed lookup.

Official MCP docs: https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor

## Install

1. Download Epic Games Launcher from the official Unreal page: https://www.unrealengine.com/download
2. Install Epic Games Launcher.
3. Create or log in to your Epic Games account.
4. Install Xcode from Apple and make sure it lives at `/Applications/Xcode.app`:
   - Apple Xcode page: https://developer.apple.com/xcode/
   - Mac App Store listing: https://apps.apple.com/us/app/xcode/id497799835
5. Open Xcode once after installation so it can finish installing required components.
6. If macOS asks for license/toolchain confirmation, accept it.
7. Select full Xcode for command-line tools:

   ```bash
   sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
   sudo xcodebuild -license accept
   ```

8. Check whether Xcode's Metal compiler runs from a normal macOS Terminal session:

   ```bash
   xcrun metal -v
   xcodebuild -showComponent MetalToolchain -json
   ```

   If the component reports `installed` but `xcrun` claims it is missing,
   refresh the lookup with `xcrun --kill-cache` and retry before considering
   an install or reboot. If it is genuinely uninstalled, review the Xcode
   diagnostic and install only with the machine owner's approval.

9. Install Unreal Engine 5.8 from the Launcher's Unreal Engine tab.
10. Open `Embermere.uproject`.
11. If Unreal asks to rebuild modules, allow it.
12. Enable or confirm these plugins:
   - Model Context Protocol / Unreal MCP
   - AllToolsets
   - PythonScriptPlugin
   - Enhanced Input
   These may already be enabled because `Embermere.uproject` requests them.
13. Restart the editor if prompted.

## Start MCP

Open the Unreal console and run:

```text
ModelContextProtocol.StartServer 8123
ModelContextProtocol.GenerateClientConfig Codex
```

Epic's default MCP port is `8000`; this project uses `8123` to avoid common local port conflicts. If you use the Editor Preferences auto-start setting instead, set the server port to `8123` there too.

The config generator is a one-time setup step unless the client configuration
changes. It writes the correct client config for the named agent. JSON clients
such as Claude Code use `.mcp.json`; Epic's docs note that Codex uses a TOML
config and that stale Codex configs may need to be removed manually before
regenerating. After the config exists, Codex can connect to Unreal MCP from this
workspace.

For this project, verify `.codex/config.toml` points to:

```toml
[mcp_servers.unreal-mcp]
url = "http://127.0.0.1:8123/mcp"
```

If it points to `8000`, update it to `8123`. Port `8000` is a common conflict point and may be used by unrelated local dev servers.

Useful MCP console commands from Epic's docs:

```text
ModelContextProtocol.StartServer [port]
ModelContextProtocol.StopServer
ModelContextProtocol.RefreshTools
ModelContextProtocol.GenerateClientConfig <Client|All>
```

Unreal 5.8 also supports command-line MCP autostart:

```text
-ModelContextProtocolStartServer -ModelContextProtocolPort=8123
```

The reliable macOS launch form for this project is:

```bash
open -n --fresh --env DEVELOPER_DIR=/Applications/Xcode_26.1.1.app/Contents/Developer \
  "/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app" \
  --args "/Users/wizard/Documents/Unreal Game/Embermere.uproject" \
  -ModelContextProtocolStartServer -ModelContextProtocolPort=8123
```

The project path must come after `--args`. Putting it before `--args` caused
`open` to launch the editor without forwarding the project. Generic
`-ExecCmds=ModelContextProtocol.StartServer 8123` also fired too early during
startup in testing; use the plugin's dedicated launch flags instead. Prefer
`Scripts/launch_unreal_editor.sh` because it checks Metal and avoids duplicate
editor sessions before using this command.

## Validate

From this repo:

```bash
zsh Scripts/check_unreal_setup.sh
```

The expected milestone states are:

- Before installing tools: project/plugin checks pass, while Epic, Xcode, Unreal, and MCP config are missing.
- After installing Epic, Xcode, Unreal, and the Metal Toolchain: all local toolchain checks pass, while MCP config is still pending.
- After opening the project and running `ModelContextProtocol.GenerateClientConfig Codex`: the MCP config check should pass too.

## Saving Editor Changes

It is safe to use `File > Save All` before quitting Unreal, especially when a level or asset tab shows an asterisk. Unreal may also rewrite project config or binary asset packages while saving, so check `git status` afterward and only commit intentional files.

After restarting Unreal, either use the launch flags above or start MCP again
from the Unreal console when Codex needs editor access:

```text
ModelContextProtocol.StartServer 8123
```

## Manual Rebuild

If Unreal shows `Embermere could not be compiled. Try rebuilding from source manually.`, run:

```bash
"/Users/Shared/Epic Games/UE_5.8/Engine/Build/BatchFiles/Mac/Build.sh" EmbermereEditor Mac Development -Project="/Users/wizard/Documents/Unreal Game/Embermere.uproject" -WaitMutex
```

Then reopen the project from Epic Games Launcher or by double-clicking `Embermere.uproject`.

## MCP Notes

- Unreal MCP is experimental.
- It runs inside the editor process and exposes editor functionality over local HTTP.
- The plugin identifier is `ModelContextProtocol`; the friendly name in the Plugin Browser is Unreal MCP.
- Toolsets come from the Toolset Registry, so `AllToolsets` must also be enabled.
- The server is loopback/local by default and has no authentication layer, so do not expose it remotely.
- Tool invocations execute serially on Unreal's game thread; avoid overlapping MCP tool calls.
- With tool search enabled, agents discover tools through `list_toolsets`, `describe_toolset`, and `call_tool`.

## First Editor Tasks

Once MCP is connected, the first editor tasks are:

1. Create `/Game/Maps/L_Embermere_Prototype`.
2. Create Blueprint children for:
   - `AEmbermereCharacter`
   - `AEmbermereEnemyCharacter`
   - `AEmbermereGameMode`
3. Create `UEmbermereRulesData` as `/Game/Data/DA_EmbermereRules`.
4. Create starter quest and reward item data assets.
5. Build a greybox village, wilderness pocket, and ruin landmark.
6. Add nameplate, target frame, hotbar, dialogue, quest tracker, and loot popup widgets.
