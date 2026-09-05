# Embermere Agent Instructions

## No Desktop Control

The user revoked desktop-control access for this project on 2026-09-05.

- Do not use Computer Use, unified-computer-use, CUA, desktop screenshots,
  accessibility automation, or OS-level mouse/keyboard control for this project.
- Do not substitute AppleScript, System Events, CGEvent, browser automation,
  or shell-driven desktop automation to work around this restriction.
- Use the dedicated Unreal MCP and Blender MCP integrations for editor work.
  Unreal-owned Slate inspection, viewport-only captures, and input through
  Unreal MCP remain allowed. Do not use CaptureEditorImage, which can capture
  broader desktop/editor windows. Blender-owned scene inspection and scripting
  through Blender MCP remain allowed. Neither integration may control the OS desktop.
- Shell builds, commandlets, tests, file edits, and Git remain allowed within
  their existing permissions. Do not change the user's macOS security settings.
- If a check requires physical input that the editor MCP cannot provide, leave
  that check explicitly unverified and ask the user to perform it. Continue
  unaffected work; do not make a desktop unlock a prerequisite for MCP work.
- Do not re-enable desktop-control plugins, bridges, or app permissions without
  a new explicit user request. This applies to scheduled builds as well.

## Handoff

Read TODO.md's Start Here section for current implementation and verification
state. Preserve existing unrelated changes and the user's saved journey.
