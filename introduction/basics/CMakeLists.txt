# Create our executable 
ADD_EXECUTABLE(basics basics.cpp)

# Build the executable into bin folder
SET_TARGET_PROPERTIES(basics
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Specify include paths and libraries needed to build our executable
TARGET_INCLUDE_DIRECTORIES(basics PRIVATE ${GLAD_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(basics glfw ${GLAD_LIBRARIES} fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader libraries
IF(UNIX)
    TARGET_LINK_LIBRARIES(basics ${CMAKE_DL_LIBS})
ENDIF(UNIX)
