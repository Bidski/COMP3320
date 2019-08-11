# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE
     shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all texture files
file(GLOB_RECURSE
     texture_files
     "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.png"
     "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.jpg"
     "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.jpeg")

# Copy texture files to build directory
copy_files("${texture_files}" "${CMAKE_BINARY_DIR}" textures)

add_custom_target(maps_data_files DEPENDS ${shaders} ${textures})

# Create our executable
add_executable(maps maps.cpp)
add_dependencies(maps maps_data_files)

# Build the executable into bin folder
set_target_properties(maps
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(maps
                           PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS}
                                   ${CMAKE_SOURCE_DIR})
target_link_libraries(maps
                      glfw
                      ${GLAD_LIBRARIES}
                      ${SOIL_LIBRARIES}
                      glm
                      fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(maps ${CMAKE_DL_LIBS})
endif(UNIX)
