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

# Generate triangle mesh from the grid
def generate_mesh(points_grid):
    """
    Convert a (nu, nv, 3) grid of points into a triangle mesh.
    Returns: vertices (N, 3), triangles (M, 3) with 0-based indices.
    """
    nu, nv, _ = points_grid.shape
    
    # flatten points to vertex array
    vertices = points_grid.reshape(-1, 3)
    
    # build triangle index array
    triangles = []
    for i in range(nu - 1):
        for j in range(nv - 1):
            # each quad has indices:
            #   v0 = i*nv + j
            #   v1 = i*nv + (j+1)
            #   v2 = (i+1)*nv + j
            #   v3 = (i+1)*nv + (j+1)
            v0 = i * nv + j
            v1 = i * nv + (j + 1)
            v2 = (i + 1) * nv + j
            v3 = (i + 1) * nv + (j + 1)
            
            # split quad into two triangles
            triangles.append([v0, v1, v2])
            triangles.append([v1, v3, v2])
    
    return vertices, np.array(triangles, dtype=int)

vertices, triangles = generate_mesh(points)
print(f"Mesh: {vertices.shape[0]} vertices, {triangles.shape[0]} triangles")

def export_obj(filename, vertices, triangles):
    """Export mesh to Wavefront OBJ format."""
    with open(filename, 'w') as f:
        f.write("# NURBS surface mesh\n")
        for v in vertices:
            f.write(f"v {v[0]} {v[1]} {v[2]}\n")
        for tri in triangles:
            # OBJ uses 1-based indexing
            f.write(f"f {tri[0]+1} {tri[1]+1} {tri[2]+1}\n")
    print(f"Exported mesh to {filename}")

# Uncomment to export:
# export_obj("nurbs_surface.obj", vertices, triangles)

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# plot mesh as wireframe or surface
ax.plot_trisurf(vertices[:,0], vertices[:,1], vertices[:,2], 
                triangles=triangles, alpha=0.7, edgecolor='k', linewidth=0.2)

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# plot control points
cp_x = control_points[:,:,0].flatten()
cp_y = control_points[:,:,1].flatten()
cp_z = control_points[:,:,2].flatten()
ax.scatter(cp_x, cp_y, cp_z, c='r', marker='^', s=50)

plt.show()
