# COMP3320 OpenGL Tutorials

This repository contains some example code for using OpenGL in a C++ environment using a CMake build system.

This code is heavily based on the tutorials provided at [learnopengl.com](https://learnopengl.com).

## Getting Started

https://learnopengl.com/

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

## Folder Structure

```
COMP3320
├── 3rdparty        3rd party libraries
├── cmake           Extra CMake utilities and Find modules
├── introduction    Basic introductory examples
│   ├── 01_basics      Setting up a simple OpenGL window and context
│   ├── 02_triangle    Rendering our first triangle
│   ├── 03_indexing    Rendering multiple triangles with element buffers
│   ├── 04_uniforms    Using uniforms to add colour
│   ├── 05_attributes  Using vertex attributes to add more colour
│   ├── 06_textures    Using images to texture objects
│   ├── 07_transforms  How to transform objects
│   ├── 08_mvp         Setting up the model, view, and projection transforms
│   ├── 09_multi_cube  Using multiple model transforms to create multiple objects
│   └── 10_movement    Adding a camera and moving around the scene
├── lighting        Examples showing how to add lighting
│   ├── 11_phong       Setting up the basic lighting equations for ambient, diffuse, and specular lighting
│   ├── 12_materials   Setting up material objects
│   ├── 13_maps        Using textures to determine lighting colours and effects
│   └── 14_casters     Using multiple different types of light (directional, point lights, spot lights)
├── assets          Examples showing how to load in assets
│   ├── 15_assimp      Example showing how to use Open Asset Importer library for loading 3D models
│   └── 16_openal      Example showing how to use OpenAL and libsndfile for loading audio
├── utility         Utility classes and functions
└── slides          Lecture slides
    ├── 00_overview    A brief overview of everything else in this lecture series
    ├── 01_basics      How to setup the windowing system and OpenGL context
    ├── 02_rendering   The OpenGL rendering pipeline and shader basics
    ├── 03_shaders     How shaders work
    ├── 04_attributes  How to add more attributes to vertices (colour and texture coordinates)
    ├── 05_lighting    Different lighting techniques and and types of lights
    └── 06_assets      Asset loading with Assimp and OpenAL
```
