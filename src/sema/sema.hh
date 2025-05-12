#ifndef __SEMA_HH__
#define __SEMA_HH__

#include <iostream>
#include <string>
#include <vector>

#include "lexer/tokens.hh"
#include "sema/symbol_table.hh"

class Sema {
    SymbolTable symbols;
    std::vector<std::wstring> errors;

  public:
    Sema() = default;

    void enter_scope() { symbols.enter_scope(); }
    void exit_scope() { symbols.exit_scope(); }

    bool declare_variable(const std::wstring &name, const std::wstring &type,
                          std::optional<std::wstring> value = std::nullopt) {
        Symbol sym{name, SymbolKind::Variable, type, value,
                   symbols.current_scope_level()};
        if (!symbols.insert(sym)) {
            errors.push_back(L"Redeclaration of variable: " + name);
            return false;
        }
        return true;
    }

    bool declare_function(const std::wstring &name, const std::wstring &type,
                          const std::vector<std::wstring> &params,
                          std::optional<std::wstring> ret_type = std::nullopt) {
        Symbol sym{name, type, params, ret_type, symbols.current_scope_level()};
        if (!symbols.insert(sym)) {
            errors.push_back(L"Redeclaration of function: " + name);
            return false;
        }
        return true;
    }

    bool declare_import(const std::wstring &name, const std::wstring &module) {
        Symbol sym{name, module, symbols.current_scope_level()};
        if (!symbols.insert(sym)) {
            errors.push_back(L"Redeclaration of import: " + name);
            return false;
        }
        return true;
    }

    std::optional<Symbol> lookup(const std::wstring &name) const {
        return symbols.lookup(name);
    }

    void report_error(const std::wstring &msg) { errors.push_back(msg); }

    bool has_errors() const { return !errors.empty(); }

    void print_errors() const {
        for (const auto &err : errors) {
            std::wcerr << L"Semantic error: " << err << std::endl;
        }
    }

    void debug_print_symbols() const { symbols.debug_print(); }
};

#endif