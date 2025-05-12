#ifndef __CODEGEN_HH__
#define __CODEGEN_HH__

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ast.hh"
#include "vm/bytecode.hh"

class CodeGenContext {
  public:
    std::unordered_map<std::wstring, size_t> globals;
    std::vector<std::unordered_map<std::wstring, size_t>> locals;
    size_t next_global = 0;
    size_t next_local = 0;

    void enter_scope();
    void exit_scope();
    size_t declare_var(const std::wstring &name, bool is_global);
    std::optional<size_t> resolve_var(const std::wstring &name);
};

class CodeGen {
    Bytecode code;

    void emit(OpCode op);
    void emit(OpCode op, int64_t i);
    void emit(OpCode op, const std::wstring &s);
    void emit(OpCode op, size_t addr);

  public:
    CodeGenContext ctx;
    Bytecode generate(const ASTNodePtr &node);

  private:
    void gen(const ASTNodePtr &node, bool is_global = false);
};

#endif  // __CODEGEN_HH__