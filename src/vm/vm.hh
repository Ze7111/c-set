#ifndef __VM_H__
#define __VM_H__

#include <any>
#include <map>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <variant>

#include "../../pkgs/base/include.hh"
namespace vm {
const u32 num_registers = 96;
using PrimType = std::variant<int, double, const char*, char*, std::string>;

struct VM_Data {
    private:
        PrimType data;
    protected:
        VM_Data() = default;
    public:
        VM_Data(int value) : data(value) {}
        VM_Data(double value) : data(value) {}
        VM_Data(const char* value) : data(value) {}
        VM_Data(char* value) : data(value) {}
        VM_Data(std::string value) : data(value) {}
    
        ~VM_Data() = default;

        template <typename T>
        auto set(T value) -> void {
            data = value;
        }

        template <typename T>
        [[nodiscard]] auto get() const -> T {
            return std::get<T>(data);
        }

        [[nodiscard]] auto get() const -> PrimType {
            return data;
        }
};
class Memory {
  private:
    std::map<u32, bool> locks;
    std::vector<u64> registers;
    friend struct Heap;
    friend struct Stack;

  public:
    Memory()
        : registers(num_registers) {
            for (u32 i = 0; i < num_registers; i++) {
            registers[i] = 0;
        }
    }
    Memory(const Memory &other) = default;
    Memory(Memory &&other) noexcept = default;
    Memory &operator=(Memory &&other) = delete;
    Memory &operator=(const Memory &other) = delete;
    ~Memory() {
        registers.clear();
        locks.clear();
    }

    class Heap {
      public:
        Heap() = delete;
        explicit Heap(Memory *parent) : parent(parent) {};
        Heap(const Heap &other) = default;
        Heap(Heap &&other) noexcept = default;
        Heap &operator=(const Heap &other) = delete;
        Heap &operator=(Heap &&other) = delete;
        ~Heap() { clear(); }

        auto lock(u32 addr) const -> void;
        auto unlock(u32 addr) const -> void;
        auto remove(u32 addr) const -> void;
        auto clear() -> void;
        template <typename T>
        auto set(u32 addr, T value) -> void;
        [[nodiscard]] auto empty() const -> bool;
        template <typename T>
        [[nodiscard]] auto get(u32 addr) const -> T;
        [[nodiscard]] auto get(u32 addr) const -> PrimType;
        [[nodiscard]] auto contains(u32 addr) const -> bool;
        [[nodiscard]] auto is_locked(u32 addr) const -> bool;
        [[nodiscard]] auto size() const -> u32;
        

      private:
        std::unordered_map<u32, VM_Data> heap;
        Memory *parent;
    };

    class Stack {
      public:
        Stack() = delete;
        explicit Stack(Memory *parent) : parent(parent) {};
        Stack(const Stack &other) = default;
        Stack(Stack &&other) noexcept = default;
        Stack &operator=(const Stack &other) = delete;
        Stack &operator=(Stack &&other) = delete;
        ~Stack() { clear(); }


        template <typename T>
        auto push(T value) -> void;
        template <typename T>
        auto pop() -> T&;
        auto clear() -> void;
        template <typename T>
        [[nodiscard]] auto front() const -> T&;
        template <typename T>
        [[nodiscard]] auto back() const -> T&;
        [[nodiscard]] auto size() const -> u32;
        [[nodiscard]] auto empty() const -> bool;
        template <typename T>
        [[deprecated("should not be used")]] auto pop(u8 index) -> T&;

      private:
        std::vector<VM_Data> stack;
        Memory *parent;
    };

    auto set_register(u32 addr, u64 value) -> void;
    [[nodiscard]] auto get_register(u32 addr) const -> u64;
    [[nodiscard]] auto get_registers() const -> std::vector<u64>;
    [[nodiscard]] auto is_locked(u32 addr) const -> bool;
    auto lock_register(u32 addr) -> void;
    auto unlock_register(u32 addr) -> void;

    Heap *heap = new Heap(this);
    Stack *stack = new Stack(this);
};

class VirtualMachine {
  public:
    VirtualMachine() = default;
    VirtualMachine(const VirtualMachine &other) = delete;
    VirtualMachine(VirtualMachine &&other) noexcept = delete;
    VirtualMachine &operator=(const VirtualMachine &other) = delete;
    VirtualMachine &operator=(VirtualMachine &&other) = delete;
    ~VirtualMachine() {
        delete memory.heap;
        delete memory.stack;
    }

    auto mem_test_x96_x84() const -> void;

  private:
    Memory memory;
};
}  // namespace vm

#endif  // __VM_H__