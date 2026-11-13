# Build Evidence: 2026-11-13

## Carved Waymarkers

Fresh settled arrival/return views informed the written brief before Blender
MCP built `SM_MarshwardWaymarker_01`:352 triangles,180x140x520 cm,base-center
pivot,one UV,zero non-manifold edges and one exact120x100x440 body box atZ220.
The original warm material is reused unchanged. Chamfered foot,body,collar,
cap and paired-face diamond relief add stone detail without a new marker.
Source preview refinement resolved a coplanar foot seam before import.

Only three existing marker mesh references changed. Numeric preservation
checks retained every other property of all45 static actors. The original
layout/greybox assets stay unchanged. The package gate deliberately maps
only the three Waymarker roles to the new separately validated mesh while
retaining the original marker gate. Current set:13 validated mesh packages,
12 installed imported mesh types,45 placements. Cube is excluded from the
imported count. Route,PlayerStart,buffers,landmarks,terrain/materials,lights,
basin-only wet peat,startup maps and all gameplay/save owners remain fixed.

## Verification

- All34 focused host checks and syntax/scoped whitespace checks passed.
- Fresh classic-FBX import passed geometry,UV,material and actual saved box
  size/center/rotation parity,true exit zero/no Python errors:
  `/tmp/embermere-nov13-waymarker-import.log`.
- Four isolated negative controls rejected expanded box,shifted center,
  changed rotation and wrong material; all edits restored,no saves,true
  exit zero/no Python errors: `/tmp/embermere-nov13-waymarker-negative.log`.
- Independent Fenwatch skeletal-material-first31-package aggregate passed
  exact skeletal,Prowler loot,53 Fab/32 original-art and validators=31
  markers,true exit zero/no Python errors:
  `/tmp/embermere-nov13-fenwatch-31-packages.log`.
- Fresh complete Causeway gate passed13 validated packages/12 installed
  mesh types/45 placements,full buffers and basin-only wet peat,true exit
  zero/no Python/material errors: `/tmp/embermere-nov13-causeway-packages.log`.
- Clean final editor passed all16 Fenwatch suites,full579 support/1104
  clearance and every prior Causeway control,plus six exact marker-body
  entries/six decorative-clear rays with no ignored actors. Snapshot:
  `/tmp/embermere-nov13-waymarker-final-editor.log`,with exact markers,
  `WAYMARKER_FINAL_DIRTY []` and the correct Causeway world.
- Settled three arrival/return/ruin editor views were inspected; normal
  1280x754 floating PIE confirmed Human Warrior Level1 XP0/100 HP100/100
  Mana50/50 via synthetic Slate and stopped. No physical walk,painted
  player-camera or first-visit wayfinding acceptance is claimed.
- Fresh discovery lists133,not another native run. November9's133/133
  zero-failure/warning/skip evidence remains authoritative; no C++ changed.

An initial read-only probe used unavailable Python KBoxElem attributes.
The final validator uses reflected editor properties and compares actual
classic-FBX rotation with the saved baseline; source-zero does not imply
imported-zero. The fresh final session has no Python errors. No raw vendor
packages were resaved; the four known physics-resave warnings remain confined
to their accepted packs. No global toolchain/security/license change occurred.

Final Causeway is All Saved outside PIE on MCP8123,dirty[]. Fenwatch and
protected save/keeper hashes stayed exact. Accepted Causeway SHA256:
`84b82b523129ccf31d089c57e4847e1451e7eb83aa70245c053ae134173d2edb`.
See `MARSHWARD_WAYMARKER.md` for exact ownership and view poses.
