project("build_tool")


set(CMAKE_CXX_STANDARD 23)
find_package(LLVM CONFIG REQUIRED)



add_definitions(${LLVM_DEFINITIONS})
set(target_name "build_tool")
add_executable("${target_name}")

TARGET_LINK_COMMON_LIBRARIES("${target_name}")

target_include_directories("${target_name}" PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries("${target_name}" PRIVATE "libclang")

target_multiple_sources("${target_name}" PRIVATE ".")

