"""Shared numeric geometry contract for source and saved waymarker parity."""

import math


def require_geometry(bounds_min,bounds_max,box_size,box_center):
    for actual,expected in [(bounds_min,(-90,-70,0)),(bounds_max,(90,70,520)),
                            (box_size,(120,100,440)),(box_center,(0,0,220))]:
        assert len(actual)==3
        assert all(math.isfinite(a) and abs(a-b)<.1 for a,b in zip(actual,expected)), (actual,expected)
