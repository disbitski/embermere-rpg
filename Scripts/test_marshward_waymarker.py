"""Host source geometry, unchanged roles and rejection controls for marker parity."""

import json
from pathlib import Path
import unittest
from marshward_waymarker_contract import require_geometry

ROOT = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment'


class WaymarkerTests(unittest.TestCase):
    def test_source_matches_original_geometry_contract(self):
        spec = json.loads((ROOT/'MarshwardWaymarker/waymarker_metrics.json').read_text())['SM_MarshwardWaymarker_01']
        box = spec['collision_boxes'][0]
        require_geometry(spec['bounds_min_cm'],spec['bounds_max_cm'],box['dimensions_cm'],box['center_cm'])
        self.assertEqual(len(spec['collision_boxes']),1)
        self.assertEqual(box['rotation_deg'],[0,0,0])
        self.assertEqual(spec['pivot_cm'],[0,0,0])
        self.assertEqual((spec['triangles'],spec['uv_channels'],spec['non_manifold_edges']),(352,1,0))
        self.assertEqual(spec['materials'],['M_MarshwardGreyboxMarker'])
        original = json.loads((ROOT/'MarshwardGreybox/kit_metrics.json').read_text())['SM_MarshwardWaymarker_Greybox']
        self.assertEqual(spec['dimensions_cm'],original['dimensions_cm'])
        self.assertEqual(original['triangles'],36)

    def test_only_three_unchanged_marker_roles(self):
        layout = json.loads((ROOT/'MarshwardGreybox/layout.json').read_text())
        self.assertEqual(len(layout['blocks'])+len(layout['kit']),22)
        self.assertEqual([r for r in layout['kit'] if r[1]=='Waymarker'],[
            ['ArrivalMarker','Waymarker',[-750,650,0],0],
            ['RuinMarker','Waymarker',[-950,6400,120],0],
            ['ReturnMarker','Waymarker',[3000,5300,0],90]])
        dressing = json.loads((ROOT/'MarshwardMarsh/placements.json').read_text())
        self.assertEqual(len(dressing['placements']),23)
        self.assertEqual(dressing['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(dressing['buffer_cm'],75)

    def test_expanded_or_shifted_visible_bounds_reject(self):
        for lo,hi in [((-91,-70,0),(90,70,520)),((-90,-70,0),(90,70,521)),
                      ((-89,-70,0),(91,70,520))]:
            with self.assertRaises(AssertionError):
                require_geometry(lo,hi,(120,100,440),(0,0,220))

    def test_expanded_or_shifted_collision_rejects(self):
        for size,center in [((121,100,440),(0,0,220)),((120,100,440),(0,0,221)),
                            ((120,100,440),(1,0,220))]:
            with self.assertRaises(AssertionError):
                require_geometry((-90,-70,0),(90,70,520),size,center)


if __name__ == '__main__':
    unittest.main()
