# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/multi_cube/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/multi_cube/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all texture files
file(
  GLOB_RECURSE
  texture_files
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/multi_cube/**.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/multi_cube/**.jpg"
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/multi_cube/**.jpeg")

# Copy texture files to build directory
copy_files("${texture_files}" "${CMAKE_BINARY_DIR}" textures)

add_custom_target(multi_cube_data_files DEPENDS ${shaders} ${textures})

# Create our executable
add_executable(multi_cube multi_cube.cpp)
add_dependencies(multi_cube multi_cube_data_files)

# Build the executable into bin folder
set_target_properties(multi_cube PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                            "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(
  multi_cube PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS}
                     ${CMAKE_SOURCE_DIR})
target_link_libraries(multi_cube glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES}
                      glm::glm fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(multi_cube ${CMAKE_DL_LIBS})
endif(UNIX)
