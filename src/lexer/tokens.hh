#ifndef __TOKENS_H__
#define __TOKENS_H__

#include <map>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "lexer/generation.hh"
#include "lexer/lexemes.hh"

namespace Lexer {
MAKE_ENUM(TokenKind : u8, TOKENS(ENUM_GEN));

struct KeyT {
    u64 k1;
    TokenKind k2;

    constexpr KeyT(u64 k1, TokenKind k2)
        : k1(k1)
        , k2(std::move(k2)) {}
    constexpr KeyT()
        : k1(0)
        , k2(TokenKind::END_OF_FILE) {}
    constexpr KeyT(const u64 k1)
        : k1(k1)
        , k2(TokenKind::END_OF_FILE) {}
    constexpr KeyT(TokenKind k2)
        : k1(0)
        , k2(std::move(k2)) {}

    constexpr KeyT(const KeyT &other) = default;
    constexpr KeyT(KeyT &&other) = default;
    constexpr KeyT &operator=(const KeyT &other) = default;
    constexpr KeyT &operator=(KeyT &&other) = default;
    constexpr ~KeyT() = default;

    constexpr bool operator==(const u64 &other) const { return k1 == other; }

    constexpr bool operator==(const TokenKind &other) const {
        return k2 == other;
    }

    constexpr bool operator==(const KeyT &other) const {
        return k1 == other.k1 || k2 == other.k2;
    }

    constexpr bool operator!=(const KeyT &other) const {
        return !(*this == other);
    }
};

struct KeyHash {
    using is_transparent = void;

    size_t operator()(const KeyT &k) const {
        return std::hash<u64>()(k.k1) ^
               std::hash<int>()(static_cast<int>(k.k2));
    }
    size_t operator()(u64 k1) const { return std::hash<u64>()(k1); }
    size_t operator()(TokenKind k2) const {
        return std::hash<int>()(static_cast<int>(k2));
    }
};

struct KeyEqual {
    using is_transparent = void;

    bool operator()(const KeyT &lhs, const KeyT &rhs) const {
        return lhs == rhs;
    }
    bool operator()(const KeyT &lhs, u64 rhs) const { return lhs.k1 == rhs; }
    bool operator()(const KeyT &lhs, TokenKind rhs) const {
        return lhs.k2 == rhs;
    }
};
}  // namespace Lexer

namespace std {
template <>
struct hash<Lexer::KeyT> {
    size_t operator()(const Lexer::KeyT &key) const {
        return hash<u64>()(key.k1) ^ (hash<Lexer::TokenKind>()(key.k2) << 1);
    }
};
}  // namespace std

namespace Lexer {

class Token {
  public:
    using TokenMap = std::unordered_map<KeyT, string>;

    MAKE_MAP(Mapping, TOKENS(MAP_GEN));

  public:
    TokenKind kind;
    string value;
    usize line;
    usize column;
    usize length;
    usize offset;
    string filename;

  public:
    Token(TokenKind kind, string value, usize line, usize column, usize length,
          usize offset, string filename)
        : kind(kind)
        , value(std::move(value))
        , line(line)
        , column(column)
        , length(length)
        , offset(offset)
        , filename(std::move(filename)) {}
    Token(TokenKind kind, string value, string filename = L"")
        : kind(kind)
        , value(std::move(value))
        , line(0)
        , column(0)
        , length(0)
        , offset(0)
        , filename(std::move(filename)) {}
    Token(TokenKind kind)
        : kind(kind)
        , line(0)
        , column(0)
        , length(0)
        , offset(0) {}
    Token()
        : kind(TokenKind::END_OF_FILE)
        , line(0)
        , column(0)
        , length(0)
        , offset(0) {}

    Token(const Token &other) = default;
    Token(Token &&other) = default;
    Token &operator=(const Token &other) = default;
    Token &operator=(Token &&other) = default;
    Token &operator=(TokenKind kind) {
        this->kind = kind;
        this->value = Mapping.at(kind);
        return *this;
    }

    ~Token() = default;

    bool operator==(const Token &other) const {
        return kind == other.kind && value == other.value &&
               line == other.line && column == other.column &&
               length == other.length && offset == other.offset &&
               filename == other.filename;
    }

    bool operator!=(const Token &other) const { return !(*this == other); }
    bool operator==(TokenKind kind) const { return this->kind == kind; }
    bool operator!=(TokenKind kind) const { return !(*this == kind); }
};
}  // namespace Lexer

#undef ENUM_GEN
#undef MAKE_ENUM
#undef MAP_GEN
#undef MAKE_MAP

#endif  // __TOKENS_H__