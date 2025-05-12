#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <any>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../pkgs/base/include.hh"

namespace lexer {
template <typename T>  // if std::string(T) is possible
concept stringable = requires(T obj) { std::string(obj); };

static bool error_occured = false;

class TTPanic {
  private:
    std::string error_message;
    std::string fix_message;
    std::vector<std::any> error_fmt_args;
    std::vector<std::any> fix_fmt_args;

  public:
    TTPanic(const TTPanic& other) = default;
    TTPanic& operator=(const TTPanic& other) = default;
    TTPanic(TTPanic&& other) = default;
    TTPanic& operator=(TTPanic&& other) = default;

    TTPanic(std::string errorMsg, std::string fixMsg)
        : error_message(std::move(errorMsg)),
          fix_message(  std::move(fixMsg)){};

    void show(const std::string &file_name, const std::string &line_str,
              u64 line, u64 col,
              int offset = 1) {
        error_occured = true;
        size_t index = 0;
        for (const auto &arg : error_fmt_args) {
            auto pos = error_message.find("%s", index);
            error_message.replace(pos, 2, std::any_cast<std::string>(arg));
            index = pos + 2;
        }

        index = 0;

        for (const auto &arg : fix_fmt_args) {
            auto pos = fix_message.find("%s", index);
            fix_message.replace(pos, 2, std::any_cast<std::string>(arg));
            index = pos + 2;
        }

        std::cout << std::string(colors::fg16::red) << "error"
                  << std::string(colors::fg16::cyan) << ": "
                  << std::string(colors::reset) << error_message << '\n';
        std::cout << "     " <<  std::string(colors::fg16::cyan) << "├─> "
                  << std::string(colors::reset) << "at "
                  << std::string(colors::fg16::green) << file_name
                  << std::string(colors::reset) << ":"
                  << std::string(colors::fg16::yellow) << line
                  << std::string(colors::reset) << ":"
                  << std::string(colors::fg16::yellow) << col
                  << std::string(colors::reset) << '\n';
        std::cout << "     " <<  std::string(colors::fg16::cyan) << ": "
                  << std::string(colors::reset) << line_str;
        std::cout << "     " <<  std::string(colors::fg16::cyan) << "│ "
                  << std::string(colors::reset) << std::string(col, ' ')
                  << std::string(colors::fg16::yellow)
                  << std::string(offset, '^') << std::string(colors::reset)
                  << '\n';
        std::cout << "  " << std::string(colors::fg16::green) << "fix"
                  << std::string(colors::fg16::cyan) << ": "
                  << std::string(colors::reset) << fix_message << '\n';
    };

    template <typename... Args>
        requires (stringable<Args> && ...)
    TTPanic &error(Args &&...args) {
        error_fmt_args = {std::forward<Args>(args)...};
        return *this;
    }

    template <typename... Args>
        requires (stringable<Args> && ...)
    TTPanic &fix(Args &&...args) {
        fix_fmt_args = {std::forward<Args>(args)...};
        return *this;
    }

    ~TTPanic() = default;
};


static TTPanic MissingVariableDeclarationError =
    TTPanic("missing variable declaration for '%s'.",
                  "declare the variable '%s' before its use.");

static TTPanic InvalidSyntaxError =
    TTPanic("syntax error near '%s'.", "check the syntax '%s'.");

static TTPanic UnknownDirectiveError =
    TTPanic("unknown directive '%s' encountered.",
                  "remove the unknown directive '%s' or check for typos.");

static TTPanic DirectiveMisuseError = TTPanic(
    "misuse of directive '%s'.", "ensure that the directive '%s' is used "
                                 "correctly according to documentation.");

static TTPanic UnexpectedCharacterError = TTPanic(
    "unexpected character '%s' found.",
    "remove the unexpected character '%s' or replace it as necessary.");

static TTPanic InbuiltFunctionError = TTPanic(
    "error in using inbuilt function '%s'.",
    "check the usage and parameters of the inbuilt function '%s'.");

static TTPanic VariableSanitizationFailure =
    TTPanic("variable '%s' failed sanitization.",
            "ensure the variable '%s' adheres to allowed characters and "
            "formatting rules.");

        // was a .tcc implementation but vscode don't like it
}       // namespace lexer
#endif  // __ERRORS_H__