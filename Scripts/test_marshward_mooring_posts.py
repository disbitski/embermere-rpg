"""Host checks for the original visual-only post pair and its bounds."""

import hashlib
import json
import math
from pathlib import Path
import unittest

from marshward_route_clearance import overlaps_corridor

SOURCE = Path(__file__).resolve().parents[1]/'ArtSource/Blender/Environment/MarshwardMarsh'


class MooringPostsTests(unittest.TestCase):
    def test_source_has_two_closed_grounded_posts_without_hulls(self):
        spec = json.loads((SOURCE/'mooring_posts_metrics.json').read_text())['SM_MarshwardMooringPosts_01']
        self.assertEqual(spec['dimensions_cm'], [280, 190, 235])
        self.assertEqual(spec['pivot_cm'], [0, 0, 0])
        self.assertEqual([spec[key] for key in ('triangles','uv_channels','non_manifold_edges','closed_pieces','collision_hulls')],
                         [120, 1, 0, 2, 0])
        self.assertEqual(spec['materials'], ['M_EmbermereTimber', 'M_MarshwardMoss'])
        left, right = spec['base_samples_cm']
        self.assertLess(left[0], 0)
        self.assertGreater(right[0], 0)
        for x, y, z in (left, right):
            self.assertTrue(all(math.isfinite(value) for value in (x, y, z)))
            self.assertLess(abs(x), 140)
            self.assertLess(abs(y), 95)
            self.assertEqual(z, 0)

    def test_only_one_new_role_keeps_all_25_prior_placements(self):
        rows = json.loads((SOURCE/'placements.json').read_text())['placements']
        self.assertEqual(len(rows), 30)
        self.assertEqual([row for row in rows if row[1]=='MooringPosts'],
                         [['MooringPostsInnerSouth','MooringPosts',[900,1750,-120],0,[1,1,1]]])
        before = json.dumps(rows[:25], separators=(',',':')).encode()
        self.assertEqual(hashlib.sha256(before).hexdigest(),
                         '79cdf3ab199fbc90278b9760eb7e05d84f39e06d0180b91aedf3eafaed92a850')

    def test_placement_stays_between_the_protected_walking_surfaces(self):
        rows = json.loads((SOURCE/'placements.json').read_text())['placements']
        posts = [row for row in rows if row[1]=='MooringPosts']
        self.assertEqual(len(posts), 1)
        x, y, _ = posts[0][2]
        spec = json.loads((SOURCE/'mooring_posts_metrics.json').read_text())['SM_MarshwardMooringPosts_01']
        half_width, half_depth = (size/2 for size in spec['dimensions_cm'][:2])
        self.assertGreaterEqual(x-half_width-325, 435)
        self.assertGreaterEqual(2025-(x+half_width), 985)
        self.assertGreater(y-half_depth, 1100)
        self.assertLess(y+half_depth, 1975)

    def test_visible_bounds_at_the_corridor_buffer_fail_even_without_collision(self):
        self.assertTrue(overlaps_corridor((465,1750),(140,95),(0,1500),(0,5400),250,75))
        self.assertFalse(overlaps_corridor((466,1750),(140,95),(0,1500),(0,5400),250,75))


if __name__ == '__main__':
    unittest.main()
