#ifndef __LEX_H__
#define __LEX_H__

#include <coroutine>
#include <fstream>
#include <iostream>
#include <print>
#include <utility>
#include <vector>

#include "lexer/generation.hh"
#include "lexer/lexemes.hh"
#include "lexer/tokens.hh"
#include "thread/thread.hh"
#include "types/estr.hh"
#include "types/gen.hh"
#include "types/rints.hh"

namespace Lexer {
using TokenList = std::vector<Token>;

template <typename ThreadManager>
    requires WorkerThreadConcept<ThreadManager>
class Lexer {
  private:
    std::wifstream file;
    const string filename;
    TokenList tokens;
    usize offset = 0;
    ThreadManager *worker;

  public:
    explicit Lexer(string filename, ThreadManager *worker)
        : filename(std::move(filename))
        , worker(worker) {
        worker->async([this] { tokens.reserve(256); });
    }

    Lexer(const Lexer &) = default;
    Lexer(Lexer &&) = default;
    Lexer &operator=(const Lexer &) = default;
    Lexer &operator=(Lexer &&) = default;

    TokenList &operator*() { return tokens; }
    TokenList *operator->() { return &tokens; }

    ~Lexer() {
        if (file.is_open()) {
            file.close();
        }

        if (file.fail()) {
            std::wcerr << L"Failed to close file: " << filename << '\n';
        }

        if (!tokens.empty()) {
            tokens.clear();
        }
    }

  public:
    generator<Token> tokenize() {
        file.open(filename);

        if (!file.is_open()) {
            std::wcerr << L"Failed to open file: " << filename << '\n';
            co_return;
        }

        usize offset = 0;
        usize line = 0;
        usize column = 0;
        wchar_t chr;
        Token token;
        string token_str;
        TokenKind kind = TokenKind::END_OF_FILE;

        for (offset = 0; file.peek() != EOF; ++offset) {
            chr = file.get();

            if (file.eof()) {
                break;
            }

            if (chr == L'\n') {
                line++;
                column = 0;
                continue;
            }

            // we need to do a few things here now:
            token_str.clear();
            token_str += chr;
            switch (chr) {
            case L'a' ... L'z':
            case L'A' ... L'Z':
            case L'_': {
                // identifier or keyword
                while (file.peek() != EOF) {
                    wchar_t next = file.peek();
                    if ((next >= L'a' && next <= L'z') ||
                        (next >= L'A' && next <= L'Z') ||
                        (next >= L'0' && next <= L'9') || next == L'_') {
                        chr = file.get();
                        token_str += chr;
                        column++;
                    } else {
                        break;
                    }
                }
                usize hash = Generation::hash_string(token_str);
                if (Token::Mapping.contains({hash})) {
                    TokenKind kind = TokenKind::IDENTIFIER;
                    for (const auto &[key, val] : Token::Mapping) {
                        if (key.k1 == hash) {
                            kind = key.k2;
                        }
                    }
                    token = Token(kind, token_str, line, column,
                                  token_str.length(), offset, filename);
                } else {
                    token = Token(TokenKind::IDENTIFIER, token_str, line,
                                  column, token_str.length(), offset, filename);
                }
                co_yield token;
                ++column;
                continue;
            }
            case L'0' ... L'9': {
                // number (integer or float)
                bool is_float = false;
                while (file.peek() != EOF) {
                    wchar_t next = file.peek();
                    if (next >= L'0' && next <= L'9') {
                        chr = file.get();
                        token_str += chr;
                        column++;
                    } else if (next == L'.' && !is_float) {
                        chr = file.get();
                        token_str += chr;
                        column++;
                        is_float = true;
                    } else {
                        break;
                    }
                }
                token = Token(TokenKind::NUMBER, token_str, line, column,
                              token_str.length(), offset, filename);
                co_yield token;
                ++column;
                continue;
            }
            case L'"':
            case L'\'': {
                // string literal
                wchar_t quote = chr;
                token_str.clear();
                while (file.peek() != EOF) {
                    chr = file.get();
                    if (chr == quote) {
                        break;
                    }
                    if (chr == L'\\') {
                        // handle escape
                        wchar_t next = file.get();
                        switch (next) {
                        case L'n':
                            token_str += L'\n';
                            break;
                        case L't':
                            token_str += L'\t';
                            break;
                        case L'\\':
                            token_str += L'\\';
                            break;
                        case L'"':
                            token_str += L'"';
                            break;
                        case L'\'':
                            token_str += L'\'';
                            break;
                        default:
                            token_str += next;
                            break;
                        }
                    } else {
                        token_str += chr;
                    }
                }
                token = Token(TokenKind::STRING, token_str, line, column,
                              token_str.length(), offset, filename);
                co_yield token;
                ++column;
                continue;
            }
            case L'#': {
                // comment: skip until end of line
                while (file.peek() != EOF) {
                    chr = file.get();
                    if (chr == L'\n') {
                        line++;
                        column = 0;
                        break;
                    }
                }
                continue;
            }
            default: {
                // punct or op
                // try to match multi character operators first
                bool matched = false;
                std::wstring punct;
                punct += chr;
                wchar_t next = file.peek();
                if (next != WEOF) {
                    // try two character operators
                    punct += next;
                    usize hash2 = Generation::hash_string(punct);
                    if (Token::Mapping.contains({hash2})) {
                        file.get();  // consume next
                        TokenKind kind = TokenKind::END_OF_FILE;
                        for (const auto &[key, val] : Token::Mapping) {
                            if (key.k1 == hash2) {
                                kind = key.k2;
                            }
                        }
                        token = Token(kind, punct, line, column, punct.length(),
                                      offset, filename);
                        co_yield token;
                        ++column;
                        matched = true;
                    }
                    punct.pop_back();
                }
                if (!matched) {
                    // try single character op or punct
                    usize hash1 = Generation::hash_string(punct);
                    if (Token::Mapping.contains({hash1})) {
                        TokenKind kind = TokenKind::END_OF_FILE;
                        for (const auto &[key, val] : Token::Mapping) {
                            if (key.k1 == hash1) {
                                kind = key.k2;
                            }
                        }
                        token = Token(kind, punct, line, column, punct.length(),
                                      offset, filename);
                        co_yield token;
                        ++column;
                        matched = true;
                    }
                }
                if (!matched && !iswspace(chr)) {
                    // unknown/unrecognized character, yield as is or skip
                    token = Token(TokenKind::END_OF_FILE, std::wstring(1, chr),
                                  line, column, 1, offset, filename);
                    co_yield token;
                    ++column;
                }
                continue;
            }
            }
        }
        // yield eof token at the end
        token = Token(TokenKind::END_OF_FILE, L"", line, column, 0, offset,
                      filename);
        co_yield token;
    }
};

}  // namespace Lexer

#endif  // __lex_h__