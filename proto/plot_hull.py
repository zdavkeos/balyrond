#!/usr/bin/env python3

import matplotlib.pyplot as plt

hull = [(99.4921,129.436),
(104.632,187.052),
(137.258,211.171),
(137.258,211.171),
(177.035,216.754),
(209.438,168.517),
(193.196,105.556),
(193.196,105.556),
(124.52,112.24)]

test_set_1 = [
    (209.4379, 168.5169), (185.5269, 184.8491), (177.0352, 216.7537), (149.9957, 207.8210),
    (137.2580, 211.1708), (193.1959, 105.5558), (104.6318, 187.0523), (196.4768, 139.2621),
    (121.8388, 162.9339), (99.49210, 129.4361), (124.5204, 112.2405), (154.6885, 121.1733)]

c_centers = [(154.689,121.173),
(196.477,139.262),
(121.839,162.934),
(185.527,184.849),
(149.996,207.821)]

hxs = [p[0] for p in hull]
hys = [p[1] for p in hull]

xs = [p[0] for p in test_set_1]
ys = [p[1] for p in test_set_1]

cxs = [p[0] for p in c_centers]
cys = [p[1] for p in c_centers]

plt.plot(hxs, hys, 'r')
plt.plot(xs, ys, '.g')
plt.plot(cxs, cys, '.b')

plt.show()