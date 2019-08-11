COMP3320 OpenGL Tutorials
=========================

This repository contains some example code for using OpenGL in a C++ environment using a CMake build system.

Getting Started
---------------

1. Build 3rdparty dependencies

```bash
mkdir 3rdparty/build
cd 3rdparty/build
cmake ..
make
cd ../../
```

2. Build the examples

```bash
mkdir build
cd build
cmake ..
make
```

3. Run the examples (from the build folder)

```bash
./bin/movement
```
