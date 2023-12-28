#!/usr/bin/env python3

""" Visualize the test case 21 """

import os
import sys

import numpy as np
import matplotlib.pyplot as plt

def to_2d(data):
    "Eliminate plane axis"

    if data[0][0] == data[0][1]:
        return [data[1], data[2]]
    elif data[1][0] == data[1][1]:
        return [data[0], data[2]]
    else:
        return [data[0], data[1]]

def get_data(f):
    points = []
    with open(f, 'r') as fd:
        count = int(fd.readline())
        for _ in range(count):
            pts = fd.readline().split('\t')
            points.append([float(pts[0]), float(pts[1]), float(pts[2])])

    points = np.transpose(points)
    return to_2d(points)


nist_micx = 445.608660
nist_micy = -701.528676
nist_micr = 161.862071

nist_mccx = 444.862249
nist_mccy = -700.997725
nist_mccr = 165.899932

nist_lscfx = 446.334021
nist_lscfy = -702.281635
nist_lscfr = 162.599100

baly_lscfx = 446.741167
baly_lscfy = -702.596951
baly_lscfr = 162.139293

fig = plt.figure()
ax = fig.add_subplot(111)

d = get_data("Circle2d/Cir2d21.ds")

ax.scatter(d[0], d[1], label="Test 21 Input Points")

c = plt.Circle((nist_micx, nist_micy), nist_micr, color='black', fill=False, label="NIST MIC")
ax.add_patch(c)

c = plt.Circle((nist_mccx, nist_mccy), nist_mccr, color='red', fill=False, label="NIST MCC")
ax.add_patch(c)

c = plt.Circle((nist_lscfx, nist_lscfy), nist_lscfr, color='blue', fill=False, label="NIST LSCF")
ax.add_patch(c)

c = plt.Circle((baly_lscfx, baly_lscfy), baly_lscfr, color='orange', fill=False, label="BALY LSCF")
ax.add_patch(c)

ax.legend()
plt.tight_layout()
plt.show()
