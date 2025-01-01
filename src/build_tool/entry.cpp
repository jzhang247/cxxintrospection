



#include "_afx.hpp"
#include "argument_save.hpp"
#include <clang-c/Index.h>
#include "introspection_builder.hpp"
#include "rtd_cmake_dump_parser.hpp"

int main(int argc, const char** argv) {    
    std::cout << "build_tool\n";


    constexpr bool use_saved_args = true;
    std::vector<const char*> argv_vec;
    if (use_saved_args) {
        if (argc > 1) {
            process_argument_storage::save(argc, argv);
        } else {
            auto saved = process_argument_storage::read();
            if (!saved.empty()) {
                argv_vec.assign_range(saved | std::views::transform([](auto&& e) { return e.c_str(); }));
            }
        }
    }

    rtd::introspection_builder::project_info project;
    project.initialize();
    project.parse_all_targets();
    //project.parse_target("build_tool_test_src");






    return 0;
}