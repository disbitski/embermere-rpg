"""Host source/placement contracts for the one sparse masonry scatter."""

import json
from pathlib import Path
import unittest
from marshward_route_clearance import overlaps_corridor

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'


class FallenMasonryTests(unittest.TestCase):
    def test_source_and_probes_match_brief(self):
        data = json.loads((ROOT/'MarshwardMarsh/fallen_masonry_metrics.json').read_text())['SM_MarshwardFallenMasonry_01']
        self.assertEqual(data['dimensions_cm'],[500,300,140])
        self.assertEqual((data['triangles'],data['uv_channels'],data['non_manifold_edges'],data['collision_hulls']),(88,1,0,2))
        self.assertEqual(data['materials'],['M_MarshwardMarshStone','M_MarshwardMoss'])
        for key,expected in [('fragment_centers_cm',[[-88.801627,-58.579589,0],[168.255939,73.714372,0]]),
                             ('ground_samples_cm',[[-88.801627,-167.639195,0],[168.255939,-25.818859,0]])]:
            self.assertEqual(len(data[key]),2)
            for actual,point in zip(data[key],expected):
                for a,b in zip(actual,point):
                    self.assertAlmostEqual(a,b,places=4)
        for a,b in zip(data['fragment_heights_cm'],[128.380938,84.928928]):
            self.assertAlmostEqual(a,b,places=4)

    def test_single_role_preserves_neighbours_and_corridors(self):
        data = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())
        self.assertEqual([r for r in data['placements'] if r[1]=='FallenMasonry'],
                         [['FallenMasonryWest','FallenMasonry',[-1130,6700,120],0,[1,1,1]]])
        self.assertEqual(data['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(data['buffer_cm'],75)
        layout = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())
        route = layout['route']
        for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
            self.assertFalse(overlaps_corridor((-1130,6700),(250,150),start,end,width,75))
        # Use the retained neighbours, not a centreline-only clearance claim.
        kit = {row[0]:row for row in layout['kit']}
        blocks = {row[0]:row for row in layout['blocks']}
        dressing = {row[0]:row for row in data['placements']}
        self.assertEqual(6550-(kit['RuinMarker'][2][1]+70),80)
        self.assertEqual((dressing['FoundationWest'][2][1]-130)-6850,140)
        pillar = blocks['RuinPillarWest']
        self.assertEqual((pillar[1][0]-pillar[2][0]/2)-(-880),290)
        self.assertGreater(-1380,-1500)
        self.assertLess(6850,7400)

    def test_intruding_alternate_is_rejected(self):
        self.assertTrue(overlaps_corridor((-250,6700),(250,150),(0,6400),(1500,6400),400,75))


if __name__ == '__main__':
    unittest.main()
