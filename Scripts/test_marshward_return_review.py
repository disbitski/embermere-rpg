"""Host guards for sparse shoulder samples and exact support ownership/heights."""

import json
from pathlib import Path
import unittest
from marshward_return_review import STATIONS,LANES,BASIN_SIDES,support_samples,clearance_segments,require_support

ROOT = Path(__file__).resolve().parents[1]


class ReturnReviewTests(unittest.TestCase):
    def test_support_plan_retains_both_sides_and_five_stations(self):
        self.assertEqual(STATIONS,(1200,2450,3100,4500,5700))
        self.assertEqual(LANES,(2150,2700,3250))
        self.assertEqual(BASIN_SIDES,(2050,3350))
        samples = support_samples()
        self.assertEqual(len(samples),len(set(samples)))
        self.assertEqual(sum(row[2]=='Marshward_EastReturn' and row[3]==0 for row in samples),15)
        self.assertEqual(sum(row[2]=='Marshward_BasinCatch' and row[3]==-120 for row in samples),10)

    def test_clearance_plan_uses_each_lane_at_two_heights(self):
        segments = clearance_segments()
        self.assertEqual(len(segments),24)
        for x in LANES:
            for a,b in zip(STATIONS,STATIONS[1:]):
                for height in (95,155):
                    self.assertIn(((x,a,height),(x,b,height)),segments)

    def test_wrong_owner_height_and_nonfinite_support_reject(self):
        require_support('Marshward_EastReturn',0,'Marshward_EastReturn',0)
        require_support('Marshward_BasinCatch',-120,'Marshward_BasinCatch',-120)
        for label,height,expected,z in [
            ('Marshward_BasinCatch',0,'Marshward_EastReturn',0),
            ('Marshward_EastReturn',-120,'Marshward_BasinCatch',-120),
            ('Marshward_EastReturn',2,'Marshward_EastReturn',0),
            ('Marshward_BasinCatch',float('nan'),'Marshward_BasinCatch',-120)]:
            with self.assertRaises(AssertionError):
                require_support(label,height,expected,z)

    def test_samples_match_original_platform_and_catch_geometry(self):
        layout = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardGreybox/layout.json').read_text())
        blocks = {r[0]:r for r in layout['blocks']}
        self.assertEqual(blocks['EastReturn'],['EastReturn',[2700,3600,-60],[1200,6400,120],'Ground'])
        self.assertEqual(blocks['BasinCatch'],['BasinCatch',[900,3400,-240],[10000,11000,240],'Basin'])
        for x,y,label,z in support_samples():
            self.assertGreater(y,400)
            self.assertLess(y,6800)
            if label=='Marshward_EastReturn':
                self.assertGreaterEqual(x,2100+50)
                self.assertLessEqual(x,3300-50)
            else:
                self.assertTrue(x<2100 or x>3300)
                self.assertGreater(x,2100-75)
                self.assertLess(x,3300+75)
        dressing = json.loads((ROOT/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json').read_text())
        self.assertEqual(dressing['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(dressing['buffer_cm'],75)


if __name__ == '__main__':
    unittest.main()
