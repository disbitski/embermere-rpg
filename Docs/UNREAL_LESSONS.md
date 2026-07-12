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

## Do Not Parallelize Unreal Commandlets

Unreal commandlets can share UnrealBuildTool log and trace files under `~/Library/Application Support/Epic/UnrealBuildTool`. Running two `UnrealEditor-Cmd` jobs at the same time can make one commandlet fail while trying to rotate or move `Trace.uba`.

Practical rule:

- run build, automation, and map-validation commandlets sequentially;
- parallelize normal file reads and lightweight shell inspection, but not `UnrealEditor-Cmd` invocations;
- if a commandlet fails with a missing or locked UBT trace/log file while another commandlet is running, rerun it by itself before treating the failure as a project issue.

Embermere example:

- running the automation suite and FabPass validator at the same time caused one commandlet to fail on `Trace.uba`;
- rerunning the FabPass validator alone exited successfully.

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

Prefer first-class Unreal MCP tools when the ChatGPT/Codex app exposes them. The tool-search path (`list_toolsets`, `describe_toolset`, `call_tool`) keeps discovery small while still reaching editor, automation, Slate, scene, actor, asset, material, and UMG capabilities. Keep direct HTTP as a fallback, not the default path.

## Unreal Python Rotator Argument Order

Do not rely on positional `unreal.Rotator(...)` arguments in project placement scripts. In the first Fab zone pass, the positional constructor mapped every intended yaw value into pitch, tilting buildings, trees, rocks, fences, and ruins.

Use explicit fields instead:

```python
rotation = unreal.Rotator()
rotation.pitch = 0.0
rotation.yaw = float(yaw)
rotation.roll = 0.0
```

Also validate saved actor rotations from a fresh commandlet. Euler normalization can make values beyond 90 degrees appear as combined pitch/yaw/roll, so prevention is safer than reconstructing intent after placement.

The same rule applies to foundational rebuild scripts, not only one-off art placement helpers. `Scripts/setup_prototype_level.py` also used the positional constructor and was corrected before the next level rebuild could reintroduce tilted actors.

## A Sun And Skylight Do Not Guarantee A Readable Sky

A prototype map can contain a directional light, skylight, and height fog yet still render with a black sky and nearly black shadowed assets.

Embermere's first Fab pass had three compounding causes:

- no `SkyAtmosphere` actor;
- a stationary captured-scene skylight that had captured black surroundings;
- black lower-hemisphere and height-fog inscattering colors.

The first Mac-friendly correction was deliberately small:

- add `SkyAtmosphere`;
- make the skylight movable with real-time capture;
- provide restrained cool ambient/lower-hemisphere fill;
- reduce the heavy black fog and add subtle cool/warm inscattering;
- keep the existing directional sun instead of compensating with excessive direct-light intensity.

Verify this visually in PIE and persist it through the map asset API. Also put the atmosphere actor in the rebuild script and require it in the fresh-process map validator so regeneration cannot silently restore the black-sky state.

## Clickable HUD Windows Need An Explicit Input Mode

Adding `UButton` rows to a runtime UMG inventory is not enough by itself. A classic game-only controller can keep the cursor hidden and prevent the player from reaching those rows.

For Embermere's first mouse-driven inventory pass:

- opening the inventory enables the cursor, click events, mouse-over events, and `FInputModeGameAndUI`;
- closing it restores `FInputModeGameOnly`, hides the cursor, clears held mouse-button state, and restores classic camera-facing behavior;
- inventory row buttons are non-focusable so clicking an item does not turn WASD into UI keyboard navigation;
- bracket-key selection remains available as a full keyboard path.

Automation can cover direct selection and invalid indices, but a clean-restart PIE pass still needs to verify real cursor capture and right-mouse camera behavior against the live viewport.

## Slate Input Is Not Asset Persistence

`SlateInspectorToolset.PressKey` can successfully deliver `Cmd+S` while PIE or the viewport has focus without saving the dirty map package. A successful input result proves the key event was accepted, not that the asset reached disk.

Practical rule:

- use Slate input for UI interaction and gameplay keys;
- use `AssetTools.save_assets` for intentional map/asset persistence;
- confirm the editor no longer reports the package dirty;
- run a fresh headless validator that reloads the map from disk.

Embermere example:

- the live editor showed 65 actors after removing three oversized shells;
- a headless validator still found the old 68-actor map after simulated `Cmd+S`;
- `save_assets(["/Game/Maps/L_Embermere_Prototype"])` persisted the map;
- the next fresh validator found 65 upright actors and passed.

## World-Space Indicators Must Clear Every Visual Layer

A target ring can be visible, unhidden, correctly materialed, and still disappear because its component height was authored against the character capsule rather than the actual surface beneath the enemy. Embermere's combat-pocket cylinder sits above the zone plane, so the original ring height placed all 24 segments inside that platform.

Practical rule:

- inspect the actor, indicator component, and supporting surface world Z values separately;
- account for raised blockout platforms, decals, terrain, and future mesh swaps;
- leave a small clearance above the highest expected walkable surface;
- verify again after a clean C++ reload because constructor-created component offsets do not update when only the source default changes.

Also avoid assuming additive is always the most readable blend mode. The gold ring washed out over the prototype's very bright ground, so the first material pass now uses an opaque unlit/emissive treatment while the ground itself uses a muted moss material.

## Visual Baselines Belong In Rebuild Scripts And Validators

A successful one-off lighting or material edit is fragile if the foundational level script can regenerate older defaults. After the first blue-sky and moss-ground pass survived PIE review, Embermere copied the exact values into `setup_prototype_level.py` and asserted them from a fresh process.

The validator now checks sun intensity and atmosphere role, skylight intensity and real-time capture, fog density/falloff/color, the Mac-friendly volumetric-fog choice, and all three ground-material overrides. For Unreal asset paths, remember that `get_path_name()` returns the full object path such as `/Game/Path/Asset.Asset`, not only the package path.

## Equipment Bonuses Must Replace, Not Accumulate

An equipment-change delegate can fire repeatedly for equip, replacement, UI refresh, or future load operations. Adding bonuses directly to live stats each time creates silent stat inflation.

Embermere stores the previously applied equipment aggregate and computes each new value as `live - old bonus + new bonus`. Reapplying the same loadout is therefore idempotent. Health and mana preserve the amount missing when maximums change, which avoids both free full heals and accidental extra damage during gear swaps. A dead character is explicitly held at zero so maximum-health gear cannot resurrect them. Armor uses a bounded mitigation curve, while zero armor preserves the original damage contract.

## Consumables Should Commit Inventory Last

A Use action should not remove an item before proving that an effect occurred. Embermere snapshots health/mana, attempts recovery, and decrements one stack only when at least one resource actually increases. At full resources, the action remains disabled and the stack is preserved. This transaction order keeps UI clicks, automation, and future server-authoritative logic aligned.

## Prototype Loot Should Be Deterministic But Data-Driven

A first playable loot loop is easiest to validate when its default drop is deterministic, but the enemy should not hard-code inventory behavior around one specific item. Embermere exposes an enemy loot item, quantity, and drop chance, then routes a successful roll through the recipient's inventory component. Marsh Prowlers currently default to a guaranteed Marsh Tonic so every combat test can reach the recovery action. Automation separately covers the roll boundary, stack delivery, and no-drop path, leaving later tuning free to lower the chance or swap loot without rewriting death handling.

## Fab Search And Import Reality

Fab's public website and listing endpoints can trigger Cloudflare checks from terminal automation. Do not assume Codex can choose and download Fab assets directly from unauthenticated shell or web requests.

Practical rule:

- use the signed-in Unreal Fab window or Epic Games Launcher for the actual add-to-library/import step;
- import third-party packs under `/Game/ThirdParty/Fab/<PackName>`;
- once assets exist locally, Codex can inspect paths, duplicate project-specific variants into `/Game/Art/Embermere`, replace blockout actors, run PIE, and commit intentional project files.
