# Marshward Bank Edge

## Ownership And Brief

A fresh view from the eastern return at (2700,3000,220), looking east,
shows a straight shelf/basin join. Author one original low, tapered apron
to soften a short section of that join without touching the walking route.
`SM_MarshwardBankEdge_01` is 1000 x 360 x 70 cm, base-center pivot, closed
low-poly geometry, one UV channel and zero authored collision. Reuse only
`M_MarshwardPeat`, `M_MarshwardMoss` and `M_MarshwardEarth`; the basin-only
wet-peat material must not spread onto this mesh. Retain source, FBX, metrics
and preview separately from the accepted Blender assets.

Place only `Marshward_BankEdgeEast` at (3910,3100,-120), yaw 90, unit scale,
tagged `EmbermereMarshwardMarsh` in `MarshwardGreybox/MarshTerrain`.
The short high edge overlaps the existing east shelf; the outer lip lies on
the basin floor. Classic FBX mirrors authored local Y. The visible X bounds
3730..4090 stay beyond the eastern corridor's protected edge at 3375.
The component must be `NoCollision`: basin and shelf retain all support and
blocking authority. This is decorative terrain, not a new step or route.

Preserve all preceding 40 actors/transforms/materials/profiles, the original
22 greybox placements, PlayerStart and full-width corridors plus 75 cm.
The map becomes 41 static placements/eight meshes. No new materials, water,
travel, quest, service, combat, save or startup-map changes are authorized.
Fenwatch and protected user data stay untouched.

## Gates

Require a fresh classic-FBX import and saved mesh/material/zero-hull checks,
exact placement/profile and full-width bounds. Retain the complete loop,
six bank and twelve stone/ground controls. Add nine unfiltered downward
apron probes that must hit the existing basin at Z=-120, and two unfiltered
join controls that must hit the existing east shelf. Inspect the return-side
join and arrival/overview views; run clean normal PIE. Automated support and
synthetic Slate do not prove physical walking or camera feel.

## Verification: 2026-11-06

Blender MCP produced the closed **46-triangle** apron with one UV channel
and zero non-manifold edges. A narrow-tip fold was removed during source
preview review before import. Fresh classic-FBX import retained the exact
size, three reused materials and zero hulls; no existing mesh/material was
resaved. Source, FBX, metrics and preview are retained with the marsh kit.

The initialized editor saved only the new actor, preserving all preceding
40 placements. Sixteen host tests cover the brief, role, probe coordinates
and all full-width corridors plus 75 cm. All 133 native tests passed with
zero failures, warnings or skips. Fresh Fenwatch 31-package and separate
eight-mesh/41-placement Causeway gates exited zero with exact success
markers and no `LogPython: Error`.

After clearing only disposable native-test worlds by relaunching, the clean
editor passed all 16 Fenwatch suites, the original 579 support / 1104
clearance rays and marker control, six bank controls, twelve stone/twelve
underlying-ground controls, plus nine unfiltered apron-to-basin rays and two
unfiltered existing-shelf join controls. The apron provides no support.
Return-side, close, arrival and overview viewport evidence was inspected;
normal picker/Human Warrior PIE reached HP100/100/Mana50/50 and stopped.
Physical walking/camera feel remain unverified. Causeway is All Saved outside
PIE with 133 freshly discoverable tests; Fenwatch and protected hashes stayed
unchanged.

Logs: `/tmp/embermere-nov06-bank-edge-import.log`,
`/tmp/embermere-nov06-fenwatch-31-packages.log`, and
`/tmp/embermere-nov06-causeway-packages.log`.
