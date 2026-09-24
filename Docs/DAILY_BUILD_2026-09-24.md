# September 24 Daily Build

## Marsh Prowler Pull-Lane Regression

The Xcode prerequisite still blocks trustworthy C++ changes, so today's
bounded milestone protects an existing playable value without touching native
gameplay or saved content. A new initialized-world native trace validator
checks all three saved Marsh Prowlers' exact home transforms and 525 cm aggro
radii. Each local stance is inside its intended radius and at least 600 cm
from either other home:

| Prowler | Stance XY (cm) | Target distance (cm) | Nearest neighbor (cm) |
| --- | ---: | ---: | ---: |
| `Starter_Enemy_01` | `(1530, -50)` | 509.3 | 1,162.5 |
| `Starter_Enemy_02` | `(1225, 1000)` | 485.4 | 972.4 |
| `Starter_Enemy_03` | `(2500, 1800)` | 500.0 | 1,063.0 |

For each stance, a downward native trace verifies the underlying zone ground.
Two local segments, staging-to-stance and stance-to-target edge, each test
center and `+/-35` cm offsets at 95 and 155 cm player heights. A separate
positive trace must hit the saved south-gate fence. The northern support ray
first hits the existing roots overlay; only that second support trace ignores
the roots actor to prove the terrain beneath it. Clearance traces ignore
nothing. The new suite is included in
`Scripts/validate_initialized_world_traces_unreal.py` as suite 14. It owns no
map, actor, AI, aggro, quest, combat, or reward mutation.

The exploratory pass rejected a south staging point embedded in an existing
rock and two alternatives intersecting the gate stone/fence. Unreal's native
collision query, not a simpler preview miss, selected the accepted path. The
final editor session was relaunched so those probe errors are absent from the
acceptance log.

## Verification

- All **100/100** native automation tests passed with zero errors, warnings,
  skipped, or in-process records. Clean PIE started and stopped.
- With Unreal down and port 8123 free, the fresh sequential **30-package**
  aggregate passed with the exact skeletal-material, aggregate, and **53
  grounded Fab / 31 original-art** markers, exit zero, and no
  `LogPython: Error`. Evidence:
  `/tmp/embermere-sep24-30-package-aggregate.log`.
- In the clean relaunched editor, all **14 initialized-world** collision/route
  suites passed sequentially with
  `EMBERMERE_INITIALIZED_WORLD_TRACES_SUCCESS`; the focused new suite emitted
  `EMBERMERE_MARSH_PROWLER_PULL_LANES_SUCCESS`. The final editor log has no
  `LogPython: Error`.
- Fresh MCP discovery returned exactly 100 Embermere tests. The real editor
  owns localhost 8123 on `/Game/Maps/L_Embermere_Prototype`, outside PIE; the
  map and shrine packages are clean.
- The user's journey save remains SHA256
  `c29a887c6e9a307221b56da34e10e80cef0af30b777a12206469893cd5b2d778`;
  the unrelated keeper material remains
  `38b159a4104a97e70abca1f96f199af293d668dc5f29727dc55b2df54052266d`.
  Neither was staged or changed.

The geometric regression does not replace a real player-controlled aggro,
retaliation, leash, or normal-camera acceptance pass. Those remain explicitly
unverified today because desktop control is revoked. Only
`/Applications/Xcode.app` is present with SDK 27.0, above UE 5.8's declared
26.9 maximum, so no known-invalid UBT build was rerun or claimed.
