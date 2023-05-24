// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <memory>

typedef struct
{
    double center_x;
    double center_y;
    double radius;
    double dfts; // Deviation from true circle
} MIC;

typedef std::tuple<double, double> Pt;

void calculateMIC2(std::vector<Pt>& pts, std::shared_ptr<MIC> out);
