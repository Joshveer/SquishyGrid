# grid/highways.py
import numpy as np
import heapq
from geom.minkowski import minkowski_gauge_vector

def integer_line_points_on_direction(theta, grid_size):
    """
    Return list of integer lattice points approximating a long line in direction theta
    crossing box [-grid_size, grid_size]^2. Simple Bresenham-like walker:
    start at center, step by rounding cumulative float position.
    """
    cx, cy = 0, 0
    pts = [(cx, cy)]
    direction = np.array([np.cos(theta), np.sin(theta)])
    pos = np.array([cx + 0.0, cy + 0.0])
    max_steps = 4 * grid_size  # generous
    for _ in range(max_steps):
        pos = pos + direction
        new = (int(round(pos[0])), int(round(pos[1])))
        if new == pts[-1]:
            continue
        # stop when leave bounding box
        if abs(new[0]) > grid_size or abs(new[1]) > grid_size:
            break
        pts.append(new)
    return pts

def build_highways_weights(verts, grid_radius=50, num_directions=36):
    """
    Build a grid-weight map (dictionary from oriented edges to weight)
    using highways in many directions.
    We will set weight for each edge on a highway to equal minkowski_gauge_vector(step).
    Also set baseline weights for axis-aligned edges to improve approximation.
    """
    grid_radius = int(grid_radius)
    weight = {}  # key ((x1,y1),(x2,y2)) with lexicographic ordering
    
    # Set baseline weights for axis-aligned edges
    w_x = minkowski_gauge_vector([1,0], verts)
    w_y = minkowski_gauge_vector([0,1], verts)
    
    # Pre-populate all possible axis-aligned edges in the grid
    for x in range(-grid_radius, grid_radius):
        for y in range(-grid_radius, grid_radius):
            # Horizontal edges (right)
            if x < grid_radius - 1:
                key = ((x, y), (x+1, y))
                key_rev = ((x+1, y), (x, y))
                weight[key] = w_x
                weight[key_rev] = w_x
            # Vertical edges (up)
            if y < grid_radius - 1:
                key = ((x, y), (x, y+1))
                key_rev = ((x, y+1), (x, y))
                weight[key] = w_y
                weight[key_rev] = w_y
    
    # Add highway weights (these will override baseline weights where highways exist)
    directions = np.linspace(0, 2*np.pi, num_directions, endpoint=False)
    for theta in directions:
        pts = integer_line_points_on_direction(theta, grid_radius)
        # assign edge weights along consecutive highway points
        for a, b in zip(pts[:-1], pts[1:]):
            step = np.array([b[0]-a[0], b[1]-a[1]])
            w = minkowski_gauge_vector(step, verts)
            key = (tuple(a), tuple(b))
            key_rev = (tuple(b), tuple(a))
            weight[key] = w
            weight[key_rev] = w
    return weight

def dijkstra_grid(weight_dict, radius):
    """
    Convert weight_dict edges into a finite grid graph around origin of radius `radius`,
    then run Dijkstra from origin to all nodes and return dist dict.
    4-neighbor adjacency. Uses weights from weight_dict (should have baseline weights set).
    """
    R = radius
    dist = {}
    pq = []
    start = (0,0)
    dist[start] = 0.0
    heapq.heappush(pq, (0.0, start))
    while pq:
        d,u = heapq.heappop(pq)
        if d > dist.get(u, 1e30): 
            continue
        x,y = u
        for dx,dy in [(1,0),(-1,0),(0,1),(0,-1)]:
            v = (x+dx, y+dy)
            if abs(v[0])>R or abs(v[1])>R: 
                continue
            key = (u, v)
            w = weight_dict.get(key, 1.0)  # fallback to 1.0 if not found
            nd = d + w
            if nd < dist.get(v, 1e30):
                dist[v] = nd
                heapq.heappush(pq, (nd, v))
    return dist
