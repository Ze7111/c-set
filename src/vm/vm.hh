#ifndef __VM_H__
#define __VM_H__

#include <any>
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../pkgs/base/include.hh"
namespace vm {
class Memory {
  private:
    std::map<u16, bool> locks;
    std::vector<std::any> registers;

  public:
    const u16 NUM_REGISTERS = 96;

    Memory(const Memory &other) = default;
    Memory &operator=(const Memory &other) = default;
    Memory(Memory &&other) noexcept = default;
    Memory &operator=(Memory &&other) = default;

    Memory() { registers.resize(NUM_REGISTERS); };

    auto set_register(u16 index, std::any value) -> void {
        if (index >= NUM_REGISTERS) {
            // panic
            std::print(colors::fg16::red, "Register index out of bounds",
                       colors::reset);
        }
        registers[index] = std::move(value);
    };

    auto get_register(u16 index) -> std::any {
        if (index >= NUM_REGISTERS) {
            // panic
            std::print(colors::fg16::red, "Register index out of bounds",
                       colors::reset);
        }
        return registers[index];
    };

    class Heap {
      private:
        std::unordered_map<u16, std::any> heap;
        Memory &parent;

      public:
        Heap(Memory &parent)
            : parent(parent) {}

        auto set(u16 addr, std::any value) -> void {
            if (parent.locks[addr]) {
                while (parent.locks[addr]) {
                    // wait
                }
            }
            parent.locks[addr] = true;
            heap[addr] = std::move(value);
            parent.locks[addr] = false;
        }

        auto get(u16 addr) -> std::any { return heap[addr]; }
        auto lock(u16 addr) -> void { parent.locks[addr] = true; }
        auto unlock(u16 addr) -> void { parent.locks[addr] = false; }
        auto is_locked(u16 addr) -> bool { return parent.locks[addr]; }
        auto clear() -> void { heap.clear(); }
        auto remove(u16 addr) -> void { heap.erase(addr); }
        auto get() -> std::unordered_map<u16, std::any> { return heap; }
        auto set(std::unordered_map<u16, std::any> new_heap) -> void {
            heap = std::move(new_heap);
        }

        auto contains(u16 addr) -> bool { return heap.contains(addr); }
        auto empty() -> bool { return heap.empty(); }
        auto validate(u16 addr) -> bool {
            // check if an addr is not null
            if (this->contains(addr)) {
                return std::any_cast<void *>(this->get(addr)) != nullptr;
            }
            return false;
        };
    };

    class Stack {
      private:
        std::vector<std::any> stack;
        Memory &parent;

      public:
        Stack(Memory &parent)
            : parent(parent) {}

        auto push(std::any value) -> void { stack.push_back(std::move(value)); }
        auto pop() -> std::any {
            if (stack.empty()) {
                // panic
                std::print(colors::fg16::red, "Stack underflow", colors::reset);
            }
            auto value = stack.back();
            stack.pop_back();
            return value;
        }

        auto top() -> std::any {
            if (stack.empty()) {
                // panic
                std::print(colors::fg16::red, "Stack underflow", colors::reset);
            }
            return stack.back();
        }

        auto clear() -> void { stack.clear(); }
        auto size() -> u16 { return stack.size(); }
        auto empty() -> bool { return stack.empty(); }
    };
    friend struct Heap;
};

class VirtualMachine {
  private:
    Memory memory;

  public:
    VirtualMachine(const VirtualMachine &other) = default;
    VirtualMachine &operator=(const VirtualMachine &other) = default;
    VirtualMachine(VirtualMachine &&other) noexcept = default;
    VirtualMachine &operator=(VirtualMachine &&other) = default;

    VirtualMachine() = default;

    auto test_mem() -> void {
        Memory mem;
        mem.set_register(0, 10);
        std::print(mem.get_register(0));
        Memory::Heap heap(mem);
        heap.set(0, 10);
        std::print(heap.get(0));
        heap.lock(0);
        heap.set(0, 20);
        std::print(heap.get(0));
    }
};
}  // namespace vm

#endif  // __VM_H__