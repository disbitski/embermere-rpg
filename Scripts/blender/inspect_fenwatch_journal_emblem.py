"""Inspect generated alpha pixels through the live bridge without scene edits."""

import bpy

path = "/Users/wizard/Documents/Unreal Game/ArtSource/Blender/UI/FenwatchJournal/T_FenwatchJournalEmblem.png"
image = bpy.data.images.load(path, check_existing=False)
try:
    width, height = image.size
    pixels = list(image.pixels)
    alpha = pixels[3::4]
    occupied = [index for index, value in enumerate(alpha) if value > 0.01]
    assert (width, height) == (512, 512)
    assert min(alpha) == 0.0 and max(alpha) == 1.0
    bounds = [min(i % width for i in occupied), min(i // width for i in occupied),
              max(i % width for i in occupied), max(i // width for i in occupied)]
    assert bounds[0] > 8 and bounds[1] > 8 and bounds[2] < 504 and bounds[3] < 504, bounds
    __result__ = {"size": [width, height], "alpha_range": [min(alpha), max(alpha)],
                  "occupied_bounds": bounds, "transparent_pixels": sum(v == 0 for v in alpha),
                  "scene_objects_unchanged": len(bpy.context.scene.objects)}
    print("FENWATCH_JOURNAL_PIXELS_OK", __result__)
finally:
    bpy.data.images.remove(image)
