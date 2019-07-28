# Set the minimum cmake version that we need to function properly
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Set the name of the project and the languages that we need
PROJECT(COMP3320 LANGUAGES C CXX)

# We use the C++14 standard
SET(CMAKE_CXX_STANDARD 14)

# We use additional modules that cmake needs to know about
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/3rdparty/build)

# Now find all of our dependencies
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(assimp REQUIRED)
FIND_PACKAGE(glew REQUIRED)
FIND_PACKAGE(fmt REQUIRED)
FIND_PACKAGE(SOIL REQUIRED)
FIND_PACKAGE(GLAD REQUIRED)

# Add tutorials
ADD_SUBDIRECTORY(introduction)

ADD_SUBDIRECTORY(lighting)
