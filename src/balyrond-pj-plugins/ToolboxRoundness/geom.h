// Generic geometry stuff

#pragma once

#include <vector>
#include <cmath>
#include <ostream>

// Points

class Pt
{
public:

    Pt (double x, double y) :
        x(x), y(y) {}

    Pt (std::tuple<double, double> p) :
        x(std::get<0>(p)), y(std::get<1>(p)) {}

    Pt () {}

    ~Pt() = default;

    Pt operator+(Pt& other);

    Pt operator-(Pt& other);

    Pt operator*(double d);

    Pt operator/(double d);

    void operator=(Pt& other);

    double x;
    double y;
};

double sqr(Pt& pt);

double abs(Pt& p);

bool operator==(Pt& pt1, Pt& pt2);

std::ostream& operator<<(std::ostream& os, Pt& p);

double cross(Pt v, Pt w);

double orient(Pt a, Pt b, Pt c);

// true if P at least as high as A (blue part)
bool above(Pt a, Pt p);

// check if [PQ] crosses ray from A
bool crossesRay(Pt a, Pt p, Pt q);

double dot(Pt v, Pt w);

bool inDisk(Pt a, Pt b, Pt p);

bool onSegment(Pt a, Pt b, Pt p);

bool inPolygon(std::vector<Pt>& poly, Pt& a, bool strict = true);

// The z-value of the cross product of segments 
// (a, b) and (a, c). Positive means c is ccw
// from (a, b), negative cw. Zero means its collinear.
float ccw(const Pt& a, const Pt& b, const Pt& c);

// Returns true if a is lexicographically before b.
bool isLeftOf(const Pt& a, const Pt& b);

// The length of segment (a, b).
float len(const Pt& a, const Pt& b);

// The unsigned distance of p from segment (a, b).
float dist(const Pt& a, const Pt& b, const Pt& p);

// Circles

class Circ
{
    public:

    Circ(Pt center, double radius) :
        c(center), r(radius) {}

    Circ() {}

    ~Circ() = default;

    void operator=(Circ& other);

    Pt c;
    double r;
};

// Triangles

class Tri
{
    public:

    Tri(Pt pa, Pt pb, Pt pc) :
        p1(pa), p2(pb), p3(pc) {}

    Tri() {}

    Pt p1;
    Pt p2;
    Pt p3;

    double
    area();

    Circ
    toCircle();
};

// Convex Hull

void quickHull(const std::vector<Pt>& v, std::vector<Pt>& hull);
