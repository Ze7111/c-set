#include "vm/codegen.hh"

#include <stdexcept>

//
void CodeGenContext::enter_scope() {
    locals.emplace_back();
    next_local = 0;
}
void CodeGenContext::exit_scope() {
    if (!locals.empty())
        locals.pop_back();
    next_local = locals.empty() ? 0 : locals.back().size();
}
size_t CodeGenContext::declare_var(const std::wstring &name, bool is_global) {
    if (is_global) {
        auto [it, inserted] = globals.emplace(name, next_global++);
        return it->second;
    } else {
        auto &scope = locals.back();
        auto [it, inserted] = scope.emplace(name, next_local++);
        return it->second;
    }
}
std::optional<size_t> CodeGenContext::resolve_var(const std::wstring &name) {
    for (auto it = locals.rbegin(); it != locals.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end())
            return found->second;
    }
    auto found = globals.find(name);
    if (found != globals.end())
        return found->second;
    return std::nullopt;
}

void CodeGen::emit(OpCode op) { code.emplace_back(op); }
void CodeGen::emit(OpCode op, int64_t i) { code.emplace_back(op, i); }
void CodeGen::emit(OpCode op, const std::wstring &s) {
    code.emplace_back(op, s);
}
void CodeGen::emit(OpCode op, size_t addr) { code.emplace_back(op, addr); }

Bytecode CodeGen::generate(const ASTNodePtr &node) {
    code.clear();
    ctx = CodeGenContext();
    gen(node, true);
    emit(OpCode::HALT);
    return code;
}

void CodeGen::gen(const ASTNodePtr &node, bool is_global) {
    if (!node)
        return;
    switch (node->kind) {
    case ASTNodeKind::Program: {
        auto prog = std::static_pointer_cast<Program>(node);
        for (auto &stmt : prog->body)
            gen(stmt, true);
        break;
    }
    case ASTNodeKind::Block: {
        ctx.enter_scope();
        auto block = std::static_pointer_cast<Block>(node);
        for (auto &stmt : block->statements)
            gen(stmt, false);
        ctx.exit_scope();
        break;
    }
    case ASTNodeKind::VarDecl: {
        auto var = std::static_pointer_cast<VarDecl>(node);
        size_t slot = ctx.declare_var(var->name, is_global);
        if (var->init) {
            gen(var->init, false);
            emit(OpCode::STORE, slot);
        }
        break;
    }
    case ASTNodeKind::Assign: {
        auto asn = std::static_pointer_cast<Assign>(node);
        gen(asn->value, false);
        auto slot = ctx.resolve_var(asn->name);
        if (!slot)
            throw std::runtime_error(
                "Undefined variable: " +
                std::string(asn->name.begin(), asn->name.end()));
        emit(OpCode::STORE, *slot);
        break;
    }
    case ASTNodeKind::Identifier: {
        auto id = std::static_pointer_cast<Identifier>(node);
        auto slot = ctx.resolve_var(id->name);
        if (!slot)
            throw std::runtime_error(
                "Undefined variable: " +
                std::string(id->name.begin(), id->name.end()));
        emit(OpCode::LOAD, *slot);
        break;
    }
    case ASTNodeKind::Number: {
        auto num = std::static_pointer_cast<Number>(node);
        emit(OpCode::PUSH_INT, std::stoll(num->value));
        break;
    }
    case ASTNodeKind::String: {
        auto str = std::static_pointer_cast<String>(node);
        emit(OpCode::PUSH_STR, str->value);
        break;
    }
    case ASTNodeKind::Binary: {
        auto bin = std::static_pointer_cast<Binary>(node);
        gen(bin->left, false);
        gen(bin->right, false);
        if (bin->op == L"+")
            emit(OpCode::ADD);
        else if (bin->op == L"-")
            emit(OpCode::SUB);
        else if (bin->op == L"*")
            emit(OpCode::MUL);
        else if (bin->op == L"/")
            emit(OpCode::DIV);
        else if (bin->op == L"%")
            emit(OpCode::MOD);
        else if (bin->op == L"==")
            emit(OpCode::EQ);
        else if (bin->op == L"!=")
            emit(OpCode::NEQ);
        else if (bin->op == L"<")
            emit(OpCode::LT);
        else if (bin->op == L"<=")
            emit(OpCode::LTE);
        else if (bin->op == L">")
            emit(OpCode::GT);
        else if (bin->op == L">=")
            emit(OpCode::GTE);
        else
            throw std::runtime_error("Unknown binary op");
        break;
    }
    case ASTNodeKind::Unary: {
        auto un = std::static_pointer_cast<Unary>(node);
        gen(un->operand, false);
        if (un->op == L"-")
            emit(OpCode::NEG);
        else if (un->op == L"!")
            emit(OpCode::NOT);
        else
            throw std::runtime_error("Unknown unary op");
        break;
    }
    case ASTNodeKind::ExprStmt: {
        auto expr = std::static_pointer_cast<ExprStmt>(node);
        gen(expr->expr, false);
        break;
    }
    case ASTNodeKind::If: {
        auto ifs = std::static_pointer_cast<IfStmt>(node);
        gen(ifs->cond, false);
        size_t jmp_false = code.size();
        // emit(OpCode::JMP_IF_FALSE, 0); // patch later
        gen(ifs->then_branch, false);
        if (ifs->else_branch) {
            size_t jmp_end = code.size();
            // emit(OpCode::JMP, 0); // patch later
            code[jmp_false].operand = code.size();
            gen(*ifs->else_branch, false);
            code[jmp_end].operand = code.size();
        } else {
            code[jmp_false].operand = code.size();
        }
        break;
    }
    case ASTNodeKind::While: {
        auto wh = std::static_pointer_cast<WhileStmt>(node);
        size_t loop_start = code.size();
        gen(wh->cond, false);
        size_t jmp_false = code.size();
        // emit(OpCode::JMP_IF_FALSE, 0); // patch later
        gen(wh->body, false);
        emit(OpCode::JMP, loop_start);
        code[jmp_false].operand = code.size();
        break;
    }
    default:
        break;
    }
}
