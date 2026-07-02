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
