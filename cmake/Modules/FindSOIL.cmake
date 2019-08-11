# * Try to find SOIL Once done this will define SOIL_FOUND - System has SOIL
#   SOIL_INCLUDE_DIRS - The SOIL include directories SOIL_LIBRARIES - The
#   libraries needed to use SOIL SOIL_DEFINITIONS - Compiler switches required
#   for using SOIL

# SOIL is a static library that depends on OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

find_path(SOIL_INCLUDE_DIR SOIL/SOIL.h)

find_library(SOIL_LIBRARY NAMES SOIL)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SOIL_FOUND to TRUE if all
# listed variables are TRUE
find_package_handle_standard_args(SOIL
                                  DEFAULT_MSG
                                  SOIL_LIBRARY
                                  SOIL_INCLUDE_DIR)

mark_as_advanced(SOIL_INCLUDE_DIR SOIL_LIBRARY)

set(SOIL_LIBRARIES ${SOIL_LIBRARY} ${OPENGL_LIBRARIES})
set(SOIL_INCLUDE_DIRS ${SOIL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
