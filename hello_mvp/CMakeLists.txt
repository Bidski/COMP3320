INCLUDE(CopyFiles)

# Find all shader files
FILE(GLOB_RECURSE shader_files
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag"
)
COPY_FILES("${shader_files}" "${CMAKE_BINARY_DIR}" shaders)

# Find all texture files
FILE(GLOB_RECURSE texture_files
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.jpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/**.jpeg"
)

COPY_FILES("${texture_files}" "${CMAKE_BINARY_DIR}" textures)

ADD_EXECUTABLE(hello_mvp hello_mvp.cpp ${shaders} ${textures})
SET_TARGET_PROPERTIES(hello_mvp
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
TARGET_INCLUDE_DIRECTORIES(hello_mvp PRIVATE ${GLAD_INCLUDE_DIRS} ${SOIL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
TARGET_LINK_LIBRARIES(hello_mvp glfw ${GLAD_LIBRARIES} ${SOIL_LIBRARIES} glm fmt::fmt)

IF(UNIX)
    TARGET_LINK_LIBRARIES(hello_mvp ${CMAKE_DL_LIBS})
ENDIF(UNIX)
