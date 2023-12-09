
# NIST Tests

The files and tests in this directory are derived from the paper "On
Algorithms and Heuristics for Constrained Least-Squares Fitting of
Circles and Spheres to Support Standards" and the NIST website.

The Matlab/Octave source files (.m) are all straight out of the paper
(with corrections). The test data is the NIST l2 reference pairs
downloaded from the NIST website (TODO: Get links.) Just the 2D Circle
data is included in the repo.

## Running the tests

The Octave/Matlab script `driver.m` will run the NIST code on the
sample data and print the results.

  > octave driver.m

## Visualizing the tests

The Python script `viz_nist.py` will plot all of the NIST test samples
along with the solution circle. All the data is rendered in 2D.

  > python3 vis_nist.py
