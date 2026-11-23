"""Host source, exact prior-role preservation and off-deck bounds checks."""

import hashlib
import json
import math
from pathlib import Path
import unittest
from marshward_route_clearance import overlaps_corridor

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'


class TimberDebrisTests(unittest.TestCase):
    def test_source_matches_visual_only_brief(self):
        spec = json.loads((ROOT/'MarshwardMarsh/timber_debris_metrics.json').read_text())['SM_MarshwardTimberDebris_01']
        self.assertEqual(spec['dimensions_cm'],[380,260,30])
        self.assertEqual((spec['triangles'],spec['uv_channels'],spec['non_manifold_edges'],spec['collision_hulls']),(84,1,0,0))
        self.assertEqual(spec['closed_pieces'],3)
        self.assertEqual(spec['pivot_cm'],[0,0,0])
        self.assertEqual(spec['materials'],['M_EmbermereTimber','M_MarshwardMoss'])
        self.assertEqual(len(spec['base_samples_cm']),3)
        self.assertEqual(len({tuple(p) for p in spec['base_samples_cm']}),3)
        for x,y,z in spec['base_samples_cm']:
            self.assertTrue(all(math.isfinite(v) for v in (x,y,z)))
            self.assertEqual(z,0)
            self.assertLess(abs(x),190)
            self.assertLess(abs(y),130)

    def test_only_one_appended_role_preserves_all_prior_rows(self):
        rows = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())['placements']
        self.assertEqual(len(rows),30)
        self.assertEqual([row for row in rows if row[1]=='TimberDebris'],
                         [['TimberDebrisInner','TimberDebris',[1200,3300,-120],0,[1,1,1]]])
        self.assertEqual(hashlib.sha256(json.dumps(rows[:24],separators=(',',':')).encode()).hexdigest(),
                         '7b765b46c28ee14c813435ead466feb52f631a8224f9a8db3002e8e99a40d2b0')

    def test_visible_bounds_leave_banks_and_full_corridors_separate(self):
        data = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())
        route = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())['route']
        self.assertEqual(data['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(data['buffer_cm'],75)
        for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
            self.assertFalse(overlaps_corridor((1200,3300),(190,130),start,end,width,75))
        self.assertGreater(1200-190,325)
        self.assertLess(1200+190,2025)
        self.assertGreater(3300-130,2825)
        self.assertLess(3300+130,3775)

    def test_no_collision_does_not_excuse_visible_deck_intrusion(self):
        self.assertTrue(overlaps_corridor((400,3300),(190,130),(0,1500),(0,5400),250,75))


if __name__ == '__main__':
    unittest.main()
