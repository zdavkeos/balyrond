// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

// Maximum Inscribed Circle (MIC) calculations
// using delauney triangulation,
// voronoi diagram and convex hull
// 

#include "mic2.h"

#include <array>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include <unordered_set>

#define JC_VORONOI_IMPLEMENTATION
#define JCV_REAL_TYPE double
#define JCV_ATAN2 atan2
#define JCV_SQRT sqrt
#define JCV_FLT_MAX DBL_MAX
#define JCV_PI 3.141592653589793115997963468544185161590576171875
#include "jc_voronoi.h"

Tri
threeClosestPoints(std::vector<std::reference_wrapper<Pt>> pts, Pt p)
{
    Pt tp1, tp2, tp3;

    auto distCmp = [=](const Pt& p1, const Pt& p2) {
        return len(p, p1) > len(p, p2);
    };

    // erase p
    for (auto it = pts.begin(); it != pts.end(); it++) {
        if (*it == p)
            pts.erase(it);
    }

    std::make_heap(pts.begin(), pts.end(), distCmp);
                    
    tp1 = pts[0];
    std::pop_heap(pts.begin(), pts.end(), distCmp);
    
    tp2 = pts[0];
    std::pop_heap(pts.begin(), pts.end() - 1, distCmp); // pop_heap is weird

    tp3 = pts[0];

    return Tri(tp1, tp2, tp3);
}

void calculateMIC2(std::vector<Pt>& pts, std::shared_ptr<MIC> out, std::shared_ptr<Circ> boundingCircle)
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
        out->center_x = pts[1].x - pts[0].x;
        out->center_y = pts[1].y - pts[0].y;
        out->radius = len(pts[0], pts[1]);
        out->dfts = 0.0;
    }

    if (pts.size() == 3) {
        Tri t(pts[0], pts[1], pts[2]);
        auto c = t.toCircle();

        out->center_x = c.c.x;
        out->center_y = c.c.y;
        out->radius = c.r;
        out->dfts = 0.0;
    }

    // Voronoi calculations
    jcv_point* points = 0;
    jcv_diagram diagram;

    points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)pts.size());
    for (int i = 0; i < pts.size(); i++) {
        points[i].x = pts[i].x;
        points[i].y = pts[i].y;
    }

    memset(&diagram, 0, sizeof(jcv_diagram));

    // generate the Delauney and Voronoi diagrams
    jcv_diagram_generate(pts.size(), (const jcv_point*)points, NULL, NULL, &diagram);

    // Compute the convex hull
    std::vector<Pt> hull;
    quickHull(pts, hull);

    // Find all the voronoi vertices inside the hull (and optional bounding circle)
    std::unordered_set<Pt> seen_points;
    std::vector<Pt> candidate_centers;
    const jcv_edge* edges = jcv_diagram_get_edges(&diagram);
    while (edges)
    {
        Pt p1(edges->pos[0].x, edges->pos[0].y);
        Pt p2(edges->pos[1].x, edges->pos[1].y);
        std::cout << "Vor vertex: " << p1 << "\n";
        std::cout << "Vor vertex: " << p2 << "\n";
        
        if (seen_points.find(p1) == seen_points.end()) {
            seen_points.insert(p1);
            if (inPolygon(hull, p1) &&
                (boundingCircle ? (len(boundingCircle->c, p1) < boundingCircle->r) : true)) {
                candidate_centers.push_back(p1);
            }
        }

        if (seen_points.find(p2) == seen_points.end()) {
            seen_points.insert(p2);
            if (inPolygon(hull, p2) &&
                (boundingCircle ? (len(boundingCircle->c, p2) < boundingCircle->r) : true)) {
                candidate_centers.push_back(p2);
            }
        }

        edges = jcv_diagram_get_next_edge(edges);
    }

    std::cout << "Candidate centers:\n";
    for (auto p : candidate_centers) {
        std::cout << p << "\n";
    }

    // Where voronoi edges cross the hull are also potential MICs
    // TODO: for(...)

    // Go through all the candidate centers and find the
    // one that forms the biggest circle with the closest 3 points
    Circ biggest(Pt(), std::numeric_limits<double>::min());
    for (auto& p : candidate_centers) {
        std::vector<std::reference_wrapper<Pt>> plist(pts.begin(), pts.end());

        Tri t = threeClosestPoints(plist, p);
        Circ c = t.toCircle();

        std::cout << "centers match? " << p << " >< " << c.c << "\n";
        std::cout << "Tri: " << t.p1 << " " << t.p2 << " " << t.p3 << "\n";

        if (c.r > biggest.r) {
            biggest = c;
        }
    }

    out->center_x = biggest.c.x;
    out->center_y = biggest.c.y;
    out->radius = biggest.r;

    jcv_diagram_free( &diagram );
}

void calculateMIC2(std::vector<std::tuple<double, double>>& pts, std::shared_ptr<MIC> out, std::shared_ptr<Circ> boundingCircle)
{
    std::vector<Pt> vp;

    for (auto& p : pts) {
        vp.emplace_back(p);
    }
    calculateMIC2(vp, out, boundingCircle);
}
