#ifndef __BYTECODE_HH__
#define __BYTECODE_HH__

#include <vector>
#include <string>
#include <variant>
#include <cstdint>

enum class OpCode : uint8_t {
    NOP,
    PUSH_INT,
    PUSH_STR,
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,
    NOT,
    EQ,
    NEQ,
    LT,
    LTE,
    GT,
    GTE,
    JMP,
    JMP_IF_FALSE,
    CALL,
    RET,
    PRINT,
    HALT,
    // ... add more as needed
};

struct Instruction {
    OpCode op;
    std::variant<int64_t, std::wstring, size_t> operand;

    Instruction(OpCode o) : op(o), operand(0) {}
    Instruction(OpCode o, int64_t i) : op(o), operand(i) {}
    Instruction(OpCode o, const std::wstring& s) : op(o), operand(s) {}
    Instruction(OpCode o, size_t addr) : op(o), operand(addr) {}
};

using Bytecode = std::vector<Instruction>;

#endif // __BYTECODE_HH__
