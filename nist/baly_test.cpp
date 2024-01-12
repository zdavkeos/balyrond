// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

// Test our algorithms against NIST's data

// clang++ --std=c++17 -g -o baly_test baly_test.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/lscf.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/geom.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/mcc.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/mic2.cpp -I../src/balyrond-pj-plugins/ToolboxRoundness/eigen-3.4.0/

#include <tuple>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../src/balyrond-pj-plugins/ToolboxRoundness/geom.h"
#include "../src/balyrond-pj-plugins/ToolboxRoundness/lscf.h"
#include "../src/balyrond-pj-plugins/ToolboxRoundness/mic2.h"
#include "../src/balyrond-pj-plugins/ToolboxRoundness/mcc.h"

struct pt3d
{
	pt3d(double a, double b, double c) :
		x(a), y(b), z(c) {}
	double x;
	double y;
	double z;
};

struct Sol
{
	double x;
	double y;
	double z;
	double nx;
	double ny;
	double nz;
	double diam;
};

void getData(int t, std::vector<std::tuple<double, double>> &data)
{
	data.clear();

	auto f = std::ifstream("Circle2d/Cir2d" + std::to_string(t) + ".ds");

	char tmp[64];
	f.getline(tmp, 64);
	int count = std::atoi(tmp);

	std::vector<pt3d> pts;
	for (int i = 0; i < count; i++) {
		double x, y, z;

		f >> x;
		f >> y;
		f >> z;
			
		pts.emplace_back(x, y, z);
	}

	for (const auto& p : pts) {
		if (pts[0].x == pts[1].x) {
			data.emplace_back(p.y, p.z);
		} else if (pts[0].y == pts[1].y) {
			data.emplace_back(p.x, p.z);
		} else {
			data.emplace_back(p.x, p.y);
		}
	}
}

void
getSolution(int t, Sol& sol)
{
	auto f = std::ifstream("Circle2d/Cir2d" + std::to_string(t) + ".fit");

	f >> sol.x;
	f >> sol.y;
	f >> sol.z;
	f >> sol.nx;
	f >> sol.ny;
	f >> sol.nz;
	f >> sol.diam;
}

int main(int argc, char** argv)
{
	constexpr double epsilon = 10e-2;

	for (int i = 1; i <= 30; i++) {
		std::cout << "\nTest: " << i << "\n";
		
		std::vector<std::tuple<double, double>> pts;
		Sol sol;

		getData(i, pts); // raw input points
		getSolution(i, sol); // NIST solution

		std::shared_ptr<LSCF> lscf = std::make_shared<LSCF>();
		leastSquaresCircleFit(pts, lscf); // balyrond LSCF

		auto mic = std::make_shared<MIC>();
		// center of MIC should be within the ls circle
		auto boundry = std::make_shared<Circ>(Pt(lscf->center_x, lscf->center_y), lscf->radius);
		calculateMIC2(pts, mic, boundry);

		auto mcc = std::make_shared<MCC>();
		calculateMCC(pts, mcc);

		std::cout << "MIC: " << mic->center_x << " " << mic->center_y << " " << mic->radius << "\n";
		std::cout << "MCC: " << mcc->center_x << " " << mcc->center_y << " " << mcc->radius << "\n";
		std::cout << "LSCF: " << lscf->center_x << " " << lscf->center_y << " " << lscf->radius << "\n";

		continue;
		
		if ((int)sol.nx == 1) {
			if (abs(lscf->center_x - sol.y) > epsilon ||
				abs(lscf->center_y - sol.z) > epsilon ||
				abs(lscf->radius - sol.diam / 2.0) > epsilon) {
				std::cout << "Error in test " << i << "\n" << std::setprecision(9);
				std::cout << lscf->center_x << "\n" << lscf->center_y << "\n" << lscf->radius << "\n";
				std::cout << sol.y << "\n" << sol.z << "\n" << sol.diam / 2.0 << "\n\n";
			}
		} else if ((int)sol.ny == 1) {
			if (abs(lscf->center_x - sol.x) > epsilon ||
				abs(lscf->center_y - sol.z) > epsilon ||
				abs(lscf->radius - sol.diam / 2.0) > epsilon) {
				std::cout << "Error in test " << i << "\n" << std::setprecision(9);
				std::cout << lscf->center_x << "\n" << lscf->center_y << "\n" << lscf->radius << "\n";
				std::cout << sol.x << "\n" << sol.z << "\n" << sol.diam / 2.0 << "\n\n";
			}
		} else if ((int)sol.nz == 1) {
			if (abs(lscf->center_x - sol.x) > epsilon ||
				abs(lscf->center_y - sol.y) > epsilon ||
				abs(lscf->radius - sol.diam / 2.0) > epsilon) {
				std::cout << "Error in test " << i << "\n" << std::setprecision(9);
				std::cout << lscf->center_x << "\n" << lscf->center_y << "\n" << lscf->radius << "\n";
				std::cout << sol.x << "\n" << sol.y << "\n" << sol.diam / 2.0 << "\n\n";
			}
		}
	}

	return 0;
}
