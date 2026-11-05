# Beyond Fenwatch: Draft Level Plan

Status: expansion roadmap with a standalone Causeway greybox (2026-11-05).
Fenwatch remains the established gameplay zone. The separate
`L_MarshwardCauseway_Greybox` now supports a normal-picker traversal review;
it is not connected travel or finished content. No new quest, enemy, reward,
or save behavior is implied by this document.

## World Progression

| Stage | Working Level | Player Promise | First Build Slice |
| --- | --- | --- | --- |
| 1 | Fenwatch (existing) | Warm hub, service loop, first Marsh Prowlers, ancient ruins | Preserve the accepted map and routes. |
| 2 | Marshward Causeway | Leave the safe road for a readable marsh crossing; choose when to take cover or retreat | Standalone loop, Blender kit, peat banks, reeds and rock clusters built; physical walk/camera review next. |
| 3 | Oldstone Hollow | Follow the ruin language into a compact, more vertical ancient site | Separate greybox map with two elevation changes and a visible exit. |

The names are working titles. Marshward Causeway should grow naturally from
Fenwatch's north-road shrine and existing marsh/ruin silhouettes, but its map
must be a distinct package. Oldstone Hollow is a later destination, not a
reason to enlarge the starter level before its player loop is accepted.

## Marshward Causeway First Slice

- A sheltered arrival edge, a raised causeway, a low marsh basin, and a ruined
  crossing form one short loop. The player can see the return path from the
  first dangerous bend; avoid a one-way drop in the first pass.
- Use three legible landmarks: the Fenwatch-facing road marker, a broken
  crossing, and a distant stone silhouette. Preserve clear camera lines and
  player-width routes before dressing the banks.
- Prototype with existing materials and collision primitives. Reuse the
  current Prowler only as an isolated encounter fixture after the route and
  sightlines are measured; no new aggro, loot, or quest contract is assumed.
- First acceptance is a greybox walk and combat/cover review in a disposable
  map, not a connected travel feature. Zone transition, spawn/return behavior,
  quest ownership, and save persistence need separate contracts and tests.

## Oldstone Hollow Later Slice

Start with an exterior ruin approach, a small central court, and one upper
overlook. The overlook should reveal the exit and the next mystery without
requiring a large dungeon or new traversal system. This remains a layout
study until the Causeway's route, camera, and return flow are playtested.

## Blender Asset Candidates

Blender MCP is available for original-art work on lighter days. Prioritize one
reusable Causeway kit piece at a time: a road marker, raised timber-and-stone
span, or marsh boundary post. Write a brief with dimensions, pivot, material
family, simple collision, and intended viewing distance before modeling.
Keep reviewed source scripts, `.blend`, FBX, preview, and metrics together;
follow `Docs/BLENDER_ASSET_PIPELINE.md` for import and validation. A Blender
render alone does not authorize placement or gameplay authority.

## Decision Gates

1. Inspect fresh Fenwatch road and shrine views through Unreal's viewport-only
   capture, then choose the Causeway arrival direction and sightline.
2. Greybox a separate map and test walkable widths, ground support, cover,
   boundaries, and a clear return route before adding original art.
3. Obtain normal-camera PIE review and explicit travel/save contracts before
   linking maps or adding persistent quest state.

The first two gates have a bounded greybox implementation. See
`Docs/MARSHWARD_CAUSEWAY_GREYBOX.md` for the exact layout, ownership,
verification, and remaining player checks. Connected travel remains future work.
