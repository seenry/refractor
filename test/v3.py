import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Read the data from surf_0.txt
data = np.loadtxt('surf_0.txt', delimiter=',')

# Extract x, y, z coordinates
x = data[:, 0]
y = data[:, 1]
z = data[:, 2]

# Create a 3D figure
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# Create scatter plot
scatter = ax.scatter(x, y, z, c=z, cmap='viridis', marker='o', s=20, alpha=0.6)

# Add labels and title
ax.set_xlabel('X', fontsize=12)
ax.set_ylabel('Y', fontsize=12)
ax.set_zlabel('Z', fontsize=12)
ax.set_title('3D Scatter Plot from surf_0.txt', fontsize=14)

# Add colorbar
colorbar = plt.colorbar(scatter, ax=ax, shrink=0.5, aspect=5)
colorbar.set_label('Z value', fontsize=10)

# Adjust viewing angle for better visualization
ax.view_init(elev=20, azim=45)

# Add grid
ax.grid(True, alpha=0.3)

# Show the plot
plt.tight_layout()
plt.show()