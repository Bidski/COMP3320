# Include a function to make it easier for us to copy files
INCLUDE(CopyFiles)

# Find all shader files
FILE(GLOB_RECURSE shader_files
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag"
)

# Copy shader files to build directory
COPY_FILES("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Create our executable 
ADD_EXECUTABLE(materials materials.cpp ${shaders} ${textures})

# Build the executable into bin folder
SET_TARGET_PROPERTIES(materials
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Specify include paths and libraries needed to build our executable
TARGET_INCLUDE_DIRECTORIES(materials PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
TARGET_LINK_LIBRARIES(materials glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES} glm fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader libraries
IF(UNIX)
    TARGET_LINK_LIBRARIES(materials ${CMAKE_DL_LIBS})
ENDIF(UNIX)
