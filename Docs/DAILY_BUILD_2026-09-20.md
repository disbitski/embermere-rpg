# September 20 Daily Build

## Gatewatch Post Accepted

Fresh east-road viewport inspection identified a useful composition gap away
from the completed west-cottage utility cluster. A compact watch shelter gives
the road edge a readable village threshold without adding gameplay authority.
The deterministic Blender MCP build wrote editable source, classic FBX,
preview, and exact metrics under
`ArtSource/Blender/Environment/FenwatchGatewatchPost/`.

The accepted `SM_EmbermereFenwatchGatewatchPost_01` contract is:

- grounded `482 x 327 x 414` cm bounds and applied unit scale;
- 5,488 triangles, one UV channel, and zero non-manifold edges;
- five shared Fenwatch stone, moss, timber, iron, and ember materials;
- seven authored UBX boxes for the platform, entry step, rear wall, and four
  structural posts;
- visual-only roof, bell, side railings, map, horn, spears, trim, and ember
  crest.

A fresh classic-`FbxFactory` commandlet imported and explicitly saved the mesh
with the expected geometry, materials, and collision. First-class Unreal MCP
then placed `Embermere_FenwatchGatewatchPost_EastRoad_01` at
`(220, -650, 0)`, yaw `20`, with its project-owned tag, original-art folder,
`BlockAll` profile, and presentation-only `StaticMeshActor` ownership. The
initial candidate was moved 110 cm farther south after a second normal-route
capture gave the signpost and player lane more breathing room.

The final center is 520.0 cm from the signpost, 1,104.0 cm from the waystone,
1,468.2 cm from the road gate, 986.2 cm from the workshop, 2,278.8 cm from
Mara, and 2,677.1 cm from PlayerStart. It remains 870.5 cm from the protected
east-road centerline. The post owns no quest, interaction, service, recovery,
reward, vendor, trainer, or persistence behavior.

## Verification

- The fresh import commandlet emitted its exact success marker with 5,488
  triangles and seven boxes, exit zero, no warnings, and no `LogPython: Error`.
  Evidence: `/tmp/embermere-sep20-gatewatch-import.log`.
- Fresh MCP discovery returned exactly 100 Embermere tests. All **100/100**
  passed with zero failures, warnings, skipped, or unrun records.
- Clean PIE started and stopped through first-class Unreal MCP.
- The focused package validator passed in the initialized editor and locked
  classic-FBX provenance, dimensions, triangles, five material assignments,
  seven boxes, exact transform/tag/profile, spacing, and road clearance.
- All **eleven initialized-world collision/route suites** passed sequentially:
  road boundary, vendor stall, west cottage, training workshop, notice board,
  communal well, handcart, firewood rack, north cottage, hitching trough, and
  gatewatch post. The new suite proved all seven purposeful surfaces, clear
  decoration, and clear PlayerStart-to-Mara, village-to-road, east-road, and
  outer-bypass lanes.
- The updated full-zone validator passed with **53 grounded Fab actors plus 29
  original-art placements**.
- After the user approved graceful shutdown, the real editor/MCP process exited
  and localhost 8123 was proven free. The fresh sequential **28-package**
  aggregate then passed with its exact aggregate marker, the exact 53-Fab/
  29-original marker, exit zero, and no `LogPython: Error`. Evidence:
  `/tmp/embermere-sep21-package-aggregate.log`.
- The aggregate emitted only the four retained vendor physics-resave warnings
  for FieldGrass, River_Rock, Medium_Boulder, and HillTree. No raw vendor
  package was resaved to hide them.
- The real editor was relaunched with the dedicated MCP flags. In that fresh
  session all eleven initialized-world suites passed again with their exact
  markers and no Python error, discovery returned exactly 100 tests, clean PIE
  started/stopped, and the map plus gatewatch mesh remained clean outside PIE.
- The saved map and new mesh report clean outside PIE.
- The protected journey save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`.
  The preserved keeper material remains SHA256
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
- Only `/Applications/Xcode.app` remains present with Xcode/SDK 27.0, above UE
  5.8's 26.9 maximum. The known-invalid build was not replayed.

One stale partial Output Log prefix contaminated the first September 20
focused-validator submission and produced a contained syntax error. That
session was not used as final evidence. The September 21 isolated aggregate
and post-relaunch live suites are the authoritative clean acceptance runs.

## Final State

The accepted map baseline is **53 grounded Fab actors plus 29 original-art
placements**. Unreal is left on `/Game/Maps/L_Embermere_Prototype`, outside
PIE, with the real editor owning MCP 8123 and both changed packages clean.
Existing Config, keeper-material, and unpublished Field Notes changes remain
excluded. No Save/Load action occurred, and desktop control remained disabled.

The compiler prerequisite remains separate and unchanged: only Xcode/SDK 27.0
is installed, above UE 5.8's declared 26.9 maximum. No known-invalid build was
replayed, engine limits were not weakened, and runtime/package evidence is not
described as a fresh C++ compile.
