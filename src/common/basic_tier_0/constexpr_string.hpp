#pragma once





template<typename _TyChar, std::size_t _length>
struct constexpr_string {
    using TyChar = _TyChar;
    static constexpr std::size_t length = _length;

    TyChar value[_length];

    constexpr constexpr_string(const _TyChar(&str)[_length]) {
        for (int t = 0; t < _length; t++) { value[t] = str[t]; }
    }
    template <typename... TyArgs>
    constexpr constexpr_string(TyArgs... args) {
        [&] <size_t... indices>(std::index_sequence<indices...>) {
            ((value[indices] = args),...);
        }(std::index_sequence_for<TyArgs...>());
    }
};








