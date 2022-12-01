import numpy as np
import matplotlib.pyplot as plt

points = np.loadtxt("Competition019.in", skiprows=1, max_rows=2500)

plt.plot(points[:, 0], points[:, 1], '.')
plt.show()
