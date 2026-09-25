# September 25 Daily Build

## Fenwatch F-Interaction Approach Regression

Xcode remains above UE 5.8's accepted Mac SDK range, so today's bounded
milestone protects the existing village interaction loop without changing C++
or saved content. A fresh overhead Unreal viewport inspection showed the
existing road and landmark composition remains readable; another grass-filling
prop was not justified. The new initialized-world validator instead checks
that every current `F` owner has a terrain-backed, player-width approach from
which the controller would select that owner rather than a neighboring
service.

| Owner | Grounded standing XY (cm) | Owner distance (cm) | Next-owner margin (cm) |
| --- | ---: | ---: | ---: |
| Mara Fenwatch | `(-2230, -1010)` | 245.0 | 484.0 |
| Fenwatch Supplies | `(-1610, -1020)` | 210.5 | 110.6 |
| Armsmaster | `(-1200, -760)` | 221.4 | 328.9 |
| Still Waters notice board | `(-1460, -450)` | 234.8 | 264.7 |
| Communal well | `(-1180, -1600)` | 248.8 | 298.5 |

The script checks the exact five saved interactable owners and transforms,
reads the native controller's `350` cm range and the well data asset's `300`
cm range, and calculates selection using player-center Z `95` cm. Ten native
vertical traces require zone ground at the five approach starts and standing
points. Thirty horizontal native rays span each local walk at center and
`+/-35` cm player width, at heights `95` and `155` cm. A separate positive
ray must still hit the saved south-gate fence. The new suite is included in
`Scripts/validate_initialized_world_traces_unreal.py` as suite 15.

Initial diagnostics found a proposed Mara path inside the west cottage, a
vendor standing point on the supply chest, and a vendor staging point resting
on the village table despite clear horizontal rays. Those were probe errors,
not reasons to move accepted art. The final points are terrain-backed and
clear at player height. The validator is read-only and owns no interaction,
quest, vendor, trainer, rest, HUD, save, or map mutation.

## Verification

- All **100/100** native automation tests passed with zero errors, warnings,
  skips, or in-process records. Clean PIE started and stopped.
- With the real editor down and MCP port 8123 free, the fresh sequential
  **30-package** commandlet passed with the exact skeletal-material,
  aggregate, and **53 grounded Fab / 31 original-art** markers, exit zero,
  and no `LogPython: Error`. Evidence:
  `/tmp/embermere-sep25-30-package-aggregate.log`.
- In a clean relaunched editor, all **15 initialized-world** collision/route
  suites passed sequentially. The focused suite emitted
  `EMBERMERE_FENWATCH_INTERACTION_APPROACHES_SUCCESS`; the aggregate emitted
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS`. No `LogPython: Error` appeared
  in that final session.
- Fresh MCP discovery returned exactly 100 Embermere tests. The real editor
  owns localhost 8123 on `/Game/Maps/L_Embermere_Prototype`, outside PIE;
  map and marshward-shrine packages are clean.
- The user's journey save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper material remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.

This regression proves approach geometry and nearest-owner resolution, not a
physical `F` press, dialogue, panel opening, transaction, quest completion,
or rest channel. Those player-input checks remain explicitly unverified under
the desktop-control restriction. Only `/Applications/Xcode.app` with
Xcode/SDK 27.0 is present, above UE 5.8's declared 26.9 maximum; no
known-invalid UBT build was rerun or claimed.
