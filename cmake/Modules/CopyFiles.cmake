FUNCTION(COPY_FILES file_list output_dir return_var)
    FOREACH(current_file ${file_list})
        # Calculate the output Directory
        FILE(RELATIVE_PATH rel_file "${CMAKE_CURRENT_SOURCE_DIR}" ${current_file})
        SET(output_file "${output_dir}/${rel_file}")
    
        # Add the file we will generate to our output
        LIST(APPEND textures ${output_file})
    
        # Create the required folder
        GET_FILENAME_COMPONENT(output_folder ${output_file} DIRECTORY)
        FILE(MAKE_DIRECTORY ${output_folder})
    
        # Copy across our file
        ADD_CUSTOM_COMMAND(
            OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND} -E copy ${current_file} ${output_file}
            DEPENDS ${current_file}
            COMMENT "Copying updated file ${rel_file}"
        )
    ENDFOREACH(current_file)
    SET(${return_var} ${textures} PARENT_SCOPE)
ENDFUNCTION(COPY_FILES)