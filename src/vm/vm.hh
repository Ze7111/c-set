#ifndef __VM_H__
#define __VM_H__

#include <any>
#include <map>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "../../pkgs/base/include.hh"
#include "../ast/tree.hh"

namespace vm {

struct Heap {
  private:
    std::unordered_map<u32, std::any> heap;

  public:
    Heap()                             = default;
    Heap(const Heap &other)            = default;
    Heap(Heap &&other) noexcept        = default;
    Heap &operator=(const Heap &other) = delete;
    Heap &operator=(Heap &&other)      = delete;
    ~Heap() {
        heap.clear();
    }

    auto set(u32 address, std::any value) -> void {
        heap[address] = std::move(value);
    }

    template <typename T>
    auto get(u32 address) -> T {
        return std::any_cast<T>(heap[address]);
    }

    auto remove(u32 address) -> void { heap.erase(address); }

    auto clear() -> void { heap.clear(); }

    auto size() -> u32 { return heap.size(); }

    auto empty() -> bool {
        bool const is_heap_empty = heap.empty();
        return is_heap_empty;
    }
};

struct Stack {
  private:
    std::vector<std::any> stack;

  public:
    Stack()                              = default;
    Stack(const Stack &other)            = default;
    Stack(Stack &&other) noexcept        = default;
    Stack &operator=(const Stack &other) = delete;
    Stack &operator=(Stack &&other)      = delete;
    ~Stack() {
        stack.clear();
    }

    auto push(std::any value) -> void { stack.push_back(std::move(value)); }

    template <typename T>
    auto pop() -> T {
        T const value = std::any_cast<T>(stack.back());
        stack.pop_back();
        return value;
    }

    auto clear() -> void { stack.clear(); }

    auto size() -> u32 { return stack.size(); }

    auto empty() -> bool {
        bool const is_stack_empty = stack.empty();
        return is_stack_empty;
    }

    auto top() -> std::any { return stack.back(); }

    auto remove(u32 index) -> void { stack.erase(stack.begin() + index); }

    auto get(u32 index) -> std::any { return stack[index]; }

    auto set(u32 index, std::any value) -> void {
        stack[index] = std::move(value);
    }

    auto insert(u32 index, std::any value) -> void {
        stack.insert(stack.begin() + index, std::move(value));
    }

    auto swap(u32 index1, u32 index2) -> void {
        std::swap(stack[index1], stack[index2]);
    }
};

class Memory {
  private:
    friend struct Heap;
    friend struct Stack;

  public:
    Memory()                               = default;
    Memory(const Memory &other)            = default;
    Memory(Memory &&other) noexcept        = default;
    Memory &operator=(const Memory &other) = delete;
    Memory &operator=(Memory &&other)      = delete;
    ~Memory() {
        delete heap;
        delete stack;
    }

    Heap  *heap  = new Heap;
    Stack *stack = new Stack;
};

class TreeWalker {
  private:
    friend struct Heap;
    friend struct Stack;

  public:
    TreeWalker()                                   = default;
    TreeWalker(const TreeWalker &other)            = default;
    TreeWalker(TreeWalker &&other) noexcept        = default;
    TreeWalker &operator=(const TreeWalker &other) = delete;
    TreeWalker &operator=(TreeWalker &&other)      = delete;
    ~TreeWalker()                                  = default;

    auto mem_test_x96_x84() -> void;
    auto execute(Tree tree) -> void;

    Memory memory = Memory();
};
}  // namespace vm

#endif  // __VM_H__