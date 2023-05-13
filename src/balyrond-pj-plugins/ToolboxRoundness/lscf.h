// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

#pragma once

#include <tuple>
#include <memory>

typedef struct
{
    double center_x;
    double center_y;
    double radius;
    double dfts; // Deviation from true circle
} LSCF;

void leastSquaresCircleFit(std::vector<std::tuple<double, double>>& data, std::shared_ptr<LSCF> lscf);
