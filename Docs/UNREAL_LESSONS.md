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
