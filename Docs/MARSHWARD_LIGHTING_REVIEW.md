# Marshward Lighting Review

## Decision

Retain the current lighting and composition. Six settled route views on
November 26 did not establish a concrete readability defect requiring a
retune. The pale crossing, warm markers and darker basin remain visually
distinct in the inspected stills. The unfinished dark horizon remains an
art gap, not a diagnosed exposure or skylight fault.

No map, asset, light, material, actor, collision, startup setting or gameplay
behavior changed. Causeway retains 53 static placements and its accepted
pool/shore, full-width walking buffers and original support geometry.

## Inspected Views

Actual editor cameras were set and read back, followed by a settle boundary
and viewport-only capture using the same explicit transform. Roll is zero
and FOV is 90 degrees. These are static editor views, not a moving player
camera or physical traversal acceptance.

| View | Position, cm | Pitch / Yaw | Observation |
| --- | --- | --- | --- |
| Arrival | (0,-450,260) | -5 / 90 | Pale deck leads toward the gateway; the warm shoulder marker is separate from the darker basin. |
| Crossing north | (0,3900,360) | -5 / 90 | Pillars and lintel frame an open lane; stone faces and ground shadows remain distinguishable. |
| Crossing south | (0,3300,360) | -5 / -90 | Return-facing deck and arrival marker remain distinct from side dressing. |
| Island forward | (0,6400,360) | -5 / 90 | The gateway frames sky and unfinished horizon; no onward destination is implied by a lighting change. |
| Island reverse | (0,6100,360) | -5 / -90 | The pale crossing, pool and side foliage remain separate, with no new visual blockage. |
| East descent | (1100,6400,360) | -8 / 0 | Pale ramp joins the return platform; the warm marker stays beside the connection rather than covering it. |

Editor light sprites and grid overlays are not gameplay UI. One earlier
south-facing image was omitted by the tool's output limit; the separately
reissued image was inspected instead.

## Measured Settings

The read-only `Scripts/audit_marshward_lighting_unreal.py` reports the loaded
world's light components and effective actor material slots, including
overrides. It refuses a different map, a running game world, dirty packages,
ambiguous labels, missing materials and duplicate owner-slot identities.
Slot identifiers must be nonnegative built-in integers: booleans, floats,
strings and null values are rejected without coercion. Integer slots 0 and
1 remain valid; Python's boolean/integer relationship must not merge their
identities with invalid boolean input.

| Owner | Component | Mobility | Raw intensity | Other saved settings |
| --- | --- | --- | --- | --- |
| Marshward_Sun | DirectionalLightComponent | Movable | 7.0 | White; shadows and atmosphere-sun enabled; pitch -40, yaw -35. |
| Marshward_SkyLight | SkyLightComponent | Stationary | 1.0 | White; shadows and real-time capture enabled; captured-scene source; black lower hemisphere enabled. |

There is one visible SkyAtmosphere component. No fog, volumetric-cloud or
post-process components, or PostProcessVolume actors, were found in the
loaded map. These are configuration observations, not measured exposure,
capture freshness, GPU cost or proof of the dark horizon's cause.

The 91 effective slots resolve to 13 Material assets. All currently report
opaque surface/default-lit shading, single-sided rendering and no connected
emissive input. The audit reuses the existing exact wet-peat, pool and shore
validators: each of those materials remains on its one intended actor slot.
The four ground owners, three markers and fourteen greybox-stone owners
retain their existing assignments. No material is reauthored or resaved.

## Verification And Limits

All 77 focused host tests passed, including five ownership-report regressions
for deterministic grouping, distinct slots, duplicate/missing ownership and
strict slot types. The boolean rejection regression first failed for both
False and True against the old integer predicate, then passed with the strict
type check. Float, string and null rejection and valid integer output remain
covered. The updated audit passed in the initialized editor:

```text
EMBERMERE_MARSHWARD_LIGHTING_AUDIT_SUCCESS: 2 lights, 13 effective materials, 91 owner slots, exact wet-peat/pool/shore ownership, no dirty packages
```

Run it from a cleared Python-mode Unreal Output Log:

```python
import runpy; runpy.run_path('/Users/wizard/Documents/Unreal Game/Scripts/audit_marshward_lighting_unreal.py', run_name='__main__')
```

The complete initialized Causeway suite passed 579 support and 1,104 clearance
rays plus every existing arrival, marker, terrain, art and pool/shore control.
No November 26 Python errors were logged. Normal 1280 x 754 floating PIE
reached the Human Warrior picker; the synthetic confirmation click focused
the button but did not reach the HUD. PIE then stopped cleanly.

Both maps and the protected save/keeper material retained their recorded
hashes. No engine build, import, package commandlet, full native run or
Fenwatch rerun was needed for this read-only addition. November 24's fresh
Causeway package proof and the earlier accepted native/Fenwatch evidence
remain authoritative, not new runs attributed to this review.

This audit does not measure contrast ratios, auto-exposure transitions,
NoCollision foliage occlusion or first-visit route interpretation. A physical
walk in both directions should still check marker recognition, deck/basin
contrast and adaptation while turning beside the ruin and descending east.
