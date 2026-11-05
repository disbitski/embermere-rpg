"""Create only the new surface material in a fresh GUI-down process."""

import unreal

DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
NAME = 'M_MarshwardWetPeat'


def main():
    assert not unreal.EditorAssetLibrary.does_asset_exist(DEST+'/'+NAME), 'Refusing material overwrite'
    mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        NAME,DEST,unreal.Material,unreal.MaterialFactoryNew())
    lib = unreal.MaterialEditingLibrary

    def node(kind,label,**properties):
        value = lib.create_material_expression(mat,getattr(unreal,'MaterialExpression'+kind))
        value.set_editor_property('desc',label)
        for key,setting in properties.items():
            value.set_editor_property(key,setting)
        return value

    world = node('WorldPosition','World')
    noise = node('Noise','Mottle',scale=.003,levels=1,quality=1,turbulence=False,
                 tiling=False,output_min=0,output_max=1,
                 noise_function=unreal.NoiseFunction.NOISEFUNCTION_VALUE_ALU)
    mask = node('Saturate','Mask')
    wet = node('Constant3Vector','WetColor',constant=unreal.LinearColor(.035,.045,.027,1))
    dry = node('Constant3Vector','DryColor',constant=unreal.LinearColor(.11,.095,.058,1))
    wet_rough = node('Constant','WetRoughness',r=.42)
    dry_rough = node('Constant','DryRoughness',r=.82)
    color = node('LinearInterpolate','Color')
    roughness = node('LinearInterpolate','Roughness')
    metallic = node('Constant','Metallic',r=0)
    position_pin = str(lib.get_material_expression_input_names(noise)[0])
    mask_pin = str(lib.get_material_expression_input_names(mask)[0])
    for source,target,pin in [(world,noise,position_pin),(noise,mask,mask_pin),
                              (wet,color,'A'),(dry,color,'B'),(mask,color,'Alpha'),
                              (wet_rough,roughness,'A'),(dry_rough,roughness,'B'),(mask,roughness,'Alpha')]:
        assert lib.connect_material_expressions(source,'',target,pin), (target.get_editor_property('desc'),pin)
    for source,prop in [(color,'MP_BASE_COLOR'),(roughness,'MP_ROUGHNESS'),(metallic,'MP_METALLIC')]:
        assert lib.connect_material_property(source,'',getattr(unreal.MaterialProperty,prop))
    lib.layout_material_expressions(mat)
    lib.recompile_material(mat)
    assert unreal.EditorAssetLibrary.save_loaded_asset(mat,False)
    unreal.log('EMBERMERE_MARSHWARD_WET_PEAT_CREATE_SUCCESS: new opaque material only, 10 nodes, no texture assets')


if __name__ == '__main__':
    main()
