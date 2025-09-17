# SquishyGrid

First Passage Percolation (FPP) simulator with discrete probability distributions and optimization algorithms.

## Structure

### C++ Core
- `ball.cpp` - Main FPP simulator with discrete distributions
- `ball_polygon.cpp` - FPP simulator with polygon target shapes
- `grad_all_angle.cpp` - Gradient-based distribution optimization
- `diff_n.cpp` - Grid size analysis
- `ball_spfa.cpp` - SPFA algorithm variant

### Experiments
- `2-point/`, `3-point/`, `4-point/` - Multi-point correlation studies
- `monotone/` - Monotonicity constraint experiments
- `U01/`, `Gamma22/`, `Gamma1010/` - Distribution comparisons
- `test_width/` - Width parameter testing

## Usage

### C++ FPP
```bash
# Standard FPP with discrete distributions
g++ -O3 -std=c++17 ball.cpp -o ball
./ball

# FPP with polygon target shapes
g++ -O3 -std=c++17 ball_polygon.cpp -o ball_polygon
./ball_polygon triangle    # or trapezoid, square

# Distribution optimization
g++ -O3 -std=c++17 grad_all_angle.cpp -o grad_all_angle
./grad_all_angle

# Grid size analysis
g++ -O3 -std=c++17 diff_n.cpp -o diff_n
./diff_n
```

## Algorithms

### FPP Implementation
- 2D lattice with 4-neighbor connectivity
- Discrete probability distributions for edge weights
- Dijkstra's algorithm for shortest paths
- Modular array indexing for memory efficiency (30K×1K grid)

### Distribution Classes
```cpp
class Rand {
    virtual double ra() = 0;
};

class Plan : public Rand {
    vector<double> p, v;  // probabilities and values
    std::discrete_distribution<> distribution;
};
```