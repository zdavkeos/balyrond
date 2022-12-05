# Balyrond - The open source Roundness Tester project

## Background


## Building

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
