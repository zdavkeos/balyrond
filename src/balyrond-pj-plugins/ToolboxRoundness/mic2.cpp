// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

// Maximum Inscribed Circle (MIC) calculations
// using delauney triangulation

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

struct Edge
{
    double a;
    double b;

    bool operator==(const Edge& other) {
        return (jcv_real_eq(a, other.a) && jcv_real_eq(b, other.b)) ||
               (jcv_real_eq(b, other.a) && jcv_real_eq(a, other.b));
    }

    bool conjoins(const Edge& other) {
        return jcv_real_eq(a, other.a) ||
            jvc_real_eq(a, other.b) ||
            jvc_real_eq(b, other.a) ||
            jvc_real_eq(b, other.b);
    }
};

typedef std::tuple<Pt, Pt, Pt> Tri;
typedef std::tuple<Pt, double> Circ;

double
triArea(Tri& t)
{
    double x1 = std::get<0>(std::get<0>(t));
    double x2 = std::get<0>(std::get<1>(t));
    double x3 = std::get<0>(std::get<2>(t));
    double y1 = std::get<1>(std::get<0>(t));
    double y2 = std::get<1>(std::get<1>(t));
    double y3 = std::get<1>(std::get<2>(t));

    // return ::abs((x1*y2+x2*y3+x3*y1-y1*x2-y2*x3-y3*x1) / 2.0));
    // https://cp-algorithms.com/geometry/oriented-triangle-area.html
    return ::abs((x2-x1)*(y3-y2) - (x3-x2)*(y2-y1));
}

void
findTriangles(std::set<Edge>& edges, std::vector<Tri>& tris)
{
    for (auto& edge : edges) {
        for (auto& e : edges) {
            if (edge == e)
                continue;
            if (
        }
    }
}

void calculateMIC2(std::vector<Pt>& pts, std::shared_ptr<MIC> out)
{
    jcv_point* points = 0;
    jcv_diagram diagram;

    points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)pts.count());
    for (int i = 0; i < pts.count(); i++) {
        points[i].x = std::get<0>(pts[i]);
        points[i].y = std::get<1>(pts[i]);
    }

    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(pts.count(), (const jcv_point*)points, NULL, NULL, &diagram);

    std::set<Edge> edges;
    jcv_delauney_iter delauney;
    jcv_delauney_begin( &diagram, &delauney );
    jcv_delauney_edge delauney_edge;
    while (jcv_delauney_next( &delauney, &delauney_edge ))
    {
        edges.emplace(delauney_edge.pos[0], delauney_edge.pos[1]);
    }

    jcv_diagram_free( &diagram );
}
