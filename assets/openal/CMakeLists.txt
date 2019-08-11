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

# Copy model files to build directory
copy_files("${model_files}" "${CMAKE_BINARY_DIR}" models)

# Find all audio files
file(GLOB_RECURSE audio_files "${CMAKE_CURRENT_SOURCE_DIR}/audio/**.wav")

# Copy audio files to build directory
copy_files("${audio_files}" "${CMAKE_BINARY_DIR}" audios)

add_custom_target(openal_data_files DEPENDS ${shaders} ${models} ${audios})

# Create our executable
add_executable(openal_audio openal.cpp)
add_dependencies(openal_audio openal_data_files)

# Build the executable into bin folder
set_target_properties(openal_audio
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(openal_audio
                           PRIVATE ${GLAD_INCLUDE_DIRS}
                                   ${SOIL_INCLUDE_DIRS}
                                   ${CMAKE_SOURCE_DIR}
                                   ${ASSIMP_INCLUDE_DIRS}
                                   ${OpenAL_INCLUDE_DIRS})
target_link_libraries(openal_audio
                      glfw
                      ${GLAD_LIBRARIES}
                      ${SOIL_LIBRARIES}
                      glm
                      fmt::fmt
                      ${ASSIMP_LIBRARIES}
                      ${OPENAL_LIBRARY}
                      SndFile::sndfile)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(openal_audio ${CMAKE_DL_LIBS})
endif(UNIX)
