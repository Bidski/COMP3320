find_path(MPG123_INCLUDE_DIR mpg123.h)

find_library(MPG123_LIBRARY NAMES mpg123)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MPG123_FOUND to TRUE if all
# listed variables are TRUE
find_package_handle_standard_args(MPG123 DEFAULT_MSG MPG123_LIBRARY
                                  MPG123_INCLUDE_DIR)

mark_as_advanced(MPG123_INCLUDE_DIR MPG123_LIBRARY)

set(MPG123_LIBRARIES ${MPG123_LIBRARY})
set(MPG123_INCLUDE_DIRS ${MPG123_INCLUDE_DIR})

add_library(MPG123::libmpg123 STATIC IMPORTED)
set_target_properties(
  MPG123::libmpg123
  PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MPG123_INCLUDE_DIRS}"
             INTERFACE_LINK_LIBRARIES "${MPG123_LIBRARIES}"
             IMPORTED_LOCATION "${MPG123_LIBRARIES}")
