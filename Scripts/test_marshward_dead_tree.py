"""Source and protected walking corridor checks for the western snag."""

import hashlib
import json
import math
from pathlib import Path
import unittest

from marshward_route_clearance import overlaps_corridor

SOURCE = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh'


class DeadTreeTests(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((SOURCE/'placements.json').read_text())
        self.tree, = [row for row in self.data['placements'] if row[1]=='DeadTree']
        self.spec = json.loads((SOURCE/'dead_tree_metrics.json').read_text())['SM_MarshwardDeadTree_01']

    def test_source_is_one_closed_trunk_without_collision(self):
        self.assertEqual(self.spec['dimensions_cm'], [720,500,800])
        self.assertEqual(self.spec['pivot_cm'], [0,0,0])
        self.assertEqual([self.spec[key] for key in ('triangles','uv_channels','non_manifold_edges','closed_pieces','collision_hulls')],
                         [260,1,0,1,0])
        self.assertEqual(self.spec['materials'], ['M_MarshwardEarth','M_EmbermereTimber','M_MarshwardMoss'])
        foot_x,foot_y,foot_z = self.spec['foot_cm']
        self.assertTrue(all(math.isfinite(value) for value in (foot_x,foot_y,foot_z)))
        self.assertLess(abs(foot_x), 360)
        self.assertLess(abs(foot_y), 250)
        self.assertEqual(foot_z, 0)

    def test_only_one_appended_role_keeps_all_prior_dressing_rows(self):
        rows = self.data['placements']
        self.assertEqual(len(rows), 29)
        self.assertEqual(self.tree, ['DeadTreeWest','DeadTree',[-2850,3350,-120],0,[1,1,1]])
        before = json.dumps(rows[:26], separators=(',',':')).encode()
        self.assertEqual(hashlib.sha256(before).hexdigest(),
                         'a9c4628519e20928f22e1ad627dcb18125be25a3f365a2e421c11759cc9d68e6')

    def test_crown_fits_in_gap_between_west_banks_and_ridge(self):
        x,y,_ = self.tree[2]
        half_x,half_y = [value/2 for value in self.spec['dimensions_cm'][:2]]
        self.assertGreater(x-half_x, -3650)
        self.assertLess(x+half_x, -325)
        self.assertGreater(y-half_y, 3050)
        self.assertLess(y+half_y, 3730)

    def test_clear_trunk_does_not_excuse_overhanging_crown(self):
        crossing = ((0,1500),(0,5400),250,75)
        self.assertFalse(overlaps_corridor((-500,3350),(80,80),*crossing))
        self.assertTrue(overlaps_corridor((-500,3350),(360,250),*crossing))


if __name__ == '__main__':
    unittest.main()
