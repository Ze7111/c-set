#ifndef __LEXEMES_H__
#define __LEXEMES_H__

#define TOKENS(GEN)            \
    /* keywords */             \
    GEN(IF, L"if")             \
    GEN(ELSE, L"else")         \
    GEN(VAR, L"var")           \
    GEN(FUNCTION, L"function") \
    GEN(IMPORT, L"import")     \
    GEN(WHILE, L"while")       \
    GEN(RETURN, L"return")     \
    GEN(BREAK, L"break")       \
    GEN(CONTINUE, L"continue") \
    GEN(FOR, L"for")           \
                               \
    /* arithmetic operators */ \
    GEN(ADD, L"+")             \
    GEN(SUB, L"-")             \
    GEN(MUL, L"*")             \
    GEN(POW, L"**")            \
    GEN(DIV, L"/")             \
    GEN(MOD, L"%")             \
                               \
    /* comparison operators */ \
    GEN(LT, L"<")              \
    GEN(GT, L">")              \
    GEN(LTE, L"<=")            \
    GEN(GTE, L">=")            \
    GEN(EQ, L"==")             \
    GEN(NEQ, L"!=")            \
                               \
    /* assignment */           \
    GEN(ASSIGN, L"=")          \
    GEN(PLUS_ASSIGN, L"+=")    \
    GEN(MINUS_ASSIGN, L"-=")   \
    GEN(MUL_ASSIGN, L"*=")     \
    GEN(DIV_ASSIGN, L"/=")     \
                               \
    /* logical operators */    \
    GEN(AND, L"&&")            \
    GEN(OR, L"||")             \
    GEN(NOT, L"!")             \
                               \
    /* punctuation */          \
    GEN(DOT, L".")             \
    GEN(COMMA, L",")           \
    GEN(SEMICOLON, L";")       \
    GEN(ARR, L"->")            \
    GEN(END_OF_FILE, L"")      \
                               \
    /* grouping symbols */     \
    GEN(OPEN_PAREN, L"(")      \
    GEN(CLOSE_PAREN, L")")     \
    GEN(OPEN_BOX, L"[")        \
    GEN(CLOSE_BOX, L"]")       \
    GEN(OPEN_BRACE, L"{")      \
    GEN(CLOSE_BRACE, L"}")     \
                               \
    /* literals */             \
    GEN(STRING, L"")           \
    GEN(NUMBER, L"")           \
    GEN(IDENTIFIER, L"")

#endif  // __LEXEMES_H__