// a wrapper to make most of the std functions available in a pythonic way
// so like instead of doing std::cout << "Hello, World!" << std::endl;
// you can do print("Hello, World!");
// print and input are already defined in quick_io.hh

/*

A
abs()
aiter()
all()
anext()
any()
ascii()

B
bin()
bool()
breakpoint()
bytearray()
bytes()

C
callable()
chr()
classmethod()
compile()
complex()

D
delattr()
dict()
dir()
divmod()

E
enumerate()
eval()
exec()

F
filter()
float()
format()
frozenset()

G
getattr()
globals()

H
hasattr()
hash()
help()
hex()

I
id()
input()
int()
isinstance()
issubclass()
iter()
L
len()
list()
locals()

M
map()
max()
memoryview()
min()

N
next()

O
object()
oct()
open()
ord()

P
pow()
print()
property()




R
range()
repr()
reversed()
round()

S
set()
setattr()
slice()
sorted()
staticmethod()
str()
sum()
super()

T
tuple()
type()

V
vars()

Z
zip()

_
__import__()
*/

#ifndef CUSTOM_STL_H
#define CUSTOM_STL_H

#include <any>
#include <array>
#include <bitset>
#include <cstddef>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace std_v2 {
    using str = std::string;
    template <typename T>
    using list = std::list<T>;
    template <typename T>
    using vec = std::vector<T>;
    template <typename T, size_t N>
    using arr = std::array<T, N>;

    namespace c_types {
        template <typename T>
        using uptr = std::unique_ptr<T>;
        template <typename T>
        using sptr = std::shared_ptr<T>;
        template <typename T>
        using wptr = std::weak_ptr<T>;
        template <typename T>
        using ptr = T *;
        template <typename T>
        using ref = T &;
        template <typename T>
        using cref = const T &;
        template <typename T>
        using rref = T &&;
        template <typename T>
        using tref = std::reference_wrapper<T>;
    }  // namespace c_types

    template <typename T>
    using Any = std::any;
    template <typename T, typename U>
    using is_same = std::is_same<T, U>;
    template <typename T>
    using remove_cv = std::remove_cv<T>;
    template <typename T>
    using remove_reference = std::remove_reference<T>;
    template <typename T>
    using remove_cvref = remove_cv<remove_reference<T>>;
    template <typename T>
    using remove_pointer = std::remove_pointer<T>;

    template <typename T>
    inline T *allocate(size_t size) {
        auto ptr = std::make_unique<T[]>(size + 1);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        if constexpr (is_same<T, char>::value) {
            ptr[size] = '\0';
        } else if constexpr (is_same<T, wchar_t>::value) {
            ptr[size] = L'\0';
        } else if constexpr (is_same<T, char16_t>::value) {
            ptr[size] = u'\0';
        } else if constexpr (is_same<T, char32_t>::value) {
            ptr[size] = U'\0';
        } else {
            ptr[size] = nullptr;  // nullptr for other types
        }

        return (T *)ptr;
    }
    template <typename T>
    inline void deallocate(T *ptr) {
        free(ptr);
    }
    template <typename T>
    inline void write(T *ptr, T value) {
        *ptr = value;
    }
    template <typename T>
    inline T *fill(T *ptr, size_t count, T value) {
        for (size_t i = 0; i < count; i++) {
            write(ptr + i, value);
        }
        return ptr;
    }
    template <typename T>
    inline T abs(T value) {
        return value < 0 ? -value : value;
    }

    namespace math {
        inline double pow(double base, double exponent) {
            return std::pow(base, exponent);
        }
        inline double hypot(double x, double y) { return std::hypot(x, y); }
        inline double sqrt(double value)  { return std::sqrt(value); }
        inline double cbrt(double value)  { return std::cbrt(value); }
        inline double exp(double value)   { return std::exp(value); }
        inline double log(double value)   { return std::log(value); }
        inline double log10(double value) { return std::log10(value); }
        inline double sin(double value)   { return std::sin(value); }
        inline double cos(double value)   { return std::cos(value); }
        inline double tan(double value)   { return std::tan(value); }
        inline double asin(double value)  { return std::asin(value); }
        inline double acos(double value)  { return std::acos(value); }
        inline double atan(double value)  { return std::atan(value); }
        inline double sinh(double value)  { return std::sinh(value); }
        inline double cosh(double value)  { return std::cosh(value); }
        inline double tanh(double value)  { return std::tanh(value); }
        inline void srand(unsigned int seed) { std::srand(seed); }
        inline int rand() { return std::rand(); }
    }  // namespace math

    template <typename T>
    size_t len(T &container) {
        return container.size();
    }

    template <typename T>
    inline vec<T> range(T start, T end, T step = 1) {
        vec<T> result;
        for (T i = start; i < end; i += step) {
            result.push_back(i);
        }
        return result;
    }
    template <typename T>
    inline vec<T> range(T end) {
        return range<T>(0, end);
    }
    template <typename T>
    inline vec<T> range(T end, T step) {
        return range<T>(0, end, step);
    }

    template <typename... Args>
    inline auto sum(Args... args) {
        return (args + ...);
    }
    template <typename T>
    inline T sum(vec<T> &container) {
        return std::accumulate(container.begin(), container.end(), 0);
    }
    template <typename T, size_t N>
    inline T sum(arr<T, N> &container) {
        return std::accumulate(container.begin(), container.end(), 0);
    }
    template <typename T>
    inline T sum(list<T> &container) {
        return std::accumulate(container.begin(), container.end(), 0);
    }

    template <typename T>
    inline T next(T &container) {
        return container.pop_front();
    }
    template <typename T>
    inline T next(vec<T> &container) {
        return container.pop_back();
    }
    template <typename T, size_t N>
    inline T next(arr<T, N> &container) {
        return container.pop_back();
    }
    template <typename T>
    inline T next(list<T> &container) {
        return container.pop_front();
    }

    inline char chr(int value) { return (char)value; }
    inline int ord(char value) { return (int)value; }

    inline str bin(int value) { return std::bitset<8>(value).to_string(); }
    inline str bin(long value) { return std::bitset<8>(value).to_string(); }
    inline str bin(long long value) {
        return std::bitset<8>(value).to_string();
    }
    inline str bin(unsigned int value) {
        return std::bitset<8>(value).to_string();
    }
    inline str bin(unsigned long value) {
        return std::bitset<8>(value).to_string();
    }
    inline str bin(unsigned long long value) {
        return std::bitset<8>(value).to_string();
    }

    inline str hex(int value) { return std::to_string(value); }
    inline str hex(long value) { return std::to_string(value); }
    inline str hex(long long value) { return std::to_string(value); }
    inline str hex(unsigned int value) { return std::to_string(value); }
    inline str hex(unsigned long value) { return std::to_string(value); }
    inline str hex(unsigned long long value) { return std::to_string(value); }

    inline str oct(int value) { return std::to_string(value); }
    inline str oct(long value) { return std::to_string(value); }
    inline str oct(long long value) { return std::to_string(value); }
    inline str oct(unsigned int value) { return std::to_string(value); }
    inline str oct(unsigned long value) { return std::to_string(value); }
    inline str oct(unsigned long long value) { return std::to_string(value); }

    inline str ascii(char value) { return std::to_string(value); }
    inline str ascii(int value) { return std::to_string(value); }
    inline str ascii(long value) { return std::to_string(value); }
    inline str ascii(long long value) { return std::to_string(value); }
    inline str ascii(unsigned int value) { return std::to_string(value); }
    inline str ascii(unsigned long value) { return std::to_string(value); }
    inline str ascii(unsigned long long value) { return std::to_string(value); }

    //inline str format(str fmt, auto... args) {
    //    return std::format(fmt, args...);
    //}

    namespace sysIO {
        using endl = struct endl {
            std::string end_l = "\n";
            endl() = default;
            explicit endl(std::string end)
                : end_l(std::move(end)) {}
            explicit endl(const char *end)
                : end_l(end) {}
            explicit endl(const char end)
                : end_l(std::string(1, end)) {}
            endl(const endl &end) = default;
            endl(endl &&) = default;  // Add move constructor
            endl &
            operator=(const endl &) = default;   // Add copy assignment operator
            endl &operator=(endl &&) = default;  // Add move assignment operator
            ~endl() = default;                   // Add destructor
            friend std::ostream &operator<<(std::ostream &oss,
                                            const endl &end) {
                oss << end.end_l;
                return oss;
            }
        };
    }  // namespace sysIO
    template <typename... Args>
    inline void print(Args &&...args) {
        if constexpr (sizeof...(args) == 0) {
            std::cout << sysIO::endl('\n');
            return;
        };
        (std::cout << ... << args);
        if constexpr (sizeof...(args) > 0) {
            if constexpr (!std::is_same_v<
                              std::remove_cv_t<std::remove_reference_t<
                                  decltype(std::get<sizeof...(args) - 1>(
                                      std::tuple<Args...>(args...)))>>,
                              sysIO::endl>) {
                std::cout << sysIO::endl('\n');
            }
        }
    }
    class input {
      private:
        std::string _input;

      public:
        input() { std::getline(std::cin, _input); }
        explicit input(const std::string &prompt) {
            print(prompt, sysIO::endl(" > "));
            std::getline(std::cin, _input);
        }
        input(const std::string &prompt, std::string end) {
            print(prompt, sysIO::endl(std::move(end)));
            std::getline(std::cin, _input);
        }
        input(const std::string &prompt, char end) {
            print(prompt, sysIO::endl(end));
            std::getline(std::cin, _input);
        }
        input(const std::string &prompt, const sysIO::endl &end) {
            print(prompt, end);
            std::getline(std::cin, _input);
        }
        template <typename... Args>
        explicit input(std::string prompt, Args &&...format) {
            print(prompt, sysIO::endl(' '), std::forward<Args>(format)...);
            std::getline(std::cin, _input);
        }
        ~input() = default;
        int to_int() { return std::stoi(_input); }
        float to_float() { return std::stof(_input); }
        double to_double() { return std::stod(_input); }
        std::string to_string() { return _input; }
        char to_char() { return _input[0]; }
        bool to_bool() { return _input == "true" || _input == "1"; }
        template <typename T>
        T to() {
            return T(_input);
        }
        template <typename T>
        explicit operator T() {
            return T(_input);
        }
        friend std::ostream &operator<<(std::ostream &oss, const input &inn) {
            oss << inn._input;
            return oss;
        }
        friend std::istream &operator>>(std::istream &iss, input &inn) {
            iss >> inn._input;
            return iss;
        }

        input(const input &) = delete;
        input(input &&) = delete;
        input &operator=(const input &) = delete;
        input &operator=(input &&) = delete;
        template <typename T>
        T operator=(T) = delete;
        template <typename T>
        T operator=(T &&) = delete;
    };
}  // namespace std_v2

#endif  // CUSTOM_STL_H