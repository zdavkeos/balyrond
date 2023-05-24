
// Adapted from:
// https://github.com/MiguelVieira/ConvexHull2D

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

using namespace std;

struct point {
	float x;
	float y;

	point(float xIn, float yIn) : x(xIn), y(yIn) { } 
};

// The z-value of the cross product of segments 
// (a, b) and (a, c). Positive means c is ccw
// from (a, b), negative cw. Zero means its collinear.
float ccw(const point& a, const point& b, const point& c) {
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Returns true if a is lexicographically before b.
bool isLeftOf(const point& a, const point& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

// The length of segment (a, b).
float len(const point& a, const point& b) {
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

// The unsigned distance of p from segment (a, b).
float dist(const point& a, const point& b, const point& p) {
	return fabs((b.x - a.x) * (a.y - p.y) - (b.y - a.y) * (a.x - p.x)) / len(a, b);
}

// Returns the index of the farthest point from segment (a, b).
size_t getFarthest(const point& a, const point& b, const vector<point>& v) {
	size_t idxMax = 0;
	float distMax = dist(a, b, v[idxMax]);

	for (size_t i = 1; i < v.size(); ++i) {
		float distCurr = dist(a, b, v[i]);
		if (distCurr > distMax) {
			idxMax = i;
			distMax = distCurr;
		}
	}

	return idxMax;
}

// Recursive call of the quickhull algorithm.
void quickHull(const vector<point>& v, const point& a, const point& b, 
			   vector<point>& hull) {
	if (v.empty()) {
		return;
	}

	point f = v[getFarthest(a, b, v)];

	// Collect points to the left of segment (a, f)
	vector<point> left;
	for (auto p : v) {
		if (ccw(a, f, p) > 0) {
			left.push_back(p);
		}
	}
	quickHull(left, a, f, hull);
	
	// Add f to the hull
	hull.push_back(f);

	// Collect points to the left of segment (f, b)
	vector<point> right;
	for (auto p : v) {
		if (ccw(f, b, p) > 0) {
			right.push_back(p);
		}
	}
	quickHull(right, f, b, hull);
}

// QuickHull algorithm. 
// https://en.wikipedia.org/wiki/QuickHull
void quickHull(const vector<point>& v, vector<point>& hull) {
	
	// Start with the leftmost and rightmost points.
	point a = *min_element(v.begin(), v.end(), isLeftOf);
	point b = *max_element(v.begin(), v.end(), isLeftOf);

	// Split the points on either side of segment (a, b)
	vector<point> left, right;
	for (auto p : v) {
		ccw(a, b, p) > 0 ? left.push_back(p) : right.push_back(p);
	}
	
	// Be careful to add points to the hull
	// in the correct order. Add our leftmost point.
	hull.push_back(a);

	// Add hull points from the left (top)
	quickHull(left, a, b, hull);

	// Add our rightmost point
	hull.push_back(b);

	// Add hull points from the right (bottom)
	quickHull(right, b, a, hull);
}

// Test data
std::vector<point> test_data = {
    point(0.000, 100.000),
    point(10.143, 102.985),
    point(20.810, 104.617),
    point(31.721, 104.571),
    point(42.511, 102.631),
    point(52.769, 98.7240),
    point(62.096, 92.9330),
    point(70.153, 85.4820),
    point(76.711, 76.7110),
    point(81.674, 67.0280),
    point(85.093, 56.8580),
    point(87.155, 46.5850),
    point(88.145, 36.5110),
    point(88.409, 26.8190),
    point(88.293, 17.5620),
    point(88.090, 8.67600),
    point(88.000, 0.00000),
    point(88.090, -8.67600),
    point(88.293, -17.5620),
    point(88.409, -26.8190),
    point(88.145, -36.5110),
    point(87.155, -46.5850),
    point(85.094, -56.8580),
    point(81.674, -67.0280),
    point(76.711, -76.7110),
    point(70.153, -85.4820),
    point(62.096, -92.9330),
    point(52.769, -98.7240),
    point(42.511, -102.631),
    point(31.721, -104.571),
    point(20.810, -104.617),
    point(10.143, -102.985),
    point(0.000, -100.0000),
    point(-9.460, -96.052),
    point(-18.208, -91.540),
    point(-26.336, -86.817),
    point(-34.026, -82.145),
    point(-41.510, -77.660),
    point(-49.018, -73.361),
    point(-56.726, -69.120),
    point(-64.711, -64.711),
    point(-72.928, -59.851),
    point(-81.200, -54.256),
    point(-89.229, -47.694),
    point(-96.631, -40.026),
    point(-102.979, -31.238),
    point(-107.864, -21.456),
    point(-110.946, -10.927),
    point(-112.000, -0.000),
    point(-110.946, 10.927),
    point(-107.864, 21.456),
    point(-102.979, 31.238),
    point(-96.631, 40.026),
    point(-89.229, 47.694),
    point(-81.200, 54.256),
    point(-72.928, 59.851),
    point(-64.711, 64.711),
    point(-56.726, 69.120),
    point(-49.018, 73.361),
    point(-41.510, 77.660),
    point(-34.026, 82.145),
    point(-26.336, 86.817),
    point(-18.208, 91.540),
    point(-9.460, 96.052)};

int
main(int argc, char** argv)
{
    vector<point> hull;

    quickHull(test_data, hull);

    vector<double> xs;
    vector<double> ys;

    for (const auto& p : test_data)
    {
        xs.push_back(p.x);
        ys.push_back(p.y);
    }

    plt::plot(xs, ys);

    vector<double> hxs;
    vector<double> hys;

    for (const auto& p : hull)
    {
        hxs.push_back(p.x);
        hys.push_back(p.y);
    }

    plt::plot(hxs, hys);

    plt::show();

    return 0;
}

