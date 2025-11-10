import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.widgets import Slider

def interp(i, k, t, knots):
    if k == 0:
        return 1.0 if knots[i] <= t < knots[i+1] else 0.0
    else:
        denom1 = knots[i+k] - knots[i]
        denom2 = knots[i+k+1] - knots[i+1]
        term1 = ((t - knots[i]) / denom1) * interp(i, k-1, t, knots) if denom1 else 0
        term2 = ((knots[i+k+1] - t) / denom2) * interp(i+1, k-1, t, knots) if denom2 else 0
        return term1 + term2

def eval(u, v, control_points, u_knots, v_knots, p, q, weights=None):
    nu, nv, _ = control_points.shape
    
    # if no weights provided, default to uniform weights (standard B-spline)
    if weights is None:
        weights = np.ones((nu, nv))
    
    # compute rational basis (NURBS)
    numerator = np.zeros(3)
    denominator = 0.0
    
    for i in range(nu):
        for j in range(nv):
            N = interp(i, p, u, u_knots)
            M = interp(j, q, v, v_knots)
            w = weights[i, j]
            basis_weighted = N * M * w
            numerator += basis_weighted * control_points[i, j]
            denominator += basis_weighted
    
    # rational divide
    return numerator / denominator if denominator != 0 else np.zeros(3)

# degrees
p, q = 3, 3

# control points grid (shape: (nu, nv, 3))
control_points = np.array([
    [[0, 0, 0], [0, 1, 0  ], [0, 2, 0  ], [0, 3, 0]],
    [[1, 0, 0], [1, 1, 0.2], [1, 2, 0.1], [1, 3, 0]],
    [[2, 0, 0], [2, 1, -0.5], [2, 2, 0.3], [2, 3, 0]],
    [[3, 0, 0], [3, 1, 0  ], [3, 2, 0  ], [3, 3, 0]],
])

# knot vectors
u_knots = np.array([0, 0, 0, 0, 1, 1, 1, 1])  # open uniform
v_knots = np.array([0, 0, 0, 0, 1, 1, 1, 1])

# weights grid (shape: (nu, nv)) - vary weights to demonstrate NURBS behavior
weights = np.ones((4, 4))
# increase weight at center control points to "pull" surface toward them
weights[1, 1] = 2.0
weights[1, 2] = 2.0
weights[2, 1] = 2.0
weights[2, 2] = 2.0

x = np.linspace(0, 1, 100)
y = np.linspace(0, 1, 100)
points = np.array([[eval(u, v, control_points, u_knots, v_knots, p, q, weights) for v in y] for u in x])

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
scat = ax.scatter(points[:,:,0], points[:,:,1], points[:,:,2], c='b', marker='o', s=1)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# plot control points
cp_x = control_points[:,:,0].flatten()
cp_y = control_points[:,:,1].flatten()
cp_z = control_points[:,:,2].flatten()
ax.scatter(cp_x, cp_y, cp_z, c='r', marker='^', s=50)

plt.show()
