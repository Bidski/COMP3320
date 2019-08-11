# * Try to find GLAD Once done this will define GLAD_FOUND - System has GLAD
#   GLAD_INCLUDE_DIRS - The GLAD include directories GLAD_LIBRARIES - The
#   libraries needed to use GLAD GLAD_DEFINITIONS - Compiler switches required
#   for using GLAD

find_path(GLAD_INCLUDE_DIR glad/glad.h
          HINTS ${CMAKE_SOURCE_DIR}/3rdparty/GLAD/include)

find_library(GLAD_LIBRARY NAMES GLAD)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GLAD_FOUND to TRUE if all
# listed variables are TRUE
find_package_handle_standard_args(GLAD
                                  DEFAULT_MSG
                                  GLAD_LIBRARY
                                  GLAD_INCLUDE_DIR)

mark_as_advanced(GLAD_INCLUDE_DIR GLAD_LIBRARY)

set(GLAD_LIBRARIES ${GLAD_LIBRARY})
set(GLAD_INCLUDE_DIRS ${GLAD_INCLUDE_DIR})
