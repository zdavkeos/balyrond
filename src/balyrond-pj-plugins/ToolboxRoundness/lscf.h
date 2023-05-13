// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

#pragma once

#include <tuple>

std::tuple<double, double, double>
leastSquaresCircleFit(std::vector<std::tuple<double, double>>& data);
