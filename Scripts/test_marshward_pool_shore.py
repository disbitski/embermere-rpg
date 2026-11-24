"""Protect the prior pool and tightly bound the new visual contact."""

import hashlib
import json
import math
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh'


class PoolShoreTests(unittest.TestCase):
    def setUp(self):
        self.rows = json.loads((SOURCE/'placements.json').read_text())['placements']
        self.shore = json.loads((SOURCE/'pool_shore_metrics.json').read_text())['SM_MarshwardPoolShore_01']
        self.pool = json.loads((SOURCE/'still_pool_metrics.json').read_text())['SM_MarshwardStillPool_01']

    def test_only_one_new_role_retains_prior_rows(self):
        self.assertEqual(self.rows[-1],['PoolShoreWest','PoolShore',[-1320,3340,-118],0,[1,1,1]])
        self.assertEqual([row for row in self.rows if row[1]=='PoolShore'],self.rows[-1:])
        digest = hashlib.sha256(json.dumps(self.rows[:-1],separators=(',',':')).encode()).hexdigest()
        self.assertEqual(digest,'593c87b02ec8c3ab1a3cfd854baaae869ee1cc16bf36feaebde9be3c267a26cf')

    def test_taper_tracks_rim_below_water_and_buries_dry_toe(self):
        self.assertEqual(self.shore['rim_stations'],[12,13,14,15,0,1,2,3])
        top = self.shore['top_rows_cm']
        self.assertEqual(len(top),24)
        for i,idx in enumerate(self.shore['rim_stations']):
            rim = self.pool['rim_xy_cm'][idx]
            inner,crest,toe = top[3*i:3*i+3]
            self.assertAlmostEqual(math.hypot(*inner[:2]),math.hypot(*rim)-9)
            self.assertAlmostEqual(math.hypot(*crest[:2]),math.hypot(*rim)+2)
            self.assertIn(crest[2],(-2.5,-.4))
            self.assertEqual(toe[2],-2.5)
            self.assertLess(max(inner[2],crest[2],toe[2]),0)
        self.assertEqual([self.shore[key] for key in ('triangles','uv_channels','non_manifold_edges','collision_hulls')],
                         [92,1,0,0])
        self.assertEqual(self.shore['materials'],['M_MarshwardShoreMud'])

    def test_footprint_overlaps_only_the_intended_pool(self):
        specs = {}
        for path in SOURCE.glob('*metrics.json'):
            specs.update(json.loads(path.read_text()))
        (lx,hx),(ly,hy),_ = self.shore['bounds_cm']
        px,py,_ = self.rows[-1][2]
        center = (px+(lx+hx)/2,py-(ly+hy)/2)
        extent = ((hx-lx)/2,(hy-ly)/2)
        touches = []
        for label,kind,(x,y,_),yaw,(sx,sy,_) in self.rows[:-1]:
            w,h,_ = specs['SM_Marshward'+kind+'_01']['dimensions_cm']
            c,s = (abs(f(math.radians(yaw))) for f in (math.cos,math.sin))
            if (abs(x-center[0])<=extent[0]+(w*sx*c+h*sy*s)/2 and
                    abs(y-center[1])<=extent[1]+(w*sx*s+h*sy*c)/2):
                touches.append(label)
        self.assertEqual(touches,['StillPoolWest'])


if __name__=='__main__':
    unittest.main()
