#pragma once
#include "CXCursorKind.hpp"

namespace rtd::introspection_builder {

inline auto to_string(CXString cxstr) -> std::string {
    const char* cstr = clang_getCString(cxstr);
    std::string stdstr(cstr);
    clang_disposeString(cxstr);
    return stdstr;
}
inline auto to_string(CXCursorKind kind) -> std::string {
    using namespace std;
    static auto chart = []()->std::map<int, std::string> {
        std::map<int, std::string> res;

        std::string_view source = enum_to_text_source::CXCursorKind;
        srell::regex regex_enum_member(R"(([0-9a-zA-Z_]+)\s+=\s+([0-9]+),)");
        srell::match_results<std::string_view::iterator> match;
        for (; regex_search(source.begin(), source.end(), match, regex_enum_member); source = std::string_view(match.suffix().first, match.suffix().second)) {
            auto enum_item_id = std::string_view(match[1].first, match[1].second);
            static std::string_view prefix = "CXCursor_";
            if (enum_item_id.starts_with(prefix))
                enum_item_id = enum_item_id.substr(prefix.length());
            res.insert_or_assign(std::stoi(match.str(2)), fmt::format("{}", enum_item_id));
        }

        return res;
    }();

    return chart[kind];
}

inline std::string getCursorNamespace(CXCursor cursor) {
    std::string res = "";
    CXCursor i = cursor;

    for (;;) {
        i = clang_getCursorSemanticParent(i);
        if (clang_Cursor_isNull(i)) break;
        if (clang_getCursorKind(i) == CXCursor_TranslationUnit) break;
        auto parent_name = to_string(clang_getCursorDisplayName(i));
        if (clang_getCursorKind(i) == CXCursor_Namespace && parent_name == "") continue;
        res = parent_name + "::" + res;
    }
    return res;

    //if (clang_Cursor_isNull(cursor)) return "";
    //if (clang_getCursorKind(cursor) == CXCursor_TranslationUnit) return "";
    //auto parent = getCursorNamespace(clang_getCursorSemanticParent(cursor));
    //auto self = to_string(clang_getCursorDisplayName(cursor));
    //return parent == "" ? self : parent + "::" + self;


}


}