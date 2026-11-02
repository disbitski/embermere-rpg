"""Fast regressions for route visibility protection, independent of Unreal."""

import unittest
import json
import math
from pathlib import Path
from marshward_route_clearance import overlaps_corridor


class CorridorTests(unittest.TestCase):
    def test_planned_asset_bounds_keep_every_corridor_clear(self):
        root = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'
        data = json.loads((root/'MarshwardMarsh/placements.json').read_text())
        metrics = json.loads((root/'MarshwardMarsh/mesh_metrics.json').read_text())
        route = json.loads((root/'MarshwardGreybox/layout.json').read_text())['route']
        for label,kind,pos,yaw,scale in data['placements']:
            size = metrics['SM_Marshward'+kind+'_01']['dimensions_cm']
            x,y = size[0]*scale[0]/2,size[1]*scale[1]/2
            sine,cosine = abs(math.sin(math.radians(yaw))),abs(math.cos(math.radians(yaw)))
            extent = (x*cosine+y*sine,x*sine+y*cosine)
            for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
                self.assertFalse(overlaps_corridor(pos,extent,start,end,width,data['buffer_cm']),label)

    def test_visible_geometry_blocks_even_without_collision(self):
        self.assertTrue(overlaps_corridor((300,1000),(100,80),(0,0),(0,2000),250,75))

    def test_margin_touch_is_rejected(self):
        self.assertTrue(overlaps_corridor((425,1000),(100,80),(0,0),(0,2000),250,75))
        self.assertFalse(overlaps_corridor((426,1000),(100,80),(0,0),(0,2000),250,75))

    def test_reversed_segment(self):
        self.assertTrue(overlaps_corridor((0,1000),(30,30),(0,2000),(0,0),250,75))

    def test_return_path_uses_full_width(self):
        self.assertTrue(overlaps_corridor((3200,1000),(100,80),(2700,6400),(2700,0),600,75))
        self.assertFalse(overlaps_corridor((3500,1000),(100,80),(2700,6400),(2700,0),600,75))

    def test_horizontal_and_endpoint_buffer(self):
        self.assertTrue(overlaps_corridor((2050,0),(40,40),(0,0),(2000,0),250,75))
        self.assertFalse(overlaps_corridor((2200,0),(40,40),(0,0),(2000,0),250,75))

    def test_invalid_routes_fail_closed(self):
        for end in [(0,0),(100,100)]:
            with self.assertRaises(ValueError):
                overlaps_corridor((0,0),(40,40),(0,0),end,250,75)


if __name__ == '__main__':
    unittest.main()
