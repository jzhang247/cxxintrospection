cmake_minimum_required (VERSION 3.24)
project ("RetroDriveCMakeSolution")

# Show build time
#set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
# Disable cache
set(CMAKE_CACHEFILE_DIR "" CACHE STRING "")


# Load macros
file(GLOB RTD_include_cmake_files "src_cmake/*.cmake")
foreach(cmake_file ${RTD_include_cmake_files})
    message("INCLUDE ${cmake_file}")
    INCLUDE(${cmake_file})
endforeach()

# Set standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
add_compile_definitions(UNICODE)

# Prepare custom variables
if(true)
    set(RTD_definition_prefix                "RTD_DEF_")
    set(RTD_DEF_BUILD_TYPE                   "${CMAKE_BUILD_TYPE}")
    set(RTD_DEF_dir_logs                     "${CMAKE_SOURCE_DIR}/logs")
    set(RTD_DEF_dir_src                      "${CMAKE_SOURCE_DIR}/src")
    set(RTD_DEF_dir_src_targets              "${CMAKE_SOURCE_DIR}/src/targets")
    set(RTD_DEF_dir_output                   "${CMAKE_BINARY_DIR}/rtd_output_dir")
    set(RTD_DEF_dir_build_runtime            "${RTD_DEF_dir_output}/runtime")
    set(RTD_DEF_dir_build_build_tools        "${RTD_DEF_dir_output}/runtime")
    set(RTD_DEF_dir_build_intermediate       "${RTD_DEF_dir_output}/intermediate")
    set(RTD_DEF_dir_cmake_dump               "${RTD_DEF_dir_output}/cmake_dump")
    set(RTD_DEF_dir_cmake_def_header         "${RTD_DEF_dir_output}/rtd_def_header")    
endif()

# Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${RTD_DEF_dir_build_runtime}>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${RTD_DEF_dir_build_runtime}>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${RTD_DEF_dir_build_runtime}>)

# Show build commands
if(true)
    set(CMAKE_EXPORT_COMPILE_COMMANDS true)
    set(RTD_DEF_EXPORT_COMPILE_COMMAND_PATH "${CMAKE_BINARY_DIR}/compile_commands.json")
endif()

# Include directories
include_directories("src")
include_directories(SYSTEM "src_extern")
include_directories(${RTD_DEF_dir_build_intermediate})



function(target_link_common_libraries tgt)
    find_path(SRELL_INCLUDE_DIRS "srell.hpp")
    target_include_directories(${tgt} PRIVATE ${SRELL_INCLUDE_DIRS})

    find_package(fmt CONFIG REQUIRED)
    target_link_libraries(${tgt} PRIVATE fmt::fmt-header-only)

    find_package(simdjson CONFIG REQUIRED)
    target_link_libraries(${tgt} PRIVATE simdjson::simdjson)

endfunction()





set(cur_tgt "build_tool_test_src")
if(true)
    add_executable(${cur_tgt})
    target_link_common_libraries(${cur_tgt})
    target_include_directories(${cur_tgt} PRIVATE "src/common")
    target_multiple_sources(${cur_tgt} PRIVATE "src/common")
    target_include_directories(${cur_tgt} PRIVATE "src/build_tool_test_src")
    target_multiple_sources(${cur_tgt} PRIVATE "src/build_tool_test_src")
    set_target_properties(${cur_tgt} PROPERTIES introspection_enabled yes)
endif()

# Make a definition for each CMake variables starting with `RTD_DEF_`
if(true)
    file_clear("${RTD_DEF_dir_cmake_def_header}/rtd_def_header.h")
    include_directories("${RTD_DEF_dir_cmake_def_header}")
    get_cmake_property(all_variable_names VARIABLES)
    list(FILTER all_variable_names INCLUDE REGEX "^${RTD_definition_prefix}.+$")
    foreach(def ${all_variable_names})
        file_append("#define ${def} ${${def}}\n")
    endforeach()
endif()

add_subdirectory("src/build_tool")


# Generate information for reflection
get_all_targets(all_targets)
foreach(tgt ${all_targets})
    get_target_property(introspection_enabled ${tgt} "introspection_enabled")
    if(NOT ${introspection_enabled})    
        continue()
    endif()

    
    
endforeach()



# Dump cmake variables and targets properties to file
#file(REMOVE_RECURSE "${RTD_DEF_dir_cmake_dump}")
file_dump_all_variables("${RTD_DEF_dir_cmake_dump}/_cmake_variables.txt")
get_all_targets(all_targets)
foreach(cur_tgt ${all_targets})
    get_target_property(target_source_dir ${cur_tgt} SOURCE_DIR)
    #message("Target: ${cur_tgt} at: ${target_source_dir}")
    file_dump_target_properties(${cur_tgt} "${RTD_DEF_dir_cmake_dump}/${cur_tgt}.txt")
endforeach()


