"""Fresh-package validation of the presentation-only quest emblem."""

import unreal

def main():
    texture = unreal.EditorAssetLibrary.load_asset("/Game/UI/Icons/T_FenwatchJournalEmblem")
    assert isinstance(texture, unreal.Texture2D), "Missing journal Texture2D"
    assert texture.blueprint_get_size_x() == 512 and texture.blueprint_get_size_y() == 512
    assert texture.get_editor_property("compression_settings") == unreal.TextureCompressionSettings.TC_EDITOR_ICON
    assert texture.get_editor_property("mip_gen_settings") == unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS
    assert texture.get_editor_property("lod_group") == unreal.TextureGroup.TEXTUREGROUP_UI
    assert texture.get_editor_property("srgb")
    unreal.log("FENWATCH_JOURNAL_VALIDATION_OK")


if __name__ == "__main__":
    main()
