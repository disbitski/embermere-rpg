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

Test discovery itself is also a useful stale-module probe. On 2026-07-14, a
fresh commandlet run discovered and passed 18 tests while first-class MCP in the
still-open interactive editor discovered only 14. That count mismatch proved
the editor needed a restart before PIE, without relying on visual guesswork.

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

## Inventory And Equipment Transfers Must Be Atomic

Returning `false` after partially changing a bag is worse than rejecting an operation up front: callers believe nothing happened while quantities, loot, or equipment have already moved. This is especially risky for same-slot gear replacement, where the incoming item leaves the bag before the old item can be returned.

Embermere now preflights add/remove capacity and treats bag/equipment movement as a transaction:

- equipping removes the candidate only after eligibility passes;
- replacement confirms the old item can return to the post-removal bag;
- any failed replacement restores the candidate stack and leaves old equipment untouched;
- unequip adds to the bag before clearing the equipment slot, with rollback if clearing fails;
- general add/remove calls reject insufficient capacity or quantity without partial mutation.

Transfers still broadcast inventory-changed events so the HUD refreshes, but they suppress the item-added acquisition event. Moving owned gear must not produce a fake loot/reward popup.

Test both the happy path and a deliberately full one-slot bag. The rollback assertion is the important part: after failure, verify exact item identity, quantity, and equipped-slot state rather than checking only the boolean result.

## Fab Search And Import Reality

Fab's public website and listing endpoints can trigger Cloudflare checks from terminal automation. Do not assume Codex can choose and download Fab assets directly from unauthenticated shell or web requests.

Practical rule:

- use the signed-in Unreal Fab window or Epic Games Launcher for the actual add-to-library/import step;
- import third-party packs under `/Game/ThirdParty/Fab/<PackName>`;
- once assets exist locally, Codex can inspect paths, duplicate project-specific variants into `/Game/Art/Embermere`, replace blockout actors, run PIE, and commit intentional project files.

## Pin The Classic FBX Factory For Authored Collision

In UE 5.8, creating `FbxImportUI` does not force a generic
`AssetImportTask` to use the classic FBX importer. Embermere's first ember-lamp
attempt went through Interchange, where the generated mesh pipeline persisted
`bCollision=false`. The source FBX contained two correctly named `UBX_` meshes,
but the resulting static mesh had zero simple collision boxes.

Practical rule:

- for reviewed Blender assets with authored `UBX_`, `UCX_`, `UCP_`, or `USP_`
  collision, assign `unreal.FbxFactory()` explicitly on the import task;
- validate `body_setup.agg_geom` before placing or saving any actor;
- validate the resulting asset import-data class is `FbxStaticMeshImportData`;
- if a failed Interchange attempt already created the asset path, atomic
  replacement can retain `InterchangeAssetImportData`; delete only that partial
  mesh package and recreate the same path through the classic factory before
  loading the referencing map;
- treat the commandlet process exit code as insufficient for Python scripts,
  because `SystemExit` or a late Python exception can still appear in the log
  after Unreal returns process status zero.

The finalized ember-lamp lane first validates import provenance, bounds, and two
box colliders, then replaces the two map lamps. A fresh-process zone validator
rechecks the same contract from disk.

## Decorative Geometry Must Declare No Collision

A translucent or low-profile encounter marker can look harmless while its
static-mesh component still uses `QueryAndPhysics`. Embermere's enemy cones and
safe/combat area cylinders were intended only as visual guides, but their saved
collision could block AI movement and make a tuning problem look like a
navigation or behavior bug.

Practical rule:

- set both the collision profile and collision-enabled state to `NoCollision`
  for every visual-only component;
- call `modify()` on the actor and component before changing editor state;
- reproduce the setting in the foundational level setup script;
- reload the saved map and assert the component collision state in the
  validator.

## Place Encounters From Collision Queries, Not Visual Bounds

Vendor foliage and ruin meshes can have simple collision that extends beyond
their visible silhouette. Moving a starter enemy until it looks clear in the
viewport is therefore insufficient. Embermere's first proposed solo-pull
triangle still intersected a hidden rock, stair, and ruin collider.

Use native WorldStatic overlap queries around the expected capsule footprint,
then validate exact home positions and minimum spacing. The resulting Prowler
triangle is visually inside the wilderness pocket, clear of vendor collision,
and far enough apart for a `525` cm aggro radius to produce one-enemy pulls.

## Imported Assets Are Not Durable Until Their Packages Are Saved

Unreal can return a generated material from the object registry and render it
in the current session even when no `.uasset` package has been written. The
first road-signpost import appeared complete until a filesystem audit found
`M_EmbermereTimber` only in memory.

Explicitly save every generated mesh and material package, verify the expected
asset path exists after the save, and include the material path in the map or
asset validator. Session visibility is not persistence.

## Keep Delays Outside Unreal Programmatic Scripts

`time.sleep()` inside a programmatic Unreal MCP script blocks the editor's game
thread, including the PIE work the script is waiting for. Trigger the operation,
return control, wait in the outer MCP orchestration layer, and query state in a
second call. When using the Unreal console, send `py import unreal; ...` or
`py exec(compile(open(...).read(), ...))` directly; quoting the entire Python
statement causes the console command to be parsed incorrectly.

## Keep Automatic Movement Out Of Manual Input Handlers

Autorun and manual forward input can share movement math without sharing the
same cancellation entry point. Calling Embermere's manual `MoveForward(1)`
from the controller tick made the autorun system indistinguishable from a real
`W` press: either autorun cancelled itself or the cancellation path needed a
fragile transient guard that could also suppress real input.

Apply automatic forward movement directly from the controller tick, and reserve
the manual-axis notification for actual nonzero player input. Cover forward,
backward, and idle cases in automation, then restart the editor before live
input verification because stale C++ modules can make a correct fix look
unchanged.

## UE 5.8 Python Line Traces Return A Hit Or None

In UE 5.8 Python, `unreal.SystemLibrary.line_trace_single(...)` returns a
`HitResult` when something blocks the trace and `None` when the route is clear.
Do not unpack it as `(did_hit, hit_result)`. Protected actor/component members
also may not be readable as normal Python properties; the stable inspection
path used by Embermere is `hit_result.to_dict()["hit_actor"]`.

An Unreal commandlet can still exit with status zero after the invoked Python
script logs an exception. Treat the process code as only one signal: require a
known validator success line and reject `LogPython: Error` in the captured log.
The road-boundary validator follows this contract while proving clear gate
lanes and solid gate/fence collision.

## Atomic Slate Chords Do Not Span A Later Player Tick

Unreal MCP's Slate `PressKey` sends a modifier chord as a press-and-release
operation on the focused widget. That is sufficient for event-driven action
bindings such as `Q`, `W`, and `S`, but it cannot prove a control that polls
`IsInputKeyDown(Ctrl)` during a later `PlayerTick`: Ctrl has already been
released by then. Keep `Ctrl+M` as a physical-key PIE check unless its game
implementation moves to an event-driven chord binding. Do not report an MCP
tool-model limitation as a gameplay regression.

## Treat UI Source Art, Packages, And Presentation As Separate Gates

A generated PNG existing on disk does not make it an Unreal asset, and a
texture loaded in one editor session does not prove its package or data-asset
reference was saved. Embermere's first icon family therefore keeps five
separate checks:

- a deterministic tracked source generator and inspectable PNG outputs;
- an explicit Unreal import/configuration script;
- explicit saves for every texture, icon-set, and item-data package;
- a fresh-process Python validator for dimensions, texture settings, mappings,
  and soft references;
- C++ resolver/layout automation plus clean-restart PIE judgment.

Keep icon selection data-driven through item soft references and a shared
category/slot/missing-art fallback asset. Fixed `SizeBox` bounds preserve layout
even when an image is collapsed and text remains the readable fallback.

The same contract now covers abilities. Each `FEmbermereAbilityDefinition`
owns a soft texture reference, `UEmbermereUiIconSet` supplies one explicit
missing-ability fallback, and the hotbar consumes the definition without
switching on class or ability ID. Keep every slot's art cell allocated even
when its image is collapsed so empty commands cannot shift neighboring cells.
When vertical space is fixed, compact the changing cooldown copy rather than
growing the control: Embermere keeps key/countdown on one line and the ability
name on the second. Attach the description, power, mana, range, and cooldown as
a tooltip so the icon remains accessible without turning the HUD into visible
instruction text.

Passing those contracts still does not prove real copy fits. Embermere's first
populated clean-PIE screenshot showed that the saved `Embermere Recruit Pack`
label clipped in its compact row and crowded the adjacent Equipment heading,
even though resolver, dimensions, fallbacks, and all 22 tests passed. Exercise
the actual longest saved labels in the real panel, not only synthetic geometry
constants. When presentation copy changes, preserve the stable item identifier,
make the player-facing label reproducible in the asset-configuration script,
and add the accepted compact text to fresh-process package validation.

One headless detail matters: `UTexture2D::GetSizeX/Y()` can report zero under
`-NullRHI` because no runtime texture resource exists. In editor automation,
inspect `Texture->Source.GetSizeX/Y()` instead; retain an independent
fresh-process check through Unreal's Python `blueprint_get_size_x/y()` surface.
The two assertions measure persisted source dimensions without pretending the
Null RHI created a render resource.

## Native Interface Objects Need A Native Test Path

Reflected `BlueprintNativeEvent` interface dispatch is not a reliable
substitute for the direct native implementation on every unattached
`NewObject<AActor>` used by object-level automation. Embermere's first
starter-effect test created a native enemy without a world; direct
`IsAlive_Implementation()` returned true while reflected `Execute_IsAlive()`
did not establish the same transient test state.

For helpers that already support native Embermere actors, cast to the native
interface and call its implementation first, then retain reflected
`Execute_...` dispatch as the fallback for other interface implementers. Assert
both the native cast and the declared interface in the focused test. Keep a
separate clean-PIE check against the actual Blueprint enemy so object-level
convenience cannot hide a runtime dispatch regression.

## Refresh Slate Focus Before Synthetic Gameplay Input

Editor inspection calls can change Slate focus even while PIE keeps running.
An early road-gate probe sent a valid `W` event to the wrong focused widget,
leaving autorun active long enough for the pawn to cross the prototype and fall
past the map. That looked like an autorun regression until a fresh accessibility
snapshot and explicit viewport click restored the expected input route.

For MCP input checks:

- capture a fresh Slate snapshot after starting each PIE session;
- click the current game-viewport/root image before every measured key action;
- prove movement and cancellation with actor transforms sampled before and
  after the action, not only with a successful `PressKey` return value;
- stop PIE promptly when a bounded movement probe finishes.

With refreshed focus, `Q` moved the Embermere pawn and independent `W` and `S`
checks each held an identical transform for a full second. Tool success means
the event was sent; state measurement proves the game received it.
