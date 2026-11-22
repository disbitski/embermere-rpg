"""Read the loaded Causeway's asset and placement budget without saving."""

from collections import Counter
import json
from pathlib import Path

MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'
SOURCE = Path(__file__).resolve().parents[1] / 'ArtSource/Blender/Environment'
RETAINED = ('/Game/Art/Embermere/Environment/MarshwardGreybox/'
            'SM_MarshwardWaymarker_Greybox.SM_MarshwardWaymarker_Greybox')
SHAPES = (
    'box_elems', 'convex_elems', 'sphere_elems', 'sphyl_elems', 'tapered_capsule_elems',
    'level_set_elems', 'skinned_level_set_elems', 'ml_level_set_elems',
    'skinned_triangle_mesh_elems',
)


def summarize(assets, placements):
    assigned = Counter()
    active_shapes = Counter()
    profiles = Counter()
    installed = {}
    lod0_triangles = lod0_sections = 0
    for placement in placements:
        path = placement['mesh']
        asset = assets[path]
        installed[path] = asset
        lod0_triangles += asset['lod0_triangles']
        lod0_sections += asset['lod0_sections']
        assigned.update(placement['materials'])
        profiles[placement['profile']] += 1
        if placement['collision_enabled']:
            active_shapes.update(asset['simple_shapes'])

    return {
        'placements': len(placements),
        'installed_meshes': len(installed),
        'source_meshes_installed': sum(asset['source_triangles'] is not None for asset in installed.values()),
        'lod0_triangles_unique': sum(asset['lod0_triangles'] for asset in installed.values()),
        'lod0_sections_unique': sum(asset['lod0_sections'] for asset in installed.values()),
        'lod0_triangles_placed': lod0_triangles,
        'lod0_sections_placed': lod0_sections,
        'source_triangles_installed_unique': sum(asset['source_triangles'] or 0 for asset in installed.values()),
        'source_triangles_placed': sum(assets[p['mesh']]['source_triangles'] or 0 for p in placements),
        'material_assignments': dict(sorted(assigned.items())),
        'active_simple_shapes': dict(sorted(active_shapes.items())),
        'profiles': dict(sorted(profiles.items())),
        'unplaced_assets': sorted(assets.keys() - installed.keys()),
    }


def source_triangles():
    files = [SOURCE / 'MarshwardGreybox/kit_metrics.json',
             SOURCE / 'MarshwardWaymarker/waymarker_metrics.json',
             *sorted((SOURCE / 'MarshwardMarsh').glob('*metrics.json'))]
    counts = {}
    for path in files:
        for name, spec in json.loads(path.read_text()).items():
            assert name not in counts, name
            counts[name] = spec['triangles']
    return counts


def main():
    import unreal

    def require_clean():
        assert not unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages()
        assert not unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()

    editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor.get_editor_world()
    assert world.get_path_name() == MAP + '.L_MarshwardCauseway_Greybox'
    assert not editor.get_game_world()
    require_clean()
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    counts = source_triangles()
    assets, placements, material_classes = {}, [], {}

    def read_mesh(mesh):
        body = mesh.get_editor_property('body_setup')
        aggregate = body.get_editor_property('agg_geom')
        shapes = {field: len(aggregate.get_editor_property(field)) for field in SHAPES}
        nanite = mesh.get_editor_property('nanite_settings')
        return {
            'source_triangles': counts.get(mesh.get_name()),
            'lod_count': mesh.get_num_lods(),
            'lod0_triangles': mesh.get_num_triangles(0),
            'lod0_sections': mesh.get_num_sections(0),
            'mesh_material_slots': len(mesh.get_editor_property('static_materials')),
            'nanite_enabled': nanite.get_editor_property('enabled'),
            'simple_shapes': {name: num for name, num in shapes.items() if num},
            'collision_trace_flag': str(body.get_editor_property('collision_trace_flag')),
        }

    for actor in actors:
        if not isinstance(actor, unreal.StaticMeshActor):
            continue
        components = actor.get_components_by_class(unreal.StaticMeshComponent)
        assert len(components) == 1
        assert not isinstance(components[0], unreal.InstancedStaticMeshComponent)
        component = components[0]
        mesh = component.static_mesh
        path = mesh.get_path_name()
        if path not in assets:
            assets[path] = read_mesh(mesh)
        materials = []
        for material in component.get_materials():
            assert material is not None, actor.get_actor_label()
            material_path = material.get_path_name()
            materials.append(material_path)
            material_classes[material_path] = material.get_class().get_name()
        placements.append({
            'label': actor.get_actor_label(), 'mesh': path, 'materials': materials,
            'profile': str(component.get_collision_profile_name()),
            'collision_enabled': component.get_collision_enabled() != unreal.CollisionEnabled.NO_COLLISION,
        })
    assert len({placement['label'] for placement in placements}) == len(placements)
    assert RETAINED not in assets
    assets[RETAINED] = read_mesh(unreal.load_asset(RETAINED))
    names = [path.rsplit('.', 1)[-1] for path in assets if path.startswith('/Game/')]
    assert len(names) == len(set(names)) and set(names) == set(counts)
    summary = summarize(assets, placements)
    require_clean()
    copies = Counter(placement['mesh'] for placement in placements)
    for path, data in sorted(assets.items()):
        row = {'path': path, 'copies': copies[path], **data}
        print('MARSHWARD_BUDGET_MESH', json.dumps(row, sort_keys=True))
    print('MARSHWARD_BUDGET_SUMMARY', json.dumps(summary, sort_keys=True))
    print('MARSHWARD_BUDGET_MATERIAL_CLASSES', json.dumps(material_classes, sort_keys=True))
    print('EMBERMERE_MARSHWARD_BUDGET_SUCCESS: loaded map audited, no dirty packages')


if __name__ == '__main__':
    main()
