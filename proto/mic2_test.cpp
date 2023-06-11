// Test mic2 (voronoi) code

// clang++ --std=c++17 -o mic2 mic2_test.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/mic2.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/geom.cpp

#include "../src/balyrond-pj-plugins/ToolboxRoundness/mic2.h"
#include "../src/balyrond-pj-plugins/ToolboxRoundness/geom.h"

//#include "matplotlibcpp.h"

#include <iostream>

int
main(int argc, char** argv)
{
    std::vector<Pt> pts {Pt(209.4379, 168.5169), Pt(185.5269, 184.8491), Pt(177.0352, 216.7537), Pt(149.9957, 207.8210),
                        Pt(137.2580, 211.1708), Pt(193.1959, 105.5558), Pt(104.6318, 187.0523), Pt(196.4768, 139.2621),
                        Pt(121.8388, 162.9339), Pt(99.49210, 129.4361), Pt(124.5204, 112.2405), Pt(154.6885, 121.1733)};
    //std::vector<std::tuple<double, double>> pts {{0,0}, {1,1}, {0,1}, {1,0}};
    auto mic = std::make_shared<MIC>();

    calculateMIC2(pts, mic);

    std::cout << "MIC: " << mic->center_x << "," << mic->center_y << "  " << mic->radius << "\n";

    // Plot

    return 0;
}