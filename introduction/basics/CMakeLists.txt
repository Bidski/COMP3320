# Create our executable
add_executable(basics basics.cpp)

# Build the executable into bin folder
set_target_properties(basics
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 "${CMAKE_BINARY_DIR}/bin")

# Specify include paths and libraries needed to build our executable
target_include_directories(basics PRIVATE ${GLAD_INCLUDE_DIRS})
target_link_libraries(basics
                      glfw
                      ${GLAD_LIBRARIES}
                      fmt::fmt)

# On linux/unix systems we also need to link against the dynamic loader
# libraries
if(UNIX)
  target_link_libraries(basics ${CMAKE_DL_LIBS})
endif(UNIX)
