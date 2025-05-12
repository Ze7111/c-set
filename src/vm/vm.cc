#include "vm/vm.hh"

#include <stdexcept>

void VM::load(const Bytecode &bc, size_t num_globals) {
    code = bc;
    ip = 0;
    stack.clear();
    str_stack.clear();
    globals.resize(num_globals, 0);
}

void VM::run() {
    while (ip < code.size()) {
        const auto &instr = code[ip++];
        switch (instr.op) {
        case OpCode::NOP:
            break;
        case OpCode::PUSH_INT:
            stack.push_back(std::get<int64_t>(instr.operand));
            break;
        case OpCode::PUSH_STR:
            str_stack.push_back(std::get<std::wstring>(instr.operand));
            break;
        case OpCode::LOAD: {
            size_t slot = std::get<size_t>(instr.operand);
            stack.push_back(globals[slot]);
            break;
        }
        case OpCode::STORE: {
            size_t slot = std::get<size_t>(instr.operand);
            if (stack.empty())
                throw std::runtime_error("Stack underflow on STORE");
            globals[slot] = stack.back();
            stack.pop_back();
            break;
        }
        case OpCode::ADD: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a + b);
            break;
        }
        case OpCode::SUB: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a - b);
            break;
        }
        case OpCode::MUL: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a * b);
            break;
        }
        case OpCode::DIV: {
            auto b = pop();
            auto a = pop();
            if (b == 0)
                throw std::runtime_error("Division by zero");
            stack.push_back(a / b);
            break;
        }
        case OpCode::MOD: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a % b);
            break;
        }
        case OpCode::NEG: {
            auto a = pop();
            stack.push_back(-a);
            break;
        }
        case OpCode::EQ: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a == b);
            break;
        }
        case OpCode::NEQ: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a != b);
            break;
        }
        case OpCode::LT: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a < b);
            break;
        }
        case OpCode::LTE: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a <= b);
            break;
        }
        case OpCode::GT: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a > b);
            break;
        }
        case OpCode::GTE: {
            auto b = pop();
            auto a = pop();
            stack.push_back(a >= b);
            break;
        }
        case OpCode::JMP: {
            ip = std::get<size_t>(instr.operand);
            break;
        }
        case OpCode::JMP_IF_FALSE: {
            auto cond = pop();
            size_t addr = std::get<size_t>(instr.operand);
            if (!cond)
                ip = addr;
            break;
        }
        case OpCode::PRINT: {
            if (!stack.empty()) {
                std::wcout << stack.back() << std::endl;
                stack.pop_back();
            } else if (!str_stack.empty()) {
                std::wcout << str_stack.back() << std::endl;
                str_stack.pop_back();
            }
            break;
        }
        case OpCode::HALT:
            return;
        default:
            throw std::runtime_error("Unknown opcode");
        }
    }
}

int64_t VM::pop() {
    if (stack.empty())
        throw std::runtime_error("Stack underflow");
    int64_t v = stack.back();
    stack.pop_back();
    return v;
}
