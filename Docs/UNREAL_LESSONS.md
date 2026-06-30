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
