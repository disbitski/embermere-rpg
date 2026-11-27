# Marshward Asset Quality Audit

## Decision And Scope

The November 27 read-only audit found no source/import mismatch requiring an
asset repair. Retain the accepted kit, materials, collision and LOD setup.
No `.blend`, FBX, mesh, material or map was rebuilt or saved. The loaded map
remains at 53 placements, 20 installed imported mesh types and 21 imported
packages including the unplaced original greybox marker. The Engine Cube is
excluded from the original-source comparison, but included in map inventory.

## Source Evidence

`Scripts/audit_marshward_sources_blender.py` reads 18 retained `.blend` files
in an isolated factory-startup/headless Blender process, not the user's open
scene. Each file's metrics manifest selects its declared exports; retained
donor objects do not become duplicate asset owners. No source is saved.

All 21 exported meshes match their retained triangle/dimension metrics and
have applied identity transforms, one UV channel, finite UV coordinates in
the measured range 0..1, and zero edges whose face incidence differs from two.
The report records each source file's SHA256. Material membership matches the
existing manifests; actual source slot order is retained for import comparison.
Manifest material lists are not an ordered slot contract.

This is not a UV-island overlap, texel-density, self-intersection, winding,
normal-quality or texture-bake acceptance. No second lightmap channel is added
without a separate lighting requirement. The active Blender scene was inspected
as healthy before the isolated audit and was not edited.

## Imported Evidence

`Scripts/audit_marshward_asset_quality_unreal.py` checks the clean, loaded
Causeway outside PIE. It requires exactly the declared imported set plus the
retained marker, verifies source-file hashes, and compares every mesh's LOD0
triangle count, UV count, material-slot order, local bounds and authored simple
box/convex counts. Full material paths must resolve to the existing Marshward
or reused PrototypeVillage packages. No mesh uses complex-as-simple collision.

Classic FBX mirrors local Y. Comparing both bounding endpoints, rather than
dimensions alone, protects asymmetric pivots such as the shore. Most assets
have centered XY bounds and a local Z=0 base. Important intentional exceptions:

| Asset | Source local Z bounds, cm | Role |
| --- | ---: | --- |
| Span | -60..0 | Top-center route-deck pivot. |
| Ramp | -40..120 | Authored wedge/support convention. |
| Still pool | -3..0 | Top-plane visual surface. |
| Pool shore | -4..-0.4 | Asymmetric strip at the unchanged pool pivot; source XY bounds approximately -49.135..483.959 / -275.231..229.475. |

The 21 imported packages total 4,016 LOD0 triangles including the retained
36-triangle marker; individual meshes range from 12 to 616 triangles. Each
currently has one render LOD and Nanite disabled. The source/import simple
inventory is three boxes and fifteen convex hulls across these unique assets.
This counts shapes, not every collider vertex, contact, or cooked body property.
Existing marker/body and registered-world route controls remain separate.

The refreshed map budget remains 4,028 unique installed triangles including
Engine Cube, 9,558 placed triangles, 91 sections, 13 effective materials,
37 BlockAll / 16 NoCollision actors, and 20 placed boxes / 32 convex hulls.
These are inventories, not valid GPU or draw-call measurements. The earlier
unavailable RHI counters do not justify new LODs, merging or geometry reduction.

## Reproduction

First run through Blender MCP with headless transport and factory startup:

```python
import runpy
report = runpy.run_path('/Users/wizard/Documents/Unreal Game/Scripts/audit_marshward_sources_blender.py')['main']()
```

This generates `/tmp/embermere-marshward-source-quality.json`, not a source
asset or public baseline. Then use Unreal's cleared Python-mode Output Log on
the settled clean Causeway map, outside PIE:

```python
import runpy; runpy.run_path('/Users/wizard/Documents/Unreal Game/Scripts/audit_marshward_asset_quality_unreal.py', run_name='__main__')
```

Require `EMBERMERE_MARSHWARD_SOURCE_QUALITY_SUCCESS` for 18 files / 21 meshes
and `EMBERMERE_MARSHWARD_ASSET_QUALITY_SUCCESS` for 21 packages, with no errors
or dirty packages. Loading packages in an initialized editor is not fresh-process
persisted-package validation; the November 24 package proof remains separate.

## Verification And Limits

All 80 focused host checks passed, including three new comparison regressions:
mirrored asymmetric bounds match; shifted/nonfinite/truncated bounds reject;
and UV, material, triangle, shape-count or complex-collision drift rejects.
The final source audit, loaded-package audit, budget, lighting/ownership and
complete 579-support / 1,104-clearance suite passed with all existing controls.
No November 27 `LogPython: Error` was logged. Earlier headless audit assumptions
about donor-object ownership and manifest material ordering were corrected;
those rejected attempts are not source defects or accepted verification.

Normal floating PIE reached the Human Warrior picker and stopped. The current
640 x 394 Slate window did not provide a new confirmed-HUD or physical-walk
check. The editor returned to clean Causeway outside PIE. Both maps and the
protected save/keeper retained their recorded hashes. No import, engine build,
full native or Fenwatch rerun was required for these read-only scripts.
Physical two-direction walking, moving-camera clearance and first-visit
wayfinding remain open; native collision rays do not prove foliage visibility.
