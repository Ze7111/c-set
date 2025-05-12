#ifndef __ESTR_H__
#define __ESTR_H__

#include <array>
#include <utility>

template <size_t N>
struct EvaluatedString {
    std::array<wchar_t, N> value;

    consteval EvaluatedString(const wchar_t (&str)[N]) {
        (void)[&]<size_t... I>(const wchar_t (&str)[N],
                               std::index_sequence<I...>) {
            ((value[I] = str[I]), ...);
        }
        (str, std::make_index_sequence<N>{});
    }
};


#endif // __ESTR_H__