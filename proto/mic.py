#!/usr/bin/env python3

# Maximum Inscribed Circle Algorithm
#
# Based on the paper:
# "A Quick Algorithm of Maximum Inscribed Circle Method for Evaluation of the Roundness Error"
# http://dx.doi.org/10.1109/ICSSEM.2011.6081316

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.

import math
import random
import itertools

import matplotlib.pyplot as plt

class Pt:
    " 'Point' helper class"
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __str__(self):
        return "({},{})".format(self.x, self.y)

# determinant of a 3x3 matrix
def det(a,b,c,d,e,f,g,h,i):
    return a*e*i + b*f*g +c*d*h - c*e*g - b*d*i - a*f*h

# Using minor matrix determinants
# https://web.archive.org/web/20161011113446/http://www.abecedarical.com/zenosamples/zs_circle3pts.html
def tri_to_circ3(p1, p2, p3):
    x1 = p1.x
    x2 = p2.x
    x3 = p3.x
    y1 = p1.y
    y2 = p2.y
    y3 = p3.y

    m11 = det(x1, y1, 1, x2, y2, 1, x3, y3, 1)
    m12 = det((x1*x1)+(y1*y1), y1, 1, (x2*x2)+(y2*y2), y2, 1, (x3*x3)+(y3*y3), y3, 1)
    m13 = det(x1*x1+y1*y1, x1, 1, x2*x2+y2*y2, x2, 1, x3*x3+y3*y3, x3, 1)
    m14 = det(x1*x1+y1*y1, x1, y1, x2*x2+y2*y2, x2, y2, x3*x3+y3*y3, x3, y3)
    
    x = .5 * (m12 / m11)
    y = -.5 * (m13 / m11)
    r = math.sqrt(abs(x*x + y*y + (m14 / m11)))

    return x, y, r

# euclidian distance
def dist(p1, p2):
    return math.sqrt((p1.x-p2.x)**2 + (p1.y-p2.y)**2)

# determine if a point is inside or on a circle
def in_circ(x, y, r, pt):
    d = dist(Pt(x,y), pt)
    return d < (r - .00001) # fudge due to numerical accuracy

# determine if all points are outside or on a circle
def is_mic(x, y, r, pts):
    for pt in pts:
        if in_circ(x, y, r, pt):
            return False
    return True

# Pick the 3 control points based on largest radius
def decide_mic(pts):
    b = -1
    bs = None
    for i in itertools.combinations(pts, 3):
        x, y, r = tri_to_circ3(*i)
        if r > b and is_mic(x, y, r, pts):
            b = r
            bs = list(i)
    return bs

def find_mic(pts):

    # initial points: Step 1
    p1 = sorted(pts, key=lambda p: p.x)[0]
    p2 = sorted(pts, key=lambda p: p.x, reverse=True)[0]
    p3 = sorted(pts, key=lambda p: p.y)[0]
    p4 = sorted(pts, key=lambda p: p.y, reverse=True)[0]
    mic = (p1, p2, p3, p4)

    while True:
        # Of the 4 points, determine the 3 control points. Step 2
        mic = decide_mic(mic)
        # plot(pts, mic) # plot each iteration
        mx, my, mr = tri_to_circ3(*mic)

        # Step 3
        if is_mic(mx, my, mr, pts):
            break

        # Pick point closest to center as next control point. Step 4
        pn = sorted(pts, key=lambda p: dist(Pt(mx, my), p))[0]
        mic.append(pn)

    # Determine Roundness Error. Step 5.
    dfts = -1
    mic_center = Pt(mx, my)
    for pt in pts:
        d = dist(mic_center, pt) - mr
        if d > dfts:
            dfts = d

    print("Maximum Deviation From MIC:")
    print(dfts)

    return mic

def plot(pts, mic):
    fig = plt.figure()
    ax = fig.add_subplot(111)

    # Plot all the points
    xs = []
    ys = []
    for p in pts:
        xs.append(p.x)
        ys.append(p.y)

    ax.scatter(xs, ys)

    # Plot control points
    txs = []
    tys = []
    for p in mic:
        txs.append(p.x)
        tys.append(p.y)

    ax.plot(txs, tys, 'r')

    # Plot the circle
    x, y, r = tri_to_circ3(*mic)
    cxs = []
    cys = []
    for t in range(360):
        t = t * (3.1415/180.0)
        cxs.append(x + r*math.cos(t))
        cys.append(y + r*math.sin(t))

    ax.plot(cxs, cys, 'g')

    plt.tight_layout()
    plt.show()

# test data

# Papers solution: x = 159.0200, y = 158.3810, r = 37.459
test_set_1 = [
    Pt(209.4379, 168.5169), Pt(185.5269, 184.8491), Pt(177.0352, 216.7537), Pt(149.9957, 207.8210),
    Pt(137.2580, 211.1708), Pt(193.1959, 105.5558), Pt(104.6318, 187.0523), Pt(196.4768, 139.2621),
    Pt(121.8388, 162.9339), Pt(99.49210, 129.4361), Pt(124.5204, 112.2405), Pt(154.6885, 121.1733)]

# Papers solution: x = 40.0016, y = 50.0003, r = 29.9865
test_set_2 = [
    Pt(70.0150, 50.0000), Pt(68.7900, 58.4734), Pt(65.4060, 65.9372), Pt(59.5675, 72.7493),
    Pt(51.3791, 77.7452), Pt(44.7944, 79.6013), Pt(40.8903, 79.9958), Pt(32.0312, 78.9306),
    Pt(27.2296, 77.1385), Pt(20.3993, 72.7076), Pt(16.1556, 68.2304), Pt(12.7184, 62.4905),
    Pt(10.6380, 56.0806), Pt(10.0183, 49.2149), Pt(11.4275, 40.8264), Pt(14.1050, 34.8682),
    Pt(18.8168, 28.7427), Pt(24.6321, 24.2200), Pt(31.6833, 21.1862), Pt(39.1626, 20.0207),
    Pt(45.5204, 20.5021), Pt(55.3996, 24.2692), Pt(62.3561, 30.0114), Pt(67.3540, 37.6492),
    Pt(69.6190, 45.2028)]

# Papers solution: x = -0.0392082, y = 0.0, r = 88.0392, dfts = 23.92158 
test_set_3 = [
    Pt(0.000, 100.000), Pt(10.143, 102.985), Pt(20.810, 104.617), Pt(31.721, 104.571),
    Pt(42.511, 102.631), Pt(52.769, 98.7240), Pt(62.096, 92.9330), Pt(70.153, 85.4820),
    Pt(76.711, 76.7110), Pt(81.674, 67.0280), Pt(85.093, 56.8580), Pt(87.155, 46.5850),
    Pt(88.145, 36.5110), Pt(88.409, 26.8190), Pt(88.293, 17.5620), Pt(88.090, 8.67600),
    Pt(88.000, 0.00000), Pt(88.090, -8.67600), Pt(88.293, -17.5620), Pt(88.409, -26.8190),
    Pt(88.145, -36.5110), Pt(87.155, -46.5850), Pt(85.094, -56.8580), Pt(81.674, -67.0280),
    Pt(76.711, -76.7110), Pt(70.153, -85.4820), Pt(62.096, -92.9330), Pt(52.769, -98.7240),
    Pt(42.511, -102.631), Pt(31.721, -104.571), Pt(20.810, -104.617), Pt(10.143, -102.985),
    Pt(0.000, -100.0000), Pt(-9.460, -96.052), Pt(-18.208, -91.540), Pt(-26.336, -86.817),
    Pt(-34.026, -82.145), Pt(-41.510, -77.660), Pt(-49.018, -73.361), Pt(-56.726, -69.120),
    Pt(-64.711, -64.711), Pt(-72.928, -59.851), Pt(-81.200, -54.256), Pt(-89.229, -47.694),
    Pt(-96.631, -40.026), Pt(-102.979, -31.238), Pt(-107.864, -21.456), Pt(-110.946, -10.927),
    Pt(-112.000, -0.000), Pt(-110.946, 10.927), Pt(-107.864, 21.456), Pt(-102.979, 31.238),
    Pt(-96.631, 40.026), Pt(-89.229, 47.694), Pt(-81.200, 54.256), Pt(-72.928, 59.851),
    Pt(-64.711, 64.711), Pt(-56.726, 69.120), Pt(-49.018, 73.361), Pt(-41.510, 77.660),
    Pt(-34.026, 82.145), Pt(-26.336, 86.817), Pt(-18.208, 91.540), Pt(-9.460, 96.052)]

if __name__ == "__main__":
    mic = find_mic(test_set_3)

    print("MIC Control Points:")
    for m in mic:
        print(m)

    print("MIC Center and Radius:")
    print(tri_to_circ3(*mic))

    plot(test_set_3, mic)
