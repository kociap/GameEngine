#ifndef CORE_ANTON_STL_CONFIG_HPP_INCLUDE
#define CORE_ANTON_STL_CONFIG_HPP_INCLUDE

// #include <cstddef>
#include <cstdint>

#ifndef ANTON_NOEXCEPT
#    define ANTON_NOEXCEPT noexcept
#endif

using anton_size_t = uint64_t;
using anton_ptrdiff_t = int64_t;
// signed counterpart of anton_size_t
using anton_ssize_t = int64_t;

namespace anton_stl {
    using size_t = uint64_t;
    using ptrdiff_t = int64_t;
    // signed counterpart of anton_size_t
    using ssize_t = int64_t;

    // Reserve_Tag
    // Allows us to create constructors that allocate certain capacity, but do not initialize/resize
    struct Reserve_Tag {
        explicit Reserve_Tag() = default; // Explicit constructor so that it may be constructed via {}
    };
    constexpr Reserve_Tag reserve;

    // Variadic_Construct_Tag
    // Allows us to create variadic constructors
    struct Variadic_Construct_Tag {
        explicit Variadic_Construct_Tag() = default; // Explicit constructors so that it may not be constructed via {}
    };
    constexpr Variadic_Construct_Tag variadic_construct;
} // namespace anton_stl

#endif // !CORE_ANTON_STL_CONFIG_HPP_INCLUDE
