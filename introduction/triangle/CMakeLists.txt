# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE
     shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

add_custom_target(triangle_data_files DEPENDS ${shaders})

# Create our executable
add_executable(triangle triangle.cpp)
add_dependencies(triangle triangle_data_files)

# Build the executable into bin folder
set_target_properties(triangle
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(triangle
                           PRIVATE ${GLAD_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(triangle
                      glfw
                      ${GLAD_LIBRARIES}
                      fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(triangle ${CMAKE_DL_LIBS})
endif(UNIX)
