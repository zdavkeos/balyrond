
#pragma once

#include <vector>

typedef struct mic
{
    double center_x;
    double center_y;
    double radius;
} MIC;

typedef std::tuple<double, double> Pt;

MIC
calculateMIC(std::vector<Pt>);
