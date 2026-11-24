"""Host checks for the detached ruin wall's bounds and collision tiers."""

import hashlib
import json
from pathlib import Path
import unittest

from marshward_route_clearance import overlaps_corridor

SOURCE = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh'


class RuinWallTests(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((SOURCE/'placements.json').read_text())
        self.wall, = [row for row in self.data['placements'] if row[1]=='RuinWall']
        self.spec = json.loads((SOURCE/'ruin_wall_metrics.json').read_text())['SM_MarshwardRuinWall_01']

    def test_source_has_staggered_cores_and_reuses_only_existing_materials(self):
        self.assertEqual(self.spec['dimensions_cm'],[900,220,420])
        self.assertEqual(self.spec['pivot_cm'],[0,0,0])
        self.assertEqual([self.spec[key] for key in ('triangles','uv_channels','non_manifold_edges','stone_count','collision_hulls')],
                         [540,1,0,9,3])
        self.assertEqual(self.spec['materials'],['M_MarshwardMarshStone','M_MarshwardMoss'])
        self.assertEqual(self.spec['cores_cm'],[[-425,423,82,0,122],[-317,312,70,122,244],[-152,178,57,244,333]])
        self.assertEqual(self.spec['crest_samples_cm'],[[-390,0,122],[-230,0,244],[0,0,333]])
        self.assertEqual(self.spec['ground_samples_cm'],[[x,y,0] for x in (-390,0,390) for y in (-160,160)])
        self.assertEqual(self.spec['air_samples_cm'],[[-390,210],[390,210]])

    def test_one_new_role_preserves_every_previous_dressing_row(self):
        rows = self.data['placements']
        self.assertEqual(len(rows),31)
        self.assertEqual(rows[28],['RuinWallNortheast','RuinWall',[1900,7600,-120],0,[1,1,1]])
        self.assertEqual(self.wall,rows[28])
        before = json.dumps(rows[:28],separators=(',',':')).encode()
        self.assertEqual(hashlib.sha256(before).hexdigest(),
                         '0a0e4c85935fda20903c15bed4dded454ad641e4487e381f4852ad314c6cdcc9')

    def test_footprint_and_view_cone_stay_beyond_island_and_east_pillar(self):
        x,y,_ = self.wall[2]
        half_x,half_y = [value/2 for value in self.spec['dimensions_cm'][:2]]
        self.assertGreater(x-half_x,1260)
        self.assertGreater(y-half_y,7400)
        for view_x,view_y in ((-500,-450),(0,-450),(500,-450),(0,4700)):
            wall_inner_edge = (x-half_x-view_x)/(y+half_y-view_y)
            pillar_outer_edge = (590-view_x)/(6710-view_y)
            self.assertGreater(wall_inner_edge,pillar_outer_edge)

    def test_moving_the_same_bounds_into_the_east_ramp_buffer_is_rejected(self):
        extent = tuple(value/2 for value in self.spec['dimensions_cm'][:2])
        ramp = ((1500,6400),(2100,6400),250,75)
        self.assertTrue(overlaps_corridor((1900,6835),extent,*ramp))
        self.assertFalse(overlaps_corridor((1900,6836),extent,*ramp))


if __name__ == '__main__':
    unittest.main()
