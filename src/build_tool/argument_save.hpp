#pragma once


#include "_afx.hpp"
#include "simdjson.h"

namespace process_argument_storage {

static std::mutex lock;
static bool is_testing = false;

inline std::filesystem::path get_saved_path() {
    if (is_testing)
        return std::filesystem::current_path() / "saved_argument_testing.json";
    else
        return std::filesystem::current_path() / "saved_argument.json";
}

inline void save(int argc, const char** argv) {
    std::lock_guard lock_access(lock);

    simdjson::ondemand::array args;
    std::ofstream output_file(get_saved_path());

    if (!output_file.is_open()) {
        throw std::runtime_error("Could not open file for saving arguments.");
    }

    // Start JSON array
    output_file << "[";
    for (int i = 0; i < argc; ++i) {
        output_file << "\"" << argv[i] << "\"";
        if (i < argc - 1) {
            output_file << ",";
        }
    }
    // Close JSON array
    output_file << "]";
    output_file.close();
}

inline std::vector<std::string> read() {
    std::lock_guard lock_access(lock);

    std::filesystem::path file_path = get_saved_path();
    if (!std::filesystem::exists(file_path)) {
        return {};
        //throw std::runtime_error("Saved argument file does not exist.");
    }

    std::ifstream input_file(file_path);
    if (!input_file.is_open()) {
        throw std::runtime_error("Could not open file for reading arguments.");
    }

    std::string json_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();

    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json_data);

    std::vector<std::string> arguments;
    for (auto arg : doc.get_array()) {
        arguments.push_back(std::string(arg.get_string().value()));
    }
    return arguments;
}

inline void run_tests() {

    {

        const char* argv[] = {
            "arg with space",
            "arg with slash\\",
            "arg with quot\"",
            "arg with 中文"
        };
        constexpr int argc = sizeof(argv) / sizeof(argv[0]) + 1;
        std::lock_guard lock_access(lock);
        is_testing = true;
        save(argc, argv);
        auto read_rslt_stdstr = read();
        std::vector<const char*> read_rslt;
        read_rslt.assign_range(read_rslt_stdstr | std::views::transform([](const auto& str) { return str.c_str(); }));
        
        

        is_testing = false;


    }





}

}