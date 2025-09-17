# shapes/targets.py
# Define target convex polygons (must contain origin in interior).
import numpy as np

def triangle_equilateral(scale=1.0):
    # Equilateral triangle centered at origin
    # side length scaled so vertices are at given coords
    verts = np.array([
        (0.0, 1.0),
        (-0.8660254037844386, -0.5),
        (0.8660254037844386, -0.5)
    ], dtype=float) * scale
    return verts

def trapezoid_sym(scale_x=1.0, scale_y=1.0):
    # Symmetric trapezoid about y-axis. Top wider than bottom.
    verts = np.array([
        (-1.0 * scale_x, 1.0 * scale_y),
        ( 1.0 * scale_x, 1.0 * scale_y),
        ( 0.6 * scale_x, -1.0 * scale_y),
        (-0.6 * scale_x, -1.0 * scale_y)
    ], dtype=float)
    return verts
