function(copy_files target_name files_to_copy output_dir)
 		add_custom_command(TARGET ${target_name}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}
			COMMAND ${CMAKE_COMMAND} -E copy_if_different 	
					${${files_to_copy}}									
					${output_dir}
			VERBATIM
			)
endfunction(copy_files)

function(copy_directory target_name dir_to_copy output_dir)
	add_custom_command(TARGET ${target_name} 
		POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory 
				${dir_to_copy} 
				${output_dir}
		VERBATIM
		)
endfunction(copy_directory)

function(disable_specific_warnings target_name)
	set(specific_warnings "")
	foreach(specific_warning ${ARGN})
		set(specific_warnings "${specific_warnings} -wd${specific_warning}")			
	endforeach()
	set_target_properties(${target_name}
				PROPERTIES COMPILE_FLAGS  ${specific_warnings}
				)
endfunction(disable_specific_warnings)

function(set_headers_prefix prefix header_list)
    set(HEADERS)
    foreach(HEADER ${${header_list}})
        set(HEADERS ${HEADERS} ${prefix}/${HEADER})
    endforeach(HEADER)
    set(HEADERS ${HEADERS} PARENT_SCOPE)
endfunction(set_headers_prefix)