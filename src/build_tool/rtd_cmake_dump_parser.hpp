#pragma once
#include "_afx.hpp"


namespace rtd {

inline auto read_rtd_cmake_dump(const std::filesystem::path& file) -> std::unordered_map<std::string, std::string> {
    std::unordered_map<std::string, std::string> res;
    auto content = read_from_file(file);
    auto lines = split_string_view(std::string_view(content), std::string_view("\n"));
    std::string current_key = "";
    std::string current_val = "";
    for (auto line : lines) {
        if (line == "") continue;
        auto split = split_string_view(line, std::string_view("="));
        if (split.size() == 2) {
            current_key = split[0];
            current_val = split[1];
        } else if (split[0].starts_with("    ")) {
            if (current_val != "")
                current_val = current_val + ";" + std::string(split[0].substr(4));
            else
                current_val = std::string(split[0].substr(4));
        } else {
            throw std::runtime_error("Invalid format");
        }        
        res.insert_or_assign(current_key, current_val);
    }
    return res;
}













}