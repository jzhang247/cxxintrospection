add_custom_target(import_dynamic_library_target ALL
	COMMENT "Copying .dll files from source to build")

macro(import_dynamic_library) #(name, path)
if(${ARGC} EQUAL 2)
	add_library(${ARGV0} SHARED IMPORTED)
	set_target_custom_property(${ARGV0} IMPORTED_IMPLIB   "${ARGV1}.lib")
	set_target_custom_property(${ARGV0} IMPORTED_LOCATION "${ARGV1}.dll")

	get_filename_component(X ${ARGV1} NAME)

	add_custom_command(TARGET import_dynamic_library_target POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ARGV1}.dll" "${RTD_DEF_dir_build_runtime}/${X}.dll"
		COMMENT "Copying dll ${ARGV1}.dll"
	)
else()
	message("import_dynamic_library invoked with incorrect number (${ARGC}) of argument(s)")
endif()
endmacro()




