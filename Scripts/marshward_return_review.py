"""Read-only sample plan for the existing return platform and adjacent basin."""

import math

STATIONS = (1200,2450,3100,4500,5700)
LANES = (2150,2700,3250)
BASIN_SIDES = (2050,3350)


def support_samples():
    return [(x,y,'Marshward_EastReturn',0) for y in STATIONS for x in LANES]+[
        (x,y,'Marshward_BasinCatch',-120) for y in STATIONS for x in BASIN_SIDES]


def clearance_segments():
    return [((x,a,height),(x,b,height)) for x in LANES
            for a,b in zip(STATIONS,STATIONS[1:]) for height in (95,155)]


def require_support(actual_label,actual_height,expected_label,expected_height):
    assert actual_label==expected_label, (actual_label,expected_label)
    assert math.isfinite(actual_height) and abs(actual_height-expected_height)<.5, (actual_height,expected_height)
