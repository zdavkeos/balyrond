
# MIC/MCC/MZC Sample Programs

## Maximum Inscribed Circle (MIC)

`mic.py` contains the algorithm documented in the paper "A Quick
Algorithm of Maximum Inscribed Circle Method for Evaluation of the
Roundness Error". It is not meant to be efficient, the demo program
was just a way to prototype the solution and wrap my head around the
algorithm.

![mic_generation.gif](mic_generation.gif)

## Simulated Annealing

`sim_anneal.cpp` follows the recommendation in "Reference Algorithms
for Chebyshev and One - sided Data Fitting for Coordinate Metrology"
by Craig Sharkarji of NIST, available here:
<https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=822103>. The
MIC and MCC are found by an objective function and simulated annealing
optimization.

## CLSF

The latest recommendation from NIST seems to be to use Constrained
Least Squares Fit to determine the circularity measurements. That
paper is "On Algorithms and Heuristics for Constrained Least-Squares
Fitting of Circles and Spheres to Support Standards", also by
Shakarji.  The approach specified in that paper is to use a
least-squares fit as an initial guess, then use a linear programming
optimizer to find the rest of the datums. TBD.
