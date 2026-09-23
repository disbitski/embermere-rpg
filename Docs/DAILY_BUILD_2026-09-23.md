# September 23 Daily Build

## North-Road Marshward Shrine Accepted

Fresh overhead and normal-route viewport inspection found a broad empty
shoulder between the Fenwatch gate and the Marsh Prowler pocket. A compact
project-owned marshward shrine now marks that transition without entering the
road or adding gameplay authority.

The deterministic factory-clean Blender MCP build wrote editable source,
classic FBX, preview, and metrics under
`ArtSource/Blender/Environment/FenwatchMarshwardShrine/`. The accepted
`SM_EmbermereFenwatchMarshwardShrine_01` is grounded at `250 x 183.238 x 258`
cm, with 2,648 triangles, one UV channel, zero non-manifold edges, applied
unit scale, and five shared Fenwatch stone, moss, timber, iron, and ember
materials. Four authored UBX boxes make the plinth, backstone, and two posts
solid. The roof, hanging ward, offering bowl, talismans, trim, and crest are
decorative.

A fresh classic-`FbxFactory` commandlet imported and explicitly saved the mesh.
First-class Unreal MCP placed
`Embermere_FenwatchMarshwardShrine_NorthRoad_01` at `(300, 1450, 0)`, yaw
`-90`, with the project-owned tag, original-art folder, and `BlockAll`
profile. The initial yaw showed the shrine's back to the road because classic
FBX mirrors local Y; the accepted yaw faces the ward and offering toward the
north-road approach. The actor is presentation-only and owns no quest,
interaction, service, recovery, reward, vendor, trainer, or persistence
behavior.

The saved center is 1,198.5 cm from the road gate, 2,101.5 cm from the
gatewatch post, 1,800.7 cm from the herb garden, 3,288.2 cm from Mara, and
3,783.2 cm from PlayerStart. The north-road centerline, west shoulder, and
north-commons routes remain open.

## Verification

- The fresh import commandlet emitted
  `EMBERMERE_FENWATCH_MARSHWARD_SHRINE_IMPORT_SUCCESS`, exited zero with no
  errors or warnings, and had no `LogPython: Error`.
  Evidence: `/tmp/embermere-sep23-marshward-shrine-import.log`.
- The focused package validator locked classic-FBX provenance, exact metrics,
  five shared material assignments, four boxes, transform, tag, profile,
  art-only class, and protected spacing.
- The first live road-centerline assertion started inside the existing north
  gate fence collider. Its start point was moved beyond the fence; the fresh
  final editor session contains no `LogPython: Error`.
- All **13 initialized-world collision/route suites** passed sequentially in
  the clean final editor session. The new suite hit all four purposeful shrine
  surfaces and cleared the hanging ward, offering bowl, front approach,
  north-road centerline, west shoulder, and north-commons route.
- Fresh MCP discovery returned exactly 100 Embermere tests. All **100/100**
  passed with zero failures, warnings, skipped, or unrun records. Clean PIE
  started and stopped.
- With the real editor down and port 8123 free, the fresh sequential
  **30-package** aggregate passed with the exact aggregate and **53 grounded
  Fab / 31 original-art** markers, exit zero, and no `LogPython: Error`.
  Evidence: `/tmp/embermere-sep23-30-package-aggregate.log`.
- The only aggregate warnings were the four known vendor physics resaves for
  FieldGrass, River_Rock, Medium_Boulder, and HillTree. No raw vendor package
  was resaved.
- After relaunch, the map and shrine asset were clean on
  `/Game/Maps/L_Embermere_Prototype`, outside PIE; the real UnrealEditor
  owned MCP 8123. The original journey save and keeper material retained
  SHA256 `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`
  and `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

Physical player traversal was not repeated because desktop control remains
revoked. The initialized-world native traces and clean PIE are the accepted
automated evidence for this art-only milestone.

The compiler prerequisite is unchanged: only Xcode/SDK 27.0 is installed,
above UE 5.8's declared 26.9 maximum. No known-invalid build was replayed or
presented as fresh C++ compilation.
