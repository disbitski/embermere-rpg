# October 5 Daily Build

## Gate Approach

A fresh viewport-only capture from `(650,390,190)` looking through the road
gate showed the decorative `FabPass_Road_Flowers_02` filling the center of the
opening. Its saved transform was `(1010,570,0)`, yaw `-20`, scale `1.2`. Native
collision rays had remained clear, so they could not protect this visual
sightline. The actor owns no gameplay, quest, service, or persistence state.

The road-boundary validator failed first on that placement. The existing actor
was moved to the grounded south shoulder at `(500,0,0)` while retaining its
authored yaw and scale. A second test-first failure exposed inherited vendor
collision; only the placed component now uses `NoCollision`. The vendor mesh
and material packages were not resaved. `Scripts/place_gate_approach_flowers_unreal.py`
is an idempotent, map-specific editor operation with an old/new-position guard.
The older broad Fab placement source records the same new position but was not
rerun, because it would recreate unrelated accepted actors.

The first fresh commandlet exited zero **with** `LogPython: Error`: a partial
MCP transform update had reset the actor's rotation and scale to identity.
That run was rejected. The editor script was tightened to set location,
rotation, scale, and collision profile explicitly, then save. The exact
saved-map validator now checks all four properties. A fresh viewport capture
after repair showed the gate opening clear, with the road and first Prowler
visible beyond it.

## Verification

- No C++ changed; the accepted native module remained loaded. Fresh discovery
  found **109** tests and the full run passed **109/109** with zero test
  failures, warnings, or skips.
- The final GUI-down `-ExecutePythonScript` commandlet exited zero. Its
  dedicated log `/tmp/embermere-oct05-final2-31-package-aggregate.log` has
  exact skeletal-material, **53 Fab / 32 original-art**, and `validators=31`
  success markers, and no `LogPython: Error`.
- The final clean editor session emitted
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS: 16 sequential suites`, including
  the strengthened gate sightline/collision check, with no `LogPython: Error`.
  Floating PIE started and stopped; the map was All Saved outside PIE on
  `/Game/Maps/L_Embermere_Prototype` with real UnrealEditor owning MCP 8123.
- The protected save remained SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper material remained SHA256
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

Four known physics-resave warnings remain confined to raw FieldGrass,
River_Rock, Medium_Boulder, and HillTree packages. They were not resaved.
Physical player-input checks remain user gates under the desktop-control ban.
Tomorrow's best start is a different bounded gameplay/content milestone from
fresh normal-route evidence, not another gate or empty-interaction retune.
