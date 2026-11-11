"""Host source and role contracts for the sparse NoCollision reed variant."""

import json
import math
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh'


class TallReedsTests(unittest.TestCase):
    def test_variant_matches_sparse_brief(self):
        data = json.loads((ROOT/'tall_reeds_metrics.json').read_text())['SM_MarshwardTallReeds_01']
        self.assertEqual(data['dimensions_cm'],[220,180,300])
        self.assertEqual((data['triangles'],data['uv_channels'],data['non_manifold_edges'],data['collision_hulls']),(264,1,0,0))
        self.assertEqual((data['stem_count'],data['leaf_count']),(3,6))
        self.assertEqual(data['materials'],['M_MarshwardReedGreen','M_MarshwardReedSeed'])
        original = json.loads((ROOT/'mesh_metrics.json').read_text())['SM_MarshwardReedClump_01']
        self.assertEqual(original['dimensions_cm'],[260,220,210])
        self.assertEqual(original['triangles'],616)
        self.assertLess(data['triangles'],original['triangles'])
        self.assertGreater(data['dimensions_cm'][2],original['dimensions_cm'][2])

    def test_only_east_roles_change_without_new_placements(self):
        rows = json.loads((ROOT/'placements.json').read_text())['placements']
        self.assertEqual(len(rows),22)
        self.assertEqual([row for row in rows if row[1]=='TallReeds'],[
            ['ReedsEastNear','TallReeds',[3550,1700,-120],0,[1,1,1]],
            ['ReedsEastFar','TallReeds',[3550,5200,-120],20,[1,1,1]]])
        self.assertEqual([row[0] for row in rows if row[1]=='ReedClump'],[
            'ReedsWestNear','ReedsWestMid','ReedsWestFar','ReedsInnerNear','ReedsInnerFar','ReedsInnerEast'])
        extent = 110*math.cos(math.radians(20))+90*math.sin(math.radians(20))
        self.assertGreater(3550-extent,3375)

    def test_three_authored_stem_bases_support_six_world_probes(self):
        data = json.loads((ROOT/'tall_reeds_metrics.json').read_text())['SM_MarshwardTallReeds_01']
        samples = data['base_samples_cm']
        self.assertEqual(len(samples),3)
        self.assertEqual(len({tuple(p) for p in samples}),3)
        for x,y,z in samples:
            self.assertTrue(all(math.isfinite(v) for v in (x,y,z)))
            self.assertEqual(z,0)
            self.assertLess(abs(x),110)
            self.assertLess(abs(y),90)


if __name__ == '__main__':
    unittest.main()
