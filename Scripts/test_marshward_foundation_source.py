"""Foundation source, adopted roles and full-width corridor bounds."""

import json
from pathlib import Path
import unittest
from marshward_route_clearance import overlaps_corridor

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'


class FoundationSourceTests(unittest.TestCase):
    def test_source_matches_brief(self):
        data = json.loads((ROOT/'MarshwardMarsh/foundation_metrics.json').read_text())['SM_MarshwardFoundation_01']
        self.assertEqual(data['dimensions_cm'],[600,260,150])
        self.assertEqual((data['triangles'],data['uv_channels'],data['non_manifold_edges'],data['collision_hulls']),(180,1,0,3))
        self.assertEqual(data['materials'],['M_MarshwardMarshStone','M_MarshwardMoss'])
        self.assertEqual(data['block_centers_cm'],[[-204,0,0],[-1,-12,0],[203,4,0]])
        self.assertEqual(data['block_heights_cm'],[150,128,140])
        self.assertEqual(data['ground_samples_cm'],[[-204,-150,0],[-1,-142,0],[203,-134,0]])

    def test_proposed_bounds_clear_every_fixed_corridor(self):
        rows = json.loads((ROOT/'MarshwardMarsh/foundation_placements.json').read_text())['placements']
        self.assertEqual(rows,[['FoundationWest','Foundation',[-1020,7120,120],0,[1,1,1]],
                               ['FoundationEast','Foundation',[1020,7120,120],0,[1,1,1]]])
        dressing = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())
        self.assertEqual(rows,[row for row in dressing['placements'] if row[1]=='Foundation'])
        self.assertEqual(dressing['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(dressing['buffer_cm'],75)
        route = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())['route']
        for label,kind,(x,y,z),yaw,scale in rows:
            self.assertGreater(y-130,6875)
            self.assertLess(abs(x)+300,1500)
            self.assertLess(y+130,7400)
            for start,end,width in zip(route,route[1:],dressing['corridor_half_widths_cm']):
                self.assertFalse(overlaps_corridor((x,y),(300,130),start,end,width,75),label)

    def test_intrusion_is_rejected(self):
        self.assertTrue(overlaps_corridor((1020,6900),(300,130),(0,6400),(1500,6400),400,75))
        self.assertFalse(overlaps_corridor((1020,7120),(300,130),(0,6400),(1500,6400),400,75))


if __name__ == '__main__':
    unittest.main()
