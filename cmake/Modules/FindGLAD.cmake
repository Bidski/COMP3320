# - Try to find GLAD
# Once done this will define
#  GLAD_FOUND - System has GLAD
#  GLAD_INCLUDE_DIRS - The GLAD include directories
#  GLAD_LIBRARIES - The libraries needed to use GLAD
#  GLAD_DEFINITIONS - Compiler switches required for using GLAD

FIND_PATH(GLAD_INCLUDE_DIR glad/glad.h HINTS ${CMAKE_SOURCE_DIR}/3rdparty/GLAD/include)

FIND_LIBRARY(GLAD_LIBRARY NAMES GLAD)

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GLAD_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLAD  DEFAULT_MSG GLAD_LIBRARY GLAD_INCLUDE_DIR)

MARK_AS_ADVANCED(GLAD_INCLUDE_DIR GLAD_LIBRARY)

SET(GLAD_LIBRARIES ${GLAD_LIBRARY})
SET(GLAD_INCLUDE_DIRS ${GLAD_INCLUDE_DIR})
