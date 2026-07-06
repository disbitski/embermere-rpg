# Unreal Lessons

This file captures project-specific Unreal lessons we want Codex and future-us to remember before making similar changes again.

## Native UMG Layout Timing

When building a pure C++ `UUserWidget` with no designer-authored root widget, create the widget tree before Unreal rebuilds the underlying Slate widget.

Use this pattern:

- override `RebuildWidget()`;
- call the native layout builder before `Super::RebuildWidget()`;
- keep `NativeConstruct()` for binding, refresh, and post-construction setup.

Why it matters:

- `NativeConstruct()` can be too late for initial pure-native layout creation;
- the C++ widget object may exist, but the visible Slate tree can still be empty;
- MCP/log smoke tests may pass because no runtime error occurs, while the player sees no UI;
- visible verification should include checking for actual HUD text/panels in PIE, not just successful map boot.

Embermere example:

- `UEmbermerePlayerHudWidget::BuildDefaultLayout()` must run from `RebuildWidget()`;
- the HUD was invisible when the layout was only built from `NativeConstruct()`;
- moving layout creation into `RebuildWidget()` made the status panel and hotbar appear immediately in PIE.

## Hot Reload And Automation Test Registrations

When adding, renaming, or substantially changing C++ automation tests while Unreal Editor is already running, the editor can retain stale automation registrations from previous hot-reloaded modules.

Symptoms:

- `ListTests` can show both the old test path and the new test path;
- a renamed test can still execute old in-memory behavior;
- source can build cleanly while MCP automation results reflect the editor's stale loaded module.

Practical rule:

- after interface-heavy C++ changes or automation test renames, restart Unreal before treating MCP automation failures as authoritative;
- keep the test name and C++ test class name aligned to reduce stale-registration confusion;
- prefer `NewObject` object-level tests over transient `UWorld::SpawnActor` tests when the behavior under test does not require registered components or world ticking;
- record whether a test ran in a fresh editor or an already-open editor in the daily handoff.

Embermere example:

- selected-target presentation added `Embermere.Combat.TargetSelectionPresentation`;
- the open editor still listed the old `Embermere.Combat.TargetClearsOnDeath` registration during the same session;
- after adding target ring components, the open editor could still execute a hot-reloaded actor registration path for the target test;
- the next reliable validation should restart Unreal, rediscover tests, and run the current test set.

## Commandlet Tests And Numbered Hot-Reload Libraries

When Unreal Editor is open, command-line C++ builds can produce numbered hot-reload libraries such as `libUnrealEditor-Embermere-0029.dylib`. A separate `UnrealEditor-Cmd` automation run may still load the base module and execute stale automation test code.

Symptoms:

- source and a numbered hot-reload library include the latest test text;
- commandlet automation reports old assertion messages from a prior version of the test;
- rebuilding appears successful, but the report stays stale.

Practical rule:

- when running headless automation after C++ test changes, prefer a build with `-NoHotReloadFromIDE`;
- this lets UnrealBuildTool clean numbered hot-reload libraries and relink the base `libUnrealEditor-Embermere.dylib`;
- still restart the interactive editor before manual PIE after this kind of build, because the open editor may have been using one of the cleaned hot-reload libraries.

Embermere example:

```bash
"/Users/Shared/Epic Games/UE_5.8/Engine/Build/BatchFiles/Mac/Build.sh" EmbermereEditor Mac Development -Project="/Users/wizard/Documents/Unreal Game/Embermere.uproject" -NoHotReloadFromIDE
```

After the no-hot-reload build, the new `Embermere.Combat.DeadCasterRejected` test ran from the current source and passed in commandlet automation.

## Xcode Metal Toolchain MobileAsset State

On macOS, `xcodebuild -downloadComponent MetalToolchain` can successfully download and mount the Metal Toolchain while Xcode's default `metal` wrapper still reports it as missing.

Quick diagnosis:

- run `xcrun metal -v`;
- confirm full Xcode is selected with `xcode-select -p`;
- confirm first launch is complete with `xcodebuild -checkFirstLaunchStatus`;
- check for a mounted Metal toolchain under `/private/var/run/com.apple.security.cryptexd/mnt`;
- run the mounted `Metal.xctoolchain/usr/metal/current/bin/metal -v` directly.

If the direct mounted compiler works but `xcrun metal -v` still fails, the download itself is not the real problem anymore. Reboot macOS before reinstalling Xcode or Unreal; the likely stale piece is Xcode's MobileAsset/cryptex lookup state.

## Direct Unreal MCP Over HTTP

If Codex does not expose Unreal MCP as first-class tools after the editor server is running, the server can still be reached directly at the generated config URL, currently `http://127.0.0.1:8123/mcp`.

Useful pattern:

- send a JSON-RPC `initialize` request;
- read the `Mcp-Session-Id` response header;
- include that session header on later requests;
- call top-level `list_toolsets`, `describe_toolset`, and `call_tool`;
- expect server-sent-event responses for long-running calls, especially automation and PIE commands.

Embermere example:

- `AutomationTestToolset.AutomationTestToolset` can discover and run the current test suite;
- `EditorToolset.EditorAppToolset` can start/stop PIE;
- viewport capture can return a very large inline PNG payload through raw MCP, so use it carefully or route it to a file instead of dumping it into the terminal.

## Fab Search And Import Reality

Fab's public website and listing endpoints can trigger Cloudflare checks from terminal automation. Do not assume Codex can choose and download Fab assets directly from unauthenticated shell or web requests.

Practical rule:

- use the signed-in Unreal Fab window or Epic Games Launcher for the actual add-to-library/import step;
- import third-party packs under `/Game/ThirdParty/Fab/<PackName>`;
- once assets exist locally, Codex can inspect paths, duplicate project-specific variants into `/Game/Art/Embermere`, replace blockout actors, run PIE, and commit intentional project files.
