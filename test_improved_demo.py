# test_improved_demo.py
# Test the improved highway system with better visualization
import numpy as np
import matplotlib.pyplot as plt
from shapes.targets import triangle_equilateral, trapezoid_sym
from grid.highways import build_highways_weights, dijkstra_grid
from geom.minkowski import gauge_unit_vector
import math

def test_single_shape(verts, shape_name, grid_radius=60, num_dirs=72, sample_radius=30):
    """Test a single shape and create detailed visualization"""
    print(f"\nTesting {shape_name}...")
    print("Building highways (grid weights)...")
    weight_dict = build_highways_weights(verts, grid_radius=grid_radius, num_directions=num_dirs)
    print(f"Generated {len(weight_dict)} edge weights")
    
    print("Running Dijkstra...")
    dist = dijkstra_grid(weight_dict, radius=sample_radius)
    print(f"Computed distances to {len(dist)} points")

    # Compute target and empirical radial functions
    angles = np.linspace(0, 2*math.pi, 180, endpoint=False)
    target_radii = []
    empirical_radii = []
    
    for theta in angles:
        # Target: 1/gauge(unit_vector)
        g = gauge_unit_vector(theta, verts)
        target_radii.append(1.0 / g)
        
        # Empirical: find best point along this direction
        best_ratio = None
        for r in range(5, sample_radius, 2):
            x = int(round(r * math.cos(theta)))
            y = int(round(r * math.sin(theta)))
            d = dist.get((x,y), None)
            if d is not None and r > 0:
                ratio = d / r
                if best_ratio is None or ratio < best_ratio:
                    best_ratio = ratio
        
        if best_ratio is not None and best_ratio > 0:
            empirical_radii.append(1.0 / best_ratio)
        else:
            empirical_radii.append(np.nan)
    
    # Create comparison plot
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Polar plot
    ax1 = plt.subplot(121, projection='polar')
    ax1.plot(angles, target_radii, 'b-', linewidth=2, label='Target shape')
    ax1.plot(angles, empirical_radii, 'r--', linewidth=2, label='Empirical (weighted grid)')
    ax1.set_title(f'{shape_name} - Polar View')
    ax1.legend()
    ax1.grid(True)
    
    # Cartesian plot for better comparison
    ax2 = plt.subplot(122)
    # Convert to Cartesian coordinates
    target_x = [r * math.cos(theta) for r, theta in zip(target_radii, angles)]
    target_y = [r * math.sin(theta) for r, theta in zip(target_radii, angles)]
    empirical_x = [r * math.cos(theta) if not np.isnan(r) else 0 for r, theta in zip(empirical_radii, angles)]
    empirical_y = [r * math.sin(theta) if not np.isnan(r) else 0 for r, theta in zip(empirical_radii, angles)]
    
    ax2.plot(target_x, target_y, 'b-', linewidth=2, label='Target shape')
    ax2.plot(empirical_x, empirical_y, 'r--', linewidth=2, label='Empirical (weighted grid)')
    ax2.set_xlabel('X')
    ax2.set_ylabel('Y')
    ax2.set_title(f'{shape_name} - Cartesian View')
    ax2.legend()
    ax2.grid(True)
    ax2.axis('equal')
    
    plt.tight_layout()
    plt.show()
    
    # Print some statistics
    valid_empirical = [r for r in empirical_radii if not np.isnan(r)]
    if valid_empirical:
        error = np.mean([abs(t - e) for t, e in zip(target_radii, empirical_radii) if not np.isnan(e)])
        print(f"Mean absolute error: {error:.4f}")
        print(f"Max target radius: {max(target_radii):.4f}")
        print(f"Max empirical radius: {max(valid_empirical):.4f}")

if __name__ == "__main__":
    print("Testing improved polygon target shape system...")
    
    # Test triangle
    verts_tri = triangle_equilateral(scale=1.0)
    test_single_shape(verts_tri, "Equilateral Triangle")
    
    # Test trapezoid
    verts_trap = trapezoid_sym(scale_x=1.0, scale_y=1.0)
    test_single_shape(verts_trap, "Symmetric Trapezoid")
    
    print("\nAll tests completed!")
