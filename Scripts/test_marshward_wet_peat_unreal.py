"""Fresh-process negative controls; restore every mutation and never save."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    contract = runpy.run_path(str(ROOT/'Scripts/validate_marshward_wet_peat_unreal.py'))
    assert unreal.EditorLoadingAndSavingUtils.load_map(contract['MAP'])
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    validate = contract['validate_material']
    validate(actors)
    mat = unreal.load_asset(contract['DEST']+'/'+contract['NAME'])
    lib = unreal.MaterialEditingLibrary
    nodes = {str(n.get_editor_property('desc')):n for n in lib.get_material_expressions(mat)}
    components = {a.get_actor_label():a.get_component_by_class(unreal.StaticMeshComponent)
                  for a in actors if isinstance(a,unreal.StaticMeshActor)}
    rejected = 0

    def control(mutate,restore):
        nonlocal rejected
        try:
            mutate()
            try:
                validate(actors)
            except AssertionError:
                rejected += 1
            else:
                raise RuntimeError('Invalid material state was accepted')
        finally:
            restore()
        validate(actors)

    control(lambda:nodes['WetRoughness'].set_editor_property('r',0),
            lambda:nodes['WetRoughness'].set_editor_property('r',.42))
    control(lambda:lib.disconnect_material_expressions(nodes['Color'],'Alpha'),
            lambda:lib.connect_material_expressions(nodes['Mask'],'',nodes['Color'],'Alpha'))
    control(lambda:mat.set_editor_property('enable_tessellation',True),
            lambda:mat.set_editor_property('enable_tessellation',False))
    route = components['Marshward_ArrivalIsland']
    original = route.get_material(0)
    control(lambda:route.set_material(0,mat),lambda:route.set_material(0,original))
    basin = components['Marshward_BasinCatch']
    control(lambda:basin.set_material(0,original),lambda:basin.set_material(0,mat))
    assert rejected == 5
    unreal.log('EMBERMERE_MARSHWARD_WET_PEAT_NEGATIVE_SUCCESS: 5 rejected controls, all restored, no packages saved')


if __name__ == '__main__':
    main()
