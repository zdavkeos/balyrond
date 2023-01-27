# Balyrond - The open source Roundness Tester project

## About

This repository exists to support the idea of an open souce roundness analyzer.

Here are the building blocks and documentation.

The main components are:

### Two PlotJuggler Plugins

PlotJuggler is an open source realtime plotting program. Included are two plugins:

- Data Streaming of roundness data from a serial port
- Roundness analysis toolbox for analyzing the data

### Firmware examples

The current target platform is a Seeeduino XIAO running micropython.

### Hardware schematics

Wiring diagrams and part numbers.

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
3. Run `build-win32.bat`
4. The `install/bin` folder is the standalone application folder

### Instructions: Linux

1. Clone this repo: `git clone <repo-url> --recurse-submodules`
3. Run `./build-linux.sh`
4. The `install/bin` folder is the standalone application folder

### Instructions: Mac

1. Clone this repo: `git clone <repo-url> --recurse-submodules`
3. Run `./build-mac.sh`
4. The `install/bin` folder is the standalone application folder
