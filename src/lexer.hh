#ifndef __LEXER_H__
#define __LEXER_H__

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>

#include "../pkgs/base/include.hh"

/*
    (! ? = | & *) // operator
    $(A..Za..z0..9-_) // identifier ($(varname))
    0..9 // number
    " // string
    @A..Za..z0..9-_ // inbuilt function
    # // comment
    \n // newline
*/

#define A_Z         \
    'A' : case 'B': \
    case 'C':       \
    case 'D':       \
    case 'E':       \
    case 'F':       \
    case 'G':       \
    case 'H':       \
    case 'I':       \
    case 'J':       \
    case 'K':       \
    case 'L':       \
    case 'M':       \
    case 'N':       \
    case 'O':       \
    case 'P':       \
    case 'Q':       \
    case 'R':       \
    case 'S':       \
    case 'T':       \
    case 'U':       \
    case 'V':       \
    case 'W':       \
    case 'X':       \
    case 'Y':       \
    case 'Z'

#define a_z         \
    'a' : case 'b': \
    case 'c':       \
    case 'd':       \
    case 'e':       \
    case 'f':       \
    case 'g':       \
    case 'h':       \
    case 'i':       \
    case 'j':       \
    case 'k':       \
    case 'l':       \
    case 'm':       \
    case 'n':       \
    case 'o':       \
    case 'p':       \
    case 'q':       \
    case 'r':       \
    case 's':       \
    case 't':       \
    case 'u':       \
    case 'v':       \
    case 'w':       \
    case 'x':       \
    case 'y':       \
    case 'z'

#define _0_9        \
    '0' : case '1': \
    case '2':       \
    case '3':       \
    case '4':       \
    case '5':       \
    case '6':       \
    case '7':       \
    case '8':       \
    case '9':       \
    case ','

#define digit _0_9

#define punctuation(next, tokens)             \
    '!' : tokens.push_back({OPTIONAL, "!"});  \
    continue;                                 \
    case '?':                                 \
        tokens.push_back({ALWAYS, "?"});      \
        continue;                             \
    case '=':                                 \
        tokens.push_back({ASSIGN, "="});      \
        continue;                             \
    case '|':                                 \
        tokens.push_back({LOGICAL_OR, "|"});  \
        continue;                             \
    case '&':                                 \
        tokens.push_back({LOGICAL_AND, "&"}); \
        continue;                             \
    case '*':                                 \
        tokens.push_back({SAFE, "*"});        \
        continue;                             \
    case '-':                                 \
        if (next == '>') {                    \
            tokens.push_back({GET, "->"});    \
            ++chr_index;                      \
            ++ ++tail;                        \
            continue;                         \
        }                                     \
        std_v2::print("invalid token start -");

#define literal     \
    A_Z : case a_z: \
    case digit:     \
    case '_'

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
    token_type type;
    std::string value;
};

const char *staticMap(char c) {
    switch (c) {
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

std::string escape_cpp(const std::string &input) {
    std::stringstream ss;
    for (char c : input) {
        const char *_str_ = staticMap(c);
        if (_str_) {
            ss << _str_;
        } else if (!isprint(static_cast<unsigned char>(c))) {
            ss << "\\u" << std::hex << std::setfill('0') << std::setw(4)
               << (static_cast<unsigned int>(static_cast<unsigned char>(c)));
        } else {
            ss << c;
        }
    }
    return ss.str();
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

#define TIME_IT(obj)  \
    auto start = std::chrono::high_resolution_clock::now(); \
    obj; \
    auto stop = std::chrono::high_resolution_clock::now(); \
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); \
    std_v2::print("line: ", __LINE__, " took ", std::string(colors::fg16::red), duration, std::string(colors::reset))






inline auto parse(std::stringstream &f_data) -> std::vector<token> {
    std::stringstream partial;
    std::vector<token> tokens = {};
    std::string line;

    u64 head = 0;
    u64 tail = 0;

    // $(varname) or @function or "string" or 0..9|L|U|_ or #comment
    bool in_var = false;             // $(varname)
    bool in_string = false;          // "string"
    bool in_literal = false;         // a..z(A..Z)(0..9)(_)
    bool in_multi_line_str = false;  // "\n\n\n" multiple lines not "\\n"

    bool in_func = false;            // @function
    bool in_digit = false;           // 0..9(L)(U)(_)

    // ======------- for each line -------====== //
    for (u64 line_num = 0; std::getline(f_data, line); line_num++) {
        if (line.empty()) {
            continue;
        }
        line += '\n';
        tail = 0;
        head = 0;
        // ======------- for each char -------====== //
        for (u64 chr_index = 0; chr_index < line.size(); chr_index++) {
            char chr = line[chr_index];

            if (in_string) {
                switch (chr) {
                case '\\':
                    head += 2;
                    ++chr_index;
                    continue;
                case '"':
                    if (in_multi_line_str) {
                        partial.write(line.data() + tail, head);
                        tokens.push_back({STRING, partial.str()});
                        partial.str("");
                        partial.clear();
                        tail = head;
                        in_string = false;
                        in_multi_line_str = false;
                        continue;
                    } else {
                        tokens.push_back({STRING, line.substr(++tail, head)});
                        tail += head;
                        in_string = false;
                        continue;
                    }
                case '\n':
                    partial.write(line.data() + tail, ++head);
                    in_multi_line_str = true;
                    std_v2::print("in multi line string");
                    continue;
                default:
                    ++head;
                    continue;
                }
            }

            if (in_digit) {
                switch (chr) {
                case literal:
                    switch (chr) {
                    case digit:
                        ++head;
                        continue;
                    default:
                        std_v2::print(
                            "got a unknown here excepted only numbers.");
                    }
                default:
                    tokens.push_back({NUMBER, line.substr(tail, ++head)});
                    tail += head;
                    in_digit = false;
                    break;
                }
            }

            if (in_literal) {
                switch (chr) {
                case literal:
                    ++head;
                    continue;
                default:
                    tokens.push_back({LITERAL, line.substr(tail, ++head)});
                    tail += head;
                    in_literal = false;
                    break;
                }
            }

            if (in_var) {
                switch (chr) {
                case literal:
                case '-':
                    ++head;
                    continue;
                case ')':
                    ++head;
                    tokens.push_back({VARTYPE, line.substr(tail, ++head)});
                    tail += head;
                    in_var = false;
                    break;
                default:
                    tokens.push_back({VARTYPE, line.substr(tail - 1, head)});
                    tail += head - 1;
                    in_var = false;
                    break;
                }
            }

            head = 0;

            // ======------- operator -------====== //
            switch (chr) {
            case '#':  // comment
                tokens.push_back({COMMENT, line.substr(chr_index)});
                chr_index = line.size();
                continue;
            case '$':
                if (chr_index + 1 < line.size() && line[chr_index + 1] == '(') {
                    in_var = true;
                    ++head;
                    ++chr_index;
                    continue;
                }
                log(line, "invalid line expected $ var");
            case literal:
                switch (chr) {
                case digit:
                    in_digit = true;
                    continue;
                }
                in_literal = true;
                continue;
            case punctuation(chr_index + 1 < line.size()
                ? line[chr_index + 1]
                : chr, tokens);
            case '"':
                ++tail;
                in_string = true;
                continue;
            case '\n':
                tokens.push_back({NEWLINE, "\n"});
                continue;
            }

            ++tail;

        }  // ======------- end char -------====== //

        if (in_string && !in_multi_line_str) {
            tokens.push_back({STRING, line.substr(tail, ++head)});
            in_string = false;
        }

        if (in_literal) {
            tokens.push_back({LITERAL, line.substr(tail, ++head)});
            in_literal = false;
        }

        if (in_var) {
            ++head;
            tokens.push_back({VARTYPE, line.substr(tail, ++head)});
            in_var = false;
        }

        if (!tokens.empty() && tokens.back().value.empty()) {
            tokens.pop_back();
        }
    }  // ======------- end line -------====== //
    return tokens;
}

inline auto lex(std::string &file) -> std::vector<token> {
    std::ifstream file_stream(file);
    if (!file_stream.is_open()) {
        std::cerr << "failed to open file: " << file << std::endl;
        return {};
    }

    std::stringstream f_data;
    f_data << file_stream.rdbuf();
    file_stream.close();

    TIME_IT(parse(f_data));
    std::vector<token> parsed = parse(f_data);
    for (const token &tok : parsed) {
        std_v2::print(lexer::token_to_string(tok));
    }

    std_v2::print(duration.count());

    return parsed;
}
}  // namespace lexer
#endif  // __LEXER_H__