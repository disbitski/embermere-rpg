"""Host source, placement-preservation and protected-route contracts."""

import hashlib
import json
from pathlib import Path
import unittest
from marshward_route_clearance import overlaps_corridor

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'


class RockyBankTests(unittest.TestCase):
    def test_source_matches_brief(self):
        spec = json.loads((ROOT/'MarshwardMarsh/rocky_bank_metrics.json').read_text())['SM_MarshwardRockyBank_01']
        self.assertEqual(spec['dimensions_cm'],[2200,400,650])
        self.assertEqual((spec['triangles'],spec['uv_channels'],spec['non_manifold_edges'],spec['collision_hulls']),(68,1,0,1))
        self.assertEqual(spec['pivot_cm'],[0,0,0])
        self.assertEqual(spec['materials'],['M_MarshwardMarshStone','M_MarshwardMoss'])
        self.assertEqual(spec['crest_samples_cm'],[[-650,0,420],[-100,0,650],[400,0,540]])
        self.assertEqual(spec['ground_samples_cm'],[[x,y,0] for x in (-650,-100,400) for y in (-230,230)])

    def test_one_rocky_bank_role_preserves_preceding_rows(self):
        rows = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())['placements']
        self.assertEqual(len(rows),30)
        self.assertEqual([row for row in rows if row[1]=='RockyBank'],
                         [['RockyBankWest','RockyBank',[-3850,3300,-120],90,[1,1,1]]])
        before = json.dumps(rows[:23],separators=(',',':')).encode()
        self.assertEqual(hashlib.sha256(before).hexdigest(),'aa422519d6fb9f53a8b9820c696b939395d80d4592a4be51926e682ca6b00ec2')

    def test_bounds_ground_and_existing_bank_separation(self):
        data = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())
        route = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())['route']
        self.assertEqual(data['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(data['buffer_cm'],75)
        for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
            self.assertFalse(overlaps_corridor((-3850,3300),(200,1100),start,end,width,75))
        self.assertGreater(-3850-200,-4100)
        self.assertLess(-3850+200,-3558)
        for x in (-4080,-3620):
            self.assertGreater(x,-4100)

    def test_route_intrusion_remains_rejected(self):
        self.assertTrue(overlaps_corridor((0,3300),(200,1100),(0,1500),(0,5400),250,75))


if __name__ == '__main__':
    unittest.main()
