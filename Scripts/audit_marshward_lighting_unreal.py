"""Read current light settings and effective material owners without saving."""

import json
from pathlib import Path
import runpy

ROOT = Path(__file__).resolve().parents[1]
MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'


def material_owners(assignments):
    owners, seen = {}, set()
    for label, slot, path in assignments:
        # bool subclasses int but is not a material slot identifier.
        if not label or not path or type(slot) is not int or slot < 0:
            raise ValueError('Expected a named owner, nonnegative integer slot and material path')
        identity = (label, slot)
        if identity in seen:
            raise ValueError('Duplicate owner slot: '+str(identity))
        seen.add(identity)
        owners.setdefault(path, []).append([label, slot])
    return {path: sorted(rows) for path, rows in sorted(owners.items())}


def main():
    import unreal

    def clean():
        assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
        assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()

    editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor.get_editor_world()
    assert world.get_path_name() == MAP+'.L_MarshwardCauseway_Greybox'
    assert not editor.get_game_world()
    clean()
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    assignments, materials, lights, atmosphere = [], {}, [], []
    labels = [actor.get_actor_label() for actor in actors]
    assert len(labels) == len(set(labels)), 'Ambiguous actor labels'

    for actor in actors:
        label = actor.get_actor_label()
        if isinstance(actor, unreal.StaticMeshActor):
            component = actor.get_component_by_class(unreal.StaticMeshComponent)
            for slot, material in enumerate(component.get_materials()):
                assert isinstance(material, unreal.Material), (label, slot, material)
                path = material.get_path_name()
                assignments.append((label, slot, path))
                materials[path] = material

        for cls, fields in (
            (unreal.DirectionalLightComponent, ('intensity', 'light_color', 'cast_shadows',
                                               'atmosphere_sun_light')),
            (unreal.SkyLightComponent, ('intensity', 'light_color', 'cast_shadows',
                                      'real_time_capture', 'source_type',
                                      'lower_hemisphere_is_black', 'lower_hemisphere_color')),
        ):
            for component in actor.get_components_by_class(cls):
                values = {}
                for field in fields:
                    value = component.get_editor_property(field)
                    if isinstance(value, unreal.Color):
                        value = [value.r, value.g, value.b, value.a]
                    elif isinstance(value, unreal.LinearColor):
                        value = [value.r, value.g, value.b, value.a]
                    elif not isinstance(value, (bool, int, float, str)):
                        value = str(value)
                    values[field] = value
                rotation = actor.get_actor_rotation()
                lights.append({'label': label, 'component': component.get_class().get_name(),
                               'visible': component.is_visible(),
                               'mobility': str(component.get_editor_property('mobility')),
                               'pitch_yaw_roll': [rotation.pitch, rotation.yaw, rotation.roll],
                               **values})
        for cls in (unreal.SkyAtmosphereComponent, unreal.ExponentialHeightFogComponent,
                    unreal.VolumetricCloudComponent, unreal.PostProcessComponent):
            for component in actor.get_components_by_class(cls):
                atmosphere.append({'label': label, 'component': component.get_class().get_name(),
                                   'visible': component.is_visible()})
        for component in actor.get_components_by_class(unreal.LightComponentBase):
            if isinstance(component, (unreal.DirectionalLightComponent, unreal.SkyLightComponent)):
                continue
            lights.append({'label': label, 'component': component.get_class().get_name(),
                           'visible': component.is_visible(),
                           'intensity': component.get_editor_property('intensity'),
                           'mobility': str(component.get_editor_property('mobility'))})

    owners = material_owners(assignments)
    lib = unreal.MaterialEditingLibrary
    report = []
    for path, material in sorted(materials.items()):
        inputs = {}
        for prop in ('MP_BASE_COLOR', 'MP_ROUGHNESS', 'MP_METALLIC', 'MP_EMISSIVE_COLOR'):
            node = lib.get_material_property_input_node(material, getattr(unreal.MaterialProperty, prop))
            inputs[prop] = node.get_class().get_name() if node else None
        report.append({'path': path, 'owners': owners[path],
                       'blend': str(material.get_editor_property('blend_mode')),
                       'domain': str(material.get_editor_property('material_domain')),
                       'shading_model': str(material.get_editor_property('shading_model')),
                       'two_sided': material.get_editor_property('two_sided'), 'inputs': inputs})

    # Reuse the existing exact graph and ownership checks, without loading maps.
    scripts = ROOT/'Scripts'
    runpy.run_path(str(scripts/'validate_marshward_wet_peat_unreal.py'))['validate_material'](actors)
    runpy.run_path(str(scripts/'validate_marshward_still_pool_unreal.py'))['validate_placement'](actors)
    runpy.run_path(str(scripts/'validate_marshward_pool_shore_unreal.py'))['validate_placement'](actors)
    clean()
    print('MARSHWARD_LIGHTING_SETTINGS', json.dumps(sorted(lights, key=lambda row: (row['label'], row['component'])),
                                                 sort_keys=True))
    print('MARSHWARD_ATMOSPHERE_COMPONENTS', json.dumps(
        sorted(atmosphere, key=lambda row: (row['label'], row['component'])), sort_keys=True))
    print('MARSHWARD_POSTPROCESS_VOLUMES',
          json.dumps(sorted(a.get_actor_label() for a in actors if isinstance(a, unreal.PostProcessVolume))))
    for row in report:
        print('MARSHWARD_LIGHTING_MATERIAL', json.dumps(row, sort_keys=True))
    print('EMBERMERE_MARSHWARD_LIGHTING_AUDIT_SUCCESS: '
          f'{len(lights)} lights, {len(materials)} effective materials, '
          f'{len(assignments)} owner slots, exact wet-peat/pool/shore ownership, no dirty packages')


if __name__ == '__main__':
    main()
