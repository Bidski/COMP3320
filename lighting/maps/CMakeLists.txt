# Include a function to make it easier for us to copy files
INCLUDE(CopyFiles)

# Find all shader files
FILE(GLOB_RECURSE shader_files
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag"
)

# Copy shader files to build directory
COPY_FILES("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all texture files
FILE(GLOB_RECURSE texture_files
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.jpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.jpeg"
)

# Copy texture files to build directory
COPY_FILES("${texture_files}" "${CMAKE_BINARY_DIR}" textures)

# Create our executable 
ADD_EXECUTABLE(maps maps.cpp ${shaders} ${textures})

# Build the executable into bin folder
SET_TARGET_PROPERTIES(maps
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Specify include paths and libraries needed to build our executable
TARGET_INCLUDE_DIRECTORIES(maps PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
TARGET_LINK_LIBRARIES(maps glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES} glm fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader libraries
IF(UNIX)
    TARGET_LINK_LIBRARIES(maps ${CMAKE_DL_LIBS})
ENDIF(UNIX)
