#ifndef __MATCH_H__
#define __MATCH_H__

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
    case '9'

#define digit _0_9 : case '-'

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
        UnexpectedCharacterError.error(std::string(1, '-')) \
            .fix(std::string(1, '-')) \
            .show(file_path, line, line_num, chr_index);

#define literal     \
    A_Z : case a_z: \
    case _0_9:      \
    case '_'

#define whitespace   \
    ' ' : case '\t': \
    case '\v':       \
    case '\f':       \
    case '\r':       \
    case '\n':       \
    case '\0'

#endif // __MATCH_H__