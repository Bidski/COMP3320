function(COPY_FILES
         file_list
         output_dir
         return_var)
  foreach(current_file ${file_list})
    # Calculate the output Directory
    file(RELATIVE_PATH
         rel_file
         "${CMAKE_CURRENT_SOURCE_DIR}"
         ${current_file})
    set(output_file "${output_dir}/${rel_file}")

    # Add the file we will generate to our output
    list(APPEND textures ${output_file})

    # Create the required folder
    get_filename_component(output_folder ${output_file} DIRECTORY)
    file(MAKE_DIRECTORY ${output_folder})

    # Copy across our file
    add_custom_command(OUTPUT ${output_file}
                       COMMAND ${CMAKE_COMMAND}
                               -E
                               copy
                               ${current_file}
                               ${output_file}
                       DEPENDS ${current_file}
                       COMMENT "Copying updated file ${rel_file}")
  endforeach(current_file)
  set(${return_var} ${textures} PARENT_SCOPE)
endfunction(COPY_FILES)
