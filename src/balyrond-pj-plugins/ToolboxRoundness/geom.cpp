// Generic geometry classes and functions

#include "geom.h"

#include <vector>
#include <cmath>

// Points

Pt Pt::operator+(Pt& other) {
    return {x+other.x, y+other.y};
}

Pt Pt::operator-(Pt& other) {
    return {x-other.x, y-other.y};
}

Pt Pt::operator*(double d) {
    return {x*d, y*d};
}

Pt Pt::operator/(double d) {
    return {x/d, y/d};
}

void Pt::operator=(Pt& other) {
    x = other.x;
    y = other.y;
}

double sqr(Pt& p) {
    return p.x*p.x + p.y*p.y;
}

double abs(Pt& p) {
    return ::sqrt(sqr(p));
}

bool operator==(Pt& p1, Pt& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

std::ostream& operator<<(std::ostream& os, Pt& p) {
    return os << "(" << p.x << "," << p.y << ")";
}

// The length of segment (a, b).
float len(const Pt& a, const Pt& b) {
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

double cross(Pt v, Pt w) {
    return v.x*w.y - v.y*w.x;
}

double orient(Pt a, Pt b, Pt c) {
    return cross(b-a,c-a);
}

// true if P at least as high as A (blue part)
bool above(Pt a, Pt p) {
    return p.y >= a.y;
}
// check if [PQ] crosses ray from A
bool crossesRay(Pt a, Pt p, Pt q) {
    return (above(a,q) - above(a,p)) * orient(a,p,q) > 0;
}

double dot(Pt v, Pt w) {
    return v.x*w.x + v.y*w.y;
}

bool inDisk(Pt a, Pt b, Pt p) {
    return dot(a-p, b-p) <= 0;
}

bool onSegment(Pt a, Pt b, Pt p) {
    return orient(a,b,p) == 0 && inDisk(a,b,p);
}

bool inPolygon(std::vector<Pt>& poly, Pt& a, bool strict)
{
    int numCrossings = 0;
    for (int i = 0, n = poly.size(); i < n; i++) {
        if (onSegment(poly[i], poly[(i+1)%n], a))
            return !strict;
        numCrossings += crossesRay(a, poly[i], poly[(i+1)%n]);
    }

    return numCrossings & 1; // inside if odd number of crossings
}

// Circles

void Circ::operator=(Circ& other) {
    c = other.c;
    r = other.r;
}

// Triangles

double
Tri::area()
{
    double x1 = p1.x;
    double x2 = p2.x;
    double x3 = p3.x;
    double y1 = p1.y;
    double y2 = p2.y;
    double y3 = p3.y;

    // https://cp-algorithms.com/geometry/oriented-triangle-area.html
    return ::abs((x2-x1)*(y3-y2) - (x3-x2)*(y2-y1));
}

// determinant of a 3x3 matrix
double det(double a, double b, double c, double d, double e, double f, double g, double h, double i)
{
    return a*e*i + b*f*g +c*d*h - c*e*g - b*d*i - a*f*h;
}

Circ
Tri::toCircle()
{
    // https://web.archive.org/web/20161011113446/http://www.abecedarical.com/zenosamples/zs_circle3pts.html
    double x1 = p1.x;
    double x2 = p2.x;
    double x3 = p3.x;
    double y1 = p1.y;
    double y2 = p2.y;
    double y3 = p3.y;

    double m11 = det(x1, y1, 1, x2, y2, 1, x3, y3, 1);
    double m12 = det((x1*x1)+(y1*y1), y1, 1, (x2*x2)+(y2*y2), y2, 1, (x3*x3)+(y3*y3), y3, 1);
    double m13 = det(x1*x1+y1*y1, x1, 1, x2*x2+y2*y2, x2, 1, x3*x3+y3*y3, x3, 1);
    double m14 = det(x1*x1+y1*y1, x1, y1, x2*x2+y2*y2, x2, y2, x3*x3+y3*y3, x3, y3);
    
    double x = .5 * (m12 / m11);
    double y = -.5 * (m13 / m11);
    double r = ::sqrt(::abs(x*x + y*y + (m14 / m11)));

    return {{x, y}, r};
}

// Convex Hull (and stuff)
// Adapted from:
// https://github.com/MiguelVieira/ConvexHull2D

// The z-value of the cross product of segments 
// (a, b) and (a, c). Positive means c is ccw
// from (a, b), negative cw. Zero means its collinear.
float ccw(const Pt& a, const Pt& b, const Pt& c) {
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Returns true if a is lexicographically before b.
bool isLeftOf(const Pt& a, const Pt& b) {
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

// The unsigned distance of p from segment (a, b).
float dist(const Pt& a, const Pt& b, const Pt& p) {
	return fabs((b.x - a.x) * (a.y - p.y) - (b.y - a.y) * (a.x - p.x)) / len(a, b);
}

// Returns the index of the farthest Pt from segment (a, b).
size_t getFarthest(const Pt& a, const Pt& b, const std::vector<Pt>& v) {
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

#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

// Recursive call of the quickhull algorithm.
void quickHull(const std::vector<Pt>& v, const Pt& a, const Pt& b, std::vector<Pt>& hull) {
	if (v.empty()) {
		return;
	}

	std::vector<double> xs;
	std::vector<double> ys;
	for (auto p : v) {
		xs.push_back(p.x);
		ys.push_back(p.y);
	}
	plt::plot(xs, ys);
	plt::show();

	Pt f = v[getFarthest(a, b, v)];

	// Collect Pts to the left of segment (a, f)
	std::vector<Pt> left;
	for (auto p : v) {
		if (ccw(a, f, p) > 0) {
			left.push_back(p);
		}
	}
	quickHull(left, a, f, hull);
	
	// Add f to the hull
	hull.push_back(f);

	// Collect Pts to the left of segment (f, b)
	std::vector<Pt> right;
	for (auto p : v) {
		if (ccw(f, b, p) > 0) {
			right.push_back(p);
		}
	}
	quickHull(right, f, b, hull);
}

// QuickHull algorithm. 
// https://en.wikipedia.org/wiki/QuickHull
void quickHull(const std::vector<Pt>& v, std::vector<Pt>& hull) {
	
	// Start with the leftmost and rightmost Pts.
	Pt a = *min_element(v.begin(), v.end(), isLeftOf);
	Pt b = *max_element(v.begin(), v.end(), isLeftOf);

	// Split the Pts on either side of segment (a, b)
	std::vector<Pt> left, right;
	for (auto p : v) {
		ccw(a, b, p) > 0 ? left.push_back(p) : right.push_back(p);
	}
	
	// Be careful to add Pts to the hull
	// in the correct order. Add our leftmost Pt.
	hull.push_back(a);

	// Add hull Pts from the left (top)
	quickHull(left, a, b, hull);

	// Add our rightmost Pt
	hull.push_back(b);

	// Add hull Pts from the right (bottom)
	quickHull(right, b, a, hull);
}
