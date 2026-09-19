# September 19 Daily Build

## Fenwatch Hitching And Feed Trough

Today's bounded milestone came from fresh viewport evidence rather than another
northwest dwelling. The west cottage already had a handcart and firewood rack,
but the open ground beyond them still read as unused space. A factory-clean
Blender MCP build ran the reviewed
`Scripts/blender/build_embermere_fenwatch_hitching_trough.py` source and wrote
editable `.blend`, classic FBX, preview, and deterministic metrics under
`ArtSource/Blender/Environment/FenwatchHitchingTrough/`.

The accepted `SM_EmbermereFenwatchHitchingTrough_01` contract is:

- grounded `367.15 x 133.903 x 189` cm bounds and applied unit scale;
- 3,876 triangles, one UV channel, and zero non-manifold edges;
- five shared Fenwatch stone, moss, timber, iron, and ember materials;
- two authored UBX boxes for the broad trough body and hitching rail;
- visual-only feed surface, stone feet, iron bands, three hitch rings, caps,
  braces, trim, and ember crest.

Classic `FbxFactory` import explicitly saved the mesh. First-class Unreal MCP
then placed `Embermere_FenwatchHitchingTrough_CottageWest_01` at
`(-3550, -520, 0)`, yaw `38`, assigned its project-owned tag and outliner
folder, retained `BlockAll`, and saved the level. The actor remains a removable
presentation-only `StaticMeshActor` with no quest, interaction, service,
recovery, reward, vendor, trainer, or persistence authority.

Normal and opposite-side viewport captures accepted the trough as part of one
readable working-yard cluster with the handcart, firewood rack, and west
cottage. Its open feed bed, hitch rail, rings, crest, and stone feet remain
legible without closing the surrounding grass lanes. Saved spacing is 648.2 cm
from the handcart, 640.3 cm from the firewood rack, 1,101.1 cm from the west
cottage, 1,336.0 cm from PlayerStart, and 1,535.9 cm from Mara. The focused
package validator locks those relationships plus classic import provenance,
geometry, materials, collision count, transform, tag, and ownership.

The initialized-world native trace suite proves the trough body and hitching
rail solid, keeps its thinner decorative forms clear, and preserves the
PlayerStart-to-Mara, west-cottage-bypass, cottage-to-handcart, west-utility,
and village-to-road routes. The map baseline is now **53 grounded Fab actors
plus 28 original-art placements**.

## Import And Placement Boundary

The first NullRHI attempt successfully imported and saved the mesh, then
crashed while `spawn_actor_from_object` entered Unreal's placement subsystem.
That did not justify weakening the asset or repeating a fragile commandlet
spawn. The final lane separates responsibilities explicitly:

- the fresh commandlet owns classic FBX import, shared-material reassignment,
  exact mesh/collision validation, and package saving;
- the real initialized editor and first-class Unreal MCP own actor placement,
  tagging, folder assignment, collision-profile inspection, and map saving;
- fresh commandlets remain authoritative for persisted package/map contracts;
- initialized live-editor traces remain authoritative for registered world
  collision.

The first headless trace also missed the new body, exactly matching the
project's documented limitation that NullRHI map loading does not guarantee
registered physics bodies. The same script passed in the initialized editor;
all retained suites passed there as well. A headless physics miss was treated
as a diagnostic, never as proof that the two saved UBX boxes were absent.

## Verification

- The only installed toolchain remains `/Applications/Xcode.app` with Xcode
  and SDK `27.0`. UE 5.8 declares `26.9` as its maximum, so no compatible
  side-by-side Xcode is present and the known-invalid C++ build was not
  replayed.
- The final classic-FBX import emitted
  `Embermere Fenwatch hitching-trough asset import passed` with 3,876 triangles
  and two boxes, no `LogPython: Error`, and exit zero. Evidence:
  `/tmp/embermere-sep19-hitching-trough-import.log`.
- The focused fresh package validator emitted its exact success marker with no
  Python error. Evidence:
  `/tmp/embermere-sep19-hitching-trough-validator.log`.
- All **ten initialized-world collision/route suites** passed sequentially:
  road boundary, vendor stall, west cottage, training workshop, notice board,
  communal well, handcart, firewood rack, north cottage, and hitching trough.
  Every suite emitted its exact marker in the real editor world.
- All **27 fresh package validators** passed sequentially with the exact
  aggregate marker, the 53-Fab/28-original full-zone result, and no
  `LogPython: Error`. Evidence:
  `/tmp/embermere-sep19-package-aggregate.log`.
- Fresh MCP discovery returned exactly 100 Embermere tests. All **100/100**
  passed with zero failures, warnings, skipped, or unrun records.
- Clean PIE started and stopped. The saved map and hitching-trough package both
  reported clean outside PIE.
- The protected journey save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  The existing local keeper material remains SHA256
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

No Save/Load action occurred. Existing Config, keeper-material, and unpublished
Field Notes changes remain excluded. Desktop control stayed disabled; visual
work used Blender MCP and Unreal viewport-only capture, while live validation
used Unreal-owned Slate and first-class MCP.

## Next Start

Check once for a compatible side-by-side Xcode. If one exists, use only its
`Contents/Developer` through process-local `DEVELOPER_DIR`, prove the real
`-NoHotReloadFromIDE` build, then review and compile the pending Ledger focus
regressions before production repair. If compatibility remains blocked, use
fresh viewport evidence to choose one distinct bounded MCP-backed milestone
outside the now-complete west-cottage utility cluster, or strengthen a
saved-package/live-world regression that protects visible player value.
