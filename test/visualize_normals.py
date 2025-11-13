import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def visualize_normals(norm_file='norm_0.txt'):
    """
    Visualize surface and normals to check if they look correct.
    Normals should point perpendicular to the surface.
    """
    # Load normal data (format: x,y,z,nx,ny,nz)
    try:
        data = np.loadtxt(norm_file, delimiter=',')
    except Exception as e:
        print(f"Could not load {norm_file}: {e}")
        print("Expected CSV format: x,y,z,nx,ny,nz")
        return
    
    # Check format
    if data.shape[1] < 6:
        print(f"Expected 6 columns (x,y,z,nx,ny,nz), got {data.shape[1]}")
        return
    
    x, y, z = data[:, 0], data[:, 1], data[:, 2]
    nx, ny, nz = data[:, 3], data[:, 4], data[:, 5]
    
    print(f"Loaded {len(x)} points from {norm_file}")
    
    # Check if normals are unit length
    lengths = np.sqrt(nx**2 + ny**2 + nz**2)
    print(f"\nNormal lengths:")
    print(f"  Min: {lengths.min():.6f}")
    print(f"  Max: {lengths.max():.6f}")
    print(f"  Mean: {lengths.mean():.6f}")
    print(f"  (Should all be ~1.0)")
    
    non_unit = np.sum(np.abs(lengths - 1.0) > 1e-3)
    if non_unit == 0:
        print(f"  ✓ All normals are unit length")
    else:
        print(f"  ✗ WARNING: {non_unit} normals are not unit length!")
    
    # Calculate surface bounds
    x_range = x.max() - x.min() if x.max() != x.min() else 1.0
    y_range = y.max() - y.min() if y.max() != y.min() else 1.0
    z_range = z.max() - z.min() if z.max() != z.min() else 1.0
    
    # Print data ranges
    print(f"\nData ranges:")
    print(f"  X: [{x.min():.6f}, {x.max():.6f}] (range: {x_range:.6f})")
    print(f"  Y: [{y.min():.6f}, {y.max():.6f}] (range: {y_range:.6f})")
    print(f"  Z: [{z.min():.6f}, {z.max():.6f}] (range: {z_range:.6f})")
    
    scale = max(x_range, y_range, z_range) * 0.1  # Scale normals to 10% of surface size
    
    # Visualize
    fig = plt.figure(figsize=(14, 6))
    
    # Plot 1: Surface with normals
    ax1 = fig.add_subplot(121, projection='3d')
    
    # Color by z-coordinate for better depth perception
    scatter = ax1.scatter(x, y, z, c=z, cmap='viridis', s=10, alpha=0.6)
    plt.colorbar(scatter, ax=ax1, label='Z coordinate', shrink=0.5)
    
    # Draw normals (subsample for clarity if too many points)
    num_points = len(x)
    step = max(1, num_points // 100)  # Show ~100 normals max
    
    ax1.quiver(x[::step], y[::step], z[::step], 
               nx[::step], ny[::step], nz[::step],
               length=scale, color='red', alpha=0.7, linewidth=1.5)
    
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    ax1.set_title(f'Surface with Normals\n({num_points} points, showing every {step})', fontsize=12)
    ax1.view_init(elev=20, azim=45)
    
    # Set equal aspect ratio for proper visualization
    # This prevents the z-axis from being compressed
    max_range = max(x_range, y_range, z_range) / 2.0
    mid_x = (x.max() + x.min()) / 2.0
    mid_y = (y.max() + y.min()) / 2.0
    mid_z = (z.max() + z.min()) / 2.0
    ax1.set_xlim(mid_x - max_range, mid_x + max_range)
    ax1.set_ylim(mid_y - max_range, mid_y + max_range)
    ax1.set_zlim(mid_z - max_range, mid_z + max_range)
    
    # Alternative: use set_box_aspect for equal scaling (requires matplotlib >= 3.3)
    try:
        ax1.set_box_aspect([1, 1, 1])
    except AttributeError:
        pass  # Older matplotlib version
    
    # Plot 2: Normal directions on unit sphere
    ax2 = fig.add_subplot(122, projection='3d')
    
    # Plot normals as arrows from origin
    ax2.quiver(np.zeros(len(nx[::step])), 
               np.zeros(len(ny[::step])), 
               np.zeros(len(nz[::step])),
               nx[::step], ny[::step], nz[::step],
               length=1.0, normalize=False, alpha=0.5, cmap='coolwarm')
    
    # Draw unit sphere for reference
    u = np.linspace(0, 2 * np.pi, 30)
    v = np.linspace(0, np.pi, 20)
    x_sphere = np.outer(np.cos(u), np.sin(v))
    y_sphere = np.outer(np.sin(u), np.sin(v))
    z_sphere = np.outer(np.ones(np.size(u)), np.cos(v))
    ax2.plot_surface(x_sphere, y_sphere, z_sphere, alpha=0.1, color='gray')
    
    ax2.set_xlabel('NX')
    ax2.set_ylabel('NY')
    ax2.set_zlabel('NZ')
    ax2.set_title('Normal Distribution\n(should lie on unit sphere)', fontsize=12)
    ax2.set_xlim([-1.2, 1.2])
    ax2.set_ylim([-1.2, 1.2])
    ax2.set_zlim([-1.2, 1.2])
    
    plt.tight_layout()
    
    output_file = 'normal_visualization.png'
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"\n✓ Visualization saved to {output_file}")
    
    plt.show()

if __name__ == "__main__":
    import sys
    
    # Allow specifying file as command line argument
    if len(sys.argv) > 1:
        norm_file = sys.argv[1]
    else:
        norm_file = 'norm_0.txt'
    
    print("=" * 50)
    print("NURBS Surface Normal Visualization")
    print("=" * 50)
    visualize_normals(norm_file)
