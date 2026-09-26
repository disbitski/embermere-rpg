# September 26 Daily Build

## Ancient Ruin Relief

Xcode remains outside UE 5.8's accepted Mac SDK range, so today's bounded
milestone changes no C++ or gameplay authority. Fresh normal-road and
first-Prowler viewport inspection found a broad blank west face on the
existing `FabPass_Ruin_Wall_A`. The ruin pocket already has enough ground
clutter; the new art mounts on that surface instead of adding another
freestanding obstacle.

The deterministic, factory-clean Blender build produced
`SM_EmbermereAncientRuinRelief_01` under
`ArtSource/Blender/Environment/AncientRuinRelief`: `206 x 55.805 x 216` cm,
2,900 triangles, one UV channel, zero non-manifold edges, four shared
project-owned stone/moss/iron/ember materials, and **zero authored simple
collision**. An initial road-view render read too much like a fitted metal
panel; the accepted revision uses moss-carved markings and a restrained ember
shard. The source `.blend`, FBX, preview, and metrics are retained.

A fresh commandlet imported through classic `FbxFactory`, remapped to the
four shared project materials, verified metrics/no collision, and explicitly
saved the mesh package. First-class Unreal MCP placed
`Embermere_AncientRuinRelief_WallA_01` at `(2302,350,0)`, yaw `90`, tagged and
foldered it as original presentation art, set its component to `NoCollision`,
and saved the map. The raw vendor wall remains untouched and solid. A reflected
`bodyInstance` edit alone did not update registered collision; the native
component setter did, and the live trace changed from hitting the relief to
clearing it. The focused suite now protects three clear rays through the
relief and three positive hits on the existing wall. All three protected
Prowler pull lanes remain covered by the retained sequential suite.

The relief owns no target, AI, quest, interaction, loot, reward, service,
navigation, or persistence behavior. No new save fields or gameplay data were
added. From the normal road view, the first Prowler and ruin silhouette remain
visible. This is editor-viewport acceptance, not a physical combat playtest.

## Verification

- Blender source validation passed: exact dimensions, 2,900 triangles, one UV
  channel, zero non-manifold edges, applied unit transform, four materials,
  and no collision geometry. Classic FBX import and reimport each emitted
  `EMBERMERE_ANCIENT_RUIN_RELIEF_IMPORT_SUCCESS`, exited zero, and had no
  `LogPython: Error`.
- All **100/100** native automation tests passed with no warnings or errors.
  Clean normal-route PIE started and stopped.
- With the real editor closed and MCP 8123 free, the fresh sequential
  **31-package** NullRHI commandlet passed. It emitted
  `EMBERMERE_FENWATCH_SKELETAL_MATERIAL_VALIDATION_SUCCESS`,
  `EMBERMERE_ANCIENT_RUIN_RELIEF_VALIDATION_SUCCESS`, the exact **53 grounded
  Fab / 32 original-art** full-zone marker, and `validators=31`; it exited
  zero with no `LogPython: Error`. Evidence:
  `/tmp/embermere-sep26-31-package-aggregate.log`.
- After a clean relaunch, all **16 initialized-world** suites passed
  sequentially with `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS` and no
  `LogPython: Error`. The focused relief suite emitted
  `EMBERMERE_ANCIENT_RUIN_RELIEF_TRACES_SUCCESS`. Fresh discovery returned
  exactly 100 Embermere tests. PIE started/stopped again; the real editor is
  outside PIE on `/Game/Maps/L_Embermere_Prototype`, owns localhost MCP 8123,
  and reports the map and new mesh clean.
- The user's save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper material remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
  Neither was staged or modified. Four known vendor physics-resave warnings
  remain confined to FieldGrass, River_Rock, Medium_Boulder, and HillTree;
  raw vendor packages were not resaved.

Only `/Applications/Xcode.app` is present with Xcode/SDK 27.0, above UE
5.8's declared 26.9 maximum. The known-invalid UBT build was not rerun or
called successful. Physical target switching, combat, and `F` interaction
remain unverified under the no-desktop-control rule. Once a compatible
side-by-side Xcode exists, use only process-local `DEVELOPER_DIR`, prove a
`-NoHotReloadFromIDE` build, then take the pending test-first Ledger focus
regressions. If it is still absent, the next useful MCP-only gate is the
existing quest-update observer's normal-PIE readability and lifecycle, not
another filler world prop.
