# geom/minkowski.py
import numpy as np
from scipy.optimize import linprog

def minkowski_gauge_vector(v, verts):
    """
    Compute gauge rho_P(v) = min sum beta_i s.t. V beta = v, beta >= 0,
    where verts is an (m,2) array of polygon vertices (columns are used by LP).
    Returns scalar >= 0.
    """
    verts = np.asarray(verts, dtype=float)
    m = verts.shape[0]
    # LP variable: beta (m,)
    # minimize c^T beta where c = ones
    c = np.ones(m)
    # linprog expects A_eq shape (neq, nvar)
    A_eq = verts.T  # shape (2, m)
    b_eq = np.asarray(v, dtype=float)
    bounds = [(0, None) for _ in range(m)]
    res = linprog(c, A_eq=A_eq, b_eq=b_eq, bounds=bounds, method='highs')
    if not res.success:
        raise RuntimeError(f"LP failed: {res.message}")
    return float(res.fun)

def gauge_unit_vector(theta, verts):
    u = np.array([np.cos(theta), np.sin(theta)])
    return minkowski_gauge_vector(u, verts)
