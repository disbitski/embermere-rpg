# Embermere Blender Source

This directory holds original Embermere source art created in Blender. Unlike
raw Fab or Marketplace packs, these files may be committed and redistributed
with the public project when they are entirely original and reasonably sized.

Expected per-asset outputs:

- reviewed procedural build script under `Scripts/blender/`;
- editable `.blend` source;
- FBX interchange file for Unreal;
- one representative preview render;
- deterministic metrics for scale, topology, UVs, materials, and collision;
- an Unreal asset under `/Game/Art/Embermere` after import and validation.

Do not place downloaded third-party models, textures, or reference packs here
unless their license explicitly permits redistribution and the source is
recorded in the asset's brief.

The current original roadside family includes the waystone, two placed ember
lamps, timber road signpost, traversable gate, two low fences, and two
rune-topped end stones. The village lane now also includes one placed supply
chest and the open-sided Fenwatch shelter behind Mara. See
`Docs/BLENDER_ASSET_PIPELINE.md` for setup, security settings, naming, import
provenance, and validation.
