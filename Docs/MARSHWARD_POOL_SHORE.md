# Marshward Pool Shore

## Brief

Settled arrival, crossing and east-return views show the west still pool
clear of the raised route. The crossing pose `(0,3300,360)` looking west
shows a thin dark line where the crossing-facing water edge meets the
flat catch. Address only that line.

Build one closed, thin mud strip following eight near-side stations of
the existing Blender pool outline. Taper both ends into the catch.
Start the strip about 9 cm inside the rim, crest just outside the rim
at 0.4 cm below the waterline, and bury the irregular outer toe 0.5 cm
below the dry catch. Use one UV, no hulls, and an independently owned
material matching the catch's world-space color and roughness. Do not
assign the catch's wet-peat asset anywhere else.

Place at the pool's unchanged origin `(-1320,3340,-118)` with unit scale
and yaw zero. The intentional visual overlap is only with the pool;
the strip must not hide the water or touch other dressing or protected
walking buffers. No collision, shadows, interaction, overlap generation,
navigation or simulation. Keep the existing basin as the first support
hit and compare the same settled viewport poses before accepting.

## Adopted Strip

`SM_MarshwardPoolShore_01` is one 92-triangle Blender mesh, with one UV,
zero non-manifold edges and no simple collision. The imported strip
occupies approximately X=-1369.1..-836.0, Y=3110.5..3615.2 and
Z=-122..-118.4 cm. Only the two exact pool and shore roles may overlap
in XY; other dressing separation and the full buffer tests are retained.
`M_MarshwardShoreMud` is an independent material with the same verified
opaque world-space noise graph as the basin, not a wet-peat assignment.
Both the pool mesh and its material were left as-is.

From the matching crossing view, the continuous dark rim recedes to
a faint edge against the mud. An offset crossing view at `(0,3470,350)`
also showed the blue-green core unobscured and the strip low against
the basin. No broad depth artifacts or raised lip were visible in those
stills. Arrival and east-return comparisons leave the deck and markers
clear. Verify motion and camera feel in a physical walk; still images
do not establish them.

## Verification

72 focused host checks passed. The fresh classic-FBX import and the
independent saved Causeway gate both exited zero with their exact markers
and no Python errors. All 52 previous static actor signatures were retained.
The live map still passes 579 route support and 1,104 clearance traces,
its existing positive controls, and 12 unfiltered original-basin hits
across the new shore crest and toe. Those probes hit the exact existing
basin at Z=-120, not the water or mud surface.

Normal floating Play-in-Editor reached the Human Warrior HUD via
synthetic Slate and stopped without loading the protected save.

The loaded inventory is now 9,558 placed LOD0 triangles, 91 sections and
13 effective materials. The 37 solid components still own 20 boxes and
32 convex hulls; the new shore is the 16th NoCollision placement. These
are inventory measurements, not frame draw-call results. The pool mesh
and material, basin material, Fenwatch map, protected save and keeper
material remain byte-identical. No C++ build or Fenwatch rerun was needed
for this isolated Causeway addition.
