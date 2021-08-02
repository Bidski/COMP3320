# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/attributes/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/attributes/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

add_custom_target(attributes_data_files DEPENDS ${shaders})

# Create our executable
add_executable(attributes attributes.cpp)
add_dependencies(attributes attributes_data_files)

# Build the executable into bin folder
set_target_properties(attributes PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                            "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(attributes PRIVATE ${GLAD_INCLUDE_DIRS}
                                              ${CMAKE_SOURCE_DIR})
target_link_libraries(attributes glfw ${GLAD_LIBRARIES} fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(attributes ${CMAKE_DL_LIBS})
endif(UNIX)
