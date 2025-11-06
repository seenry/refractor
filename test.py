import matplotlib.pyplot as plt
import numpy as np

def blend(knot, i, p, t):
    tol = 1e-6

    if p == 0:
        return (knot[i] <= t + tol and t - tol < knot[i + 1]) and 1.0 or 0.0

    d1 = knot[i + p] - knot[i]
    c1 = (0.0 if d1 < tol else (t - knot[i]) / d1)
    d2 = knot[i + p + 1] - knot[i + 1]
    c2 = (0.0 if d2 < tol else (knot[i + p + 1] - t) / d2)

    return c1 * blend(knot, i, p - 1, t) + c2 * blend(knot, i + 1, p - 1, t)

def curve(c, w, k, degree, t):
    n = len(c)
    numerator = (0.0, 0.0)
    denominator = 0.0

    for i in range(n):
        b = blend(k, i, degree, t)
        numerator = (
            numerator[0] + b * w[i] * c[i][0],
            numerator[1] + b * w[i] * c[i][1]
        )
        denominator += b * w[i]

    return (
        numerator[0] / denominator if denominator != 0 else 0.0,
        numerator[1] / denominator if denominator != 0 else 0.0
    )

# Example usage and visualization
ctrls = [
    [(-4, -4), (-2, 4), (2, -4), (4, 4)]
]
weights = [
    [1, 1, 1, 1]
]
knots = [
    [0, 0, 0, 0, 1, 1, 1, 1]
]
degree = 3
ts = np.linspace(0, 1, 100)
curve_points_x = [curve(
    ctrls[0],
    weights[0],
    knots[0], degree, t) for t in ts]
curve_points_y = [curve(
    ctrls[0],
    weights[0],
    knots[0], degree, t) for t in ts]

plt.plot(*zip(*curve_points_x), color='red')
ctrlx, ctrly = zip(*ctrls[0])
plt.scatter(ctrlx, ctrly, color='black')
plt.show()
