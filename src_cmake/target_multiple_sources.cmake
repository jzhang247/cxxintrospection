function(target_multiple_sources target_name access_modifier)
	foreach(dir ${ARGN})
		file(GLOB_RECURSE all_selected_files "${dir}/*")

		foreach(selected_file ${all_selected_files})
			if(${selected_file} MATCHES ".*\\.(c|cpp|cc|h|hpp|hh|nativs)")
			target_sources(${target_name} ${access_modifier} ${selected_file})
			endif()
		endforeach()
	endforeach()
endfunction()










