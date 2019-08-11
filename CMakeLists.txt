# Set the minimum cmake version that we need to function properly
cmake_minimum_required(VERSION 3.0)

# Set the name of the project and the languages that we need
project(COMP3320 LANGUAGES C CXX)

# We use the C++14 standard
set(CMAKE_CXX_STANDARD 14)

# Force enable diagnostic colours for clang and gcc
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  add_compile_options(-fcolor-diagnostics)
endif()

# We use additional modules that cmake needs to know about
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${PROJECT_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/3rdparty/build)

# Now find all of our dependencies
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glew REQUIRED)
find_package(fmt REQUIRED)
find_package(SOIL REQUIRED)
find_package(GLAD REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenAL REQUIRED)
find_package(SndFile REQUIRED)

# Add tutorials
add_subdirectory(introduction)

add_subdirectory(lighting)

add_subdirectory(assets)
