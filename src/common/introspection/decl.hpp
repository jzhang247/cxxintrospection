#pragma once

#if !defined(INCLUDED_INTROSPECTION_DECL_HPP)
#define INCLUDED_INTROSPECTION_DECL_HPP

namespace introspection {


template <typename Ty>
struct type_data;

template <int, typename Ty>
decltype(auto) get_field_name();

template <int, typename Ty>
auto& get_field(Ty& obj);
template <int, typename Ty>
const auto& get_field(const Ty& obj);
template <int, typename Ty>
auto&& get_field(Ty&& obj);
template <int, typename Ty>
const auto&& get_field(const Ty&& obj);


template <typename Ty>
struct has_reflection {
    static constexpr bool value = requires{ std::construct_at<type_data<Ty>>(nullptr); };
};


template <typename Ty>
static constexpr bool has_reflection_v = has_reflection<std::remove_cvref_t<Ty>>::value;


}

#endif