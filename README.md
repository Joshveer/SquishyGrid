# SquishyGrid

First Passage Percolation (FPP) simulator with discrete distributions and convex polygon target shapes.

## Structure

### C++ Core
- `ball.cpp` - Main FPP simulator with discrete distributions
- `grad_all_angle.cpp` - Gradient-based optimization
- `diff_n.cpp` - Grid size analysis

### Experiments
- `2-point/`, `3-point/`, `4-point/` - Multi-point correlation studies
- `monotone/` - Monotonicity constraints
- `U01/`, `Gamma22/`, `Gamma1010/` - Distribution comparisons
- `test_width/` - Width parameter testing

### Python Polygon System
- `shapes/targets.py` - Polygon definitions
- `geom/minkowski.py` - Minkowski gauge computation
- `grid/highways.py` - Highway construction and Dijkstra
- `integration_demo.py` - C++ weight export

## Usage

### C++ FPP
```bash
g++ -O3 -std=c++17 ball.cpp -o ball
./ball
```

### Python Polygons
```bash
pip install numpy scipy networkx matplotlib
python integration_demo.py
```

## Algorithms

### FPP Implementation
- 2D lattice with 4-neighbor connectivity
- Discrete probability distributions for edge weights
- Dijkstra's algorithm for shortest paths
- Modular array indexing for memory efficiency

### Distribution Classes
```cpp
class Rand {
    virtual double ra() = 0;
};

class Plan : public Rand {
    vector<double> p, v;  // probabilities and values
};
```

### Polygon Targeting
- Minkowski gauge: ρ_P(v) = min{λ ≥ 0 : v ∈ λP}
- Linear programming via scipy.optimize.linprog
- Highway construction with gauge-based weights
- Weight matrix export for C++ integration

## Output Files
- `output_ball.txt` - Distance field coordinates
- `output_angle_len.txt` - Angular measurements
- `record.txt` - Optimization logs
- `*_weights.txt` - Exported weight matrices

## Custom Polygons
```python
from shapes.targets import triangle_equilateral
from grid.highways import build_highways_weights

# Define vertices (must contain origin)
verts = np.array([(x1,y1), (x2,y2), ...])

# Generate weights
weight_dict = build_highways_weights(verts, grid_radius=80, num_directions=100)
```