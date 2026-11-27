"""Host controls for source/import comparisons, including an asymmetric pivot."""

import copy
import unittest

from audit_marshward_asset_quality_unreal import compare_mesh


class AssetQualityTests(unittest.TestCase):
    def setUp(self):
        self.source = {'triangles': 92, 'uv_channels': 1, 'materials': ['Mud'],
                       'bounds_min': [-49, -275, -4], 'bounds_max': [484, 229, -.4],
                       'simple_shapes': {'box_elems': 0, 'convex_elems': 0}}
        self.imported = {**copy.deepcopy(self.source), 'bounds_min': [-49, -229, -4],
                         'bounds_max': [484, 275, -.4], 'complex_as_simple': False}

    def test_fbx_mirrored_asymmetric_bounds_and_empty_shapes_match(self):
        compare_mesh(self.source, self.imported)

    def test_pivot_shift_and_nonfinite_or_truncated_bounds_reject(self):
        for bounds in ([-49, -275, -4], [float('nan'), -229, -4], [-49, -229]):
            with self.subTest(bounds=bounds), self.assertRaises(AssertionError):
                compare_mesh(self.source, {**self.imported, 'bounds_min': bounds})

    def test_uv_material_geometry_and_collision_drift_reject(self):
        for key, value in (('triangles', 90), ('uv_channels', 2),
                           ('materials', ['Other']), ('simple_shapes', {'box_elems': 1}),
                           ('complex_as_simple', True)):
            with self.subTest(key=key), self.assertRaises(AssertionError):
                compare_mesh(self.source, {**self.imported, key: value})


if __name__ == '__main__':
    unittest.main()
