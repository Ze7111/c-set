#ifndef __LEXER_H__
#define __LEXER_H__

/*
    (! ? = | & *) // operator
    $(A..Za..z0..9-_) // identifier ($(varname))
    0..9 // number
    " // string
    @A..Za..z0..9-_ // inbuilt function
    # // comment
    \n // newline
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>  // maybe remove

#include "../../pkgs/base/include.hh"
#include "../shared/match.hh"
#include "errors.hh"
#include "token.hh"

namespace lexer {
/*
error: unknown delimeter ';'
     ╰─> c++.c-set-template/.template:10:28
$(NUMBER)       = 1,282,382;
                           ^
fix: remove ';' from the line
*/



inline auto parse_line(const std::string &file_path, std::stringstream &partial,
                       std::vector<token> &tokens, std::string &line, u64 &head,
                       u64 &tail, bool &in_var, bool &in_string,
                       bool &in_literal, bool &in_multi_line_str, bool &in_func,
                       bool &in_digit, char &chr, u64 &line_num) -> void {
    for (u64 chr_index = 0; chr_index < line.size(); chr_index++) {
        chr = line[chr_index];

        if (in_string) {
            switch (chr) {
            case '\\':
                head += 2;
                ++chr_index;
                continue;
            case '"':
                if (in_multi_line_str) {
                    partial.write(line.data() + tail, head);
                    tokens.push_back({line_num, chr_index, STRING, partial.str()});
                    partial.str("");
                    partial.clear();
                    tail = head;
                    in_string = false;
                    in_multi_line_str = false;
                    continue;
                } else {
                    tokens.push_back({line_num, chr_index, STRING, line.substr(++tail, head)});
                    tail += head;
                    in_string = false;
                    continue;
                }
            case '\n':
                partial.write(line.data() + tail, ++head);
                in_multi_line_str = true;
                continue;
            default:
                ++head;
                continue;
            }
        }

        if (in_digit) {
            switch (chr) {
            case literal:
            case ',':
                switch (chr) {
                case digit:
                    ++head;
                    continue;
                default:
                    // error not a digit
                    UnexpectedCharacterError.error(std::string(1, chr))
                        .fix(std::string(1, chr))
                        .show(file_path, line, line_num, chr_index);
                }
            default:
                tokens.push_back({line_num, chr_index, NUMBER, line.substr(tail, ++head)});
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
                tokens.push_back({line_num, chr_index, LITERAL, line.substr(tail, ++head)});
                tail += head;
                in_literal = false;
                break;
            }
        }

        if (in_var) {
            switch (chr) {
            case literal:
                ++head;
                continue;
            case ')':
                ++ ++head;
                if (line[tail] == ' ') {
                    tokens.push_back({line_num, chr_index, VARTYPE, line.substr(tail + 1, head)});
                } else {
                    tokens.push_back({line_num, chr_index, VARTYPE, line.substr(tail, head)});
                }
                tail += head;
                in_var = false;
                break;
            default:
                // invalid line expected ) var
                InvalidSyntaxError.error(line.substr(tail, head))
                    .fix("excepted $(...)")
                    .show(file_path, line, line_num, chr_index);
            }
        }

        if (in_func) {
            switch (chr) {
            case literal:
                ++head;
                continue;
            case whitespace:
                tokens.push_back({line_num, chr_index, INBUILT, line.substr(tail, head + 1)});
                tail += head;
                in_func = false;
                break;
            default:
                InbuiltFunctionError.error(std::string(1, chr))
                    .fix(std::string(1, chr))
                    .show(file_path, line, line_num, chr_index);
            }
        }

        head = 0;

        // ======------- operator -------====== //
        switch (chr) {
        case '#':  // comment
            tokens.push_back({line_num, chr_index, COMMENT, line.substr(chr_index)});
            chr_index = line.size();
            continue;
        case '$':
            if (chr_index + 1 < line.size() && line[chr_index + 1] == '(') {
                in_var = true;
                ++head;
                ++chr_index;
                continue;
            }
            // invalid line expected $ var
            InvalidSyntaxError.error("$").fix("$").show(file_path, line,
                                                        line_num, chr_index);
        case '@':
            in_func = true;
            continue;
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
                : chr, tokens, line_num, chr_index);
            case '"':
                ++tail;
                in_string = true;
                continue;
            case '\n':
                tokens.push_back({line_num, chr_index, NEWLINE, "\n"});
                continue;
        }

        ++tail;
    }
}
inline auto parse(std::stringstream &f_data,
                  const std::string &file_path) -> std::vector<token> {
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

    bool in_func = false;   // @function
    bool in_digit = false;  // 0..9(L)(U)(_)

    char chr = '\0';

    // ======------- for each line -------====== //
    for (u64 line_num = 0; std::getline(f_data, line); line_num++) {
        if (line.empty()) {
            continue;
        }
        // ======------- reset -------====== //
        line += '\n';
        tail = 0;
        head = 0;

        // ======------- for each char -------====== //
        parse_line(file_path, partial, tokens, line, head, tail, in_var,
                   in_string, in_literal, in_multi_line_str, in_func, in_digit,
                   chr, line_num);
        
        // ======--------- end char ---------====== //

        if (in_string && !in_multi_line_str) {
            tokens.push_back({line_num, line.size(), STRING, line.substr(tail, ++head)});
            in_string = false;
        }

        if (in_literal) {
            tokens.push_back({line_num, line.size(), LITERAL, line.substr(tail, ++head)});
            in_literal = false;
        }

        if (in_var) {
            ++head;
            tokens.push_back({line_num, line.size(), VARTYPE, line.substr(tail, ++head)});
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

    // TIME_IT(parse(f_data, file));
    std::vector<token> parsed = parse(f_data, file);

    if (!error_occured) {
        for (const token &tok : parsed) {
            std::print(lexer::token_to_string(tok));
        }
    }

    return parsed;
}
}  // namespace lexer
#endif  // __LEXER_H__
