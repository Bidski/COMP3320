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

Folder Structure
----------------

```
COMP3320
├── 3rdparty        3rd party libraries
├── cmake
│   └── Modules     Extra CMake utilities and Find modules
├── introduction    Basic introductory examples
│   ├── basics      Setting up a simple OpenGL window and context
│   ├── triangle    Rendering our first triangle
│   ├── indexing    Rendering multiple triangles with element buffers
│   ├── uniforms    Using uniforms to add colour
│   ├── attributes  Using vertex attributes to add more colour
│   ├── textures    Using images to texture objects
│   ├── transforms  How to transform objects
│   ├── mvp         Setting up the model, view, and projection transforms
│   ├── multi_cube  Using multiple model transforms to create multiple objects
│   └── movement    Adding a camera and moving around the scene
├── lighting        Examples showing how to add lighting
│   ├── phong       Setting up the basic lighting equations for ambient, diffuse, and specular lighting
│   ├── materials   Setting up material objects
│   ├── maps        Using textures to determine lighting colours and effects
│   └── casters     Using multiple different types of light (directional, point lights, spot lights)
├── assets          Examples showing how to load in assets
│   ├── assimp      Example showing how to use Open Asset Importer library for loading 3D models
│   └── openal      Example showing how to use OpenAL and libsndfile for loading audio
├── utility         Utility classes
└── slides          Lecture slides
```
