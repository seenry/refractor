import argparse
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import re

def parse_line(line):
    """Parse a line in the format: (u, v), (x, y, z)"""
    # Remove whitespace and split by '), ('
    line = line.strip()
    # Use regex to extract the coordinates
    match = re.match(r'\(([^)]+)\),\s*\(([^)]+)\)', line)
    if match:
        # Parse the second tuple (x, y, z)
        xyz = match.group(2)
        coords = [float(val.strip()) for val in xyz.split(',')]
        return coords
    return None

def visualize_file(filename):
    """Read file and create 3D scatter plot"""
    x_coords = []
    y_coords = []
    z_coords = []
    
    with open(filename, 'r') as f:
        for line in f:
            coords = parse_line(line)
            if coords and len(coords) == 3:
                x_coords.append(coords[0])
                y_coords.append(coords[1])
                z_coords.append(coords[2])
    
    # Create 3D scatter plot
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    scatter = ax.scatter(x_coords, y_coords, z_coords, c=z_coords, 
                        cmap='viridis', marker='o', s=1)
    
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title(f'3D Scatter Plot from {filename}')
    
    # Add colorbar
    plt.colorbar(scatter, ax=ax, label='Z value')
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Visualize 3D coordinates from file')
    parser.add_argument('filename', type=str, help='Path to the input file')
    
    args = parser.parse_args()
    visualize_file(args.filename)