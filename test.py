import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.widgets import Slider

# Open ./test/curve_1_out.csv and plot
data = np.loadtxt('./test/curve_1_out.csv', delimiter=',')
x = data[:, 0]
y = data[:, 1]
z = data[:, 2]

fig = plt.figure(figsize=(8, 6))
ax = fig.add_subplot(111, projection='3d')
ax.labels = ('X axis', 'Y axis', 'Z axis')
sc = ax.scatter(x, y, z)
plt.subplots_adjust(bottom=0.25)
ax_elev = plt.axes([0.15, 0.12, 0.7, 0.03])
ax_azim = plt.axes([0.15, 0.05, 0.7, 0.03])
slider_elev = Slider(ax_elev, 'elevation', -90, 90, valinit=30, valstep=1)
slider_azim = Slider(ax_azim, 'azimuth', 0, 360, valinit=45, valstep=1)

def update(val):
    elev = slider_elev.val
    azim = slider_azim.val
    ax.view_init(elev=elev, azim=azim)
    fig.canvas.draw_idle()

slider_elev.on_changed(update)
slider_azim.on_changed(update)

plt.show()
