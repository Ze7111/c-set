#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "../../pkgs/base/include.hh"

namespace lexer {
    enum token_type {
    /* =====-------- operators -------===== */
    OPTIONAL,     // !
    ALWAYS,       // ?
    ASSIGN,       // =
    LOGICAL_OR,   // |
    LOGICAL_AND,  // &
    SAFE,         // *
    GET,          // ->
    /* =====------- identifiers ------===== */
    LITERAL,
    /* =====-------- vartypes --------===== */
    VARTYPE,
    /* =====--------- numbers --------===== */
    NUMBER,
    /* =====--------- strings --------===== */
    STRING,
    /* =====---- inbuilt functions ---===== */
    INBUILT,
    /* =====-------- comments --------===== */
    COMMENT,
    /* =====-------- newlines --------===== */
    NEWLINE
};

struct token {
    u64 row;
    u64 col;
    token_type type;
    std::string value;
};

inline const char *staticMap(char chr) {
    switch (chr) {
    case '\a':
        return "\\a";
    case '\b':
        return "\\b";
    case '\t':
        return "\\t";
    case '\n':
        return "\\n";
    case '\v':
        return "\\v";
    case '\f':
        return "\\f";
    case '\r':
        return "\\r";
    }
    return nullptr;
}

inline std::string escape_cpp(const std::string &input) {
    std::stringstream s_stream;
    for (char chr : input) {
        const char *_str_ = staticMap(chr);
        if (_str_ != nullptr) {
            s_stream << _str_;
        } else if (isprint(static_cast<unsigned char>(chr)) == 0) {
            s_stream << "\\u" << std::hex << std::setfill('0') << std::setw(4)
               << (static_cast<unsigned int>(static_cast<unsigned char>(chr)));
        } else {
            s_stream << chr;
        }
    }
    return s_stream.str();
}

inline auto token_to_string(const token &tok) -> std::string {
    std::string type;
    switch (tok.type) {
    case OPTIONAL:
        type = "OPTIONAL";
        break;
    case ALWAYS:
        type = "ALWAYS";
        break;
    case ASSIGN:
        type = "ASSIGN";
        break;
    case LOGICAL_OR:
        type = "LOGICAL_OR";
        break;
    case LOGICAL_AND:
        type = "LOGICAL_AND";
        break;
    case SAFE:
        type = "SAFE";
        break;
    case LITERAL:
        type = "LITERAL";
        break;
    case VARTYPE:
        type = "VARTYPE";
        break;
    case NUMBER:
        type = "NUMBER";
        break;
    case STRING:
        type = "STRING";
        break;
    case INBUILT:
        type = "INBUILT";
        break;
    case COMMENT:
        type = "COMMENT";
        break;
    case NEWLINE:
        type = "NEWLINE";
        break;
    case GET:
        type = "GET";
        break;
    default:
        type = "UNKNOWN";
        break;
    }
    return std::string(colors::fg16::cyan) + "token" +
           std::string(colors::reset) + "(" +
           std::string(colors::fg16::yellow) + "type" +
           std::string(colors::reset) + "<" + std::string(colors::fg16::cyan) +
           type + std::string(colors::reset) + ">" + +", " +
           std::string(colors::fg16::yellow) + "value" +
           std::string(colors::reset) + ": " +
           std::string(colors::fg16::green) + '"' + escape_cpp(tok.value) +
           '"' + std::string(colors::reset) + ")";
}
}

#endif // __TOKEN_H__