# run_triangle_trapezoid_demo.py
import numpy as np
import matplotlib.pyplot as plt
from shapes.targets import triangle_equilateral, trapezoid_sym
from grid.highways import build_highways_weights, dijkstra_grid
from geom.minkowski import gauge_unit_vector
import math

def compute_euclidean_ray_angles(num=180):
    return np.linspace(0, 2*math.pi, num, endpoint=False)

def compute_target_radii(angles, verts):
    # For each angle, compute r such that (r*cos, r*sin) lies on boundary of unit shape.
    # That is 1 / gauge(unit_vector(theta))
    radii = []
    for theta in angles:
        g = gauge_unit_vector(theta, verts)
        radii.append(1.0 / g)
    return np.array(radii)

def build_and_visualize(verts, grid_radius=80, num_dirs=64, sample_radius=40):
    print("Building highways (grid weights)...")
    weight_dict = build_highways_weights(verts, grid_radius=grid_radius, num_directions=num_dirs)
    print("Running Dijkstra...")
    dist = dijkstra_grid(weight_dict, radius=sample_radius)

    # Convert distances to radial distances: for each angle, find farthest point roughly along that angle and record dist/r
    angles = compute_euclidean_ray_angles(360)
    empirical = []
    for theta in angles:
        # sample points along ray
        best = None
        best_ratio = 0.0
        for r in range(5, sample_radius, 1):
            x = int(round(r * math.cos(theta)))
            y = int(round(r * math.sin(theta)))
            d = dist.get((x,y), None)
            if d is None: 
                continue
            ratio = d / r
            # We expect ratio ≈ gauge(unit_vector)
            if r > 0:
                best = ratio
                best_ratio = ratio
        empirical.append(best_ratio if best is not None else np.nan)

    angles_deg = np.degrees(angles)
    target_radii = compute_target_radii(angles, verts)

    # Plot polar comparison: target radial function vs empirical radial (converted)
    fig, ax = plt.subplots(subplot_kw={'projection':'polar'}, figsize=(8,6))
    ax.plot(angles, 1.0/target_radii, label='Target radial (unit ball boundary)')
    # convert empirical ratio (d/r) to radial length ~ 1/(d/r)
    empirical_radial = []
    for v in empirical:
        if np.isnan(v) or v <= 0:
            empirical_radial.append(np.nan)
        else:
            empirical_radial.append(1.0 / v)
    ax.plot(angles, empirical_radial, label='Empirical radial from weighted grid')
    ax.set_title("Target vs empirical radial distance (triangle/trapezoid demo)")
    ax.legend()
    plt.show()

if __name__ == "__main__":
    print("Demo for EQUILATERAL TRIANGLE:")
    verts_tri = triangle_equilateral(scale=1.0)
    build_and_visualize(verts_tri, grid_radius=80, num_dirs=72, sample_radius=40)

    print("Demo for SYMMETRIC TRAPEZOID:")
    verts_trap = trapezoid_sym(scale_x=1.0, scale_y=1.0)
    build_and_visualize(verts_trap, grid_radius=80, num_dirs=72, sample_radius=40)
