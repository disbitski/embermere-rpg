"""Conservative visible-bounds protection for axis-aligned walking corridors."""


def overlaps_corridor(center, extent, start, end, half_width, margin):
    if start[0] == end[0] and start[1] != end[1]:
        lo = (start[0]-half_width-margin, min(start[1],end[1])-margin)
        hi = (start[0]+half_width+margin, max(start[1],end[1])+margin)
    elif start[1] == end[1] and start[0] != end[0]:
        lo = (min(start[0],end[0])-margin, start[1]-half_width-margin)
        hi = (max(start[0],end[0])+margin, start[1]+half_width+margin)
    else:
        raise ValueError('Expected a nonzero axis-aligned route segment')
    return all(center[i]+extent[i] >= lo[i] and center[i]-extent[i] <= hi[i] for i in range(2))
