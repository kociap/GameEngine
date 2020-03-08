#ifndef CORE_ANTON_STL_CONFIG_HPP_INCLUDE
#define CORE_ANTON_STL_CONFIG_HPP_INCLUDE

namespace anton_engine::anton_stl {
    // Reserve_Tag
    // Allows us to create constructors that allocate certain capacity, but do not initialize/resize
    struct Reserve_Tag {
        explicit Reserve_Tag() = default; // Explicit constructor so that it may not be constructed via {}
    };
    inline constexpr Reserve_Tag reserve;

    // Variadic_Construct_Tag
    // Allows us to create variadic constructors
    struct Variadic_Construct_Tag {
        explicit Variadic_Construct_Tag() = default; // Explicit constructors so that it may not be constructed via {}
    };
    inline constexpr Variadic_Construct_Tag variadic_construct;

    // Range_Construct_Tag
    // For templated constructor overloads that take a pair of iterators
    struct Range_Construct_Tag {
        explicit Range_Construct_Tag() = default; // Explicit constructors so that it may not be constructed via {}
    };
    inline constexpr Range_Construct_Tag range_construct;
} // namespace anton_engine::anton_stl
#endif // !CORE_ANTON_STL_CONFIG_HPP_INCLUDE
