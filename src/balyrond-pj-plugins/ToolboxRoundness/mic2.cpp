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
                        if (len(p, p1) < len(p, p2)) {
                            return true;
                        } else {
                            return false;
                        }
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
    std::pop_heap(pts.begin(), pts.end(), distCmp);

    tp3 = pts[0];

    return Tri(tp1, tp2, tp3);
}

void calculateMIC2(std::vector<Pt>& pts, std::shared_ptr<MIC> out)
{
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

    // Find all the voronoi sites inside the hull
    std::vector<Pt> candidate_centers;
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for (int i = 0; i < diagram.numsites; i++)
    {
        const jcv_site* site = &sites[i];
        Pt p(site->p.x, site->p.y);
        if (inPolygon(hull, p)) {
            candidate_centers.push_back(p);
        }
    }

    // Where voronoi edges cross the hull are also potential MICs
    // TODO: for(...)

    // Go through all the candidate centers and find the
    // one that forms the biggest circle with the closest 3 points
    Circ biggest(Pt(), std::numeric_limits<double>::min());
    for (auto& p : candidate_centers) {
        Pt tp1, tp2, tp3;
        std::vector<std::reference_wrapper<Pt>> plist(pts.begin(), pts.end());

        Tri t = threeClosestPoints(plist, p);
        Circ c = t.toCircle();

        if (c.r > biggest.r) {
            biggest = c;
        }
    }

    jcv_diagram_free( &diagram );
}

void calculateMIC2(std::vector<std::tuple<double, double>>& pts, std::shared_ptr<MIC> out)
{
    std::vector<Pt> vp;

    for (auto& p : pts) {
        vp.emplace_back(p);
    }
    calculateMIC2(vp, out);
}