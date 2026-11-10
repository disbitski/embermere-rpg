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
        metrics.update(json.loads((root/'MarshwardMarsh/shelf_metrics.json').read_text()))
        metrics.update(json.loads((root/'MarshwardMarsh/rock_metrics.json').read_text()))
        metrics.update(json.loads((root/'MarshwardMarsh/bank_edge_metrics.json').read_text()))
        metrics.update(json.loads((root/'MarshwardMarsh/foundation_metrics.json').read_text()))
        metrics.update(json.loads((root/'MarshwardMarsh/fallen_masonry_metrics.json').read_text()))
        route = json.loads((root/'MarshwardGreybox/layout.json').read_text())['route']
        for label,kind,pos,yaw,scale in data['placements']:
            size = metrics['SM_Marshward'+kind+'_01']['dimensions_cm']
            x,y = size[0]*scale[0]/2,size[1]*scale[1]/2
            sine,cosine = abs(math.sin(math.radians(yaw))),abs(math.cos(math.radians(yaw)))
            extent = (x*cosine+y*sine,x*sine+y*cosine)
            for start,end,width in zip(route,route[1:],data['corridor_half_widths_cm']):
                self.assertFalse(overlaps_corridor(pos,extent,start,end,width,data['buffer_cm']),label)

    def test_corridor_contract_cannot_shrink(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json'
        data = json.loads(path.read_text())
        self.assertEqual(data['corridor_half_widths_cm'],[600,250,250,400,400,250,300,600,600])
        self.assertEqual(data['buffer_cm'],75)

    def test_only_two_outer_roles_use_the_shelf(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json'
        rows = json.loads(path.read_text())['placements']
        self.assertEqual([r[0] for r in rows if r[1]=='PeatShelf'],['BermWestFar','BermEast'])
        self.assertEqual(sum(r[1]=='PeatBerm' for r in rows),4)
        self.assertEqual(sum(r[1]=='ReedClump' for r in rows),8)

    def test_shelf_matches_the_written_brief(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/shelf_metrics.json'
        data = json.loads(path.read_text())['SM_MarshwardPeatShelf_01']
        self.assertEqual(data['dimensions_cm'],[2600,900,240])
        self.assertEqual((data['triangles'],data['uv_channels'],data['non_manifold_edges']),(66,1,0))
        self.assertEqual(data['materials'],['M_MarshwardPeat','M_MarshwardMoss','M_MarshwardEarth'])

    def test_visible_geometry_blocks_even_without_collision(self):
        self.assertTrue(overlaps_corridor((300,1000),(100,80),(0,0),(0,2000),250,75))

    def test_rock_cluster_matches_brief(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/rock_metrics.json'
        data = json.loads(path.read_text())['SM_MarshwardRockCluster_01']
        self.assertEqual(data['dimensions_cm'],[460,340,260])
        self.assertEqual((data['triangles'],data['uv_channels'],data['non_manifold_edges'],data['collision_hulls']),(144,1,0,3))
        self.assertEqual(data['materials'],['M_MarshwardMarshStone','M_MarshwardMoss'])

    def test_rock_roles_are_sparse_and_grounded(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json'
        rows = json.loads(path.read_text())['placements']
        rocks = [row for row in rows if row[1]=='RockCluster']
        self.assertEqual(len(rows),22)
        self.assertEqual([r[0] for r in rocks],['RocksWestNear','RocksInnerSouth','RocksNorthwest','RocksOuterEast'])
        for row in rocks:
            self.assertEqual(row[2][2],-120)
            self.assertEqual(row[4],[1,1,1])

    def test_each_stone_has_flat_grounded_center(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/rock_metrics.json'
        centers = json.loads(path.read_text())['SM_MarshwardRockCluster_01']['stone_centers_cm']
        self.assertEqual(len(centers),3)
        for x,y,z in centers:
            self.assertEqual(z,0)
            self.assertLess(abs(x),230)
            self.assertLess(abs(y),170)

    def test_bank_edge_matches_brief_without_collision(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/bank_edge_metrics.json'
        data = json.loads(path.read_text())['SM_MarshwardBankEdge_01']
        self.assertEqual(data['dimensions_cm'],[1000,360,70])
        self.assertEqual((data['triangles'],data['uv_channels'],data['non_manifold_edges'],data['collision_hulls']),(46,1,0,0))
        self.assertEqual(data['materials'],['M_MarshwardPeat','M_MarshwardMoss','M_MarshwardEarth'])

    def test_single_east_apron_has_fixed_off_route_role(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/placements.json'
        rows = json.loads(path.read_text())['placements']
        self.assertEqual([row for row in rows if row[1]=='BankEdge'],
                         [['BankEdgeEast','BankEdge',[3910,3100,-120],90,[1,1,1]]])

    def test_apron_probes_have_basin_and_shelf_controls(self):
        path = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh/bank_edge_metrics.json'
        data = json.loads(path.read_text())['SM_MarshwardBankEdge_01']
        self.assertEqual(data['ground_samples_cm'],[[x,y,0] for x in (-250,0,250) for y in (-100,-40,0)])
        self.assertEqual(data['join_samples_cm'],[[0,140,0],[250,140,0]])

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
