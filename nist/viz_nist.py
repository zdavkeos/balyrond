#!/usr/bin/env python3

""" Visualize the test data and solutions in 2D """

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

def get_solution(f):
    with open(f, 'r') as fd:
        xc = float(fd.readline())
        yc = float(fd.readline())
        zc = float(fd.readline())
        xn = float(fd.readline())
        yn = float(fd.readline())
        zn = float(fd.readline())
        d  = float(fd.readline())

        if xn != 0:
            c = {'x': yc, 'y': zc}
        elif yn != 0:
            c = {'x': xc, 'y': zc}
        else:
            c = {'x': xc, 'y': yc}

        c['d'] = d
        return c
        

fig = plt.figure()
ax = fig.add_subplot(111)

for i in range(1, 31):
    d = get_data("Circle2d/Cir2d" + str(i) + ".ds")
    s = get_solution("Circle2d/Cir2d" + str(i) + ".fit")

    ax.scatter(d[0], d[1], label="Test " + str(i))

    c = plt.Circle((s['x'], s['y']), s['d']/2.0, color='black', fill=False)
    ax.add_patch(c)

ax.legend()
plt.tight_layout()
plt.show()
