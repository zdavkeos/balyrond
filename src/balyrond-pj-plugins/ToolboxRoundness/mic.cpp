
// Maximum Inscribed Circle (MIC) calculations
// Ported from the Python prototype

#include "mic.h"

typedef std::tuple<double, double, double> Tri;

// determinant of a 3x3 matrix
double det(double a, double b, double c, double d, double e, double f, double g, double h, double i)
{
    return a*e*i + b*f*g +c*d*h - c*e*g - b*d*i - a*f*h;
}

// Triangle to Circle
// Using minor matrix determinants
// https://web.archive.org/web/20161011113446/http://www.abecedarical.com/zenosamples/zs_circle3pts.html
Tri
tri_to_circ3(Pt& p1, Pt& p2, Pt& p3)
{
    double x1 = std::get<0>(p1);
    double x2 = std::get<0>(p2);
    double x3 = std::get<0>(p3);
    double y1 = std::get<1>(p1);
    double y2 = std::get<1>(p2);
    double y3 = std::get<1>(p3);

    double m11 = det(x1, y1, 1, x2, y2, 1, x3, y3, 1);
    double m12 = det((x1*x1)+(y1*y1), y1, 1, (x2*x2)+(y2*y2), y2, 1, (x3*x3)+(y3*y3), y3, 1);
    double m13 = det(x1*x1+y1*y1, x1, 1, x2*x2+y2*y2, x2, 1, x3*x3+y3*y3, x3, 1);
    double m14 = det(x1*x1+y1*y1, x1, y1, x2*x2+y2*y2, x2, y2, x3*x3+y3*y3, x3, y3);
    
    double x = .5 * (m12 / m11);
    double y = -.5 * (m13 / m11);
    double r = ::sqrt(::abs(x*x + y*y + (m14 / m11)));

    return {x, y, r};
}

// euclidian distance
double
dist(Pt& p1, Pt& p2)
{
    double p1x = std::get<0>(p1);
    double p2x = std::get<0>(p2);
    double p1y = std::get<1>(p1);
    double p2y = std::get<1>(p2);
    return ::sqrt((p1x-p2x)*(p1x-p2x) + (p1y-p2y)*(p1y-p2y));
}

// determine if a point is inside or on a circle
bool
in_circ(Pt& circ_center, double r, Pt& pt)
{
    double d = dist(circ_center, pt);
    return d < (r - .00001); // fudge due to numerical accuracy
}

// determine if all points are outside or on a circle
bool
is_mic(Pt& circ_center, double r, std::vector<Pt&> pts)
{
    for (auto& pt : pts) {
        if(in_circ(circ_center, r, pt)) {
            return false;
        }
    }
    return true;
}

// Pick the 3 control points based on largest radius
decide_mic(pts):
    b = -1
    bs = None
    for i in itertools.combinations(pts, 3):
        x, y, r = tri_to_circ3(*i)
        if r > b and is_mic(x, y, r, pts):
            b = r
            bs = list(i)
    return bs