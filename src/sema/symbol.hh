#ifndef __SYMBOL_HH__
#define __SYMBOL_HH__

#include <optional>
#include <string>
#include <variant>
#include <vector>

enum class SymbolKind { Variable, Function, Parameter, Import };

struct Symbol {
    std::wstring name;
    SymbolKind kind;
    std::wstring type;
    std::optional<std::wstring> value;
    int scope_level;

    std::vector<std::wstring> parameters;
    std::optional<std::wstring> return_type;

    std::optional<std::wstring> import_module;

    Symbol(const std::wstring &n, SymbolKind k, const std::wstring &t,
           std::optional<std::wstring> v, int scope)
        : name(n)
        , kind(k)
        , type(t)
        , value(v)
        , scope_level(scope) {}

    Symbol(const std::wstring &n, const std::wstring &t,
           const std::vector<std::wstring> &params,
           std::optional<std::wstring> ret, int scope)
        : name(n)
        , kind(SymbolKind::Function)
        , type(t)
        , value(std::nullopt)
        , scope_level(scope)
        , parameters(params)
        , return_type(ret) {}

    Symbol(const std::wstring &n, const std::wstring &mod, int scope)
        : name(n)
        , kind(SymbolKind::Import)
        , type(L"import")
        , value(std::nullopt)
        , scope_level(scope)
        , import_module(mod) {}
};

#endif  // __SYMBOL_HH__
