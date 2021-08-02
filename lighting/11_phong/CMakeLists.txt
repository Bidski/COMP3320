# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/phong/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/phong/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

add_custom_target(phong_data_files DEPENDS ${shaders})

# Create our executable
add_executable(phong phong.cpp)
add_dependencies(phong phong_data_files)

# Build the executable into bin folder
set_target_properties(phong PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                       "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(
  phong PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(phong glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES} glm::glm
                      fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(phong ${CMAKE_DL_LIBS})
endif(UNIX)
