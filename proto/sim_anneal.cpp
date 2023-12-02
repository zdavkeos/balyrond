
// Copyright 2023 Zach Davis
// SPDX-License-Identifier: MIT

// Requires Ensmallen and Armadillo libraries
// > brew install ensmallen

#include <ensmallen.hpp>

#include <vector>


class MinDisc
{
public:

    MinDisc()
    {
        // do least-squares fit and translate the data to be centered at the lsq center
        arma::mat A(lobe.size(), 3);
        arma::vec b(lobe.size());

        int row = 0;
        for (const auto& d : lobe)
        {
            double x = d[0];
            double y = d[1];

            A(row, 0) = x;
            A(row, 1) = y;
            A(row, 2) = 1.0;
            b(row) = x*x + y*y;
            row++;
        }

        arma::vec s = arma::solve(A, b);

        xc = s(0) / 2.0;
        yc = s(1) / 2.0;
        double r = ::sqrt(4*s(2) + s(0)*s(0) + s(1)*s(1)) / 2.0;

        for (auto& p : lobe) {
            tlobe.push_back(arma::mat({p[0] - xc, p[1] - yc}));
        }
        
    }
    
    double Evaluate(const arma::mat& x)
    {
        double _mx = std::numeric_limits<double>::min();
        double _mn = std::numeric_limits<double>::max();
    
        for (const auto& p : tlobe) {
            const arma::mat diff = p - x;
            double d = std::sqrt(diff[0]*diff[0] + diff[1]*diff[1]);

            if (d > _mx)
                _mx = d;
            if (d < _mn)
                _mn = d;
        }

        mx = _mx;
        mn = _mn;
        return _mn;
    }

    /*
    std::vector<arma::mat> lobe = {
        arma::mat("70.0150 50.0000"), arma::mat("68.7900 58.4734"), arma::mat("65.4060 65.9372"), arma::mat("59.5675 72.7493"),
        arma::mat("51.3791 77.7452"), arma::mat("44.7944 79.6013"), arma::mat("40.8903 79.9958"), arma::mat("32.0312 78.9306"),
        arma::mat("27.2296 77.1385"), arma::mat("20.3993 72.7076"), arma::mat("16.1556 68.2304"), arma::mat("12.7184 62.4905"),
        arma::mat("10.6380 56.0806"), arma::mat("10.0183 49.2149"), arma::mat("11.4275 40.8264"), arma::mat("14.1050 34.8682"),
        arma::mat("18.8168 28.7427"), arma::mat("24.6321 24.2200"), arma::mat("31.6833 21.1862"), arma::mat("39.1626 20.0207"),
        arma::mat("45.5204 20.5021"), arma::mat("55.3996 24.2692"), arma::mat("62.3561 30.0114"), arma::mat("67.3540 37.6492"),
        arma::mat("69.6190 45.2028")};
    */

        std::vector<arma::mat> lobe = {
            arma::mat("0.000 100.000"), arma::mat("10.143 102.985"), arma::mat("20.810 104.617"), arma::mat("31.721 104.571"),
            arma::mat("42.511 102.631"), arma::mat("52.769 98.7240"), arma::mat("62.096 92.9330"), arma::mat("70.153 85.4820"),
            arma::mat("76.711 76.7110"), arma::mat("81.674 67.0280"), arma::mat("85.093 56.8580"), arma::mat("87.155 46.5850"),
            arma::mat("88.145 36.5110"), arma::mat("88.409 26.8190"), arma::mat("88.293 17.5620"), arma::mat("88.090 8.67600"),
            arma::mat("88.000 0.00000"), arma::mat("88.090 -8.67600"), arma::mat("88.293 -17.5620"), arma::mat("88.409 -26.8190"),
            arma::mat("88.145 -36.5110"), arma::mat("87.155 -46.5850"), arma::mat("85.094 -56.8580"), arma::mat("81.674 -67.0280"),
            arma::mat("76.711 -76.7110"), arma::mat("70.153 -85.4820"), arma::mat("62.096 -92.9330"), arma::mat("52.769 -98.7240"),
            arma::mat("42.511 -102.631"), arma::mat("31.721 -104.571"), arma::mat("20.810 -104.617"), arma::mat("10.143 -102.985"),
            arma::mat("0.000 -100.0000"), arma::mat("-9.460 -96.052"), arma::mat("-18.208 -91.540"), arma::mat("-26.336 -86.817"),
            arma::mat("-34.026 -82.145"), arma::mat("-41.510 -77.660"), arma::mat("-49.018 -73.361"), arma::mat("-56.726 -69.120"),
            arma::mat("-64.711 -64.711"), arma::mat("-72.928 -59.851"), arma::mat("-81.200 -54.256"), arma::mat("-89.229 -47.694"),
            arma::mat("-96.631 -40.026"), arma::mat("-102.979 -31.238"), arma::mat("-107.864 -21.456"), arma::mat("-110.946 -10.927"),
            arma::mat("-112.000 -0.000"), arma::mat("-110.946 10.927"), arma::mat("-107.864 21.456"), arma::mat("-102.979 31.238"),
            arma::mat("-96.631 40.026"), arma::mat("-89.229 47.694"), arma::mat("-81.200 54.256"), arma::mat("-72.928 59.851"),
            arma::mat("-64.711 64.711"), arma::mat("-56.726 69.120"), arma::mat("-49.018 73.361"), arma::mat("-41.510 77.660"),
            arma::mat("-34.026 82.145"), arma::mat("-26.336 86.817"), arma::mat("-18.208 91.540"), arma::mat("-9.460 96.052")};

    
    std::vector<arma::mat> tlobe; // translated lobe
    double xc, yc; // lsqr's center
    double mx = std::numeric_limits<double>::min();
    double mn = std::numeric_limits<double>::max();
};


int main()
{
    // Initial guess
    arma::mat x("0.0 0.0");

    // Create simulated annealing optimizer with default options.
    // The ens::SA<> type can be replaced with any suitable ensmallen optimizer
    // that is able to handle arbitrary functions.
    ens::SA<> optimizer;
    MinDisc f; // Create function to be optimized.
    optimizer.Optimize(f, x);

    std::cout << "Minimum enclosing disc found with simulated annealing is " << x[0] + f.xc << "   " << x[1] + f.yc << "\n";
    std::cout << "Minimum enclosing disc found with simulated annealing is " << x;
    std::cout << "mn: " << f.mn << "\n";
    std::cout << "mx: " << f.mx << "\n";
}
