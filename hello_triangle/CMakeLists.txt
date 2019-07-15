# Find all shader files
FILE(GLOB_RECURSE shader_files
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/**.frag"
)

# Copy shader files to build directory
FOREACH(shader_file ${shader_files})
    # Calculate the output Directory
    FILE(RELATIVE_PATH output_file "${CMAKE_CURRENT_SOURCE_DIR}" ${shader_file})
    SET(output_file "${CMAKE_BINARY_DIR}/${output_file}")

    # Add the file we will generate to our output
    LIST(APPEND shader_code ${output_file})

    # Create the required folder
    GET_FILENAME_COMPONENT(output_folder ${output_file} DIRECTORY)
    FILE(MAKE_DIRECTORY ${output_folder})

    # Copy across our file
    ADD_CUSTOM_COMMAND(
        OUTPUT ${output_file}
        COMMAND ${CMAKE_COMMAND} -E copy ${shader_file} ${output_file}
        DEPENDS ${shader_file}
        COMMENT "Copying updated shader file ${shader_file}"
    )
ENDFOREACH(shader_file)

ADD_EXECUTABLE(hello_triangle hello_triangle.cpp ${shader_code})
SET_TARGET_PROPERTIES(hello_triangle
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
TARGET_INCLUDE_DIRECTORIES(hello_triangle PRIVATE ${GLAD_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
TARGET_LINK_LIBRARIES(hello_triangle glfw ${GLAD_LIBRARIES} fmt::fmt)

IF(UNIX)
    TARGET_LINK_LIBRARIES(hello_triangle ${CMAKE_DL_LIBS})
ENDIF(UNIX)
