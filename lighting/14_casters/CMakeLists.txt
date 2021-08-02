# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/casters/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/casters/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all texture files
file(
  GLOB_RECURSE
  texture_files
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/casters/**.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/casters/**.jpg"
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/casters/**.jpeg")

# Copy texture files to build directory
copy_files("${texture_files}" "${CMAKE_BINARY_DIR}" textures)

add_custom_target(casters_data_files DEPENDS ${shaders} ${textures})

# Create our executable
add_executable(casters casters.cpp)
add_dependencies(casters casters_data_files)

# Build the executable into bin folder
set_target_properties(casters PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                         "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(
  casters PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(casters glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES} glm::glm
                      fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(casters ${CMAKE_DL_LIBS})
endif(UNIX)
