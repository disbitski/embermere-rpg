"""Persisted surface contract and initialized basin first-hit probes."""

import unreal

DEST = '/Game/Art/Embermere/Environment/MarshwardMarsh'
MAP = '/Game/Maps/L_MarshwardCauseway_Greybox'
NAME = 'SM_MarshwardStillPool_01'
MATERIAL = 'M_MarshwardStillPool'
LABEL = 'Marshward_StillPoolWest'
POS = (-1320, 3340, -118)
COLORS = {'ShoreTint': (.09, .084, .057), 'PoolTint': (.023, .075, .085)}
SCALARS = {'ShoreRoughness': .55, 'PoolRoughness': .18, 'Specular': .60, 'Metallic': 0}
LINKS = {'SurfaceTint': ('ShoreTint', 'PoolTint', 'CoreMask'),
         'SurfaceRoughness': ('ShoreRoughness', 'PoolRoughness', 'CoreMask')}
OUTPUTS = {'MP_BASE_COLOR': 'SurfaceTint', 'MP_ROUGHNESS': 'SurfaceRoughness',
           'MP_SPECULAR': 'Specular', 'MP_METALLIC': 'Metallic'}


def validate_mesh():
    mesh = unreal.load_asset(DEST+'/'+NAME)
    assert mesh and mesh.get_num_triangles(0) == 96
    meshes = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
    assert meshes.get_num_uv_channels(mesh, 0) == 1 and meshes.has_vertex_colors(mesh)
    assert mesh.get_editor_property('asset_import_data').get_class().get_name() == 'FbxStaticMeshImportData'
    bounds = mesh.get_bounds()
    origin, extent = bounds.origin, bounds.box_extent
    assert all(abs(a-b) < .1 for a,b in zip((origin.x,origin.y,origin.z,extent.x,extent.y,extent.z),
                                            (0,0,-1.5,420,260,1.5)))
    assert mesh.get_editor_property('lod_for_collision') == 0
    assert not mesh.get_editor_property('nanite_settings').get_editor_property('enabled')
    body = mesh.get_editor_property('body_setup')
    assert body.get_editor_property('collision_trace_flag') != unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE
    agg = body.get_editor_property('agg_geom')
    for field in ('box_elems', 'convex_elems', 'sphere_elems', 'sphyl_elems', 'tapered_capsule_elems',
                  'level_set_elems', 'skinned_level_set_elems', 'ml_level_set_elems',
                  'skinned_triangle_mesh_elems'):
        assert not agg.get_editor_property(field), field
    slots = list(mesh.get_editor_property('static_materials'))
    assert len(slots) == 1 and str(slots[0].material_slot_name) == MATERIAL
    material = slots[0].material_interface
    assert material.get_path_name() == DEST+'/'+MATERIAL+'.'+MATERIAL
    return material


def validate_material(material):
    assert isinstance(material, unreal.Material)
    assert material.get_editor_property('blend_mode') == unreal.BlendMode.BLEND_OPAQUE
    assert material.get_editor_property('material_domain') == unreal.MaterialDomain.MD_SURFACE
    for name in ('two_sided', 'enable_tessellation', 'phys_material'):
        assert not material.get_editor_property(name), name
    lib = unreal.MaterialEditingLibrary
    expressions = list(lib.get_material_expressions(material))
    nodes = {str(n.get_editor_property('desc')): n for n in expressions}
    types = {'CoreMask': 'VertexColor', **dict.fromkeys(COLORS, 'Constant3Vector'),
             **dict.fromkeys(SCALARS, 'Constant'), **dict.fromkeys(LINKS, 'LinearInterpolate')}
    assert len(expressions) == len(nodes) and set(nodes) == set(types)
    for label, kind in types.items():
        assert nodes[label].get_class().get_name() == 'MaterialExpression'+kind
    for label, expected in COLORS.items():
        color = nodes[label].get_editor_property('constant')
        assert all(abs(a-b) < 1e-6 for a,b in zip((color.r,color.g,color.b),expected))
    for label, expected in SCALARS.items():
        assert abs(nodes[label].get_editor_property('r')-expected) < 1e-6
    for label, inputs in LINKS.items():
        actual = [n for n in lib.get_inputs_for_material_expression(material, nodes[label]) if n]
        assert actual == [nodes[n] for n in inputs], label
    for prop, label in OUTPUTS.items():
        assert lib.get_material_property_input_node(material, getattr(unreal.MaterialProperty, prop)) == nodes[label]
    for prop in ('MP_NORMAL', 'MP_WORLD_POSITION_OFFSET', 'MP_OPACITY', 'MP_OPACITY_MASK', 'MP_EMISSIVE_COLOR'):
        assert not lib.get_material_property_input_node(material, getattr(unreal.MaterialProperty, prop))


def validate_placement(actors):
    material = validate_mesh()
    validate_material(material)
    owners = []
    for actor in actors:
        if not isinstance(actor, unreal.StaticMeshActor):
            continue
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        if material in component.get_materials():
            owners.append(actor)
    pool, = owners
    assert pool.get_actor_label() == LABEL
    assert len([a for a in actors if a.get_actor_label() == LABEL]) == 1
    component = pool.get_component_by_class(unreal.StaticMeshComponent)
    assert component.static_mesh.get_path_name() == DEST+'/'+NAME+'.'+NAME
    assert str(component.get_collision_profile_name()) == 'NoCollision'
    assert component.get_collision_enabled() == unreal.CollisionEnabled.NO_COLLISION
    for name in ('cast_shadow', 'generate_overlap_events', 'can_ever_affect_navigation'):
        assert not component.get_editor_property(name), name
    assert not component.is_simulating_physics()
    return pool


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    assert world.get_path_name().startswith(MAP+'.')
    actors = list(unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors())
    validate_placement(actors)
    basin, = [actor for actor in actors if actor.get_actor_label() == 'Marshward_BasinCatch']
    for dx,dy in ((0,0), (-290,0), (290,0), (0,-175), (0,175)):
        x,y = POS[0]+dx,POS[1]+dy
        hit = unreal.SystemLibrary.line_trace_single(
            world, unreal.Vector(x,y,200), unreal.Vector(x,y,-140),
            unreal.TraceTypeQuery.ECC_VISIBILITY, False, [], unreal.DrawDebugTrace.NONE, True)
        assert hit and hit.to_dict()['hit_actor'] == basin, (x,y)
        assert abs(hit.to_dict()['impact_point'].z+120) < .5
    unreal.log('EMBERMERE_MARSHWARD_STILL_POOL_TRACES_SUCCESS: 5 unfiltered basin hits, no new support, overlap, physics or navigation authority')


if __name__ == '__main__':
    main()
