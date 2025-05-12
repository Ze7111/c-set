#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ast/ast.hh"
#include "lexer/lexer.hh"
#include "lexer/tokens.hh"
#include "sema/sema.hh"

class ParserContext {
  public:
    Sema sema;
    void enter_scope() { sema.enter_scope(); }
    void exit_scope() { sema.exit_scope(); }
};

class Parser {
    using TokenIter = std::vector<Lexer::Token>::const_iterator;
    std::vector<Lexer::Token> tokens;
    TokenIter current;
    ParserContext &ctx;

    Lexer::Token peek(int offset = 0) const {
        auto it = current;
        std::advance(it, offset);
        return (it != tokens.end())
                   ? *it
                   : Lexer::Token(Lexer::TokenKind::END_OF_FILE, L"", 0, 0, 0,
                                  0, L"");
    }
    Lexer::Token advance() {
        return (current != tokens.end())
                   ? *current++
                   : Lexer::Token(Lexer::TokenKind::END_OF_FILE, L"", 0, 0, 0,
                                  0, L"");
    }
    bool match(Lexer::TokenKind kind) {
        if (peek().kind == kind) {
            advance();
            return true;
        }
        return false;
    }
    bool check(Lexer::TokenKind kind) const { return peek().kind == kind; }

  public:
    Parser(const std::vector<Lexer::Token> &toks, ParserContext &c)
        : tokens(toks)
        , current(tokens.begin())
        , ctx(c) {}

    ASTNodePtr parse_program() {
        auto prog = std::make_shared<Program>();
        while (!check(Lexer::TokenKind::END_OF_FILE)) {
            auto stmt = parse_declaration();
            if (stmt)
                prog->body.push_back(stmt);
            else
                advance();
        }
        return prog;
    }

    ASTNodePtr parse_declaration() {
        if (match(Lexer::TokenKind::IMPORT))
            return parse_import();
        if (match(Lexer::TokenKind::VAR))
            return parse_var_decl();
        if (match(Lexer::TokenKind::FUNCTION))
            return parse_func_decl();
        return parse_statement();
    }

    ASTNodePtr parse_import() {
        if (!check(Lexer::TokenKind::IDENTIFIER))
            return nullptr;
        auto name = advance().value;
        ctx.sema.declare_import(name, name);
        match(Lexer::TokenKind::SEMICOLON);
        return std::make_shared<Import>(name);
    }

    ASTNodePtr parse_var_decl() {
        if (!check(Lexer::TokenKind::IDENTIFIER))
            return nullptr;
        auto name = advance().value;
        ASTNodePtr init = nullptr;
        if (match(Lexer::TokenKind::ASSIGN)) {
            init = parse_expression();
        }
        ctx.sema.declare_variable(name, L"auto", std::nullopt);
        match(Lexer::TokenKind::SEMICOLON);
        return std::make_shared<VarDecl>(name, init);
    }

    ASTNodePtr parse_func_decl() {
        if (!check(Lexer::TokenKind::IDENTIFIER))
            return nullptr;
        auto name = advance().value;
        std::vector<std::wstring> params;
        if (!match(Lexer::TokenKind::OPEN_PAREN))
            return nullptr;
        while (!check(Lexer::TokenKind::CLOSE_PAREN) &&
               !check(Lexer::TokenKind::END_OF_FILE)) {
            if (check(Lexer::TokenKind::IDENTIFIER)) {
                params.push_back(advance().value);
                if (!check(Lexer::TokenKind::CLOSE_PAREN))
                    match(Lexer::TokenKind::COMMA);
            } else {
                break;
            }
        }
        if (!match(Lexer::TokenKind::CLOSE_PAREN))
            return nullptr;
        ctx.enter_scope();
        for (const auto &param : params)
            ctx.sema.declare_variable(param, L"auto", std::nullopt);
        ASTNodePtr body = parse_block();
        ctx.exit_scope();
        ctx.sema.declare_function(name, L"auto", params, std::nullopt);
        return std::make_shared<FuncDecl>(name, params, body);
    }

    ASTNodePtr parse_statement() {
        if (match(Lexer::TokenKind::IF))
            return parse_if();
        if (match(Lexer::TokenKind::WHILE))
            return parse_while();
        if (match(Lexer::TokenKind::RETURN))
            return parse_return();
        if (match(Lexer::TokenKind::BREAK)) {
            match(Lexer::TokenKind::SEMICOLON);
            return std::make_shared<BreakStmt>();
        }
        if (match(Lexer::TokenKind::CONTINUE)) {
            match(Lexer::TokenKind::SEMICOLON);
            return std::make_shared<ContinueStmt>();
        }
        if (match(Lexer::TokenKind::OPEN_BRACE))
            return parse_block(true);
        return parse_expression_statement();
    }

    ASTNodePtr parse_if() {
        auto cond = parse_expression();
        ASTNodePtr then_branch = parse_block();
        std::optional<ASTNodePtr> else_branch;
        if (match(Lexer::TokenKind::ELSE)) {
            if (check(Lexer::TokenKind::IF)) {
                advance();
                else_branch = parse_if();
            } else {
                else_branch = parse_block();
            }
        }
        return std::make_shared<IfStmt>(cond, then_branch, else_branch);
    }

    ASTNodePtr parse_while() {
        auto cond = parse_expression();
        ASTNodePtr body = parse_block();
        return std::make_shared<WhileStmt>(cond, body);
    }

    ASTNodePtr parse_return() {
        ASTNodePtr value = nullptr;
        if (!check(Lexer::TokenKind::SEMICOLON))
            value = parse_expression();
        match(Lexer::TokenKind::SEMICOLON);
        return std::make_shared<ReturnStmt>(value);
    }

    ASTNodePtr parse_block(bool already_matched = false) {
        if (!already_matched && !match(Lexer::TokenKind::OPEN_BRACE))
            return nullptr;
        auto block = std::make_shared<Block>();
        ctx.enter_scope();
        while (!check(Lexer::TokenKind::CLOSE_BRACE) &&
               !check(Lexer::TokenKind::END_OF_FILE)) {
            auto stmt = parse_declaration();
            if (stmt)
                block->statements.push_back(stmt);
            else
                advance();
        }
        match(Lexer::TokenKind::CLOSE_BRACE);
        ctx.exit_scope();
        return block;
    }

    ASTNodePtr parse_expression_statement() {
        auto expr = parse_expression();
        match(Lexer::TokenKind::SEMICOLON);
        return std::make_shared<ExprStmt>(expr);
    }

    ASTNodePtr parse_expression() { return parse_assignment(); }

    ASTNodePtr parse_assignment() {
        auto left = parse_binary();
        if (match(Lexer::TokenKind::ASSIGN)) {
            if (auto id = std::dynamic_pointer_cast<Identifier>(left)) {
                auto value = parse_assignment();
                ctx.sema.declare_variable(id->name, L"auto", std::nullopt);
                return std::make_shared<Assign>(id->name, value);
            }
        }
        return left;
    }

    ASTNodePtr parse_binary(int prec = 0) {
        auto left = parse_unary();
        while (true) {
            int curr_prec = get_precedence(peek().kind);
            if (curr_prec < prec)
                break;
            auto op_token = advance();
            auto right = parse_binary(curr_prec + 1);
            left = std::make_shared<Binary>(op_token.value, left, right);
        }
        return left;
    }

    ASTNodePtr parse_unary() {
        if (peek().kind == Lexer::TokenKind::NOT ||
            peek().kind == Lexer::TokenKind::SUB) {
            auto op = advance().value;
            auto operand = parse_unary();
            return std::make_shared<Unary>(op, operand);
        }
        return parse_primary();
    }

    ASTNodePtr parse_primary() {
        if (check(Lexer::TokenKind::NUMBER)) {
            auto val = advance().value;
            return std::make_shared<Number>(val);
        }
        if (check(Lexer::TokenKind::STRING)) {
            auto val = advance().value;
            return std::make_shared<String>(val);
        }
        if (check(Lexer::TokenKind::IDENTIFIER)) {
            auto name = advance().value;
            if (check(Lexer::TokenKind::OPEN_PAREN)) {
                advance();
                std::vector<ASTNodePtr> args;
                if (!check(Lexer::TokenKind::CLOSE_PAREN)) {
                    do {
                        args.push_back(parse_expression());
                    } while (match(Lexer::TokenKind::COMMA));
                }
                match(Lexer::TokenKind::CLOSE_PAREN);
                return std::make_shared<Call>(name, args);
            }
            return std::make_shared<Identifier>(name);
        }
        if (match(Lexer::TokenKind::OPEN_PAREN)) {
            auto expr = parse_expression();
            match(Lexer::TokenKind::CLOSE_PAREN);
            return expr;
        }
        return nullptr;
    }

    int get_precedence(Lexer::TokenKind kind) {
        switch (kind) {
        case Lexer::TokenKind::OR:
            return 1;
        case Lexer::TokenKind::AND:
            return 2;
        case Lexer::TokenKind::EQ:
        case Lexer::TokenKind::NEQ:
            return 3;
        case Lexer::TokenKind::LT:
        case Lexer::TokenKind::LTE:
        case Lexer::TokenKind::GT:
        case Lexer::TokenKind::GTE:
            return 4;
        case Lexer::TokenKind::ADD:
        case Lexer::TokenKind::SUB:
            return 5;
        case Lexer::TokenKind::MUL:
        case Lexer::TokenKind::DIV:
        case Lexer::TokenKind::MOD:
            return 6;
        default:
            return 0;
        }
    }
};

#endif  // __PARSER_HH__
