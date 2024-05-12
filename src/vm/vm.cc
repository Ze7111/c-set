#include <any>
#include <cmath>
#include <map>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../../pkgs/base/include.hh"
#include "vm.hh"

namespace vm {
/* ------------------------------- Memory base ------------------------------ */




/* --------------------------- Virtual Machine ------------------------------ */
auto TreeWalker::mem_test_x96_x84() -> void {
    memory.heap->set(0x07ADAF71, "Hello, World!");
    std::print(memory.heap->get<const char *>(0x07ADAF71));

    memory.stack->push(1);
    memory.stack->push(2);
    memory.stack->push(3);

    std::print(memory.stack->pop<int>());
    std::print(memory.stack->pop<int>());
    std::print(memory.stack->pop<int>());

    std::print("stack memory");
};
} // namespace vm