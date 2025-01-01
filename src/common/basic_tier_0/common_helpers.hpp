#pragma once


// Alternative expression of pointer
// Intended for C++ style explicit cast `ptr<TyDerived*>(pBase)`
// which eliminates annoying parenthesis
template <typename Ty>
using PTR = Ty*;
template <typename Ty>
using CPTR = const PTR<Ty>;

// ptrrv
//  Get the pointer of a right value
template<typename Ty>
Ty* ptrrv(Ty&& v) { return &v; }




#pragma region min max
#define NOMINMAX
#undef max
#undef min

template <typename T>
const T& max(const T& a, const T& b) { return a >= b ? a : b; }
template <typename T>
const T& min(const T& a, const T& b) { return a <= b ? a : b; }

template <typename TyFirst, typename... TyRest>
auto va_min(TyFirst& first, TyRest&... rest) -> const auto& {
    
}


#pragma endregion



#pragma region array length
// Here we define the term length to be number of elements and size to be number of bytes
template <size_t len, typename Ty>
std::tuple<size_t, Ty*> arr_ptr_len(Ty(&arg)[len]) {
    return std::tuple<size_t, Ty*>(len, arg);
}
template <typename Ty, size_t len>
constexpr size_t arr_len(const Ty(&)[len]) {
    return len;
}

template <typename Ty>
struct arr_type_len_impl {};

template <typename Ty, size_t len>
struct arr_type_len_impl<Ty[len]> {
    static constexpr size_t value = len;
};

template <typename Ty>
static constexpr size_t arr_type_len = arr_type_len_impl<Ty>::value;
#pragma endregion



// actual_sizeof
#pragma region
// actual_sizeof
//      return 0 if type is empty otherwise sizeof type
template <typename Ty>
struct actual_sizeof {
    static constexpr size_t value = std::is_empty_v<Ty> ? 0 : sizeof(Ty);
};

template <typename Ty>
using actual_sizeof_v = typename actual_sizeof<Ty>::value;
#pragma endregion




// ptr_offset_raw
template <typename Ty>
Ty* ptr_offset_raw(Ty* ptr, std::ptrdiff_t diff) {
    return PTR<Ty>(size_t(ptr) + diff);
}



// template struct storage_dummy
#pragma region
template <size_t size, size_t align>
struct storage_dummy {
    alignas(align) char m[size];
};


template <typename Ty>
struct storage_dummy_by_type_impl {
    static const size_t size = sizeof(Ty);
    static const size_t align = alignof(Ty);
    using type = storage_dummy<size, align>;
};

template <typename Ty>
using storage_dummy_by_type = typename storage_dummy_by_type_impl<Ty>::type;
#pragma endregion

// Trim math functions
#pragma region

// TrimCeiling
//      the smallest whole multiple of `trim` greater than `value`
template <typename Ty>
constexpr Ty TrimCeiling(Ty value, Ty trim) {
    return (value + trim - 1) / trim * trim;
}


// DivCeiling
//      ceiling division
template <typename Ty>
constexpr Ty DivCeiling(Ty value, Ty trim) {
    return (value + trim - 1) / trim;
}

// Log2Ceiling
//      the smallest power of 2 greater than `value`
template <typename Ty>
constexpr Ty Log2Ceiling(Ty value) {
    static_assert(std::is_integral_v<Ty>, "Ty should be a integral value");
    value -= 1;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    value += 1;
    return value;
}

// SumBitwise
//      return number of bits set to 1
template <typename Ty>
constexpr Ty SumBitwise(Ty value) {
    static_assert(std::is_integral_v<Ty>, "Ty should be a integral value");

    // return value filled with required number of bits segments alternatingly all set to zero and one
    // lower bits set to true
    auto get_filter = [](int level)->Ty {
        Ty value = 0;
        for (unsigned int t = 0; t < 8 * sizeof(Ty); t++) {
            value *= 2;
            value += (t / level) % 2;
        }
        return value;
    };

    size_t count_iter = 6;
    for (size_t t = 0; t < count_iter; t++) {
        Ty filter = get_filter(1 << t);
        Ty high_part = value & ~filter;
        Ty low_part = value & filter;
        value = (high_part >> (1 << t)) + low_part;
    }
    return value;
}

#pragma endregion


// method memswap
#pragma region
inline void memswap(void* p1, void* p2, size_t length) {
    auto pp1 = (char*)p1;
    auto pp2 = (char*)p2;

    static_assert(sizeof(*pp1) == 1, "Implementation error.");

    if (size_t(pp1 - pp2) < length || size_t(pp2 - pp1) < length) {
        // error
        return;
    }


    for (size_t t = 0; t < length; t++) {
        char swap = pp1[t];
        pp1[t] = pp2[t];
        pp2[t] = swap;
    }
}
#pragma endregion




// SFINAE
#pragma region
struct SFINAE_Match {
    char m[1];
};
struct SFINAE_Miss {
    char m[2];
};
#pragma endregion

// Member detection
#pragma region
#define DO_DEFINE_MemberDetector_2Arg(M,H)                                  \
template<typename T> struct H { private:                                    \
    template<typename T1> static SFINAE_Match test(decltype(&T1::M));       \
    template<typename T1> static SFINAE_Miss  test(...);                    \
public: enum { value = (sizeof(test<T>(0)) == sizeof(SFINAE_Match)), }; };


#define DO_DEFINE_MemberDetector_1Arg(M) DO_DEFINE_MemberDetector_2Arg(M, CONCATE(has_, M))
// Define a member detector
// DO_DEFINE_MemberDetector(member_name, [detector name])
#define DO_DEFINE_MemberDetector(...) MACRO_OVERLOAD_2(__VA_ARGS__, DO_DEFINE_MemberDetector_2Arg, DO_DEFINE_MemberDetector_1Arg, ...)(__VA_ARGS__)
#pragma endregion

// Late template eval
#pragma region

template <template <typename...> typename TyTemplate, typename... _TyArgs>
struct typename_expr {
    template <typename... TyArgs>
    using rebind_args = TyTemplate<TyArgs...>;

    template <template <typename...> typename TyOther = TyTemplate>
    using rebind_template = TyOther<_TyArgs...>;

    template <typename _discarded = void>
    using evaluate = TyTemplate<_TyArgs...>;
};

template <typename Ty>
struct eval_typename_expr {
    using type = Ty;
};
template <template <typename...> typename TyTemplate, typename... _TyArgs>
struct eval_typename_expr<typename_expr<TyTemplate, _TyArgs...>> {
    using type = typename typename_expr<TyTemplate, _TyArgs...>::template evaluate<>;
};

template <typename Ty>
using eval_typename_expr_t = typename eval_typename_expr<Ty>::type;
#pragma endregion

// is_standard_lifecycle
#pragma region
template <typename Ty>
struct is_standard_lifecycle {
    static constexpr bool value = true
        && std::is_nothrow_move_constructible_v<Ty>
        && std::is_copy_constructible_v<Ty>
        && std::is_nothrow_destructible_v<Ty>;
};
template <typename Ty>
constexpr bool is_standard_lifecycle_v = is_standard_lifecycle<Ty>::value;
#pragma endregion


// Template manipulation
namespace template_manipulation {

template <typename... Ty>
struct va_types {};







#pragma region bind rebind
template <template<typename...>typename Tp1, typename Ty2>
struct bind_template_impl;
template <template<typename...>typename Tp1, template<typename...>typename Tp2, typename... VArgs2>
struct bind_template_impl<Tp1, Tp2<VArgs2...>> {
    using type = Tp1<VArgs2...>;
};
//   bind_template<Tp1, Tp2<VArgs2...>>
// ->Tp1<VArgs2...>
template<template<typename...>typename Tp1, typename Ty2>
using bind_template = typename bind_template_impl<Tp1, Ty2>::type;

template <typename Ty1, typename Ty2>
struct rebind_template_impl;
template <template<typename...>typename Tp1, typename... VArgs1, template<typename...>typename Tp2, typename... VArgs2>
struct rebind_template_impl<Tp1<VArgs1...>, Tp2<VArgs2...>> {
    using type = Tp1<VArgs2...>;
};
//   rebind_template<Tp1<VArgs1...>, Tp2<VArgs2...>>
// ->Tp1<VArgs2...>
template <typename Ty1, typename Ty2>
using rebind_template = typename rebind_template_impl<Ty1, Ty2>::type;
#pragma endregion

#pragma region va_append_right
template <typename Ty1, typename Ty2>
struct va_append_right_impl;
template <typename... VArgs1, typename Ty2>
struct va_append_right_impl<va_types<VArgs1...>, Ty2> {
    using type = va_types<VArgs1..., Ty2>;
};
//   va_append_right_impl<Tp1<VArgs1...>, Ty2>
// ->Tp1<Vargs1..., Ty2>
template <typename Ty1, typename Ty2>
using va_append_right = typename va_append_right_impl<Ty1, Ty2>::type;
#pragma endregion

#pragma region va_inverse
template <typename Ty>
struct va_inverse_impl;
template <typename ArgFirst, typename... ArgRest>
struct va_inverse_impl<va_types<ArgFirst, ArgRest...>> {
    using type = va_append_right<typename va_inverse_impl<va_types<ArgRest...>>::type, ArgFirst>;
};
template <>
struct va_inverse_impl<va_types<>> {
    using type = va_types<>;
};
//   va_inverse<Tp<Arg1, Arg2, ..., ArgN>
// ->Tp<ArgN, ..., Arg2, Arg1>
template <typename Ty>
using va_inverse = rebind_template<Ty, typename va_inverse_impl<bind_template<va_types, Ty>>::type>;
#pragma endregion

#pragma region va_get

template <typename TyVaTypes, size_t index>
struct va_get_impl;
template <typename TyFirst, typename... TyRest, size_t index>
struct va_get_impl<va_types<TyFirst, TyRest...>, index> {
    using type = va_get_impl<va_types<TyRest...>, index - 1>;
};
template <typename TyFirst, typename... TyRest>
struct va_get_impl<va_types<TyFirst, TyRest...>, 0> {
    using type = TyFirst;
};
template <typename TyVaTypes, size_t index>
using va_get = typename va_get_impl<TyVaTypes, index>::type;


#pragma endregion


#pragma region va_count
template <typename... TyArgs>
struct va_count_impl;
template <>
struct va_count_impl<> {
    static constexpr int value = 0;
};
template <typename TyFirst, typename... TyRest>
struct va_count_impl<TyFirst, TyRest...> {
    static constexpr int value = 1 + va_count_impl<TyRest...>::value;
};

template <typename... VTy>
static constexpr int va_count = va_count_impl<VTy...>::value;
template <typename Ty>
static constexpr int type_va_count = bind_template<va_count_impl, Ty>::value;
#pragma endregion

#pragma region va_sequence
template <int... t>
struct va_sequence;

template <int next, int remaining, int... sequence>
struct make_va_sequence_impl;

template <int next, int remaining, int... sequence>
struct make_va_sequence_impl {
    using type = typename make_va_sequence_impl<next + 1, remaining - 1, sequence..., next>::type;
};

template <int next, int... sequence>
struct make_va_sequence_impl<next, 0, sequence...> {
    using type = va_sequence<sequence...>;
};

template <int t>
using make_va_sequence = typename make_va_sequence_impl<0, t>::type;

#pragma endregion

#pragma region is_any_same
template <typename Ty, typename... TyOthers>
struct is_any_same_impl {
    static constexpr bool value = (false || ... || std::is_same_v<Ty, TyOthers>);
};

template <typename Ty, typename... TyOthers>
inline constexpr bool is_any_same = is_any_same_impl<Ty, TyOthers...>::value;

#pragma endregion

#pragma region has_same_template
// template comparasion
// Terminology clearification:
// `std::tuple` is a type template
// `std::tuple<int, string>` is a tempalted type
template <typename _Left, typename _Right>
struct has_same_template_impl {
    static constexpr bool value = false;
};
template <template <typename...> typename _Same, typename... ArgsLeft, typename... ArgsRight>
struct has_same_template_impl<_Same<ArgsLeft...>, _Same<ArgsRight...>> {
    static constexpr bool value = true;
};
template <typename _Left, typename _Right>
constexpr bool has_same_template = has_same_template_impl<_Left, _Right>::value;
#pragma endregion
}






#pragma region invoke_type
template <typename Ty>
struct invoke_type_impl {
    using type = Ty;
};
template <typename Ty>
    requires(requires{  Ty::operator(); })
struct invoke_type_impl<Ty> {
    using type = decltype(Ty::operator());
};
template <typename Ty>
using invoke_type = invoke_type_impl<Ty>::type;
#pragma endregion

#pragma region func_type_info
template <typename Ty>
struct func_type_info;

template <typename _TyRet, typename... _TyArgs>
struct func_type_info<_TyRet(_TyArgs...)> {
    using TyVaArgs = template_manipulation::va_types<_TyArgs...>;
    using TyRet = _TyRet;
    using TyThis = void;
};
template <typename _TyRet, typename... _TyArgs>
struct func_type_info<_TyRet(&)(_TyArgs...)> {
    using TyVaArgs = template_manipulation::va_types<_TyArgs...>;
    using TyRet = _TyRet;
    using TyThis = void;
};
template <typename _TyRet, typename... _TyArgs>
struct func_type_info<_TyRet(*)(_TyArgs...)> {
    using TyVaArgs = template_manipulation::va_types<_TyArgs...>;
    using TyRet = _TyRet;
    using TyThis = void;
};

template <typename _TyRet, typename _TyThis, typename... _TyArgs>
struct func_type_info<_TyRet(_TyThis::*)(_TyArgs...)&> {
    using TyVaArgs = template_manipulation::va_types<_TyArgs...>;
    using TyRet = _TyRet;
    using TyThis = _TyThis&;
};

template <typename _TyRet, typename _TyThis, typename... _TyArgs>
struct func_type_info<_TyRet(_TyThis::*)(_TyArgs...)const&> {
    using TyVaArgs = template_manipulation::va_types<_TyArgs...>;
    using TyRet = _TyRet;
    using TyThis = const _TyThis&;
};

template <typename _TyRet, typename _TyThis, typename... _TyArgs>
struct func_type_info<_TyRet(_TyThis::*)(_TyArgs...)&&> {
    using TyVaArgs = template_manipulation::va_types<_TyArgs...>;
    using TyRet = _TyRet;
    using TyThis = _TyThis&&;
};
#pragma endregion




// carrier
#pragma region
template <typename Ty>
struct carrier {
    Ty m;

    //carrier(const carrier<Ty>& that) requires(std::is_copy_constructible_v<Ty>) = default;
    //carrier(carrier<Ty>&& that) requires(std::is_move_constructible_v<Ty>) = default;

    template <typename... TyArgs>
    carrier(TyArgs&&... args) : m(std::forward<TyArgs>(args)...) {}

};

template <typename Ty>
void swap(carrier<Ty>& left, carrier<Ty>& right)
    requires(requires { swap(left.m, right.m); }) {
    swap(left.m, right.m);
}

template <typename Ty>
auto operator<=>(const carrier<Ty>& left, const carrier<Ty>& right) requires(requires{ left.m <=> right.m; }) {
    return left.m <=> right.m;
}

template <typename Ty>
auto operator==(const carrier<Ty>& left, const carrier<Ty>& right) requires(requires{left.m == right.m; }) {
    return left.m == right.m;
}

template <>
struct carrier<void> {};


template <bool cond, typename Ty>
struct carrier_cond {
    using type = carrier<std::conditional_t<cond, Ty, void>>;
};

template <bool cond, typename Ty>
using carrier_cond_t = typename carrier_cond<cond, Ty>::type;

#pragma endregion






