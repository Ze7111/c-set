#ifndef __KEY_H__
#define __KEY_H__

#include "types/rints.hh"

struct TokenMappingKey {
    u64    k1;
    string k2;

    constexpr TokenMappingKey(u64 k1, string k2) : k1(k1), k2(std::move(k2)) {}
    constexpr TokenMappingKey(const TokenMappingKey &other) = default;
    constexpr TokenMappingKey(TokenMappingKey &&other) = default;
    constexpr TokenMappingKey &operator=(const TokenMappingKey &other) = default;
    constexpr TokenMappingKey &operator=(TokenMappingKey &&other) = default;
    constexpr ~TokenMappingKey() = default;
    
    constexpr TokenMappingKey() : k1(0), k2(L"") {}
    constexpr TokenMappingKey(const u64 k1) : k1(k1), k2(L"") {}
    constexpr TokenMappingKey(string k2) : k1(0), k2(std::move(k2)) {}


    constexpr bool operator==(const u64 &other) const {
        return k1 == other;
    }

    constexpr bool operator==(const string &other) const {
        return k2 == other;
    }

    constexpr bool operator==(const TokenMappingKey &other) const {
        return k1 == other.k1 && k2 == other.k2;
    }

    constexpr bool operator!=(const TokenMappingKey &other) const {
        return !(*this == other);
    }
};

namespace std {
    template <>
    struct hash<TokenMappingKey> {
        size_t operator()(const TokenMappingKey &key) const {
            return hash<u64>()(key.k1) ^ (hash<std::wstring>()(key.k2) << 1);
        }
    };
}

#endif // __KEY_H__