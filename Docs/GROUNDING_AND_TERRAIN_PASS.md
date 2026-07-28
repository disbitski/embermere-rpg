# Grounding And Terrain Pass

## Problem

The starter zone currently uses a flat, uniform moss-green foundation. It keeps
the prototype readable, but it provides almost no scale, depth, or surface
variation. A July 27 gameplay capture also showed:

- a suspended Paragon ruin canopy with no visible structural support;
- foliage clusters whose white or incomplete material treatment weakens depth;
- props whose true mesh-bottom, pivot, and ground relationship need measurement;
- little grass, mud, stone, or path breakup around object bases.

A ground texture can improve contact perception, but it cannot correct a real
Z gap. Geometry and material problems must be separated before art dressing.

## Grounding Audit

Run the audit in the initialized live editor because commandlet-loaded worlds
may not register native collision bodies.

1. Enumerate static-mesh actors along PlayerStart, Mara, the road threshold,
   Prowler pocket, and ruin route.
2. Record each actor's world render-bounds minimum Z.
3. Trace downward beside or through the expected support footprint to find the
   actual ground surface.
4. Calculate the visible gap or burial depth:
   `bounds_min_z - ground_hit_z`.
5. Classify meaningful positive gaps as floating and excessive negative values
   as buried. Ignore deliberate overhead spans only when their supports and
   composition make structural sense.
6. Correct placement through the owning setup/import script, then update exact
   saved-map validator assertions. Do not make one-off viewport-only fixes.
7. Recheck player collision, route clearance, target-ring visibility, and
   terrain contact from the normal gameplay camera.

Some meshes have decorative bounds below their actual feet or pivots far from
their support surface. Use several support-footprint traces and visual review,
not one blind center trace, before changing an actor.

## Material And Foliage Audit

- Identify white or default-material foliage in the gameplay capture.
- Determine whether each case is a missing material, unsupported vendor shader,
  overexposure, incorrect two-sided foliage setup, or intentionally pale art.
- Prefer a project-owned wrapper material or a compatible existing material
  assignment rather than editing and resaving raw vendor packages.
- Replace visually broken foliage when a clean wrapper is impractical.
- Keep decorative grass, ferns, reeds, and leaf cards `NoCollision`.
- Preserve Mara, enemy, nameplate, target-ring, and road readability.

## Ground Surface Pass

First inspect the already installed `KiteDemo`, `SoulCave`, and Paragon content
for compatible earth, moss, mud, stone, and path materials. If none fits the
Stylized Classic direction, build a project-owned material under
`/Game/Art/Embermere/Environment/Ground/` with:

- moss-green and peat-brown color breakup;
- a readable dirt or worn-road lane;
- low-frequency macro variation so the plane no longer reads as one flat color;
- restrained normal and roughness variation;
- no dark photoreal noise that obscures characters or target rings;
- Mac-friendly material complexity.

Sparse grass and marsh ground cover can hide tiny residual seams after
placement is correct. It must not be used to conceal large geometry gaps.

## Useful Fab Search Criteria

Search only for UE-compatible products, not UEFN-only products:

- `stylized landscape material Unreal Engine`
- `stylized forest ground Unreal Engine`
- `stylized swamp environment Unreal Engine`
- `stylized grass foliage Unreal Engine`
- `fantasy moss dirt path Unreal Engine`

Prefer a small cohesive pack with UE 5 support, landscape or tiling ground
materials, grass/reed/fern meshes, LODs, clean collision, and licensing suitable
for a public game project. A pack that matches Embermere is more useful than a
large photoreal library.

## Acceptance

- No unintended prop or foliage gaps are visible from the normal route.
- Intended overhead architecture reads as supported rather than accidentally
  suspended.
- Ground variation establishes scale and object contact without visual noise.
- The road from spawn to Mara and the Prowler pocket remains obvious.
- Decorative ground cover does not collide or hide enemies, target rings,
  nameplates, quest markers, or original props.
- The map, placement scripts, and validators reproduce every accepted fix.

## Completed 2026-07-27

The first measured pass is now saved and reproducible:

- the actual foundation top is `Z=0`; 67 ordinary art actors that inherited the
  old `Z=20` convention were lowered to the real surface;
- the unsupported SoulCave arch and pillar accents plus three visual enemy
  markers were removed instead of hidden with more foliage;
- 21 KiteDemo tree and foliage placements received project-owned component
  material overrides without resaving raw vendor packages;
- `/Game/Art/Embermere/Environment/M_EmbermereGround` now contains a
  texture-free 38-expression material graph with broad crossed moss variation,
  a route-aligned peat path, and a `300` cm `PathHalfWidthCm`;
- `SM_EmbermereMarshReedCluster_01` adds a deterministic 1,012-triangle,
  four-material ground-cover asset whose low footprint shares the terrain
  material; four exact saved placements are explicitly `NoCollision`;
- saved-map validation now accepts 59 grounded `FabPass_` actors and 14
  project-owned placements, while live viewport review accepts the road,
  Prowler pocket, gate lanes, target presentation, and reed silhouettes.

The material graph must be built outside PIE. A rebuild attempted during PIE
changed the live object but could not save the package, leaving the in-memory
graph invalid while the disk package remained healthy. A controlled editor
restart recovered the saved 38-node graph. Treat graph construction, package
persistence, and live visual acceptance as three separate gates.

Some imported KiteDemo meshes still log absent internal vendor material and
texture references in a fresh commandlet. The component overrides make the
visible map readable, but they do not repair the imported mesh package's
dependency graph. Replace those meshes with project-owned art or a complete
signed-in pack over time; do not edit or commit raw vendor content.
