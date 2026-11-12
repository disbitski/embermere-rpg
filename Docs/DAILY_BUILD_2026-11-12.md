# Build Evidence: 2026-11-12

## Low Marsh Scrub

Fresh settled east-shoulder viewport evidence informed a written brief before
Blender MCP built `SM_MarshwardLowScrub_01`:240 x180 x95 cm,540 triangles,
three grounded stems/18 broad leaves,one UV,zero non-manifold edges and
zero collision. It reuses existing reed-green/earth materials without edits.
Independent source,FBX,metrics,preview and builder are retained; see
`MARSHWARD_LOW_SCRUB.md`.

One NoCollision `Marshward_ScrubEast` at(3550,2450,-120),yaw0,unit scale
is saved outside the eastern return buffer and separate from the apron.
Numeric preservation checks retained every preceding44 static placement.
Causeway now has45 static actors/twelve imported kit meshes:original22
plus23 dressing. Route support,PlayerStart,full-width corridors plus75 cm,
landmarks,lights,basin-only wet peat,all existing sources and materials are
unchanged. No C++,gameplay,water,hazard,travel or save changes.

## Verification

- All30 focused host tests and script syntax/whitespace checks passed.
- Fresh classic-FBX import passed exact size/540tri/one-UV/zero-hull/two-
  reused-material marker,true exit zero,no Python errors:
  `/tmp/embermere-nov12-low-scrub-import.log`. The earlier sandbox startup
  failure was not counted; the identical host-permission retry passed.
- Independent Fenwatch skeletal-material-first31-package validation passed
  every marker,including Prowler loot,53 Fab/32 original-art and validators=31,
  true exit zero,no Python errors:
  `/tmp/embermere-nov12-fenwatch-31-packages.log`.
- Fresh separate complete Causeway package validation passed12meshes,
  original22 plus23dressing,full buffers and unchanged basin-only wet peat,
  true exit zero,no Python/material errors:
  `/tmp/embermere-nov12-causeway-packages.log`.
- Final clean editor passed all16 Fenwatch live suites,complete Causeway579
  support/1104 clearance,every previous control and three new unfiltered
  scrub-base-to-basin rays atZ-120. Native rays do not certify visual occlusion.
  Snapshot `/tmp/embermere-nov12-low-scrub-final-editor.log` contains exact
  success markers,`LOW_SCRUB_FINAL_DIRTY []` and the correct Causeway world.
- Settled shoulder and three arrival poses were inspected. Normal1280x754
  floating PIE confirmed Human Warrior Level1 XP0/100 HP100/100 Mana50/50
  through synthetic Slate,then stopped cleanly. No physical input,painted
  player-camera,complete walk or moving-camera acceptance is claimed.
- Fresh discovery lists133 native tests,not another full run. November9's
  133/133 zero-failure/warning/skip evidence remains authoritative. No C++
  rebuild or raw vendor resave was needed; the four known vendor physics-
  resave warnings remain confined to their accepted packs.

The final editor owns MCP8123 on Causeway,All Saved outside PIE,dirty[],
no Python errors. Fenwatch and protected save/keeper hashes remain exact.
Causeway SHA256:
`0ab9d967d7910c829900d1d4b6124dc6ae3d9712b6838809846639ae6e4e4717`.
Physical walking in both directions,south-turn wayfinding and moving-camera
foliage readability remain user gates.
