#ifndef __GENERATION_H__
#define __GENERATION_H__

#include <tuple>
#include <utility>

#include "types/estr.hh"
#include "types/key.hh"
#include "types/rints.hh"

#define ENUM_GEN(x, y) x,
#define MAKE_ENUM(name, X) enum class name { X }

#define MAP_GEN(x, y) {{Generation::hash_string<y>(), TokenKind::x}, y},
#define MAKE_MAP(name, X) \
    inline static const TokenMap name { X }

#define HASH_MAX 0xFFFFFFFFFFFFFFFFULL

namespace Lexer {
namespace Generation {
    consteval usize evaluated_str_length(const wchar_t *str, usize len = 0) {
        if (str[len] != '\0') {
            return evaluated_str_length(str, ++len);
        }

        return len;
    }

    constexpr u64 get_char_hash(const wchar_t chr, usize pos, u64 hash = 0) {
        return (((static_cast<u64>(chr) << ((pos & 7) + 1)) |
                 (static_cast<u64>(chr) >> ((pos & 3) + 1))) ^
                pos) ^
               (hash << 1) ^ (hash >> 3);
    }

    template <const usize N>
    consteval u64 hash_string(const wchar_t *str) {
        static_assert(N <= 256, "String too long for hashing");
        u64 hash = 0;

        (void)[&]<usize... I>(const wchar_t *s, std::index_sequence<I...>) {
            ((hash ^= get_char_hash(s[I], I, hash)), ...);
        }

        (str, std::make_index_sequence<N>{});

        return hash & HASH_MAX;
    }

    u64 hash_string(const string &str) {
        if (str.length() >= 256) {
            throw std::runtime_error("String too long for hashing");
        }

        u64 hash = 0;

        for (usize i = 0; i < str.length(); ++i) {
            hash ^= get_char_hash(str[i], i, hash);
        }

        return hash & HASH_MAX;
    }

    template <const EvaluatedString str>
    consteval u64 hash_string() {
        return hash_string<evaluated_str_length(str.value.data())>(
            str.value.data());
    }

    template <const EvaluatedString str>
    consteval std::tuple<u64, const wchar_t *> make_mapping_value() {
        if constexpr (evaluated_str_length(str.value.data()) == 0) {
            return {HASH_MAX, L""};
        }

        return {hash_string<str>(), str.value.data()};
    }
}  // namespace Generation
}  // namespace Lexer

#endif  // __GENERATION_H__