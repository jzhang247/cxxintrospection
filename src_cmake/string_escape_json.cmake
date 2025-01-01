function(escape_json input output)
  set(result "${input}")
  string(REPLACE "\\" "\\\\" result "${result}")  # Escape backslashes
  string(REPLACE "\"" "\\\"" result "${result}")  # Escape double quotes
  string(REPLACE "\n" "\\n" result "${result}")  # Escape newlines
  string(REPLACE "\t" "\\t" result "${result}")  # Escape tabs
  # Add additional escapes as needed
  set(${output} "${result}" PARENT_SCOPE)
endfunction()