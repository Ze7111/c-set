#ifndef __THEAD_H__
#define __THEAD_H__

#include <concepts>
#include <functional>

/// \brief A concept that checks if a type is a worker thread manager.
/// \details The type must be default constructible and must have an `async`
///          method should maintain call order and not shuffle the calls.
///
/// \tparam T The type to check.
/// \return `true` if the type is a worker thread manager, `false` otherwise.
///
/// \note This concept is used to ensure that the `Lexer` class can work with
///       any type that implements the required interface for a worker thread
///       manager. The `async` method is expected to take a callable and
///       execute it asynchronously. The `std::default_initializable` concept
///       is used to ensure that the type can be default constructed, which is
///       necessary for the `Lexer` class to create an instance of the worker
///       thread manager. The `requires` clause checks that the `async` method
///       is callable with a `std::function<void()>` argument and that it
///       returns `void`. This ensures that the `async` method can be used to
///       execute any callable that takes no arguments and returns no value.
template <typename T>
concept WorkerThreadConcept =
    std::default_initializable<T> && requires(T t, std::function<void()> fn) {
        { t.async(fn) } -> std::same_as<void>;
    };

#endif  // __THEAD_H__