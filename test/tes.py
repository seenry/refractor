import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

def read_tessellation(filename):
    """Read tessellation tiles from file"""
    tiles = []
    current_tile = []
    
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:  # Empty line separates tiles
                if current_tile:
                    tiles.append(np.array(current_tile))
                    current_tile = []
            else:
                coords = [float(x) for x in line.split(',')]
                current_tile.append(coords)
    
    # Don't forget the last tile
    if current_tile:
        tiles.append(np.array(current_tile))
    
    return tiles

def visualize_tessellation(filename):
    """Visualize the tessellation as a 3D surface mesh"""
    tiles = read_tessellation(filename)
    
    print(f"Loaded {len(tiles)} tiles")
    
    # Create figure
    fig = plt.figure(figsize=(14, 10))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot each tile as a quad
    for i, tile in enumerate(tiles):
        # Remove the duplicate last point (which closes the quad)
        tile = tile[:-1]
        
        # Create a polygon for this tile
        # Use the first 4 points to form a quad
        verts = [tile[:4]]
        
        # Color based on depth (approximate by tile index)
        color = plt.cm.viridis(i / len(tiles))
        
        # Create polygon collection
        poly = Poly3DCollection(verts, alpha=0.7, facecolors=color, 
                                edgecolors='black', linewidths=0.5)
        ax.add_collection3d(poly)
    
    # Set axis labels and limits
    all_points = np.vstack(tiles)
    ax.set_xlabel('X', fontsize=12)
    ax.set_ylabel('Y', fontsize=12)
    ax.set_zlabel('Z', fontsize=12)
    ax.set_title(f'NURBS Surface Tessellation ({len(tiles)} tiles)', fontsize=14)
    
    # Set axis limits based on data
    ax.set_xlim([all_points[:, 0].min(), all_points[:, 0].max()])
    ax.set_ylim([all_points[:, 1].min(), all_points[:, 1].max()])
    ax.set_zlim([all_points[:, 2].min(), all_points[:, 2].max()])
    
    # Adjust viewing angle
    ax.view_init(elev=30, azim=45)
    
    plt.tight_layout()
    plt.show()

def visualize_tessellation_2d(filename):
    """Visualize the tessellation in 2D parameter space (u,v)"""
    tiles = read_tessellation(filename)
    
    print(f"Loaded {len(tiles)} tiles for 2D view")
    
    fig, ax = plt.subplots(figsize=(10, 10))
    
    for i, tile in enumerate(tiles):
        # Remove duplicate last point
        tile = tile[:-1]
        
        # Plot the quad in parameter space (assuming some mapping)
        # For now, just plot XY projection
        quad = np.vstack([tile, tile[0]])  # Close the quad
        color = plt.cm.viridis(i / len(tiles))
        ax.plot(quad[:, 0], quad[:, 1], 'k-', linewidth=0.5)
        ax.fill(quad[:, 0], quad[:, 1], color=color, alpha=0.3)
    
    ax.set_xlabel('X (or u)', fontsize=12)
    ax.set_ylabel('Y (or v)', fontsize=12)
    ax.set_title(f'Tessellation Tiles ({len(tiles)} tiles)', fontsize=14)
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

def visualize_both(filename):
    """Visualize both 3D and 2D views side by side"""
    tiles = read_tessellation(filename)
    
    print(f"Loaded {len(tiles)} tiles")
    
    # Create figure with two subplots
    fig = plt.figure(figsize=(18, 8))
    
    # 3D subplot on the left
    ax1 = fig.add_subplot(121, projection='3d')
    
    # Plot each tile as a quad in 3D
    for i, tile in enumerate(tiles):
        # Remove the duplicate last point (which closes the quad)
        tile_points = tile[:-1]
        
        # Create a polygon for this tile
        verts = [tile_points[:4]]
        
        # Color based on depth (approximate by tile index)
        color = plt.cm.viridis(i / len(tiles))
        
        # Create polygon collection
        poly = Poly3DCollection(verts, alpha=0.7, facecolors=color, 
                                edgecolors='black', linewidths=0.5)
        ax1.add_collection3d(poly)
    
    # Set axis labels and limits for 3D
    all_points = np.vstack(tiles)
    ax1.set_xlabel('X', fontsize=12)
    ax1.set_ylabel('Y', fontsize=12)
    ax1.set_zlabel('Z', fontsize=12)
    ax1.set_title(f'3D Surface Tessellation ({len(tiles)} tiles)', fontsize=14)
    
    ax1.set_xlim([all_points[:, 0].min(), all_points[:, 0].max()])
    ax1.set_ylim([all_points[:, 1].min(), all_points[:, 1].max()])
    ax1.set_zlim([all_points[:, 2].min(), all_points[:, 2].max()])
    
    ax1.view_init(elev=30, azim=45)
    
    # 2D subplot on the right
    ax2 = fig.add_subplot(122)
    
    for i, tile in enumerate(tiles):
        # Remove duplicate last point
        tile_points = tile[:-1]
        
        # Plot the quad in XY projection
        quad = np.vstack([tile_points, tile_points[0]])  # Close the quad
        color = plt.cm.viridis(i / len(tiles))
        ax2.plot(quad[:, 0], quad[:, 1], 'k-', linewidth=0.5)
        ax2.fill(quad[:, 0], quad[:, 1], color=color, alpha=0.3)
    
    ax2.set_xlabel('X', fontsize=12)
    ax2.set_ylabel('Y', fontsize=12)
    ax2.set_title(f'2D Projection ({len(tiles)} tiles)', fontsize=14)
    ax2.set_aspect('equal')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

def visualize_rays(tessellation_file, ray_intersects_file):
    """Visualize tessellation with rays and intersection points"""
    tiles = read_tessellation(tessellation_file)
    
    # Read ray intersection points
    intersections = []
    with open(ray_intersects_file, 'r') as f:
        for line in f:
            coords = [float(x) for x in line.strip().split(',')]
            intersections.append(coords)
    
    intersections = np.array(intersections)
    print(f"Loaded {len(tiles)} tiles and {len(intersections)} ray intersections")
    
    # Create figure
    fig = plt.figure(figsize=(18, 8))
    
    # 3D view on the left
    ax1 = fig.add_subplot(121, projection='3d')
    
    # Plot tessellation
    for i, tile in enumerate(tiles):
        tile_points = tile[:-1]
        verts = [tile_points[:4]]
        color = plt.cm.viridis(i / len(tiles))
        poly = Poly3DCollection(verts, alpha=0.3, facecolors=color, 
                                edgecolors='gray', linewidths=0.3)
        ax1.add_collection3d(poly)
    
    # Plot intersection points
    ax1.scatter(intersections[:, 0], intersections[:, 1], intersections[:, 2],
                c='red', s=20, marker='o', label='Intersections', alpha=0.8)
    
    # Plot rays (from intersection + 1.0 in z, pointing down)
    ray_starts = intersections.copy()
    ray_starts[:, 2] += 1.0  # Rays start 1.0 unit above intersection
    
    # Draw ray lines (sampling for visual clarity)
    step = max(1, len(intersections) // 50)  # Show ~50 rays max
    for i in range(0, len(intersections), step):
        start = ray_starts[i]
        end = intersections[i]
        ax1.plot([start[0], end[0]], [start[1], end[1]], [start[2], end[2]],
                 'b-', linewidth=0.5, alpha=0.5)
    
    # Mark ray origins (sampled to match lines)
    ax1.scatter(ray_starts[::step, 0], ray_starts[::step, 1], ray_starts[::step, 2],
                c='blue', s=10, marker='^', label='Ray origins', alpha=0.6)
    
    all_points = np.vstack(tiles)
    ax1.set_xlabel('X', fontsize=12)
    ax1.set_ylabel('Y', fontsize=12)
    ax1.set_zlabel('Z', fontsize=12)
    ax1.set_title(f'Ray Tracing Test\n{len(tiles)} tiles, {len(intersections)} rays', fontsize=14)
    ax1.legend()
    ax1.view_init(elev=25, azim=45)
    
    # Set limits to show everything
    all_z = np.concatenate([all_points[:, 2], intersections[:, 2], ray_starts[:, 2]])
    ax1.set_xlim([all_points[:, 0].min(), all_points[:, 0].max()])
    ax1.set_ylim([all_points[:, 1].min(), all_points[:, 1].max()])
    ax1.set_zlim([all_z.min() - 0.1, all_z.max() + 0.1])
    
    # 2D top-down view on the right
    ax2 = fig.add_subplot(122)
    
    # Plot tessellation tiles
    for i, tile in enumerate(tiles):
        tile_points = tile[:-1]
        quad = np.vstack([tile_points, tile_points[0]])
        color = plt.cm.viridis(i / len(tiles))
        ax2.plot(quad[:, 0], quad[:, 1], 'k-', linewidth=0.3)
        ax2.fill(quad[:, 0], quad[:, 1], color=color, alpha=0.2)
    
    # Plot intersection points (XY projection)
    ax2.scatter(intersections[:, 0], intersections[:, 1],
                c='red', s=15, marker='o', label='Intersections', alpha=0.8, zorder=5)
    
    # Plot ray origins (XY projection, sampled)
    ax2.scatter(ray_starts[::step, 0], ray_starts[::step, 1],
                c='blue', s=8, marker='^', label='Ray origins', alpha=0.6, zorder=5)
    
    ax2.set_xlabel('X', fontsize=12)
    ax2.set_ylabel('Y', fontsize=12)
    ax2.set_title(f'Top-Down View (XY Projection)\n{len(intersections)} intersection points', fontsize=14)
    ax2.set_aspect('equal')
    ax2.grid(True, alpha=0.3)
    ax2.legend()
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == 'rays':
        # Show rays and intersections
        tessellation_file = sys.argv[2] if len(sys.argv) > 2 else 'tessellation.txt'
        ray_file = sys.argv[3] if len(sys.argv) > 3 else 'ray_intersects.txt'
        print(f"Visualizing tessellation with rays...")
        visualize_rays(tessellation_file, ray_file)
    else:
        # Show just tessellation
        filename = sys.argv[1] if len(sys.argv) > 1 else 'tessellation.txt'
        print("Visualizing tessellation...")
        visualize_both(filename)