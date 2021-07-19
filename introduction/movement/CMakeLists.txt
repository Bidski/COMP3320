# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/movement/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/movement/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all texture files
file(
  GLOB_RECURSE
  texture_files
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/movement/**.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/movement/**.jpg"
  "${CMAKE_CURRENT_SOURCE_DIR}/textures/movement/**.jpeg")

# Copy texture files to build directory
copy_files("${texture_files}" "${CMAKE_BINARY_DIR}" textures)

add_custom_target(movement_data_files DEPENDS ${shaders} ${textures})

# Create our executable
add_executable(movement movement.cpp)
add_dependencies(movement movement_data_files)

# Build the executable into bin folder
set_target_properties(movement PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                          "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(
  movement PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS}
                   ${CMAKE_SOURCE_DIR})
target_link_libraries(movement glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES}
                      glm::glm fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(movement ${CMAKE_DL_LIBS})
endif(UNIX)
