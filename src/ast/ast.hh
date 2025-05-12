#ifndef __AST_HH__
#define __AST_HH__

#include <memory>
#include <optional>
#include <string>
#include <vector>

enum class ASTNodeKind {
    Program,
    Block,
    VarDecl,
    FuncDecl,
    Param,
    Import,
    If,
    While,
    Return,
    Break,
    Continue,
    ExprStmt,
    Assign,
    Binary,
    Unary,
    Call,
    Identifier,
    Number,
    String
};

struct ASTNode {
    ASTNodeKind kind;
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

struct Program : ASTNode {
    std::vector<ASTNodePtr> body;
    Program() { kind = ASTNodeKind::Program; }
};

struct Block : ASTNode {
    std::vector<ASTNodePtr> statements;
    Block() { kind = ASTNodeKind::Block; }
};

struct VarDecl : ASTNode {
    std::wstring name;
    ASTNodePtr init;
    VarDecl(const std::wstring &n, ASTNodePtr i)
        : name(n)
        , init(i) {
        kind = ASTNodeKind::VarDecl;
    }
};

struct FuncDecl : ASTNode {
    std::wstring name;
    std::vector<std::wstring> params;
    ASTNodePtr body;
    FuncDecl(const std::wstring &n, std::vector<std::wstring> p, ASTNodePtr b)
        : name(n)
        , params(std::move(p))
        , body(b) {
        kind = ASTNodeKind::FuncDecl;
    }
};

struct Import : ASTNode {
    std::wstring module;
    Import(const std::wstring &m)
        : module(m) {
        kind = ASTNodeKind::Import;
    }
};

struct IfStmt : ASTNode {
    ASTNodePtr cond;
    ASTNodePtr then_branch;
    std::optional<ASTNodePtr> else_branch;
    IfStmt(ASTNodePtr c, ASTNodePtr t, std::optional<ASTNodePtr> e)
        : cond(c)
        , then_branch(t)
        , else_branch(e) {
        kind = ASTNodeKind::If;
    }
};

struct WhileStmt : ASTNode {
    ASTNodePtr cond;
    ASTNodePtr body;
    WhileStmt(ASTNodePtr c, ASTNodePtr b)
        : cond(c)
        , body(b) {
        kind = ASTNodeKind::While;
    }
};

struct ReturnStmt : ASTNode {
    ASTNodePtr value;
    ReturnStmt(ASTNodePtr v)
        : value(v) {
        kind = ASTNodeKind::Return;
    }
};

struct BreakStmt : ASTNode {
    BreakStmt() { kind = ASTNodeKind::Break; }
};

struct ContinueStmt : ASTNode {
    ContinueStmt() { kind = ASTNodeKind::Continue; }
};

struct ExprStmt : ASTNode {
    ASTNodePtr expr;
    ExprStmt(ASTNodePtr e)
        : expr(e) {
        kind = ASTNodeKind::ExprStmt;
    }
};

struct Assign : ASTNode {
    std::wstring name;
    ASTNodePtr value;
    Assign(const std::wstring &n, ASTNodePtr v)
        : name(n)
        , value(v) {
        kind = ASTNodeKind::Assign;
    }
};

struct Binary : ASTNode {
    std::wstring op;
    ASTNodePtr left, right;
    Binary(const std::wstring &o, ASTNodePtr l, ASTNodePtr r)
        : op(o)
        , left(l)
        , right(r) {
        kind = ASTNodeKind::Binary;
    }
};

struct Unary : ASTNode {
    std::wstring op;
    ASTNodePtr operand;
    Unary(const std::wstring &o, ASTNodePtr opd)
        : op(o)
        , operand(opd) {
        kind = ASTNodeKind::Unary;
    }
};

struct Call : ASTNode {
    std::wstring callee;
    std::vector<ASTNodePtr> args;
    Call(const std::wstring &c, std::vector<ASTNodePtr> a)
        : callee(c)
        , args(std::move(a)) {
        kind = ASTNodeKind::Call;
    }
};

struct Identifier : ASTNode {
    std::wstring name;
    Identifier(const std::wstring &n)
        : name(n) {
        kind = ASTNodeKind::Identifier;
    }
};

struct Number : ASTNode {
    std::wstring value;
    Number(const std::wstring &v)
        : value(v) {
        kind = ASTNodeKind::Number;
    }
};

struct String : ASTNode {
    std::wstring value;
    String(const std::wstring &v)
        : value(v) {
        kind = ASTNodeKind::String;
    }
};

#endif  // __AST_HH__
