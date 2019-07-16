INCLUDE(CopyFiles)

# Find all shader files
FILE(GLOB_RECURSE shader_files
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag"
)

# Copy shader files to build directory
COPY_FILES("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

ADD_EXECUTABLE(hello_uniforms hello_uniforms.cpp ${shaders})
SET_TARGET_PROPERTIES(hello_uniforms
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
TARGET_INCLUDE_DIRECTORIES(hello_uniforms PRIVATE ${GLAD_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
TARGET_LINK_LIBRARIES(hello_uniforms glfw ${GLAD_LIBRARIES} fmt::fmt)

IF(UNIX)
    TARGET_LINK_LIBRARIES(hello_uniforms ${CMAKE_DL_LIBS})
ENDIF(UNIX)
