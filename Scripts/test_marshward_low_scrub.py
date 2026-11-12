"""Host contracts for one restrained scrub role with original ground ownership."""

import json
import math
from pathlib import Path
import unittest
from marshward_route_clearance import overlaps_corridor

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'


class LowScrubTests(unittest.TestCase):
    def test_source_matches_brief(self):
        spec = json.loads((ROOT/'MarshwardMarsh/low_scrub_metrics.json').read_text())['SM_MarshwardLowScrub_01']
        self.assertEqual(spec['dimensions_cm'],[240,180,95])
        self.assertEqual((spec['triangles'],spec['uv_channels'],spec['non_manifold_edges'],spec['collision_hulls']),(540,1,0,0))
        self.assertEqual((spec['stem_count'],spec['leaf_count']),(3,18))
        self.assertEqual(spec['materials'],['M_MarshwardReedGreen','M_MarshwardEarth'])
        self.assertEqual(len(spec['base_samples_cm']),3)
        self.assertEqual(len({tuple(p) for p in spec['base_samples_cm']}),3)
        for x,y,z in spec['base_samples_cm']:
            self.assertTrue(all(math.isfinite(v) for v in (x,y,z)))
            self.assertEqual(z,0)
            self.assertLess(abs(x),120)
            self.assertLess(abs(y),90)

    def test_only_one_off_route_role(self):
        data = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())
        self.assertEqual(len(data['placements']),23)
        self.assertEqual([r for r in data['placements'] if r[1]=='LowScrub'],
                         [['ScrubEast','LowScrub',[3550,2450,-120],0,[1,1,1]]])
        self.assertEqual(data['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(data['buffer_cm'],75)
        route = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())['route']
        for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
            self.assertFalse(overlaps_corridor((3550,2450),(120,90),start,end,width,75))
        self.assertGreater(3550-120,3375)
        self.assertLess(3550+120,3730)
        self.assertLess(2450+90,2600)

    def test_visible_intrusion_is_rejected_despite_no_collision(self):
        self.assertTrue(overlaps_corridor((3480,2450),(120,90),(2700,6400),(2700,0),600,75))


if __name__ == '__main__':
    unittest.main()
