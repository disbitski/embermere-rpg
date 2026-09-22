# September 22 Daily Build

## North-Commons Herb Garden Accepted

Fresh viewport inspection deliberately moved outside the completed west-cottage
utility cluster and east-road gatewatch lane. The north commons beyond the
covered cottage remained open but visually empty, so a low project-owned herb
garden now gives that edge a cultivated village use without adding gameplay
authority or a route-blocking building.

The deterministic factory-clean Blender MCP build wrote editable source,
classic FBX, preview, and exact metrics under
`ArtSource/Blender/Environment/FenwatchHerbGarden/`. The accepted
`SM_EmbermereFenwatchHerbGarden_01` contract is:

- grounded `356 x 212 x 195` cm bounds and applied unit scale;
- 4,312 triangles, one UV channel, and zero non-manifold edges;
- five shared Fenwatch stone, moss, timber, iron, and ember materials;
- four authored UBX boxes for the two raised beds and two trellis posts;
- visual-only central aisle, herbs, trellis beam/leaves, signs, pail, trim, and
  ember crest.

A fresh classic-`FbxFactory` commandlet imported and explicitly saved the mesh.
First-class Unreal MCP then placed
`Embermere_FenwatchHerbGarden_NorthCommons_01` at `(-1500, 1400, 0)`, yaw
`-15`, with its project-owned tag, original-art folder, `BlockAll` profile, and
presentation-only `StaticMeshActor` ownership. Overhead, village-approach, and
east-side viewport captures accepted the low grounded composition.

The final center is 1,232.2 cm from the north cottage, 2,316.2 cm from Mara,
2,751.4 cm from PlayerStart, 1,661.1 cm from the notice board, and 2,676.0 cm
from the gatewatch post. The garden owns no quest, interaction, service,
recovery, reward, vendor, trainer, or persistence behavior.

## Verification

- The fresh import commandlet emitted its exact success marker with 4,312
  triangles and four boxes, exit zero, no warnings, and no `LogPython: Error`.
  Evidence: `/tmp/embermere-sep22-herb-garden-import.log`.
- Fresh MCP discovery returned exactly 100 Embermere tests. All **100/100**
  passed with zero failures, warnings, skipped, or unrun records.
- Clean PIE started and stopped through first-class Unreal MCP.
- The focused package validator locked classic-FBX provenance, dimensions,
  triangles, five material assignments, four boxes, exact transform/tag/
  profile, art-only ownership, and protected spacing.
- All **twelve initialized-world collision/route suites** passed sequentially:
  road boundary, vendor stall, west cottage, training workshop, notice board,
  communal well, handcart, firewood rack, north cottage, hitching trough,
  gatewatch post, and herb garden. The garden suite proved all four purposeful
  surfaces, clear aisle/decoration, and clear north-commons south/perimeter,
  cottage-approach, and village-road lanes.
- The first garden trace draft began the cottage approach inside the existing
  cottage collider. That assertion was corrected instead of moving valid
  world art; the clean post-relaunch suite is the authoritative result.
- The full-zone validator passed with **53 grounded Fab actors plus 30
  original-art placements**.
- With the real editor/MCP process down and localhost 8123 free, the fresh
  sequential **29-package** aggregate passed with its exact aggregate and
  53-Fab/30-original markers, exit zero, and no `LogPython: Error`. Evidence:
  `/tmp/embermere-sep22-package-aggregate.log`.
- The aggregate retained only the four known vendor physics-resave warnings
  for FieldGrass, River_Rock, Medium_Boulder, and HillTree. No raw vendor
  package was resaved to hide them.
- The real editor was relaunched with the dedicated MCP flags. In that fresh
  session all twelve initialized-world suites passed with exact markers and no
  Python error, discovery returned exactly 100 tests, clean PIE started and
  stopped, and the map plus garden mesh remained clean outside PIE.
- The protected journey save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  The preserved keeper material remains SHA256
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
- Only `/Applications/Xcode.app` is present with Xcode/SDK 27.0, above UE
  5.8's 26.9 maximum. The known-invalid build was not replayed.

## Final State

The accepted map baseline is **53 grounded Fab actors plus 30 original-art
placements**. Unreal is left on `/Game/Maps/L_Embermere_Prototype`, outside
PIE, with the real editor owning MCP 8123 and both changed packages clean.
Existing Config, keeper-material, and unpublished Field Notes changes remain
excluded. No Save/Load action occurred, and desktop control remained disabled.

The compiler prerequisite remains separate and unchanged: only Xcode/SDK 27.0
is installed, above UE 5.8's declared 26.9 maximum. No known-invalid build was
replayed, engine limits were not weakened, and runtime/package evidence is not
described as a fresh C++ compile.
