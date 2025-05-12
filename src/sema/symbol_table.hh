#ifndef __SYMBOL_TABLE_HH__
#define __SYMBOL_TABLE_HH__

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "sema/symbol.hh"

class SymbolTable {
    std::vector<std::unordered_map<std::wstring, Symbol>> scopes;

  public:
    SymbolTable() { enter_scope(); }

    void enter_scope() { scopes.emplace_back(); }

    void exit_scope() {
        if (!scopes.empty())
            scopes.pop_back();
    }

    bool insert(const Symbol &sym) {
        auto &current = scopes.back();
        if (current.count(sym.name))
            return false;  // redeclaration
        current[sym.name] = sym;
        return true;
    }

    bool remove(const std::wstring &name) {
        auto &current = scopes.back();
        return current.erase(name) > 0;
    }

    std::optional<Symbol> lookup(const std::wstring &name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end())
                return found->second;
        }
        return std::nullopt;
    }

    std::vector<Symbol> current_scope_symbols() const {
        std::vector<Symbol> result;
        if (!scopes.empty()) {
            for (const auto &[_, sym] : scopes.back()) {
                result.push_back(sym);
            }
        }
        return result;
    }

    int current_scope_level() const {
        return static_cast<int>(scopes.size()) - 1;
    }

    void debug_print() const {
        int level = 0;
        for (const auto &scope : scopes) {
            std::wcout << L"Scope " << level++ << L":\n";
            for (const auto &[name, sym] : scope) {
                std::wcout << L"  " << name << L" ("
                           << static_cast<int>(sym.kind) << L")\n";
            }
        }
    }
};

#endif  // __SYMBOL_TABLE_HH__