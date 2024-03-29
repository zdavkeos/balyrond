# Balyrond - The open source Roundness Tester project

## About

This repository exists to support the idea of an open souce
[roundness](https://en.wikipedia.org/wiki/Roundness) analyzer. By
roundness, we mean in the 
[GT&D Circularity](https://www.gdandtbasics.com/circularity) sense.

The typical setup involves a high-precision way to spin the object
being inspected (spindle). The data acquisition component is two
pieces: a way to tell where on the object we are measuring (encoder)
and a fixed distance measurement device. The distance transducer is
preferrably a capacitive sensor, but could be inductive, direct, or laser.

Commercial products for doing very precise inspection are available,
but are very expensive. This repository provides the hardware and software
building blocks, along with some documentation so you can build your
own setup. The license is open, so use what you want.

The main components are:

### Two PlotJuggler Plugins

[PlotJuggler](https://plotjuggler.io/) is an open-source realtime data
capture and plotting program that is easy to extend. Included in this
repo are two plugins:

- Data Streaming of roundness data from a serial port
- Roundness analysis toolbox for analyzing the data

### Firmware examples

The current target platform is a Seeeduino XIAO RP2040.

### Hardware schematics

Wiring diagrams and part numbers.

## Features

- Live plotting of data
- Save captured data to CSV
- Loading of data from CSV
- Polar plots
- Least Squares Circle Fit
- MIC, MCC and MCZ (in process)

## Brief Tutorial: Looking at data

First, review the general [PlotJuggler tutorial](https://facontidavide.github.io/PlotJuggler/visualization_howto/index.html).

In the test folder, there is a csv file with some sample "roundness"
data.

In PlotJuggler:

- Open the file by clicking the Data button at the top left.
- In the popup, select "Use row number as X axis"
- Click "Ok"
- Go to Tools -> Roundness Analysis
- Drag the `angle` and `distance` fields into the top plot viewer
- Click "Calculate"

![Sample Data](img/balyrond_sample.png)

## Design

This project wasn't so much 'designed' as it was assembled from things
on hand and software I was familiar with.

The hardware (microcontroller and endcoders) were things I already
had. The choice of microcontroller (XAIO RP2040) turned out to be
especially convenient: the PIO handles the encoder counts and the USB
interface provides all the power plus the virtual serial port. Data
transfers at a reasonable rate. The ability to prototype using python
and then switch to C for the final product was great.

The decision to base the desktop software on PlotJuggler was a selfish
one - it was a platform that I was familiar with. It is powerful is
easy to hack on and extend. The fact that it already supports loading
CSV files made testing easy. It works on Windows, Linux and MacOS
which is a bonus. Having Qt and qwt available plus access to several
powerful OSS C++ libraries (Eigen, jc_voronoi, etc.) made for quick
development.

## Notes on the analysis algorithms

There is considerable acedemic literature on the _theory_ of analyzing
roundness and there is substantial engineering literature on how to
_interpret_ the meaning of the roundness tests but there is very
little information on _implementing_ the tests themselves. Part of my
hope with this project is to share what I've learned and maybe offer
some advice for others attempting to do the same. The reference
implemntations for
[MIC](src/balyrond-pj-plugins/ToolboxRoundness/mic2.cpp),
[MCC](src/balyrond-pj-plugins/ToolboxRoundness/mcc.cpp),
[MZC](src/balyrond-pj-plugins/ToolboxRoundness/mcz.cpp) and
[Least Squares Fit](src/balyrond-pj-plugins/ToolboxRoundness/lscf.cpp)
here are free and open for anyone interested.

The easiest of the bunch (especially if you leverage
[Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)) is the
Least Squares Fit. It is very robust and gives good bounds for the
rest of the circle fitting algorithms. It finds the "best" circle that
fits the data presented.

For MIC I tried two approaches. The first
[try](src/balyrond-pj-plugins/ToolboxRoundness/mic.cpp) was very
promising but proved to be extremely brittle. It would pass the
reference data with flying colors, but completely fall apart with real
world data. The
[second attempt](src/balyrond-pj-plugins/ToolboxRoundness/mic2.cpp) is
much more robust. It is based on voronoi diagrams and seems to be
preferred by those practicing in the field.

The [MCC](src/balyrond-pj-plugins/ToolboxRoundness/mcc.cpp) algorithm
is based on
[Welzl's algorithm](https://en.wikipedia.org/wiki/Smallest-circle_problem)
and seems to be the preferred approach in computational geomety
circles. So far it seems to perform adequetely.

[MCZ](src/balyrond-pj-plugins/ToolboxRoundness/mcz.cpp) is still a
work in progess. Should we us gradient descent based on the Least
Squares Fit? Or should we do a Least Squares Fit based on the area
inside and outside the circle? Or maybe use the voronoi data we are
already computing for the MIC fit?

According to
[NIST](https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=821955)
(as I understand) the "best" approach is to start with the Least
Squares Fit and then do gradient descent for the other three tests
(MIC, MCC and MCC). I found the NIST papers after going on a bunch of
tangents, but I want to at least bring up the idea here.

## Building the Plugins

In general, follow the prerequisites for PlotJuggler.

### Prerequisites (Windows)

- git
- Visual Studio 2019 (v16)
- vcpkg
- Qt 15

### Instructions: Windows standalone

1. Clone this repo: `git clone <repo-url> --recurse-submodules`
2. Fix the path to vcpkg in `build-win32.bat`
3. Run `build-windows.bat`
4. The `install/bin` folder is the standalone application folder

### Instructions: Linux and Mac

1. Clone this repo: `git clone <repo-url> --recurse-submodules`
3. Run `./build-nix.sh`
4. The `install/bin` folder is the standalone application folder
