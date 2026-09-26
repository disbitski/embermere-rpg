# Ancient Ruin Relief Contract

The broad west-facing surface of `FabPass_Ruin_Wall_A` is visually blank from
the first Marsh Prowler approach. A small project-owned carved relief may give
the ruin a distinct identity without adding another freestanding obstacle in
the combat pocket.

## Ownership

- `SM_EmbermereAncientRuinRelief_01` is decorative world art only. It owns no
  collision, navigation, targeting, AI, quest, interaction, reward, loot,
  service, or persistence behavior.
- `FabPass_Ruin_Wall_A` remains the structural wall and retains its existing
  transform and collision. The relief is a separate, removable actor; do not
  resave or modify the raw vendor package.
- The relief uses shared project-owned stone, moss, iron, and restrained ember
  materials. It should read as old carved masonry, not a quest marker or a
  second cyan target circle.

## Placement And Acceptance

- Mount the asset on the wall's west face at `(2302, 350, 0)`, yaw `90`, at
  unit scale. Its ground-level pivot and shallow depth keep it close to the
  wall without protruding into the first Prowler's combat space.
- The accepted authored render mesh is `206 x 55.805 x 216` cm, 2,900
  triangles, one UV channel, zero non-manifold edges, and four shared
  stone/moss/iron/ember materials. The faint ember shard is a carved ruin
  accent, not a marker, prompt, or target indicator.
- Author with a deterministic reviewed Blender script, applied transforms,
  one UV channel, manifold render topology, and an explicit triangle budget.
  Import through classic `FbxFactory`, remap to project-owned materials, save
  the mesh package, and validate it in a fresh commandlet.
- It has **zero authored simple colliders** and the placed component uses
  `NoCollision`. Verify both the saved package and initialized editor actor;
  the existing wall must still give a positive collision hit.
- Inspect from the normal road approach and from the first solo-pull stance.
  The first Prowler, its nameplate and cyan circle, the ruin silhouette, and
  all three accepted solo-pull lanes must remain readable and clear.
- Preserve the existing saved map, 53 Fab actors, all other original art,
  daylight, terrain, services, and player routes. No new gameplay data or save
  fields are permitted in this slice.
