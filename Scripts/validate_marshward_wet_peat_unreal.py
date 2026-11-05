"""Persisted material graph and basin-only presentation ownership."""

import json
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')
MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'
DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
NAME = 'M_MarshwardWetPeat'


def validate_material(actors):
    mat = unreal.load_asset(DEST+'/'+NAME)
    assert isinstance(mat, unreal.Material), 'Missing wet peat material'
    assert mat.get_editor_property('blend_mode') == unreal.BlendMode.BLEND_OPAQUE
    assert mat.get_editor_property('material_domain') == unreal.MaterialDomain.MD_SURFACE
    assert not mat.get_editor_property('two_sided')
    assert not mat.get_editor_property('enable_tessellation')
    assert not mat.get_editor_property('phys_material')
    lib = unreal.MaterialEditingLibrary
    expressions = list(lib.get_material_expressions(mat))
    nodes = {str(n.get_editor_property('desc')): n for n in expressions}
    types = {'World':'WorldPosition', 'Mottle':'Noise', 'Mask':'Saturate',
             'WetColor':'Constant3Vector', 'DryColor':'Constant3Vector',
             'WetRoughness':'Constant', 'DryRoughness':'Constant',
             'Color':'LinearInterpolate', 'Roughness':'LinearInterpolate',
             'Metallic':'Constant'}
    assert len(expressions) == len(nodes) == 10 and set(nodes) == set(types)
    for label, kind in types.items():
        assert nodes[label].get_class().get_name() == 'MaterialExpression'+kind
    for label, expected in [('WetColor',(.035,.045,.027)),('DryColor',(.11,.095,.058))]:
        color = nodes[label].get_editor_property('constant')
        assert all(abs(a-b)<1e-6 for a,b in zip((color.r,color.g,color.b),expected))
    for label, expected in [('WetRoughness',.42),('DryRoughness',.82),('Metallic',0)]:
        assert abs(nodes[label].get_editor_property('r')-expected)<1e-6
    noise = nodes['Mottle']
    assert abs(noise.get_editor_property('scale')-.003)<1e-6
    assert noise.get_editor_property('levels') == noise.get_editor_property('quality') == 1
    assert noise.get_editor_property('noise_function') == unreal.NoiseFunction.NOISEFUNCTION_VALUE_ALU
    assert not noise.get_editor_property('turbulence') and not noise.get_editor_property('tiling')
    assert noise.get_editor_property('output_min') == 0 and noise.get_editor_property('output_max') == 1
    for label, inputs in [('Mottle',['World']),('Mask',['Mottle']),
                          ('Color',['WetColor','DryColor','Mask']),
                          ('Roughness',['WetRoughness','DryRoughness','Mask'])]:
        actual = [n for n in lib.get_inputs_for_material_expression(mat,nodes[label]) if n]
        assert actual == [nodes[n] for n in inputs], label+' graph changed'
    for prop, label in [('MP_BASE_COLOR','Color'),('MP_ROUGHNESS','Roughness'),('MP_METALLIC','Metallic')]:
        assert lib.get_material_property_input_node(mat,getattr(unreal.MaterialProperty,prop)) == nodes[label]
    for prop in ['MP_NORMAL','MP_WORLD_POSITION_OFFSET','MP_OPACITY',
                 'MP_OPACITY_MASK','MP_EMISSIVE_COLOR']:
        assert not lib.get_material_property_input_node(mat,getattr(unreal.MaterialProperty,prop))
    owners = []
    layout = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())
    materials = {label:family for label,_,_,family in layout['blocks']}
    for actor in actors:
        if not isinstance(actor,unreal.StaticMeshActor):
            continue
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        for index in range(component.get_num_materials()):
            if component.get_material(index) == mat:
                owners.append((actor.get_actor_label(),index))
        label = actor.get_actor_label().removeprefix('Marshward_')
        if label in materials and label != 'BasinCatch':
            expected = '/Game/Art/Embermere/Environment/MarshwardGreybox/M_MarshwardGreybox'+materials[label]
            assert component.get_material(0).get_path_name() == expected+'.'+expected.rsplit('/',1)[1]
    assert owners == [('Marshward_BasinCatch',0)], str(owners)
    unreal.log('EMBERMERE_MARSHWARD_WET_PEAT_SUCCESS: opaque 10-node graph, 1 noise level, basin-only override, no displacement or physics authority')


def main():
    assert unreal.EditorLoadingAndSavingUtils.load_map(MAP)
    validate_material(list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()))


if __name__ == '__main__':
    main()
