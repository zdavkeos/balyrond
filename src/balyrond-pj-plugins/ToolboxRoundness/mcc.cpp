// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

#include "mcc.h"

#include <iostream>
#include <random>

// Maximum Circumscribed Circle

// Based on MiniDiscWithPoints from "Computational Geometry, Berg et al"
// This could also be done with voronoi data

struct Disc
{
    Disc() {}

    bool pointIsIn(Pt& p)
    {
        if (pts.size() < 2) {
            return false;
        } else if (pts.size() < 3) {
            Pt c((pts[0].x + pts[1].x) / 2.0,
                 (pts[0].y + pts[1].y) / 2.0);
            double r = len(pts[0], pts[1]) / 2.0;
            
            return len(c, p) < r;
        } else {
            auto c = toCircle();
            return len(c.c, p) < c.r;
        }
    }

    Circ toCircle() {
        Tri t(pts[0], pts[1], pts[2]);
        return t.toCircle();
    }

    std::vector<Pt> pts;
};

std::ostream &operator<<(std::ostream &out, Disc& d) {
    for (const auto& p : d.pts) {
        out << p << " ";
    }
    return out;
}

Disc Welzl(std::vector<Pt> p, Disc r)
{
    std::cout << "R: " << r << "\n";
    std::cout << "P: ";
    for (const auto& a : p) {
        std::cout << a << " ";
    }
    std::cout << "\n\n";

    if (p.size() == 0 || r.pts.size() == 3) {
        return r;
    }

    Pt pp = p.back();
    p.pop_back();

    Disc d = Welzl(p, r);

    if (d.pointIsIn(pp)) {
        return d;
    }

    d.pts.push_back(pp);
    return Welzl(p, d);
}

void calculateMCC(std::vector<Pt>& pts, std::shared_ptr<MCC> out)
{
    // Cover the special cases first
    if (pts.size() < 2) {
        std::cout << "Too few points to calculate MIC!\n";

        out->center_x = 0.0;
        out->center_y = 0.0;
        out->radius = 0.0;

        return;
    }

    if (pts.size() == 2) {
        out->center_x = (pts[1].x + pts[0].x) / 2.0;
        out->center_y = (pts[1].y + pts[0].y) / 2.0;
        out->radius = len(pts[0], pts[1]) / 2.0;
        out->dfts = 0.0;

         return;
    }

    if (pts.size() == 3) {
        Tri t(pts[0], pts[1], pts[2]);
        auto c = t.toCircle();

        out->center_x = c.c.x;
        out->center_y = c.c.y;
        out->radius = c.r;
        out->dfts = 0.0;

        return;
    }

    // Create a copy of the input points
    std::vector<Pt> spts(pts.begin(), pts.end());
    Disc mcc;

    // Input points should be randomized
    std::shuffle(spts.begin(), spts.end(), std::default_random_engine());

    Welzl(spts, mcc);

    if (mcc.pts.size() == 3) {
        auto c = mcc.toCircle();

        out->center_x = c.c.x;
        out->center_y = c.c.y;
        out->radius = c.r;
    } else {
        std::cout << "MCC calculation failed.\n";
    }
}

void calculateMCC(std::vector<std::tuple<double, double>>& pts, std::shared_ptr<MCC> out)
{
    std::vector<Pt> vp;

    for (auto& p : pts) {
        vp.emplace_back(p);
    }
    calculateMCC(vp, out);
}

