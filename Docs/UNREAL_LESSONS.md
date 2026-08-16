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

## World-Space Indicators Need Surface And Footprint Contracts

A target ring can be visible, unhidden, correctly materialed, and still
disappear because its component height was authored against the character
capsule rather than the actual surface beneath the enemy. Embermere's
combat-pocket cylinder sat above the zone plane, so the original fixed-height
ring could place all of its segments inside the platform.

Practical rule:

- inspect the actor, indicator component, and supporting surface world Z values separately;
- trace toward the supporting surface while the indicator is visible and keep
  a PIE-proven explicit clearance above the hit;
- size from transformed visual bounds plus padding rather than assuming every
  creature fits the same capsule-sized circle;
- retain a conservative minimum and fallback offset for actors or worlds that
  do not resolve a valid surface;
- account for raised blockout platforms, terrain, and future mesh swaps;
- verify again after a clean C++ reload because constructor-created component offsets do not update when only the source default changes.

Embermere's accepted target circle uses 48 overlapping, non-colliding plane
segments, bounds-aware radius, a downward surface trace, and a restrained
cyan-blue emissive treatment. Its effective `16` cm trace clearance is visually
flush because the prototype's rendered moss surface sits above its collision
surface. It does not rotate, so target identity reads immediately instead of
competing with timed-status world VFX. Selection automation proves that the
circle appears on selection, moves to the new target, and clears from the old
target on switch or deselect; the existing enemy lifecycle clears it on death,
hide, and respawn transitions.

Also avoid assuming additive is always the most readable blend mode. The
earlier gold ring washed out over the prototype's bright ground, so the target
material remains opaque unlit/emissive while the ground uses muted moss and
earth values.

## Test The Actual Blueprint-Generated Runtime Class

A native object-level test can pass while a saved Blueprint subclass still
constructs different inherited native components. Embermere's native enemy
constructor produced the intended 48 target-circle segments, but the saved
`BP_StarterEnemy` generated class retained 24 older native component templates.
Compiling and saving the Blueprint did not rebuild that inherited template set,
so the native test was green while the real PIE enemy lacked the complete
runtime contract.

For native presentation-component migrations:

- inspect both a direct native instance and an instance created from the saved
  Blueprint's `GeneratedClass`;
- assert the exact component count and collision contract on the latter;
- do not assume Blueprint compile/save refreshes inherited native subobject
  templates after constructor component counts or names change;
- reconcile bounded presentation-only components during runtime initialization
  when replacing the Blueprint is unnecessary or risky;
- reuse valid inherited components, create only missing transient components,
  and reassert mesh, material, mobility, collision, navigation, shadow, and
  initial visibility properties;
- finish with clean PIE, because runtime component eligibility still does not
  prove surface readability.

Embermere now reconciles target-circle segments in
`PostInitializeComponents`, then its Prowler presentation test instantiates the
actual Blueprint-generated class and proves exactly 48 non-colliding runtime
segments. This is intentionally a presentation repair; targeting, combat,
aggro, loot, and lifecycle authority remain unchanged.

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

## Decorative UMG Layers Must Not Own Interaction

A paper-doll illustration can make an equipment panel feel like an RPG without
becoming a second equipment system. Keep the decorative image and the
interactive slot grid as separate children of one overlay:

- add the illustration first and the slot grid second so controls remain on top;
- keep the image hit-test invisible and size it explicitly;
- resolve it from project data and collapse it cleanly when art is missing;
- preserve every slot's existing dimensions, labels, tooltips, and drop logic;
- test persisted source size and intended presentation bounds separately, then
  judge contrast and readability in clean PIE.

This keeps presentation replaceable while the atomic equipment contract remains
owned by the same buttons and components as before.

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

## Register Presentation Metadata After Gameplay Succeeds

Timed-status UI should not rediscover combat rules by switching on ability
names, nor should it start a second timer that can drift from gameplay. In
Embermere, the combat component first asks the stats component to apply the
generic buff or movement effect. Only after that operation succeeds does it
register the source `FEmbermereAbilityDefinition` as presentation metadata.

The stats component then exposes read-only snapshots containing source ability
data, remaining world time, and beneficial/harmful intent. The HUD consumes
those snapshots to resolve existing icons, names, descriptions, colors, and
countdowns. Reapplication refreshes one record by stable ability ID, while
expiration and the existing vital-reset path clear both mechanics and
presentation.

This ordering creates three useful gates:

- failed gameplay never produces a lying status icon;
- UI remains data-driven and cannot change effect power, duration, or movement;
- future VFX can subscribe to the same successful-effect metadata without
  becoming another gameplay authority.

Test the source ability ID, duplicate behavior, countdown, fixed layout bounds,
and respawn-style clearing at the object level. Then use clean PIE to judge
contrast, hover behavior, neighboring-panel stability, and whether a real
player can understand the status at a glance.

## Use Unreal Key Names And Paired Input Commands For Bounded PIE Probes

Synthetic key helpers do not all speak the same naming language. Slate accepted
`Four`, not the display label `4`, and a successful one-shot call still did not
prove that Embermere's later controller poll observed the intended state. For
repeatable hotbar probes, the editor console route was more reliable:

```text
Input.+Key Four
Input.-Key Four
```

Always release the key, refresh viewport focus before the next measured action,
and prove gameplay state through stats, transforms, logs, or visible UI. The
`slomo` command is useful for presentation inspection: speed up only long
cooldowns, restore normal time, then slow the world briefly to hold a status
row for a screenshot. Never leave altered time dilation or a pressed synthetic
key behind after the bounded check.

## Headless Map Loading Does Not Guarantee Registered Collision Bodies

Fresh commandlets are authoritative for persisted packages, object properties,
mesh bounds, materials, tags, transforms, and authored collision-element
counts. They are not automatically authoritative for world physics. A July 27
commandlet loaded the correct road-boundary map and assets but native line
traces missed even the known gate support because the headless editor world had
not registered its collision bodies. The same ray hit at the expected height in
the initialized live editor, and the full live trace validator passed.

Split the contract deliberately:

- use fresh-process validators for durable package and saved-map assertions;
- use an initialized editor or PIE world for native overlap and trace behavior;
- require each validator's explicit success marker and reject
  `LogPython: Error`, regardless of process exit status;
- treat a headless physics miss as a diagnostic to investigate, never as proof
  that saved collision is absent or present.

## A Prop's Front Is Part Of Its Placement Contract

Asset thumbnails and isolated Blender previews cannot establish how a prop
reads from the player's normal route. The supply chest imported cleanly and
passed every technical gate, yet its first valid yaw hid the lock and strongest
plank silhouette from PlayerStart. Inspect route-facing props from the actual
approach, then lock the accepted transform in the saved-map validator.

For interactable-looking or directional props, validate both kinds of truth:
technical eligibility covers mesh, materials, collision, persistence, and exact
transform; world acceptance covers front-facing readability, terrain contact,
route clearance, scale, and visual belonging.

The Fenwatch vendor stall added a less obvious variant: classic FBX mirrored
Blender local Y. Its first Unreal placement passed dimensions, materials,
transform, package, and collider-count checks, and even looked plausible in the
viewport. A native trace through the authored counter proved it faced away from
the quartermaster. Lock directional collision probes to meaningful local
features, not only actor yaw. The accepted map rotates the stall to yaw `180`
so the counter faces the customer approach, while separate traces keep the
approach and bypass clear.

## Blueprint Defaults Do Not Rewrite Saved Placed Instances

Changing a component or soft asset reference on a Blueprint class default
object does not guarantee that already-placed level instances inherit it. The
Marsh Prowler Blueprint CDO held the correct skeletal mesh and six animations,
but all three saved map actors still serialized `CharacterMesh0=None`.

For presentation migrations:

- update and save the Blueprint CDO;
- enumerate every relevant saved level instance;
- apply the same component and presentation contract where stale overrides
  exist;
- save the Blueprint and level packages explicitly;
- reload the map in a fresh process and validate every instance, not only the
  class default.

Class validity and placed-world validity are separate acceptance gates.

## Build Material Graphs Outside PIE

Unreal can let a script mutate a material graph while PIE is active and then
refuse to save the package. That creates a dangerous split: the live object may
be disconnected or partially rebuilt while the package on disk is still
healthy. Continuing to inspect only the live graph can make a successful saved
asset look corrupt.

Stop PIE before destructive graph construction. Validate exact expression
classes, count, parameter names, and output connections; save the package; then
restart or reload in a fresh process before visual acceptance. If a PIE-time
save fails, do not keep repairing the in-memory object blindly. Confirm the
disk package first and use a controlled restart to restore its authoritative
state.

## Use The Dedicated Unreal MCP Launch Flags

UE 5.8 can start the MCP listener during editor launch with:

```text
-ModelContextProtocolStartServer -ModelContextProtocolPort=8123
```

On macOS, the `.uproject` path must follow `open ... --args` so it is forwarded
to Unreal. Passing the app bundle directly with `-n` avoids LaunchServices
reducing a spaced project path to its basename:

```bash
open -n "/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app" \
  --args "/Users/wizard/Documents/Unreal Game/Embermere.uproject" \
  -ModelContextProtocolStartServer -ModelContextProtocolPort=8123
```

Putting the project before `--args` opened the editor without forwarding the
project. Generic `-ExecCmds=ModelContextProtocol.StartServer 8123` also ran too
early during startup. Use the plugin-specific flags or start the listener from
the editor console after initialization.

## Keep GUI And Commandlet Verification Isolated

The editor GUI, PIE, and commandlets all touch shared Unreal state. Run
commandlets sequentially with the GUI closed when authoritative package checks
matter. A commandlet exit code is not enough for Python validators: require the
expected success marker and reject any `LogPython: Error`.

Before closing the GUI, explicitly save every intentional asset and map package.
Do not treat a Slate Save shortcut or the editor's unsaved-status indicator as
proof of disk persistence. A stuck status interaction can block MCP and normal
shutdown; after explicit package saves, a fresh commandlet reload is the
authoritative recovery check.

## Component Overrides Do Not Repair Vendor Dependencies

A project-owned component material override can make a vendor mesh render
correctly in the level while the underlying static-mesh package still
references missing vendor materials or textures. That is a valid temporary
presentation fix, not a repaired dependency graph.

Keep the override in reproducible placement scripts and validate visible
components, but continue treating fresh-process load warnings as real. Replace
incomplete vendor meshes with project-owned assets or a complete signed-in pack
instead of editing or redistributing raw Marketplace packages.

## Let World VFX Subscribe To Gameplay-Owned Status Metadata

Once timed-status metadata already has a reliable gameplay owner, world VFX
should consume that read-only contract instead of inventing a second effect
system. Embermere's first status aura attaches eight fixed non-colliding mesh
segments to the base character and reads only
`GetActiveStatusEffects()`. Gameplay does not know that those segments exist.

This keeps the dependency direction clean:

- combat applies the generic buff or control effect;
- stats owns duration, refresh, expiration, death, and respawn clearing;
- HUD and world VFX independently render the same successful-effect snapshots;
- presentation can change material, color, scale, pulse, or rotation without
  changing power, armor, movement, targeting, or timing.

Give overlapping effects a deterministic visual priority rather than stacking
unbounded geometry. Embermere currently lets harmful control outrank beneficial
buff presentation, then chooses stable palettes by effect category. Test
component count, material provenance, hidden/dead state, representative effect
colors, priority, and clearing in automation. Use clean PIE for the acceptance
question automation cannot answer: does the effect read at the real camera
distance without obscuring the character, target ring, nameplate, or HUD?

## World VFX Need Both Footprint And Surface Contracts

A presentation test can prove eight visible components, correct colors, no
collision, and authoritative status clearing while the actual gameplay effect
is still unreadable. Embermere's first harmful aura passed those structural
checks but orbited the character root, which placed most of it inside the Marsh
Prowler's torso at the normal camera distance.

Grounded creature indicators need two independent answers:

- footprint: derive a bounded radius from the transformed visual mesh rather
  than assuming the player-sized default fits every creature;
- surface: trace the supporting world surface and apply a PIE-proven clearance
  rather than treating actor origin or capsule center as ground;
- hierarchy: keep the harmful inner aura visually inside the calmer target
  identity circle so status and selection do not compete;
- fallback: retain conservative radius and capsule-base behavior when a mesh or
  initialized world is unavailable;
- acceptance: assert geometry, containment, state, and palette contrast in
  automation, then use normal-camera PIE to decide whether the result reads.

The accepted Marsh Prowler harmful aura resolves to a `94.352` cm radius and
`21` cm above the support hit. Snare uses marsh green; Frost Root uses a lighter
ice-white cyan (`0.46, 0.92, 1.0`) so its eight moving inner segments remain
distinct from the stationary outer cyan target circle. The gameplay effect
contract did not change; only its subscriber learned the creature and ground.

## Temporarily Change Mobility Only For PIE Placement Diagnosis

A static mesh component can silently reject a runtime transform change in PIE
because its mobility is `Static`. The MCP call can succeed while the actor stays
put, making a good placement candidate look ineffective. That happened while
diagnosing the supply chest that blocked straight autorun near Mara.

For a bounded diagnostic:

1. Record the original component mobility and transform.
2. In PIE only, set the component to `Movable`.
3. Apply the candidate transform and prove the route with pawn transforms or
   native traces;
4. stop PIE so the diagnostic mutation is discarded;
5. apply the accepted transform to the editor world, restore the intended
   static contract, save the exact map package, and validate it in a fresh
   process.

The saved-map validator should enforce both the accepted transform and a
geometric clearance invariant. An initialized-world trace should separately
prove the player-height route. This converts a camera-side visual fix into a
reproducible traversal contract without leaving production scenery movable.

## Finite Prototype Worlds Need A Recovery Contract

A sustained synthetic `Q` probe succeeded and kept moving after it cleared the
known village blocker. Transform sampling exposed the next truth: the player
eventually left the finite foundation and fell to roughly `Z=-249000`.
Input success alone would never have revealed that state.

Treat long synthetic movement as a measured experiment:

1. refresh and focus the current PIE viewport;
2. pair every key press with a release unless intentionally testing a hold;
3. sample actor transforms to prove movement and its eventual destination;
4. restore or recover any state the probe leaves behind.

For a finite prototype, define a controller-owned out-of-bounds plane and feed
it into the existing death/recovery system. Embermere's `Z=-1000` contract
cancels autorun and forces death even during temporary damage immunity. Recovery
uses physics-aware teleporting, clears velocity, restores the walking movement
mode, initializes vitals, and then reapplies short damage protection.

Validate the whole state vector, not just the destination: death message,
autorun off, frozen movement, respawn location, health, movement mode, velocity,
and protection feedback. A teleport that leaves falling velocity or disabled
movement behind is not a successful respawn.

## Validate Architecture As Composition And Traversal

Static-mesh acceptance does not end at topology, materials, bounds, import
provenance, collision, and a legal actor transform. Embermere's first Fenwatch
shelter passed all of those checks at its initial placement and still failed:
from the normal gameplay camera it hid Mara, the first quest giver.

Treat village architecture as two simultaneous contracts:

- composition: the building supports NPC silhouette, marker, nameplate, and the
  intended approach rather than competing with them;
- traversal: authored supports are solid, intended openings and overhead spans
  are clear, and existing player routes remain measurable;
- persistence: the accepted mesh, transform, removed placeholders, and collider
  count survive a fresh saved-map load;
- judgment: clean PIE can reject a technically valid transform and require a
  new one before the package is accepted.

The final Fenwatch shelter uses four support-only `UBX_` boxes and a clear
center. Native traces prove those physical roles independently. A transform-
measured `Q` probe plus an independent `W` cancellation proves the real spawn
route, while clean PIE confirms Mara reads in front of the shelter. Exact tests
protect eligibility; the gameplay camera decides belonging.

## Reconcile Blueprint Templates And Placed Instances Separately

Swapping a mesh on a Blueprint-backed actor does not guarantee that either the
saved class template or an already placed actor has shed the placeholder's old
state. Mara's new keeper mesh initially inherited the cube visual's offset,
stretched scale, facing, and blocking collision even though the asset reference
itself was correct.

Treat a Blueprint visual replacement as two persistence contracts:

- class template: in editor Python, use `SubobjectDataSubsystem` to locate and
  update the real SCS component template; querying a transient CDO/native actor
  can miss the Blueprint-authored node;
- placed instance: reconcile the serialized component on every saved map actor
  that can retain per-instance mesh, transform, material, or collision
  overrides;
- automation: when the contract belongs to an SCS component, load the saved
  Blueprint and inspect the actual SCS node template instead of constructing a
  transient `NewObject` fixture that never executes the construction script;
- validation: save both packages, reload them in a fresh process, and assert the
  mesh, relative transform, scale, collision, and instance tag independently;
- acceptance: run clean PIE to verify grounding, facing, marker/name clearance,
  interaction, and composition from the normal approach.

This keeps gameplay ownership asset-agnostic while preventing historical
placeholder state from silently surviving an otherwise successful art swap.

## Make Static-To-Skeletal NPC Swaps An Executable Contract

Saying that NPC art is "swappable" is not enough. Without a concrete owner for
both presentation lanes, a static placeholder tends to accumulate interaction,
service, transform, and collision assumptions that make a later rigged upgrade
another migration.

Embermere's `AEmbermereNpcPresentationActor` makes the boundary executable:

- static and skeletal components live behind one project-owned wrapper;
- both consume one authored relative transform;
- mesh and animation references are soft and presentation-only;
- only the resolved lane is visible, with deterministic fallback;
- both lanes are permanently non-colliding and nav-irrelevant;
- the actor exposes no interaction, vendor, trainer, dialogue, or quest state.

Test the swap itself, not only the first asset. The native contract test loads
a static NPC, switches to a real skeletal asset, verifies that the transform is
unchanged, and rejects collision or interaction ownership in both modes. A
second test loads the saved quartermaster mesh and exact world-facing contract.
Fresh map validation then proves the chosen lane and transform survived disk,
while clean PIE decides whether the merchant silhouette belongs beside the
chest without competing with Mara or the player route.

This pattern does not replace a service actor. It gives that future service a
stable visual dependency whose implementation can evolve independently.

## Do Not Make Native CDOs Depend On Generated Data Assets

The first vendor-service build used a constructor `FObjectFinder` for
`DA_FenwatchQuartermasterStock`. It worked after the asset existed, but a fresh
commandlet exposed load errors during the bootstrap pass that creates the data
asset itself. The class default object had made generic service construction
depend on project content that was not guaranteed to exist yet.

Keep reusable native classes content-neutral. Let the saved actor instance,
Blueprint default, or explicit setup script assign generated data assets, then
validate that serialized reference in a fresh process. This lets the service
class construct during asset creation, tests, migrations, and stripped maps
without producing false load failures or silently inheriting one merchant's
stock.

## Transaction Tests Need Rejection State, Not Only Success

A vendor purchase is not safe because the happy path subtracts copper and adds
an item. It is safe when every rejected path leaves all owners unchanged.

Embermere's purchase contract preflights request validity, finite stock,
affordability, and complete bag capacity before mutation. Commit charges the
wallet, attempts the complete inventory add, refunds an unexpected add failure,
and decrements finite stock only after success. Tests assert copper, stock, and
item quantity after success, sold-out retry, full bag, and insufficient funds.

The UI consumes the same preflight result to disable impossible actions and
show useful copy, but it is not a security boundary. The component remains
authoritative when called from tests, future Blueprints, or another UI.

Clean PIE added the final judgment gate: the first successful purchase worked
but its wrapped result crossed the footer. Reserving a fixed two-line status
cell and repeating the actual buy proved the transaction and its presentation
belonged together without making layout part of gameplay rules.

## Rollback Needs An Exact Inverse At Each Ownership Boundary

Buying, selling, and buyback are related UI verbs, but they are not one generic
transaction. Each crosses wallet and inventory ownership in a different order,
and each needs a deliberate inverse for the mutation that can fail second.

Embermere's accepted order is explicit:

- purchase spends copper, attempts the full inventory add, and refunds before
  finite stock changes if delivery unexpectedly fails;
- sale credits copper, removes the exact selected item identity, and retracts
  the credit if removal unexpectedly fails;
- buyback spends the recorded sale price, restores that exact identity without
  fake loot feedback, and refunds if capacity changed before delivery.

Preflight prevents normal rejection paths from mutating anything. The inverse
operation protects against the narrow gap between preflight and commit. Buyback
history is recorded only after a sale commits, and decremented only after a
buyback delivers, so rollback never fabricates or destroys a history entry.

Test the complete state vector after every failure: copper, item quantity and
identity, finite stock, and buyback quantity. Then exercise the same sequence in
PIE because correct numbers do not prove that selection, disabled states, chat,
or fixed panel copy communicate the transaction correctly.

## Treat A Save File As An Untrusted Multi-Owner Transaction

A save that deserializes is not necessarily safe to apply. It can reference a
deleted asset, carry an old format version, exceed current bag capacity, name a
vendor that no longer exists, or encode quest progress that would pay a reward
twice. Mutating each owner while resolving later records turns one bad field
into a partially loaded world.

Embermere loads in two phases. First it resolves every data-asset path and
checks the paired stable ID, version, quantity, slot, level, capacity, quest,
vendor-set, and finite-stock invariant. Only a completely eligible candidate
reaches the commit phase. Rejection leaves the live wallet, inventory,
equipment, quest, stock, and session buyback unchanged.

Make restore operations idempotent too. Equipment bonuses must replace prior
bonuses rather than stack on them, and a completed quest must remain unable to
pay again. Test the same valid save twice, not only once.

Finally, prove disk persistence across world lifetimes. Embermere's live
validator builds progression through real vendor, quest, inventory, and
equipment APIs, saves, ends PIE, starts a fresh PIE world, loads, and checks the
whole state vector twice. An in-memory archive test proves serialization logic;
the two-session run proves the actual slot and lifecycle integration.

## Let Save UI Request A Proven Contract

A player-facing Save/Load panel should not become a second persistence system.
Embermere Chronicle only inspects the local slot, presents its summary, asks for
deliberate confirmation, and forwards Save or Load to the controller's existing
atomic persistence boundary. The persistence library still owns validation,
error meaning, and mutation.

Keep inspection read-only and loading authoritative. A lightweight slot summary
can identify an empty, unreadable, or unsupported-version file before enabling
Load, but missing assets and full cross-owner eligibility still belong to the
real load preflight. Surface that returned failure to the player instead of
duplicating asset-resolution rules in the widget.

Confirmation is transient UI state, not durable game state. Test the cancel
path independently, make Inventory/Vendor/Save panels mutually exclusive, and
restore game-only input on close. Then repeat the existing two-session proof
through the new surface. A polished button proves nothing if it bypasses the
idempotent load contract beneath it.

## Separate Saved Animation Intent From Live Playback State

A skeletal component can look fully configured while no animation player
exists. Embermere's first NPC Idle pass held the exact skeletal mesh, exact
animation asset, `AnimationSingleNode` mode, looping intent, playing intent,
and `0.75` saved play rate. Native tests and fresh package inspection were all
green, but live PIE remained at position `0.0`, reported `playing=false`, and
returned a runtime play rate of `0.0`.

The UE 5.8 lifecycle explained the mismatch. `SetAnimInstanceClass(nullptr)`
clears the transient animation instance but does not change the component's
single-node mode. `OverrideAnimationData()` then sees no mode transition, so it
serializes correct intent without calling the path that creates a new
`UAnimSingleNodeInstance`. Calling `SetAnimation`, `SetPlayRate`, or `Play`
cannot repair that state because each operation first asks for the missing
single-node instance.

The accepted sequence is:

1. Resolve a skeletal mesh and skeleton-compatible Idle asset.
2. Clear any Anim Blueprint class.
3. Store the exact asset, loop, playing, position, and rate through
   `OverrideAnimationData()` so construction and package state remain durable.
4. If the component is registered, call `InitAnim(true)` to create a runtime
   instance from that saved data.
5. Prove playback, not configuration: sample `IsPlaying()`, `GetPlayRate()`,
   and `GetPosition()` twice in live PIE.

The final Embermere run moved from `0.0` to `1.4153` seconds while remaining
playing at `0.75x` and `NoCollision`. The diagnostic changed only the PIE copy
of the quartermaster wrapper and was discarded when PIE stopped; a fresh
validator separately proved the saved quartermaster still uses its reviewed
static lane.

The broader lesson is useful beyond animation. Serialized intent, resolved
component state, and active runtime behavior are three different claims. Test
all three whenever an Unreal feature has both construction-time and transient
instances.

## Keep Source And Imported Topology As Separate Contracts

An FBX importer may legally change geometry even when the asset succeeds.
Blender reported 2,824 triangles for the Fenwatch armsmaster, while Unreal's
classic `FbxFactory` removed 24 degenerate triangles and saved a 2,800-triangle
static mesh. Treating either number as universally authoritative would make the
other acceptance gate look broken.

Record both artifacts deliberately:

- the Blender metrics file locks authored topology, dimensions, UV channels,
  manifold state, pivot, and scale;
- the Unreal package test locks the post-import triangle count, bounds,
  materials, collision, and import provenance.

The difference itself is not a failure. An unexplained or drifting difference
is. This distinction also makes future Blender or Unreal importer upgrades
reviewable instead of silently normalizing the baseline.

## Observe SOutputLog Before Driving Its Python Textbox

The Unreal MCP Slate snapshot initially exposed the bottom Output Log as one
menu containing only the `Cmd`/`Python` selector. The visible command textbox
had no ref, so reported-success paste and key calls could not execute the live
road validator. macOS accessibility correctly rejected a process-level
keystroke fallback.

The supported solution is narrower and more reliable:

1. Snapshot the editor window and identify the Output Log menu ref.
2. Select Python mode.
3. Call `Observe` on the Output Log menu itself with sufficient depth.
4. Snapshot that subtree. The internal `SMultiLineEditableTextBox` then appears
   as a focused `textbox` ref.
5. Use Slate `Type` on that exact ref with `submit=true`.
6. Read logs through `EditorToolset.LogsToolset` and require the validator's
   explicit success marker with no `LogPython: Error`.

This preserves the localhost MCP and native Slate boundary, avoids OS-level UI
injection, and works for initialized-world validators that cannot be accepted
from a null-render commandlet alone.

## Separate Skeletal Cleanup, Creation, And Validation Lifecycles

Deleting a SkeletalMesh and its Skeleton immediately before rebuilding both in
one Unreal commandlet looks deterministic, but the editor process can still
hold UObject references to the deleted packages. Embermere's first rigged
armsmaster import reached a half-valid state: the files had been recreated, yet
the imported mesh no longer exposed a valid Skeleton to the next operation.

The first durable importer treated a valid saved Skeleton as shared state:

1. List existing generated assets and load the expected Skeleton first.
2. Reject an existing SkeletalMesh whose Skeleton is unexpectedly missing.
3. Import with replacement enabled and explicitly reuse the valid Skeleton.
4. Save the SkeletalMesh, Skeleton, and AnimationSequence packages.
5. Validate all three from a fresh commandlet before opening PIE.

The quartermaster pass exposed another UE 5.8 edge: replacing an existing
skeletal package could silently attach `InterchangeAssetImportData` even when
the script supplied a classic `FbxFactory`. An importer cannot prove classic
provenance merely because it requested that factory.

The stronger lifecycle is now:

1. A routine rerun loads existing assets, rejects bad provenance or missing
   dependencies, and returns without importing when the complete classic-FBX
   contract is already eligible.
2. An intentional source rebuild runs a narrowly scoped cleanup-only script in
   its own Unreal process.
3. A second fresh process creates the SkeletalMesh, Skeleton, and animation
   through classic `FbxFactory`; replacement is disabled because no generated
   package should exist.
4. A third fresh commandlet validates package class, classic import data,
   Skeleton, bone hierarchy, materials, bounds, animation duration, and saved
   wrapper state.
5. Live PIE independently proves that the animation clock advances.

Do not combine deletion and creation in one process, and do not use replacement
as a provenance mechanism. Normal reruns are idempotent because they validate
eligible assets; rebuilds are deterministic because cleanup, creation, and
acceptance do not share stale UObject state.

Unreal Python exposed a related API boundary during the same pass:
`USkeletalMesh.set_material()` is not available. Update each
`FSkeletalMaterial.material_interface` in the mesh's material array, write the
array back, save, and validate exact slot order in a fresh process.

## Validate Authored Bones And The Imported Root Separately

Blender authored nine named bones for the Fenwatch armsmaster, but Unreal's
classic FBX pipeline preserved the Blender Armature object as one additional
root. A native test that expected only nine Unreal bones failed even though all
nine authored bones, weights, animation, and hierarchy were correct.

The accepted contract records both artifacts:

- Blender metrics require the nine reviewed authored names, applied armature
  scale, complete rigid weights, and the exact 3.2-second action;
- Unreal requires ten reference-skeleton bones, all nine authored names, and
  the authored `root` parented beneath the one imported root;
- the AnimationSequence must share that exact Skeleton and advance in live PIE.

Raw bone count alone is not a portable truth. Validate the authored semantic
hierarchy and document any importer-owned node so a future exporter or engine
upgrade produces a reviewable contract change instead of a mysterious failure.

## Migrate Blueprint-Backed NPC Art Without Replacing Gameplay Ownership

The armsmaster and quartermaster already had separate art and service actors,
so their static-to-skeletal upgrades changed only wrapper references. Mara was
different: her saved `BP_QuestGiver` already owned the interactable component,
display name, dialogue, quest data, marker, and reward flow. Replacing that
actor with a presentation wrapper would have moved authority or forced a second
copy of gameplay state.

The accepted migration starts by fingerprinting the live Blueprint CDO, SCS
template, and saved map instance. Keep the gameplay actor and every authoritative
component. Clear only the old render mesh, preserve its local transform and
`NoCollision` state as a dormant fallback template, then compose the actor and
visual transforms into a colocated art-only wrapper. Move the project-art tag
to the actor that actually renders the art so saved-map counts remain honest.

Validate both halves independently:

1. The Blueprint keeps its exact class, transform, interactable, name, dialogue,
   quest reference, marker settings, and reward ownership.
2. The wrapper keeps the exact SkeletalMesh, Skeleton, Idle, static fallback,
   world transform, `NoCollision`, and absence of quest or interaction
   components.
3. Fresh package and map validators prove the serialized contract.
4. Native tests construct the real generated class rather than only a native
   approximation.
5. PIE proves visual composition and animation-clock advancement; a player
   still walks into the real interaction radius for final dialogue/quest
   acceptance.

This keeps presentation asset-agnostic without pretending every NPC began with
the same architecture. A migration is successful when art changes owners and
gameplay authority does not.

## Let Context Presentation Observe Authority Instead Of Recreating It

A contextual NPC greeting can look like a tiny UI feature while quietly
becoming a second quest system. If presentation derives its own progress,
handles input, or grants outcomes, the world can disagree with the actual
quest log and replay rewards.

Embermere's Mara greeting keeps the dependency one-way:

1. The quest data asset owns the short available, active, ready, and completed
   copy alongside the authoritative quest definition.
2. The presentation wrapper holds an explicit reference to the existing
   `BP_QuestGiver` instead of guessing authority from proximity or tags.
3. It subscribes only to the player's authoritative quest-state signal and
   resolves a read-only visual state.
4. A fixed, hit-test-invisible, `NoCollision` widget handles range visibility
   and presentation without input, dialogue, marker, progression, or reward
   behavior.
5. Native automation proves every state and the absence of gameplay authority;
   clean PIE still uses physical `F` against the original interactable and
   verifies exact rewards plus no replay on a second interaction.

This pattern scales beyond quests. A service NPC can react to stock, training,
faction, or world state without becoming its owner. Treat the authoritative
system as a publisher and presentation as a disposable subscriber.

## Treat Preview Framing As Part Of The Asset Contract

A deterministic Blender build can pass topology, bounds, UV, scale, and
collision checks while its evidence render is still misleading. The first
Fenwatch cottage preview clipped the roof because the camera's far plane was
too short for the new architectural depth. Nothing was wrong with the mesh,
but the image could not prove that.

Keep preview camera location, lens, target, and clipping ranges in the tracked
build script. Render at the accepted resolution and inspect the original file,
not only a thumbnail. Once the cottage fit in frame, that full-size review
caught small door-jamb gaps that numeric checks could not see.

Closed architecture also needs paired collision evidence:

1. exact saved-package checks prove the intended body and doorstep colliders
   exist;
2. initialized-world traces prove those surfaces actually block;
3. negative traces prove roof, chimney, trim, and other visual details do not
   create invisible walls;
4. route traces prove the solid building did not consume neighboring travel
   lanes;
5. normal PIE proves the new silhouette improves composition without hiding
   markers, services, or contextual presentation.

A preview is not decoration for the pipeline. It is one of the acceptance
artifacts, and its own correctness should be reproducible.
