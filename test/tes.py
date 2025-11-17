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

if __name__ == "__main__":
    import sys
    
    filename = sys.argv[1] if len(sys.argv) > 1 else 'tessellation.txt'
    
    print("Visualizing tessellation...")
    
    # Show both views side by side
    visualize_both(filename)