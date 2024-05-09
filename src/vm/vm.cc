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
auto Memory::set_register(u32 addr, u64 value) -> void {
    if (addr >= num_registers) {
        throw std::out_of_range("register addr out of range");
    }
    
    lock_register(addr);
    registers[addr] = value;
    unlock_register(addr);
}

auto Memory::get_register(u32 addr) const -> u64 {
    if (addr >= num_registers) {
        throw std::out_of_range("register addr out of range");
    }
    
    if (is_locked(addr)) {
        return 0;
    }

    return registers.at(addr);
}

auto Memory::get_registers() const -> std::vector<u64> {
    return registers;
}

auto Memory::lock_register(u32 addr) -> void {
    if (addr >= num_registers) {
        throw std::out_of_range("register addr out of range");
    }

    if (is_locked(addr)) {
        throw std::runtime_error("register is already locked");
    }

    locks[addr] = true;
}

auto Memory::unlock_register(u32 addr) -> void {
    if (addr >= num_registers) {
        throw std::out_of_range("register addr out of range");
    }
    locks[addr] = false;
}

auto Memory::is_locked(u32 addr) const -> bool {
    if (addr >= num_registers) {
        throw std::out_of_range("register addr out of range");
    }
    return locks.at(addr);
}
/* --------------------------------- Stack ---------------------------------- */
template <typename T>
auto Memory::Stack::push(T value) -> void {
    stack.push_back(value);
}

template <typename T>
auto Memory::Stack::pop() -> T& {
    if (stack.empty()) {
        throw std::runtime_error("stack is empty");
    }

    T value = stack.back();
    stack.pop_back();
    return value;
}

auto Memory::Stack::clear() -> void {
    stack.clear();
}

template <typename T>
auto Memory::Stack::front() const -> T& {
    if (stack.empty()) {
        throw std::runtime_error("stack is empty");
    }

    return stack.front();
}

template <typename T>
auto Memory::Stack::back() const -> T& {
    if (stack.empty()) {
        throw std::runtime_error("stack is empty");
    }

    return stack.back();
}

auto Memory::Stack::size() const -> u32 {
    return stack.size();
}

auto Memory::Stack::empty() const -> bool {
    const bool is_empty = stack.empty();
    return is_empty;
}

template <typename T>
auto Memory::Stack::pop(u8 index) -> T& {
    throw std::runtime_error("deprecated, this is marked for removal and is considered unsafe");
}

/* ---------------------------------- Heap ---------------------------------- */

auto Memory::Heap::lock(u32 addr) const -> void {
    if (is_locked(addr)) {
        throw std::runtime_error("address is already locked");
    }
    
    parent->locks[addr] = true;
}

auto Memory::Heap::unlock(u32 addr) const -> void {
    if (!is_locked(addr)) {
        throw std::runtime_error("address is not locked");
    }

    parent->locks[addr] = false;
}

auto Memory::Heap::clear() -> void {
    heap.clear();
}

template <typename T>
auto Memory::Heap::set(u32 addr, T value) -> void {
    if (contains(addr)) {
        throw std::runtime_error("address already contains a value");
    }

    lock(addr);
    heap[addr] = VM_Data(value);
    unlock(addr);
}

auto Memory::Heap::empty() const -> bool {
    return heap.empty();
}

template <typename T>
auto Memory::Heap::get(u32 addr) const -> T {
    if (!contains(addr)) {
        throw std::runtime_error("address does not contain a value");
    }

    if (is_locked(addr)) {
        throw std::runtime_error("address is locked");
    }

    return std::any_cast<T>(heap.at(addr).get());
}

auto Memory::Heap::get(u32 addr) const -> PrimType {
    if (!contains(addr)) {
        throw std::runtime_error("address does not contain a value");
    }

    if (is_locked(addr)) {
        throw std::runtime_error("address is locked");
    }

    return heap.at(addr).get();
}

auto Memory::Heap::contains(u32 addr) const -> bool {
    return heap.contains(addr);
}





/* --------------------------- Virtual Machine ------------------------------ */

auto VirtualMachine::mem_test_x96_x84() const -> void {
    memory.heap->set(0x07CA053E7, "yes");
    memory.heap->set(0x07CA053E8, 42);
    memory.heap->set(0x07CA053E9, 3.14);

    
    std::print(memory.heap->get<const char*>(0x07CA053E7));
    std::print(memory.heap->get<int>(0x07CA053E8));
    std::print(memory.heap->get<double>(0x07CA053E9));
};

} // namespace vm