"""Host regressions for the isolated decorative pool footprint."""

import hashlib
import json
import math
from pathlib import Path
import unittest

from marshward_route_clearance import overlaps_corridor

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'
SOURCE = ROOT/'MarshwardMarsh'


class StillPoolTests(unittest.TestCase):
    def setUp(self):
        self.rows = json.loads((SOURCE/'placements.json').read_text())['placements']
        self.pool = self.rows[29]
        self.layout = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())
        self.spec = json.loads((SOURCE/'still_pool_metrics.json').read_text())['SM_MarshwardStillPool_01']

    def test_only_one_addition_keeps_the_prior_29_placements(self):
        self.assertEqual(len(self.rows), 31)
        self.assertEqual(self.pool, ['StillPoolWest','StillPool',[-1320,3340,-118],0,[1,1,1]])
        self.assertEqual([row for row in self.rows if row[1] == 'StillPool'], [self.pool])
        before = json.dumps(self.rows[:29], separators=(',', ':')).encode()
        self.assertEqual(hashlib.sha256(before).hexdigest(),
                         'b1e8a1c61b2153d2e504a6a1ece31fc46fc1d8e03debba7e1f0134118341fce0')

    def test_waterline_is_flat_shallow_and_below_the_route(self):
        self.assertEqual(self.spec['dimensions_cm'], [840,520,3])
        self.assertEqual(self.spec['pivot_cm'], [0,0,0])
        self.assertEqual(self.spec['top_plane_cm'], 0)
        self.assertEqual(self.spec['materials'], ['M_MarshwardStillPool'])
        self.assertEqual([self.spec[key] for key in ('triangles','uv_channels','non_manifold_edges','collision_hulls')],
                         [96,1,0,0])
        basin, = [b for b in self.layout['blocks'] if b[0] == 'BasinCatch']
        basin_top = basin[1][2]+basin[2][2]/2
        waterline = self.pool[2][2]
        self.assertEqual(waterline-basin_top, 2)
        self.assertLess(waterline-self.spec['dimensions_cm'][2], basin_top)
        self.assertLess(waterline, min(p[2] for p in self.layout['route']))

    def test_footprint_clears_both_west_banks_and_reeds(self):
        specs = {}
        for path in SOURCE.glob('*metrics.json'):
            specs.update(json.loads(path.read_text()))
        px,py,_ = self.pool[2]
        for label,kind,(x,y,_),yaw,(sx,sy,_) in self.rows[:29]:
            width,depth,_ = specs['SM_Marshward'+kind+'_01']['dimensions_cm']
            c,s = (abs(f(math.radians(yaw))) for f in (math.cos,math.sin))
            gap_x = abs(px-x)-420-(width*sx*c+depth*sy*s)/2
            gap_y = abs(py-y)-260-(width*sx*s+depth*sy*c)/2
            self.assertGreater(max(gap_x, gap_y), 0, label)

    def test_no_collision_does_not_allow_touching_a_protected_buffer(self):
        cross = ((0,1500,120),(0,5400,120),250,75)
        self.assertTrue(overlaps_corridor((-745,3340), (420,260), *cross))
        self.assertFalse(overlaps_corridor((-746,3340), (420,260), *cross))


if __name__ == '__main__':
    unittest.main()
