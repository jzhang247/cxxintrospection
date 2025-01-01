#pragma once
#include "constexpr_string.hpp"
#include "string_split.hpp"

namespace rtd {




template <typename TyEnum>
struct enum_info {
    struct enum_item_info {
        TyEnum m_value;
        std::string_view m_text;
    };
    std::map<TyEnum, enum_item_info> m_items;
    bool m_wz_bit_mask = false;
    bool m_style_compact = false;

    void init(std::string_view source, const std::vector<TyEnum>& values) {
        std::vector<std::string_view> names;
        names.assign_range(
            split_string_view(std::string_view(source), std::string_view(","))
            | std::views::transform([](std::string_view e) { return split_string_view(e, std::string_view("="))[0]; })
            | std::views::transform([](std::string_view e) { return trim_string_view(e, [](char e) { return std::string_view("\n\t ").contains(e); }); })
            | std::views::filter([](std::string_view e) { return e.size() != 0; })
        );
        if (names.size() != values.size()) {
            throw std::runtime_error(fmt::format("`source` and `values` has non-matching number of items. {} vs {}.", names.size(), values.size()));
        }
        std::vector<enum_item_info> items;
        for (size_t i : std::views::iota(size_t(0), names.size())) {
            items.push_back(enum_item_info{
                .m_text = names[i],
                .m_value = values[i] });
        }
        init(items);
    }
    void init(const std::vector<enum_item_info>& items) {
        size_t all_sum = 0;
        size_t all_or = 0;
        for (auto& i : items) {
            size_t enum_bits = size_t(i.m_value);
            all_sum += enum_bits;
            all_or |= enum_bits;
        }
        if (all_sum == all_or) {
            m_wz_bit_mask = true;
        }

        for (auto& i : items) {
            m_items.emplace(i.m_value, i);
        }
    }


    auto display(TyEnum value) const -> std::string {
        if (!m_wz_bit_mask) {
            auto find = m_items.find(value);
            if (find == m_items.end()) return fmt::format("Unknown {:X}", size_t(value));
            return fmt::format("{}", find->second.m_text);
        }

        std::string res;
        size_t bits = size_t(value);
        for (auto& i : m_items) {
            bool valid = bits & size_t(i.first);
            if (m_style_compact) {
                res += valid ? i.second.m_text : (fmt::format("{: >{}}", "", i.second.m_text.size()));
            } else {
                if (valid) {
                    if (res.size() != 0) { res += " | "; }
                    res += i.second.m_text;
                }
            }
        }
        return res;
    }
};





namespace test_enum_display {


enum test_enum {
    v0 = 1,
    v1 = 2,
    v2 = 4,
    v3 = 8,
};

constexpr char test_enum_source[] = R"(
    v0,
    v1,
    v2,
    v3,
)";

inline void test() {

    //parse_macro_source<test_enum, "v0,v1,v2,v3", v0, v1, v2, v3>();
    enum_info<test_enum> info;
    info.init({
        { v0, "v0" },
        { v1, "v1" },
        { v2, "v2" },
        { v3, "v3" },
        });
    enum_info<test_enum> info2;
    info2.init("v0,v1,v2,v3", { v0,v1,v2,v3 });
    auto display = info.display(test_enum(15));
}

}



}










