#ifndef __VM_H__
#define __VM_H__

#include <any>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "../../pkgs/base/include.hh"
namespace vm {
class Memory {
  private:
    std::map<u32, bool> locks;
    std::vector<std::any> registers;
    friend struct Heap;
    friend struct Stack;

  public:
    const  u32 num_registers = 96;
    struct Heap;
    struct Stack;

    struct Data {
      private:
        std::pair<std::any, std::type_index> val =
            std::make_pair(std::any(), std::type_index(typeid(void)));

      public:
        Data() = default;
        Data(const Data &other) = default;
        Data(Data &&other) noexcept = default;
        Data &operator=(const Data &other) = default;
        Data &operator=(Data &&other) noexcept = default;
        Data(std::any val, std::type_index type)
            : val(std::move(val), type){};
        ~Data() = default;

        template <typename T>
        auto data() -> T* { return &std::any_cast<T>(&val.first); }
        auto type() -> std::decay_t<decltype(val.second)>* { return &val.second; }
    };

    Memory(): registers(num_registers){};
    Memory(const Memory &other) = default;
    Memory(Memory &&other) noexcept = default;
    Memory &operator=(Memory &&other) = delete;
    Memory &operator=(const Memory &other) = delete;
    ~Memory() {
        registers.clear();
        locks.clear();
    }

    auto set_register(u32 index, u64 value) -> void;
    [[nodiscard]] auto get_register(u32 index) const -> u64;
    [[nodiscard]] auto get_registers() const -> std::vector<u64>;

    class Heap {
      public:
        explicit Heap(Memory *parent);
        ~Heap() { clear(); }

        auto lock(u32 addr) -> void;
        auto unlock(u32 addr) -> void;
        auto remove(u32 addr) -> void;
        auto clear() -> void;
        auto set(u32 addr, Memory::Data value) -> void;
        [[nodiscard]] auto empty() const -> bool;
        [[nodiscard]] auto get(u32 addr) const -> Memory::Data;
        [[nodiscard]] auto contains(u32 addr) const -> bool;
        [[nodiscard]] auto is_locked(u32 addr) const -> bool;
        [[nodiscard]] auto get() const -> std::unordered_map<u32, Memory::Data>;

      private:
        std::unordered_map<u32, Memory::Data> heap;
        Memory *parent;
    };

    class Stack {
      public:
        explicit Stack(Memory *parent);
        ~Stack() { clear(); }

        auto push(std::any value) -> void;
        auto pop() -> std::any;
        auto clear() -> void;
        [[nodiscard]] auto front() const -> std::any;
        [[nodiscard]] auto back() const -> std::any;
        [[nodiscard]] auto size() const -> u32;
        [[nodiscard]] auto empty() const -> bool;
        [[deprecated("should not be used")]] auto pop(u8 index) -> std::any;

      private:
        std::vector<std::any> stack;
        Memory *parent;
    };
};

class VirtualMachine {
  public:
    VirtualMachine();
    void mem_test() {
        heap.set(0x07CA053E7, Memory::Data("Hello, World!", typeid(std::string)));
        heap.set(0x07CA053E8, Memory::Data(42, typeid(u64)));
        heap.set(0x07CA053E9, Memory::Data(3.14, typeid(double)));
    };

  private:
    Memory memory;
    Memory::Heap heap   = Memory::Heap(&memory);
    Memory::Stack stack = Memory::Stack(&memory);
};
}  // namespace vm

#endif  // __VM_H__