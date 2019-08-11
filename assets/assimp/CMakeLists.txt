# Include a function to make it easier for us to copy files
include(CopyFiles)

# Find all shader files
file(GLOB_RECURSE
     shader_files
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag")

# Copy shader files to build directory
copy_files("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all model files
file(GLOB_RECURSE
     model_files
     "${CMAKE_CURRENT_SOURCE_DIR}/models/**.png"
     "${CMAKE_CURRENT_SOURCE_DIR}/models/**.obj"
     "${CMAKE_CURRENT_SOURCE_DIR}/models/**.blend"
     "${CMAKE_CURRENT_SOURCE_DIR}/models/**.mtl")

# Copy shader files to build directory
copy_files("${model_files}" "${CMAKE_BINARY_DIR}" models)

add_custom_target(data_files DEPENDS ${shaders} ${models})

# Create our executable
add_executable(assimp_imp assimp.cpp)
add_dependencies(assimp_imp data_files)

# Build the executable into bin folder
set_target_properties(assimp_imp
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(assimp_imp
                           PRIVATE ${GLAD_INCLUDE_DIRS}
                                   ${SOIL_INCLUDE_DIRS}
                                   ${CMAKE_SOURCE_DIR}
                                   ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(assimp_imp
                      glfw
                      ${GLAD_LIBRARIES}
                      ${SOIL_LIBRARIES}
                      glm
                      fmt::fmt
                      assimp::assimp)
# ${ASSIMP_LIBRARY_DIRS}/lib${ASSIMP_LIBRARIES}.so)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(assimp_imp ${CMAKE_DL_LIBS})
endif(UNIX)
