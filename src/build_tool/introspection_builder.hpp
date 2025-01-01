
#pragma once



#pragma once
#include "common/_index.hpp"
#include "libclang_helpers/helpers.hpp"
#include "rtd_cmake_dump_parser.hpp"
#include <clang-c/Index.h>


namespace rtd::introspection_builder {











struct target_info {
    std::string command;
    std::vector<std::filesystem::path> built_files;
    std::vector<std::filesystem::path> introspected_files;
};

struct project_info {

    struct cmake_compile_command {
        std::string m_command;
        std::string m_command_reduced;
        std::string m_input;
        std::string m_output;
        std::vector<std::string> m_command_args;
    };

    simdjson::dom::parser m_json_parser;
    simdjson::padded_string m_compile_commands_json_file_content;
    std::map<std::string, std::vector<cmake_compile_command>> m_cmake_compile_commands;

    std::map<std::string, target_info> m_targets;

    void initialize() {
        m_compile_commands_json_file_content = m_compile_commands_json_file_content.load(TOSTRING(RTD_DEF_EXPORT_COMPILE_COMMAND_PATH));

        auto doc = m_json_parser.parse(m_compile_commands_json_file_content);



        for (const auto& js_ele : doc.get_array()) {
            auto&& js_obj = js_ele.get_object();
            auto js_obj_get = [&](const std::string_view& key) -> std::string {
                return std::string(js_obj.at_key(key).get_string().value());
            };
            cmake_compile_command cmd{
                .m_command = js_obj_get("command"),
                .m_input = js_obj_get("file"),
                .m_output = js_obj_get("output"),
            };
            srell::regex regex(R"(.*CMakeFiles\\(.+)\.dir\\.+\.obj)", srell::regex_constants::ECMAScript);
            srell::match_results<decltype(cmd.m_output.begin())> results;
            if (!srell::regex_match(cmd.m_output.begin(), cmd.m_output.end(), results, regex)) {
                throw std::runtime_error("mismatch");
            }
            if (results.size() < 1) {
                throw std::runtime_error("mismatch");
            }
            auto target_name = results[1];

            cmd.m_command_reduced = cmd.m_command;
            cmd.m_command_reduced = plain_replace(cmd.m_command_reduced, fmt::format(" /Fo{}", cmd.m_output), "");
            cmd.m_command_reduced = plain_replace(cmd.m_command_reduced, fmt::format("-- {}", cmd.m_input), "");
            cmd.m_command_reduced = cmd.m_command_reduced.substr(cmd.m_command_reduced.find(" ") + 1);


            auto& target_info = m_targets.try_emplace(target_name).first->second;
            if (target_info.command.empty()) {
                target_info.command = cmd.m_command_reduced;
            } else if (target_info.command != cmd.m_command_reduced) {
                throw std::runtime_error("compile units in a target should have same commands");
            }
            target_info.built_files.emplace_back(cmd.m_input);

            m_cmake_compile_commands.try_emplace(target_name).first->second.push_back(cmd);
        }

        for (auto& target : m_targets) {
            auto cmake_dump = read_rtd_cmake_dump(fmt::format("../cmake_dump/{}.txt", target.first));
            auto& sources = cmake_dump["SOURCES"];
            auto sources_split = split_string_view(std::string_view(sources), std::string_view(";"));
            for (auto& i : sources_split) {
                auto content = read_from_file(i);
                if (content.contains("#pragma" " introspection_enabled")) {
                    target.second.introspected_files.push_back(i);
                }
            }
        }


        return;

    }
    void parse_all_targets() {
        for (auto& i : m_targets) {
            parse_target(i.first);
        }
    }
    void parse_target(const std::string& target_name) {
        auto target_find = m_targets.find(target_name);
        if (target_find == m_targets.end()) return;
        auto& target = target_find->second;

        fmt::print("project {}\n", target_name);


        CXIndexOptions index_options = {};
        index_options.Size = sizeof(index_options);
        index_options.ExcludeDeclarationsFromPCH = false;
        index_options.DisplayDiagnostics = true;


        CXIndex index = clang_createIndexWithOptions(&index_options);
        for (const auto& source_file : target.introspected_files) {
            fmt::print("file  {}\n", source_file);
            const char* commands[] = { target.command.c_str(), };

            auto tu = clang_parseTranslationUnit(index, source_file.string().c_str(), commands, arr_len(commands), nullptr, 0, NULL);
            if (tu == NULL) {
                fmt::print("translation failed\n");
            }

            struct type_info_t {
                std::vector<std::string> members;
            };
            struct file_info_t {
                std::unordered_map<std::string, type_info_t> types;
            };

            file_info_t file_info;
            clang_visitChildren(clang_getTranslationUnitCursor(tu), [](CXCursor cur, CXCursor parent, CXClientData client)->CXChildVisitResult {
                auto& file_info = *((file_info_t*)client);
                auto kind = clang_getCursorKind(cur);
                auto name = to_string(clang_getCursorDisplayName(cur));
                fmt::print("{}{}\n", getCursorNamespace(cur), name);
                if (kind == CXCursor_StructDecl || kind == CXCursor_StructDecl) {
                    type_info_t type_info;
                    clang_visitChildren(cur, [](CXCursor cur, CXCursor parent, CXClientData client)->CXChildVisitResult {
                        auto& type_info = *((type_info_t*)client);

                        auto kind = clang_getCursorKind(cur);
                        if (kind == CXCursor_FieldDecl) {
                            type_info.members.push_back(to_string(clang_getCursorDisplayName(cur)));
                            fmt::print("    {}\n", to_string(clang_getCursorDisplayName(cur)));
                        }
                        return CXChildVisit_Continue;
                    }, &type_info);
                    file_info.types.emplace(fmt::format("{}{}", getCursorNamespace(cur), name), std::move(type_info));
                }


                return CXChildVisit_Recurse;
            }, &file_info);

            std::string output = R"r(

#pragma once
#if !defined(INCLUDED_INTROSPECTION_DECL_HPP)
#error has to include introspection decl first.
#endif
namespace introspection{
)r";
            for (auto& type_info : file_info.types) {
                auto& classname = type_info.first;
                output += fmt::format("template<> struct type_data <{}>{{ static constexpr size_t field_count = {}; }};\n", classname, type_info.second.members.size());
                for (size_t t = 0; auto & field : type_info.second.members) {

                    output += fmt::format("template<> inline decltype(auto) get_field_name<{},{}>(){{ static const char name[] = \"{}\"; return std::string_view(name); }};\n", t, classname, field);
                    output += fmt::format("template<> inline const auto&    get_field<{}>(const {} &  obj){{ return           obj.{} ; }}\n", t, classname, field);
                    output += fmt::format("template<> inline       auto&    get_field<{}>(      {} &  obj){{ return           obj.{} ; }}\n", t, classname, field);
                    output += fmt::format("template<> inline const auto&&   get_field<{}>(const {} && obj){{ return std::move(obj.{}); }}\n", t, classname, field);
                    output += fmt::format("template<> inline       auto&&   get_field<{}>(      {} && obj){{ return std::move(obj.{}); }}\n", t, classname, field);
                    t++;
                }
            }
            auto rel = std::filesystem::relative(source_file, TOSTRING(RTD_DEF_dir_src));
            auto output_path = fmt::format(TOSTRING(RTD_DEF_dir_build_intermediate) "/introspection/{}", rel);
            output += "}";
            write_to_file(output_path, output);
        }




    }
};










}

