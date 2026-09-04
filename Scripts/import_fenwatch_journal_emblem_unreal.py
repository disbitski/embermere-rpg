"""Import only the original Blender-rendered quest emblem texture."""

from pathlib import Path
import unreal

root = Path(__file__).resolve().parents[1]
source = root / "ArtSource/Blender/UI/FenwatchJournal/T_FenwatchJournalEmblem.png"
assert source.is_file(), source
task = unreal.AssetImportTask()
task.filename = str(source)
task.destination_path = "/Game/UI/Icons"
task.destination_name = "T_FenwatchJournalEmblem"
task.automated = True
task.replace_existing = True
task.save = False
task.factory = unreal.TextureFactory()
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
texture = unreal.EditorAssetLibrary.load_asset("/Game/UI/Icons/T_FenwatchJournalEmblem")
assert isinstance(texture, unreal.Texture2D), "Expected journal Texture2D"
texture.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_EDITOR_ICON)
texture.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
texture.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
texture.set_editor_property("srgb", True)
assert unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False)
unreal.log("FENWATCH_JOURNAL_IMPORT_OK")
