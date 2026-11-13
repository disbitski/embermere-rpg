"""Isolated unsaved negative controls for the persisted marker parity gate."""

import runpy
from pathlib import Path
import unreal

ROOT = Path('/Users/wizard/Documents/Unreal Game')


def main():
    contract = runpy.run_path(str(ROOT/'Scripts/validate_marshward_waymarker_unreal.py'))
    mesh = unreal.load_asset(contract['NEW'])
    body = mesh.get_editor_property('body_setup')
    original = body.get_editor_property('agg_geom').copy()
    material = mesh.get_material(0)
    accepted = 0
    contract['validate_mesh'](mesh)
    try:
        for field,value in [('x',130),('center',unreal.Vector(1,0,220)),('rotation',None),('material',None)]:
            aggregate = original.copy()
            if field=='material':
                mesh.set_material(0,unreal.load_asset('/Game/Art/Embermere/Environment/MarshwardGreybox/M_MarshwardGreyboxStone'))
            else:
                box = aggregate.get_editor_property('box_elems')[0].copy()
                if field=='rotation':
                    value = box.get_editor_property('rotation')
                    value.yaw += 15
                box.set_editor_property(field,value)
                aggregate.set_editor_property('box_elems',[box])
                body.set_editor_property('agg_geom',aggregate)
            try:
                contract['validate_mesh'](mesh)
            except AssertionError:
                accepted += 1
            else:
                raise RuntimeError('Parity accepted counterfactual '+field)
            finally:
                body.set_editor_property('agg_geom',original.copy())
                mesh.set_material(0,material)
        contract['validate_mesh'](mesh)
        assert accepted==4
    finally:
        body.set_editor_property('agg_geom',original)
        mesh.set_material(0,material)
    unreal.log('EMBERMERE_MARSHWARD_WAYMARKER_NEGATIVE_SUCCESS: expanded box, shifted center, changed rotation and wrong material rejected; all transient edits restored, no saves')


if __name__ == '__main__':
    main()
