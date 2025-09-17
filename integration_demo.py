# integration_demo.py
# Shows how the polygon target system integrates with FPP experiments
import numpy as np
import matplotlib.pyplot as plt
from shapes.targets import triangle_equilateral, trapezoid_sym
from grid.highways import build_highways_weights, dijkstra_grid
from geom.minkowski import gauge_unit_vector
import math

def generate_fpp_weights_for_polygon(verts, grid_radius=100, num_directions=120):
    """
    Generate edge weights that approximate the polygon target shape.
    This can be used to initialize your C++ FPP experiments.
    """
    print(f"Generating FPP weights for polygon with {len(verts)} vertices...")
    
    # Build the highway system
    weight_dict = build_highways_weights(verts, grid_radius=grid_radius, num_directions=num_directions)
    
    # Convert to a format suitable for C++ integration
    # Create a 2D grid of weights
    size = 2 * grid_radius + 1
    weights_horizontal = np.ones((size, size-1))  # rightward edges
    weights_vertical = np.ones((size-1, size))    # upward edges
    
    # Fill in the weights from our dictionary
    for ((x1, y1), (x2, y2)), weight in weight_dict.items():
        # Convert to array indices (shift by grid_radius)
        i1, j1 = y1 + grid_radius, x1 + grid_radius
        i2, j2 = y2 + grid_radius, x2 + grid_radius
        
        # Check bounds
        if 0 <= i1 < size and 0 <= j1 < size and 0 <= i2 < size and 0 <= j2 < size:
            if j2 == j1 + 1 and i2 == i1:  # horizontal edge
                weights_horizontal[i1, j1] = weight
            elif i2 == i1 + 1 and j2 == j1:  # vertical edge
                weights_vertical[i1, j1] = weight
    
    return weights_horizontal, weights_vertical, weight_dict

def analyze_polygon_approximation(verts, shape_name):
    """Analyze how well our weighted grid approximates the target polygon"""
    print(f"\nAnalyzing {shape_name}...")
    
    # Generate weights
    w_h, w_v, weight_dict = generate_fpp_weights_for_polygon(verts, grid_radius=80, num_directions=100)
    
    # Run Dijkstra
    dist = dijkstra_grid(weight_dict, radius=50)
    
    # Sample points on the boundary
    angles = np.linspace(0, 2*math.pi, 360, endpoint=False)
    target_points = []
    empirical_points = []
    
    for theta in angles:
        # Target point on polygon boundary
        g = gauge_unit_vector(theta, verts)
        target_r = 1.0 / g
        target_x = target_r * math.cos(theta)
        target_y = target_r * math.sin(theta)
        target_points.append((target_x, target_y))
        
        # Find empirical point
        best_ratio = None
        for r in range(10, 50, 2):
            x = int(round(r * math.cos(theta)))
            y = int(round(r * math.sin(theta)))
            d = dist.get((x,y), None)
            if d is not None and r > 0:
                ratio = d / r
                if best_ratio is None or ratio < best_ratio:
                    best_ratio = ratio
        
        if best_ratio is not None and best_ratio > 0:
            empirical_r = 1.0 / best_ratio
            empirical_x = empirical_r * math.cos(theta)
            empirical_y = empirical_r * math.sin(theta)
            empirical_points.append((empirical_x, empirical_y))
        else:
            empirical_points.append((0, 0))
    
    # Create visualization
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Plot 1: Shape comparison
    target_x, target_y = zip(*target_points)
    empirical_x, empirical_y = zip(*empirical_points)
    
    ax1.plot(target_x, target_y, 'b-', linewidth=3, label='Target polygon')
    ax1.plot(empirical_x, empirical_y, 'r--', linewidth=2, label='Weighted grid approximation')
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_title(f'{shape_name} - Shape Comparison')
    ax1.legend()
    ax1.grid(True)
    ax1.axis('equal')
    
    # Plot 2: Error analysis
    errors = []
    for i, (tx, ty) in enumerate(target_points):
        ex, ey = empirical_points[i]
        if ex != 0 or ey != 0:  # valid empirical point
            error = math.sqrt((tx - ex)**2 + (ty - ey)**2)
            errors.append(error)
    
    if errors:
        ax2.hist(errors, bins=30, alpha=0.7, edgecolor='black')
        ax2.set_xlabel('Point-wise Error')
        ax2.set_ylabel('Frequency')
        ax2.set_title(f'{shape_name} - Error Distribution')
        ax2.grid(True)
        print(f"Mean error: {np.mean(errors):.4f}")
        print(f"Max error: {np.max(errors):.4f}")
        print(f"Std error: {np.std(errors):.4f}")
    
    plt.tight_layout()
    plt.show()
    
    return w_h, w_v

def export_weights_for_cpp(weights_h, weights_v, filename_prefix):
    """Export weights in a format that can be easily read by C++"""
    print(f"Exporting weights to {filename_prefix}_weights.txt...")
    
    with open(f"{filename_prefix}_weights.txt", 'w') as f:
        f.write("# Horizontal edge weights (rightward)\n")
        f.write(f"# Shape: {weights_h.shape}\n")
        for i in range(weights_h.shape[0]):
            for j in range(weights_h.shape[1]):
                f.write(f"{weights_h[i,j]:.6f} ")
            f.write("\n")
        
        f.write("\n# Vertical edge weights (upward)\n")
        f.write(f"# Shape: {weights_v.shape}\n")
        for i in range(weights_v.shape[0]):
            for j in range(weights_v.shape[1]):
                f.write(f"{weights_v[i,j]:.6f} ")
            f.write("\n")

if __name__ == "__main__":
    print("Polygon Target Shape Integration Demo")
    print("=" * 50)
    
    # Test triangle
    print("\n1. Equilateral Triangle Analysis")
    verts_tri = triangle_equilateral(scale=1.0)
    w_h_tri, w_v_tri = analyze_polygon_approximation(verts_tri, "Equilateral Triangle")
    export_weights_for_cpp(w_h_tri, w_v_tri, "triangle")
    
    # Test trapezoid
    print("\n2. Symmetric Trapezoid Analysis")
    verts_trap = trapezoid_sym(scale_x=1.0, scale_y=1.0)
    w_h_trap, w_v_trap = analyze_polygon_approximation(verts_trap, "Symmetric Trapezoid")
    export_weights_for_cpp(w_h_trap, w_v_trap, "trapezoid")
    
    print("\n" + "=" * 50)
    print("Integration complete!")
    print("Weight files exported for C++ integration:")
    print("- triangle_weights.txt")
    print("- trapezoid_weights.txt")
    print("\nThese can be used to initialize your C++ FPP experiments")
    print("with deterministic weights that target the polygon shapes.")
