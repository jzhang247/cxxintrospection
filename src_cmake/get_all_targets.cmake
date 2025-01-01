# Ripped off from
#   https://stackoverflow.com/questions/37434946/how-do-i-iterate-over-all-cmake-targets-programmatically
#   https://stackoverflow.com/questions/31906905/how-to-tell-whether-a-given-target-is-a-library-or-executable

function(get_all_targets var)    
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(X ${current_targets})
        get_target_property(target_type ${X} TYPE)        
        if(${target_type} MATCHES "(EXECUTABLE)|(SHARED_LIBRARY)|(OBJECT_LIBRARY)")
        #if (target_type STREQUAL "EXECUTABLE" OR target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "OBJECT")
            list(APPEND ${targets} ${X})
        endif()
    endforeach()

endmacro()

