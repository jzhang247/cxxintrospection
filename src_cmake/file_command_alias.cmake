macro(file_clear file_path)
    file(WRITE ${file_path})
    set(current_output_file_path ${file_path})
endmacro()

macro(file_append)
    file(APPEND ${current_output_file_path} ${ARGN})
endmacro()