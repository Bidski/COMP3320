find_path(LAME_INCLUDE_DIR lame/lame.h)

find_library(LAME_LIBRARY NAMES mp3lame)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SOIL_FOUND to TRUE if all
# listed variables are TRUE
find_package_handle_standard_args(Lame DEFAULT_MSG LAME_LIBRARY
                                  LAME_INCLUDE_DIR)

mark_as_advanced(LAME_INCLUDE_DIR LAME_LIBRARY)

set(LAME_LIBRARIES ${LAME_LIBRARY})
set(LAME_INCLUDE_DIRS ${LAME_INCLUDE_DIR})

add_library(Lame::Lame STATIC IMPORTED)
set_target_properties(
  Lame::Lame
  PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${LAME_INCLUDE_DIRS}"
             INTERFACE_LINK_LIBRARIES "${LAME_LIBRARIES}"
             IMPORTED_LOCATION "${LAME_LIBRARIES}")
