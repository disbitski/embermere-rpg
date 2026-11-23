"""Host checks for the western ground accent and its separation."""

import hashlib
import json
import math
from pathlib import Path
import unittest

from marshward_route_clearance import overlaps_corridor

SOURCE = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh'


class WestTussockTests(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((SOURCE/'placements.json').read_text())
        self.tussock, = [row for row in self.data['placements'] if row[1]=='WestTussock']
        self.spec = json.loads((SOURCE/'west_tussock_metrics.json').read_text())['SM_MarshwardWestTussock_01']

    def test_source_has_a_flat_foot_and_nine_closed_blades(self):
        self.assertEqual(self.spec['dimensions_cm'],[240,170,72])
        self.assertEqual(self.spec['pivot_cm'],[0,0,0])
        self.assertEqual([self.spec[key] for key in ('triangles','uv_channels','non_manifold_edges','closed_pieces','collision_hulls')],
                         [304,1,0,10,0])
        self.assertEqual(self.spec['materials'],['M_MarshwardPeat','M_MarshwardMoss','M_MarshwardReedGreen'])
        samples = self.spec['foot_samples_cm']
        self.assertEqual(len(samples),3)
        self.assertEqual(len({tuple(sample) for sample in samples}),3)
        for x,y,z in samples:
            self.assertTrue(all(math.isfinite(value) for value in (x,y,z)))
            self.assertLess(abs(x),120)
            self.assertLess(abs(y),85)
            self.assertEqual(z,0)

    def test_only_one_new_role_keeps_all_prior_dressing_rows(self):
        rows = self.data['placements']
        self.assertEqual(len(rows),30)
        self.assertEqual(self.tussock,['WestTussock','WestTussock',[-2290,3480,-120],0,[1,1,1]])
        before = json.dumps(rows[:27],separators=(',',':')).encode()
        self.assertEqual(hashlib.sha256(before).hexdigest(),
                         'eaef7625c4b107e8b1815fef06dbcf6935652c7fe8e0590feff728d492b13854')

    def test_cushion_is_separate_from_the_entire_tree_and_banks(self):
        tree, = [row for row in self.data['placements'] if row[1]=='DeadTree']
        size = json.loads((SOURCE/'dead_tree_metrics.json').read_text())['SM_MarshwardDeadTree_01']['dimensions_cm']
        x,y,_ = self.tussock[2]
        half_x,half_y = [value/2 for value in self.spec['dimensions_cm'][:2]]
        self.assertGreaterEqual((x-half_x)-(tree[2][0]+size[0]/2),80)
        self.assertGreater(y-half_y,3050)
        self.assertLess(y+half_y,3730)

    def test_visible_footprint_cannot_touch_the_route_buffer(self):
        extent = tuple(value/2 for value in self.spec['dimensions_cm'][:2])
        crossing = ((0,1500),(0,5400),250,75)
        self.assertFalse(overlaps_corridor((-446,3480),extent,*crossing))
        self.assertTrue(overlaps_corridor((-445,3480),extent,*crossing))


if __name__ == '__main__':
    unittest.main()
