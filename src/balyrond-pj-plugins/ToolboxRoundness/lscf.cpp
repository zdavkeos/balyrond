// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

#include "lscf.h"


#include <Eigen/Dense>

void
leastSquaresCircleFit(std::vector<std::tuple<double, double>>& data, std::shared_ptr<LSCF> lscf)
{
   // https://eigen.tuxfamily.org/dox/group__LeastSquares.html
   // https://lucidar.me/en/mathematics/least-squares-fitting-of-circle/

    Eigen::MatrixXd A = Eigen::MatrixXd::Random(data.size(), 3);
    Eigen::VectorXd b = Eigen::VectorXd::Random(data.size());

    int row = 0;
    for (const auto& d : data)
    {
      double x = std::get<0>(d);
      double y = std::get<1>(d);

      A(row, 0) = x;
      A(row, 1) = y;
      A(row, 2) = 1.0;
      b(row) = x*x + y*y;
      row++;
    }

    Eigen::Vector3d s = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);

    double xc = s(0) / 2.0;
    double yc = s(1) / 2.0;
    double r = ::sqrt(4*s(2) + s(0)*s(0) + s(1)*s(1)) / 2.0;

    // std::cout << xc << " " << yc << " " << r << "\n";

    lscf->center_x = xc;
    lscf->center_y = yc;
    lscf->radius = r;
    // TODO lscf->
}
