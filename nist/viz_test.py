#!/usr/bin/env python3

""" Visualize individual test cases """

import re
import os
import sys

import numpy as np
import matplotlib.pyplot as plt

def to_2d(data):
    "Eliminate plane axis"

    if data[0][0] == data[0][1]:
        return [data[1], data[2]]  # in X plane
    elif data[1][0] == data[1][1]: # in Y plane
        return [data[0], data[2]]
    else:                          # in Z plane
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

def get_nist_sols(test):
    with open("octave_output.txt", 'r') as fd:
        lines = fd.readlines()

    for line in range(len(lines)):
        l = lines[line].strip()

        if re.search(r'cir2d{}\.ds'.format(test), l):
            nist_micx = float(lines[line+2].split(':')[1])
            nist_micy = float(lines[line+3].split(':')[1])
            nist_micr = float(lines[line+4].split(':')[1])

            nist_mccx = float(lines[line+6].split(':')[1])
            nist_mccy = float(lines[line+7].split(':')[1])
            nist_mccr = float(lines[line+8].split(':')[1])

            nist_lscfx = float(lines[line+10].split(':')[1])
            nist_lscfy = float(lines[line+11].split(':')[1])
            nist_lscfz = float(lines[line+12].split(':')[1])
            nist_lscfr = float(lines[line+13].split(':')[1])

            nx, ny, nz = map(float, lines[line+15][10:].split(' '))

            if abs(nx) > 0.0:
                nist_lscfx = nist_lscfy
                nist_lscfy = nist_lscfz
            elif abs(ny) > 0.0:
                nist_lscfy = nist_lscfz

            print(nist_micx, nist_micy, nist_micr, nist_mccx, nist_mccy, nist_mccr, nist_lscfx, nist_lscfy, nist_lscfr)
            return nist_micx, nist_micy, nist_micr, nist_mccx, nist_mccy, nist_mccr, nist_lscfx, nist_lscfy, nist_lscfr

def get_baly_sols(test):
    with open("balyrond_output.txt", 'r') as fd:
        lines = fd.readlines()

    for line in range(len(lines)):
        l = lines[line].strip()

        if re.match(r'^Test: {}$'.format(test), l):
            baly_micx, baly_micy, baly_micr = map(float, lines[line+1][5:].split(" "))
            baly_mccx, baly_mccy, baly_mccr = map(float, lines[line+2][5:].split(" "))
            baly_lscfx, baly_lscfy, baly_lscfr = map(float, lines[line+3][6:].split(" "))

            return baly_micx, baly_micy, baly_micr, baly_mccx, baly_mccy, baly_mccr, baly_lscfx, baly_lscfy, baly_lscfr
        
try:
    test = int(sys.argv[1])
except:
    print("Usage: {} [test no. 1-30]".format(sys.argv[0]))
    sys.exit(0)

fig = plt.figure()
ax = fig.add_subplot(111)

d = get_data("Circle2d/Cir2d{}.ds".format(test))

nist_micx, nist_micy, nist_micr, nist_mccx, nist_mccy, nist_mccr, nist_lscfx, nist_lscfy, nist_lscfr = get_nist_sols(test)
baly_micx, baly_micy, baly_micr, baly_mccx, baly_mccy, baly_mccr, baly_lscfx, baly_lscfy, baly_lscfr = get_baly_sols(test)

ax.scatter(d[0], d[1], label="Test {} Input Points".format(test))

c = plt.Circle((nist_micx, nist_micy), nist_micr, color='black', fill=False, label="NIST MIC")
ax.add_patch(c)

c = plt.Circle((nist_mccx, nist_mccy), nist_mccr, color='red', fill=False, label="NIST MCC")
ax.add_patch(c)

c = plt.Circle((nist_lscfx, nist_lscfy), nist_lscfr, color='blue', fill=False, label="NIST LSCF")
ax.add_patch(c)

c = plt.Circle((baly_micx, baly_micy), baly_micr, color='orange', fill=False, label="BALY MIC")
ax.add_patch(c)

c = plt.Circle((baly_mccx, baly_mccy), baly_mccr, color='green', fill=False, label="BALY MCC")
ax.add_patch(c)

c = plt.Circle((baly_lscfx, baly_lscfy), baly_lscfr, color='indigo', fill=False, label="BALY LSCF")
ax.add_patch(c)

ax.set_aspect('equal', 'box')
ax.legend()
plt.tight_layout()
plt.show()
