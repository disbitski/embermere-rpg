# Marsh Prowler Art Brief

## Purpose

Create Embermere's first original creature: a readable Stylized Classic marsh
dire wolf that can replace the current Marsh Prowler placeholder without
changing targeting, combat, loot, leash, quest, or respawn rules.

The user-provided concept reference currently lives at:

`/Users/wizard/Downloads/prowler.jpeg`

Use it for silhouette and mood, not as an exact copy. The finished creature
should belong specifically to Embermere.

## Implemented 2026-07-27

The bounded vertical slice is complete and accepted as Embermere's first
original rigged creature:

- deterministic Blender source:
  `Scripts/blender/build_embermere_marsh_prowler.py`;
- saved source, skeletal FBX, six animation FBXs, metrics, previews, and action
  contact sheet under
  `ArtSource/Blender/Characters/Enemies/MarshProwler/`;
- 7,464 render triangles, 3,878 vertices, applied scale, five materials, 26
  authored bones, and two authored physics/collision proxy shapes;
- bind-pose dimensions of approximately `414.734 x 104 x 209.5` cm;
- Idle, Walk, Run, Attack, Hit, and Death actions imported as six
  `AnimSequence` assets;
- skeletal mesh, skeleton, physics asset, materials, and animation packages
  explicitly saved under
  `/Game/Art/Embermere/Characters/Enemies/MarshProwler/`;
- generic enemy state routes the six animation roles while the existing
  capsule, combat, loot, targeting, quest, leash, death, and respawn rules
  remain authoritative;
- all three saved starter-enemy instances retain the visual mesh and complete
  animation set, with the placeholder still available as fallback.

Clean PIE accepted the swamp palette, terrain contact, Idle/Walk/Run routing,
Strike and retaliation, Attack/Hit/Death presentation, target clearing, Marsh
Tonic loot, hide, and respawn. `Embermere.Enemy.MarshProwlerPresentation`
guards the asset-agnostic contract in the 27-test suite.

This first pass deliberately landed below the original 12,000-triangle target
and above the planned three-material target. The simpler 7,464-triangle mesh
reads clearly at the gameplay camera, while five small material roles preserve
peat, charcoal, moss, mud/bone, and amber separation. Future polish should
respond to concrete PIE feedback rather than increasing detail by default.

## Visual Identity

- Dire-wolf proportions with a heavy shoulder hump, deep chest, low stalking
  head, long forelegs, large paws, and a slightly leaner rear body.
- Strong triangular ears, thick neck ruff, angular cheek fur, expressive amber
  eyes, visible fangs, and a broad dark nose.
- Swamp palette rather than the concept's warm woodland palette:
  - charcoal and peat-brown base fur;
  - muted moss green along the mane, spine, paws, and tail;
  - damp blue-gray shadow fur around the legs and belly;
  - restrained ochre or ember-amber eyes;
  - pale bone teeth and dark iron-gray claws.
- Add asymmetrical marsh character through a few clumped fur planes, moss
  patches, reed-like tufts, mud-darkened lower legs, or one subtle scar.
- Avoid photoreal hair simulation, excessive spikes, undead decay, glowing
  fantasy markings, or a boss-scale silhouette. This is a dangerous starter
  predator that appears in groups later, not a raid creature.

## Silhouette Targets

- Read clearly at Embermere's normal third-person camera distance.
- Feel larger and more dangerous than a real wolf while remaining compatible
  with the current `96` cm target-ring radius and `225` cm nameplate height.
- Initial target dimensions:
  - nose-to-tail length: `260-300` cm;
  - shoulder height: `130-150` cm;
  - chest width: `80-95` cm;
  - ground pivot beneath the body center.
- Keep the head, shoulders, forepaws, and tail readable in side, front
  three-quarter, and gameplay rear views.

## Modeling Contract

- Asset name: `SK_EmbermereMarshProwler_01`.
- Source folder:
  `ArtSource/Blender/Characters/Enemies/MarshProwler/`.
- Unreal folder:
  `/Game/Art/Embermere/Characters/Enemies/MarshProwler/`.
- Target `12,000-20,000` triangles for the first LOD0.
- Applied object scale, clean normals, no non-manifold geometry, no accidental
  internal faces, and a reproducible metrics file.
- Prefer a clean contiguous body mesh with deliberate loops around shoulders,
  hips, elbows, knees, jaw, and tail base. Small fur cards or shell pieces are
  acceptable only where they improve the silhouette.
- Use one `2048` atlas when practical. Keep the material count to three or
  fewer: swamp fur/body, claws/nose/teeth, and eyes or accents.
- Preserve a neutral quadruped bind pose with enough leg separation for clean
  skinning and a relaxed jaw that can open for attack animation.

## Rig And Animation Contract

- Use a dedicated quadruped skeleton with a true root bone at world origin.
- Include pelvis, spine chain, chest, neck, head, jaw, tail chain, ears, and
  complete four-leg chains with stable paw/toe controls.
- Deform bones use predictable project-owned names. Control bones must not be
  exported unless intentionally required.
- Prototype locomotion is in place because the existing enemy actor owns world
  movement.
- First acceptance animations:
  - idle with breathing, weight shift, ear movement, and restrained tail;
  - walk cycle matching the current `165 cm/s` chase speed;
  - run or aggressive trot suitable for closing distance.
- Next animations when the first slice is stable:
  - bite or claw attack;
  - hit reaction;
  - death and grounded hold;
  - optional alert/snarl.
- Keep root, feet, and cycle timing deterministic enough for automated export
  and reimport.

## Unreal Integration Contract

- Import through an explicit skeletal FBX path and save every skeletal mesh,
  skeleton, material, texture, animation, and physics package deliberately.
- Keep the existing `AEmbermereEnemyCharacter` gameplay rules authoritative.
  The visual asset must be swappable through data or Blueprint configuration,
  not hard-coded into combat, loot, targeting, or quest logic.
- Retain the character capsule as the gameplay collision authority for the
  first pass. Create a restrained physics asset for editor inspection and later
  reactions, but do not let decorative fur or tail bodies block traversal.
- Recheck nameplate height, target-ring radius/height, attack range, camera
  readability, terrain contact, leash movement, death hiding, and respawn after
  the visual swap.
- Do not remove the placeholder visual path until the skeletal mesh and at least
  idle/walk playback are stable in PIE.

## Completed Vertical Slice

1. Inspect the concept and current enemy Blueprint/capsule in Unreal.
2. Build and review the deterministic base mesh, proportions, palette, UVs, and
   preview turntable in Blender.
3. Add the export skeleton and skin weights.
4. Create idle and walk cycles; add an aggressive run if time remains.
5. Export FBX and import into a project-owned Unreal folder.
6. Validate dimensions, topology, materials, bone hierarchy, animation ranges,
   package persistence, and physics asset.
7. Wire the visual through an asset-agnostic enemy presentation path.
8. Run PIE for terrain contact, animation, target ring/nameplate, chasing,
   attack distance, death, and respawn.
9. Build, run the full automation suite, document remaining polish, and keep
   the placeholder as fallback until a later production replacement is
   intentionally accepted.

## Live Acceptance 2026-07-28

Clean PIE exercised both sides of the complete prototype contract:

- real 6-damage retaliation at the 2-second cadence killed the player, triggered
  recovery protection, and sent the engaged Prowler home;
- a bounded PIE-only zero-damage diagnostic preserved enough health to prove
  four Strikes, target clear, Marsh Tonic loot, death hold, hide, 12-second
  respawn, and restored `100/100` health;
- the other two Prowlers remained at their separated homes during the focused
  kill loop;
- the saved skeletal mesh, six animation roles, capsule, target ring,
  nameplate, quest, loot, leash, and respawn behavior remained independently
  intact.

The temporary damage override was never saved. It existed only to expose the
later death/loot/respawn states without weakening the accepted combat baseline.
Future attack, transition, material, physics, or silhouette work should start
from a concrete normal-route observation rather than increasing complexity by
default.

## Acceptance Questions

- Does the creature read as an Embermere marsh predator at gameplay distance?
- Is the silhouette threatening without looking like a boss?
- Do moss, peat, mud, and amber accents remain readable in the current zone
  lighting?
- Do shoulders, hips, paws, jaw, and tail deform cleanly?
- Do paws stay planted during in-place cycles?
- Can the player still see the target ring, nameplate, and attack tells?
- Does the model survive chase, leash, death, hide, and respawn without changing
  gameplay behavior?
