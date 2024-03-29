// Test mic2 (voronoi) code

// clang++ --std=c++17 -g -o mic2 mic2_test.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/mic2.cpp ../src/balyrond-pj-plugins/ToolboxRoundness/geom.cpp

#include "../src/balyrond-pj-plugins/ToolboxRoundness/mic2.h"
#include "../src/balyrond-pj-plugins/ToolboxRoundness/geom.h"

//#include "matplotlibcpp.h"

#include <iostream>

int
main(int argc, char** argv)
{
    // make sure we didnt break triToCirc...
    Tri t(Pt(-1.0, 0.0), Pt(1.0, 0.0), Pt(0.0, 1.0));
    std::cout << "Center should be 0,0: " << t.toCircle().c << "\n";

    Tri t2(Pt(0.0, 5.0), Pt(5.0, 10.0), Pt(10.0, 5.0));
    std::cout << "Center should be 5,5: " << t2.toCircle().c << "\n";

    // check we didn't break threeClosest...
    std::vector<Pt> pts3 = {Pt(1,1), Pt(3,3), Pt(-1,-1), Pt(-1,1)};
    std::vector<std::reference_wrapper<Pt>> ptsr(pts3.begin(), pts3.end());
    auto tr = threeClosestPoints(ptsr, Pt(0,0));
    std::cout << "Tri pts: " << tr.p1 << " " << tr.p2 << " " << tr.p3 << "\n";
    std::cout << "Circ cent: " << tr.toCircle().c << "\n";

    //  Papers solution: x = 159.0200, y = 158.3810, r = 37.459
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