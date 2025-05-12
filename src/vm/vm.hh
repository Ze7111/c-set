#ifndef __VM_HH__
#define __VM_HH__

#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "vm/bytecode.hh"

class VM {
    std::vector<int64_t> stack;
    std::vector<std::wstring> str_stack;
    std::vector<int64_t> globals;
    size_t ip = 0;
    Bytecode code;

  public:
    void load(const Bytecode &bc, size_t num_globals = 0);
    void run();

  private:
    int64_t pop();
};

#endif  // __VM_HH__