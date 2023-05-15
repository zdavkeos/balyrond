// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

// Maximum Inscribed Circle (MIC) calculations
// Ported from the Python prototype

#include "mic.h"

#include <array>
#include <algorithm>

typedef std::tuple<Pt, Pt, Pt> Tri;
typedef std::tuple<Pt, double> Circ;

// determinant of a 3x3 matrix
double det(double a, double b, double c, double d, double e, double f, double g, double h, double i)
{
    return a*e*i + b*f*g +c*d*h - c*e*g - b*d*i - a*f*h;
}

// Triangle to Circle
// Using minor matrix determinants
// https://web.archive.org/web/20161011113446/http://www.abecedarical.com/zenosamples/zs_circle3pts.html
Circ
tri_to_circ3(Tri& t)
{
    double x1 = std::get<0>(std::get<0>(t));
    double x2 = std::get<0>(std::get<1>(t));
    double x3 = std::get<0>(std::get<2>(t));
    double y1 = std::get<1>(std::get<0>(t));
    double y2 = std::get<1>(std::get<1>(t));
    double y3 = std::get<1>(std::get<2>(t));

    double m11 = det(x1, y1, 1, x2, y2, 1, x3, y3, 1);
    double m12 = det((x1*x1)+(y1*y1), y1, 1, (x2*x2)+(y2*y2), y2, 1, (x3*x3)+(y3*y3), y3, 1);
    double m13 = det(x1*x1+y1*y1, x1, 1, x2*x2+y2*y2, x2, 1, x3*x3+y3*y3, x3, 1);
    double m14 = det(x1*x1+y1*y1, x1, y1, x2*x2+y2*y2, x2, y2, x3*x3+y3*y3, x3, y3);
    
    double x = .5 * (m12 / m11);
    double y = -.5 * (m13 / m11);
    double r = ::sqrt(::abs(x*x + y*y + (m14 / m11)));

    return {{x, y}, r};
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
in_circ(Circ& circ, Pt& pt)
{
    double d = dist(std::get<0>(circ), pt);
    return d < (std::get<1>(circ) - .00001); // fudge due to numerical accuracy
}

// determine if all points are outside or on a circle
bool
is_mic(Circ& circ, std::vector<Pt>&& pts)
{
    for (auto& pt : pts) {
        if(in_circ(circ, pt)) {
            return false;
        }
    }
    return true;
}

bool
is_mic(Circ& circ, std::vector<Pt>& pts)
{
    for (auto& pt : pts) {
        if(in_circ(circ, pt)) {
            return false;
        }
    }
    return true;
}

// Pick the 3 control points based on largest radius
Tri
decide_mic(std::array<Pt, 4> pts)
{
    Pt& p1 = pts[0];
    Pt& p2 = pts[1]; 
    Pt& p3 = pts[2];
    Pt& p4 = pts[3];

    double best_r = -1.0;
    Tri best_t;

    // all 4 combinations
    std::vector<Tri> tris = {{p1,p2,p4},
                             {p1,p3,p4},
                             {p2,p3,p4}};
    
    for (auto& t : tris) {
        auto circ = tri_to_circ3(t);
        if (std::get<1>(circ) > best_r && is_mic(circ, {p1, p2, p3, p4})) {
            best_r = std::get<1>(circ);
            best_t = t;
        }
    }

    return best_t;
}

void
calculateMIC(std::vector<Pt>& pts, std::shared_ptr<MIC> out)
{
    // initial points: Step 1
    Pt maxx = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    Pt minx = {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};
    Pt maxy = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    Pt miny = {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};

    std::array<Pt, 4> mic = {maxx, minx, maxy, miny};
    Circ circ;

    while (true)
    {
        // Of the 4 points, determine the 3 control points. Step 2
        auto t = decide_mic(mic);
        circ = tri_to_circ3(t);

        // Step 3
        if (is_mic(circ, pts)) {
            break;
        }

        // Pick point closest to center as next control point. Step 4
        auto pn = std::min_element(pts.begin(), pts.end(),
                                   [&](auto& p1, auto& p2){
                                       return dist(p1, std::get<0>(circ)) < dist(p2, std::get<0>(circ));
                                   }
        );

        mic[3] = *pn;
    }

    // Determine Roundness Error. Step 5.
    double dfts = std::numeric_limits<double>::min();
    auto mic_center = std::get<0>(circ);
    for (auto& pt : pts) {
        double d = dist(mic_center, pt) - std::get<1>(circ);
        if (d > dfts) {
            dfts = d;
        }
    }

    out->center_x = std::get<0>(std::get<0>(circ));
    out->center_y = std::get<1>(std::get<0>(circ));
    out->radius = std::get<1>(circ);
    out->dfts = dfts;
}
