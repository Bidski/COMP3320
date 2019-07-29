# Include a function to make it easier for us to copy files
INCLUDE(CopyFiles)

# Find all shader files
FILE(GLOB_RECURSE shader_files
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag"
)

# Copy shader files to build directory
COPY_FILES("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all model files
FILE(GLOB_RECURSE model_files
    "${CMAKE_CURRENT_SOURCE_DIR}/models/**.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/**.obj"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/**.blend"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/**.mtl"
)

# Copy shader files to build directory
COPY_FILES("${model_files}" "${CMAKE_BINARY_DIR}" models)

ADD_CUSTOM_TARGET(data_files DEPENDS ${shaders} ${models})

# Create our executable 
ADD_EXECUTABLE(assimp_imp assimp.cpp)
ADD_DEPENDENCIES(assimp_imp data_files)

# Build the executable into bin folder
SET_TARGET_PROPERTIES(assimp_imp
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Specify include paths and libraries needed to build our executable
TARGET_INCLUDE_DIRECTORIES(assimp_imp PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${ASSIMP_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(assimp_imp glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES} glm fmt::fmt ${ASSIMP_LIBRARIES})

# On linux/unix systems we also need to link against the dynamic loader libraries
IF(UNIX)
    TARGET_LINK_LIBRARIES(assimp_imp ${CMAKE_DL_LIBS})
ENDIF(UNIX)
