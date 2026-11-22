"""Host checks for the read-only Causeway inventory accounting."""

import unittest

from audit_marshward_budget_unreal import summarize


def mesh(triangles, sections, source_triangles, **shapes):
    return {
        'lod0_triangles': triangles,
        'lod0_sections': sections,
        'source_triangles': source_triangles,
        'simple_shapes': shapes,
    }


def placement(path, materials, profile='BlockAll', collision_enabled=True):
    return {
        'mesh': path,
        'materials': materials,
        'profile': profile,
        'collision_enabled': collision_enabled,
    }


class MarshwardBudgetTests(unittest.TestCase):
    def setUp(self):
        self.assets = {
            'wall': mesh(100, 2, 100, convex_elems=3),
            'cube': mesh(48, 1, None, box_elems=1),
            'retained': mesh(36, 1, 36, box_elems=1),
        }
        self.placements = [
            placement('wall', ['stone', 'moss']),
            placement('wall', ['stone', 'earth'], 'NoCollision', False),
            placement('cube', ['peat']),
        ]
        self.result = summarize(self.assets, self.placements)

    def test_lod0_unique_and_placed_counts_are_distinct(self):
        self.assertEqual(self.result['placements'], 3)
        self.assertEqual(self.result['installed_meshes'], 2)
        self.assertEqual(self.result['lod0_triangles_unique'], 148)
        self.assertEqual(self.result['lod0_triangles_placed'], 248)
        self.assertEqual(self.result['lod0_sections_unique'], 3)
        self.assertEqual(self.result['lod0_sections_placed'], 5)

    def test_source_counts_exclude_engine_mesh_and_unplaced_asset(self):
        self.assertEqual(self.result['source_meshes_installed'], 1)
        self.assertEqual(self.result['source_triangles_installed_unique'], 100)
        self.assertEqual(self.result['source_triangles_placed'], 200)
        self.assertEqual(self.result['unplaced_assets'], ['retained'])

    def test_effective_materials_count_overrides_but_disabled_collision_does_not(self):
        self.assertEqual(self.result['material_assignments'],
                         {'stone': 2, 'moss': 1, 'earth': 1, 'peat': 1})
        self.assertEqual(self.result['active_simple_shapes'],
                         {'box_elems': 1, 'convex_elems': 3})
        self.assertEqual(self.result['profiles'], {'BlockAll': 2, 'NoCollision': 1})


if __name__ == '__main__':
    unittest.main()
