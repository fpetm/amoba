# amoba

[![ci](https://github.com/fpetm/amoba/actions/workflows/ci.yml/badge.svg)](https://github.com/fpetm/amoba/actions/workflows/ci.yml)

## Build instructions

0. Clone repository: `git clone https://github.com/fpetm/amoba.git`

1. Update git submodules: `git submodule update --init --recursive`

2. Generate build configuration: `cmake -Bbuild .`

3. Configure build configuration: `ccmake build` or `cmake-gui build`

4. Build project: `cmake --build build`  
The amoba executable is located in build/amoba/amoba
