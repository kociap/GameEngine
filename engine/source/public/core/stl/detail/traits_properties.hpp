#ifndef CORE_ANTON_STL_DETAIL_TRAITS_PROPERTIES_HPP_INCLUDE
#define CORE_ANTON_STL_DETAIL_TRAITS_PROPERTIES_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/detail/traits_base.hpp>
#include <core/stl/detail/traits_common.hpp>
#include <core/stl/detail/traits_function.hpp>
#include <core/stl/detail/traits_transformations.hpp>

namespace anton_engine::anton_stl {
    // Is_Bounded_Array
    //
    template <typename T>
    struct Is_Bounded_Array: anton_stl::False_Type {};

    template <typename T, usize N>
    struct Is_Bounded_Array<T[N]>: anton_stl::True_Type {};

    template <typename T>
    constexpr bool is_bounded_array = Is_Bounded_Array<T>::value;

    // Is_Unbounded_Array
    template <typename T>
    struct Is_Unbounded_Array: anton_stl::False_Type {};

    template <typename T>
    struct Is_Unbounded_Array<T[]>: anton_stl::True_Type {};

    template <typename T>
    constexpr bool is_unbounded_array = Is_Unbounded_Array<T>::value;

    // Is_Array
    //
    template <typename T>
    struct Is_Array: detail::Is_Array<T> {};

    template <typename T>
    constexpr bool is_array = Is_Array<T>::value;

    // Rank
    //
    template <typename T>
    struct Rank: anton_stl::Integral_Constant<usize, 0> {};

    template <typename T>
    struct Rank<T[]>: anton_stl::Integral_Constant<usize, Rank<T>::value + 1> {};

    template <typename T, usize N>
    struct Rank<T[N]>: anton_stl::Integral_Constant<usize, Rank<T>::value + 1> {};

    template <typename T>
    constexpr usize rank = Rank<T>::value;

    // Extent
    //
    template <typename T, usize N = 0>
    struct Extent: anton_stl::Integral_Constant<usize, 0> {};

    template <typename T>
    struct Extent<T[], 0>: anton_stl::Integral_Constant<usize, 0> {};

    template <typename T, usize N>
    struct Extent<T[], N>: anton_stl::Integral_Constant<usize, Extent<T, N - 1>::value> {};

    template <typename T, usize E>
    struct Extent<T[E], 0>: anton_stl::Integral_Constant<usize, E> {};

    template <typename T, usize E, usize N>
    struct Extent<T[E], N>: anton_stl::Integral_Constant<usize, Extent<T, N - 1>::value> {};

    template <typename T>
    constexpr usize extent = Extent<T>::value;

    // Is_Pointer
    //
    template <typename T>
    struct Is_Pointer: False_Type {};

    template <typename T>
    struct Is_Pointer<T*>: True_Type {};

    template <typename T>
    struct Is_Pointer<T* const>: True_Type {};

    template <typename T>
    constexpr bool is_pointer = Is_Pointer<T>::value;

    // Is_Lvalue_Reference
    //
    template <typename T>
    struct Is_Lvalue_Reference: False_Type {};

    template <typename T>
    struct Is_Lvalue_Reference<T&>: True_Type {};

    template <typename T>
    constexpr bool is_lvalue_reference = Is_Lvalue_Reference<T>::value;

    // Is_Rvalue_Reference
    //
    template <typename T>
    struct Is_Rvalue_Reference: False_Type {};

    template <typename T>
    struct Is_Rvalue_Reference<T&&>: True_Type {};

    template <typename T>
    constexpr bool is_rvalue_reference = Is_Rvalue_Reference<T>::value;

    // Is_Reference
    //
    template <typename T>
    struct Is_Reference: anton_stl::False_Type {};

    template <typename T>
    struct Is_Reference<T&>: anton_stl::True_Type {};

    template <typename T>
    struct Is_Reference<T&&>: anton_stl::True_Type {};

    template <typename T>
    constexpr bool is_reference = Is_Reference<T>::value;

    // Is_Same
    //
    using detail::Is_Same;
    using detail::is_same;

    // Is_Any_Of
    //
    template <typename T, typename... Types>
    struct Is_Any_Of: anton_stl::conditional<(... || anton_stl::is_same<T, Types>), anton_stl::True_Type, anton_stl::False_Type> {};

    template <typename T, typename... Types>
    constexpr bool is_any_of = Is_Any_Of<T, Types...>::value;

    // Is_Void
    //
    // Note: Does not support volatile qualified void as it has been deprecated in C++20.
    //
    template <typename T>
    struct Is_Void: Bool_Constant<is_same<void, remove_const<T>>> {};

    template <typename T>
    constexpr bool is_void = Is_Void<T>::value;

    namespace detail {
        template <typename T>
        struct Is_Complete {
        private:
            template <typename T, usize = sizeof(T)>
            static anton_stl::True_Type test(int);

            template <typename T>
            static conditional<is_function<T>, anton_stl::True_Type, anton_stl::False_Type> test(...);

        public:
            using type = decltype(test<T>(0));
        };
    } // namespace detail

    // Is_Complete_Type
    // Checks whether a type is a complete type
    //
    template <typename T>
    struct Is_Complete_Type: detail::Is_Complete<T>::type {};

    template <typename T>
    constexpr bool is_complete_type = Is_Complete_Type<T>::value;

    // Is_Empty
    //
    template <typename T>
    struct Is_Empty: public Bool_Constant<__is_empty(T)> {};

    template <typename T>
    constexpr bool is_empty = Is_Empty<T>::value;

    // Is_Trivial
    //
    template <typename T>
    struct Is_Trivial: public Bool_Constant<__is_trivial(T)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivial = Is_Trivial<T>::value;

    // Is_Assignable
    //
    // Note: We assume that all compilers we use support __is_assignable,
    // which apparently is the case for Clang, MSVC and GCC
    //
    template <typename To, typename From>
    struct Is_Assignable: Bool_Constant<__is_assignable(To, From)> {
        static_assert(disjunction<Is_Complete_Type<To>, Is_Void<To>, Is_Unbounded_Array<To>, Is_Complete_Type<From>, Is_Void<From>, Is_Unbounded_Array<From>>,
                      "Template arguments must be complete types, void or unbounded arrays.");
    };

    template <typename T, typename U>
    constexpr bool is_assignable = Is_Assignable<T, U>::value;

    // Is_Copy_Assignable
    //
    template <typename T>
    struct Is_Copy_Assignable: Is_Assignable<add_lvalue_reference<T>, add_lvalue_reference<T>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_copy_assignable = Is_Copy_Assignable<T>::value;

    // Is_Move_Assignable
    //
    template <typename T>
    struct Is_Move_Assignable: Is_Assignable<add_lvalue_reference<T>, add_rvalue_reference<T>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_move_assignable = Is_Move_Assignable<T>::value;

    // Is_Trivially_Assignable
    //
    template <typename To, typename From>
    struct Is_Trivially_Assignable: Bool_Constant<__is_trivially_assignable(To, From)> {
        static_assert(disjunction<Is_Complete_Type<To>, Is_Void<To>, Is_Unbounded_Array<To>, Is_Complete_Type<From>, Is_Void<From>, Is_Unbounded_Array<From>>,
                      "Template arguments must be complete types, void or unbounded arrays.");
    };

    template <typename To, typename From>
    constexpr bool is_trivially_assignable = Is_Trivially_Assignable<To, From>::value;

    // Is_Trivially_Copy_Assignable
    //
    template <typename T>
    struct Is_Trivially_Copy_Assignable: Is_Trivially_Assignable<add_lvalue_reference<T>, add_lvalue_reference<T>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivially_copy_assignable = Is_Trivially_Copy_Assignable<T>::value;

    // Is_Trivially_Move_Assignable
    //
    template <typename T>
    struct Is_Trivially_Move_Assignable: Is_Trivially_Assignable<add_lvalue_reference<T>, add_rvalue_reference<T>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivially_move_assignable = Is_Trivially_Move_Assignable<T>::value;

    // Is_Constructible
    //
    // Note: We assume that all compilers we use support __is_constructible,
    // which apparently is the case for Clang, MSVC and GCC
    //
    template <typename T, typename... Args>
    struct Is_Constructible: Bool_Constant<__is_constructible(T, Args...)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T, typename... Args>
    constexpr bool is_constructible = Is_Constructible<T, Args...>::value;

    // Is_Default_Constructible
    template <typename T>
    struct Is_Default_Constructible: Is_Constructible<T> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_default_constructible = Is_Default_Constructible<T>::value;

    // Is_Copy_Constructible
    //
    template <typename T>
    struct Is_Copy_Constructible: Is_Constructible<T, add_lvalue_reference<T const>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_copy_constructible = Is_Copy_Constructible<T>::value;

    // Is_Move_Constructible
    //
    template <typename T>
    struct Is_Move_Constructible: Is_Constructible<T, add_rvalue_reference<T>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_move_constructible = Is_Move_Constructible<T>::value;

    // Is_Trivially_Constructible
    //
    template <typename T, typename... Args>
    struct Is_Trivially_Constructible: public Bool_Constant<__is_trivially_constructible(T, Args...)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T, typename... Args>
    constexpr bool is_trivially_constructible = Is_Trivially_Constructible<T, Args...>::value;

    // Is_Trivially_Copy_Constructible
    //
    template <typename T>
    struct Is_Trivially_Copy_Constructible: public Bool_Constant<__is_trivially_constructible(T, add_lvalue_reference<T const>)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivially_copy_constructible = Is_Trivially_Copy_Constructible<T>::value;

    // Is_Trivially_Move_Constructible
    //
    template <typename T>
    struct Is_Trivially_Move_Constructible: public Bool_Constant<__is_trivially_constructible(T, add_rvalue_reference<T>)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivially_move_constructible = Is_Trivially_Move_Constructible<T>::value;

    namespace detail {
        // Is_Implicitly_Default_Constructible
        //
        template <typename T, typename = void>
        struct Is_Implicitly_Default_Constructible: False_Type {};

        template <typename T>
        void check_implicit_default_construct(T const&);

        template <typename T>
        struct Is_Implicitly_Default_Constructible<T, void_trait<decltype(check_implicit_default_construct<T>({}))>>: True_Type {};

        template <typename T>
        constexpr bool is_implicitly_default_constructible = Is_Implicitly_Default_Constructible<T>::value;
    } // namespace detail

    // Is_Convertible
    //
#if ANTON_COMPILER_ID == ANTON_COMPILER_CLANG || ANTON_COMPILER_ID == ANTON_COMPILER_MSVC
    // Both Clang and MSVC support __is_convertible_to

    template <typename From, typename To>
    struct Is_Convertible: Bool_Constant<__is_convertible_to(From, To)> {
        static_assert(disjunction<Is_Complete_Type<From>, Is_Complete_Type<To>, Is_Void<From>, Is_Void<To>, Is_Unbounded_Array<From>, Is_Unbounded_Array<To>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename From, typename To>
    constexpr bool is_convertible = Is_Convertible<From, To>::value;
#else
    // Sadly GCC does not offer any intrinsic similar to __is_convertible_to

    namespace detail {
        template <typename From, typename To, bool = disjunction<Is_Void<To>, Is_Array<To>, Is_Function<To>>>
        class Is_Convertible_Helper {
            template <typename F, typename T, typename = enable_if<is_void<F> && is_void<T>>>
            static True_Type test(int);

            template <typename, typename>
            static False_Type test(...);

        public:
            using type = decltype(test<From, To>(0));
        };

        template <typename From, typename To>
        class Is_Convertible_Helper<From, To, false> {
            template <typename T>
            static void test_aux(T);

            template <typename F, typename T, typename = decltype(test_aux<T>(anton_stl::declval<F>()))>
            static True_Type test(int);

            template <typename, typename>
            static False_Type test(...);

        public:
            using type = decltype(test<From, To>(0));
        };
    } // namespace detail

    template <typename From, typename To>
    struct Is_Convertible: detail::Is_Convertible_Helper<From, To>::type {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename From, typename To>
    constexpr bool is_convertible = Is_Convertible<From, To>::value;
#endif // ANTON_COMPILER_ID == ANTON_COMPILER_CLANG || ANTON_COMPILER_ID == ANTON_COMPILER_MSVC

    namespace detail {
        // If T is a function, void or an array of unknown bound, return false.
        // If T is a reference or anton_stl::declval<T&>().~T() is well formed return true.
        // otherwise, return false.
        //
        template <typename T, bool = disjunction<Is_Void<T>, Is_Unbounded_Array<T>, Is_Function<T>>>
        struct Is_Destructible {
            using type = anton_stl::False_Type;
        };

        template <typename T>
        struct Is_Destructible<T, false> {
        private:
            template <typename U, typename = anton_stl::void_trait<decltype(anton_stl::declval<U&>().~U())>>
            static anton_stl::True_Type test(int);

            template <typename U>
            static anton_stl::False_Type test(...);

        public:
            using type = decltype(test<T>(0));
        };
    } // namespace detail

    // Is_Destructible
    //
    template <typename T>
    struct Is_Destructible: detail::Is_Destructible<T>::type {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_destructible = Is_Destructible<T>::value;

    // Is_Trivially_Destructible
    //
#if ANTON_COMPILER_ID == ANTON_COMPILER_CLANG || ANTON_COMPILER_ID == ANTON_COMPILER_MSVC
    template <typename T>
    struct Is_Trivially_Destructible: Bool_Constant<__is_trivially_destructible(T)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivially_destructible = Is_Trivially_Destructible<T>::value;
#else
    // GCC provides __has_trivial_destructor instead of __is_trivially_destructible

    template <typename T>
    struct Is_Trivially_Destructible: Bool_Constant<__has_trivial_destructor(T)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    constexpr bool is_trivially_destructible = Is_Trivially_Destructible<T>::value;
#endif // ANTON_COMPILER_ID == ANTON_COMPILER_CLANG || ANTON_COMPILER_ID == ANTON_COMPILER_MSVC

    namespace detail {
        template <typename T>
        struct Is_Integral: False_Type {};
        // clang-format off
        template<> struct Is_Integral<bool>: True_Type {};
        template<> struct Is_Integral<char>: True_Type {};
        template<> struct Is_Integral<signed char>: True_Type {};
        template<> struct Is_Integral<unsigned char>: True_Type {};
        // TODO: char8_t
        // template<> struct Is_Integral<char8_t>: True_Type {};
        template<> struct Is_Integral<char16_t>: True_Type {};
        template<> struct Is_Integral<char32_t>: True_Type {};
        template<> struct Is_Integral<wchar_t>: True_Type {};
        template<> struct Is_Integral<short>: True_Type {};
        template<> struct Is_Integral<unsigned short>: True_Type {};
        template<> struct Is_Integral<int>: True_Type {};
        template<> struct Is_Integral<unsigned int>: True_Type {};
        template<> struct Is_Integral<long>: True_Type {};
        template<> struct Is_Integral<unsigned long>: True_Type {};
        template<> struct Is_Integral<long long>: True_Type {};
        template<> struct Is_Integral<unsigned long long>: True_Type {};
        // clang-format on
    } // namespace detail

    // Is_Integral
    template <typename T>
    struct Is_Integral: detail::Is_Integral<remove_const<T>> {};

    template <typename T>
    inline constexpr bool is_integral = Is_Integral<T>::value;

    namespace detail {
        template <typename T>
        struct Is_Floating_Point: False_Type {};
        // clang-format off
        template<> struct Is_Floating_Point<float>: True_Type {};
        template<> struct Is_Floating_Point<double>: True_Type {};
        template<> struct Is_Floating_Point<long double>: True_Type {};
        // clang-format on
    } // namespace detail

    // Is_Floating_Point
    template <typename T>
    struct Is_Floating_Point: detail::Is_Floating_Point<remove_const<T>> {};

    template <typename T>
    inline constexpr bool is_floating_point = Is_Floating_Point<T>::value;

    // Is_Arithmetic
    template <typename T>
    struct Is_Arithmetic: Bool_Constant<is_integral<T> || is_floating_point<T>> {};

    template <typename T>
    inline constexpr bool is_arithmetic = Is_Arithmetic<T>::value;

    // Is_Signed
    template <typename T, bool arithmetic = is_arithmetic<T>, bool integral = is_integral<T>>
    struct Is_Signed: False_Type {};

    template <typename T>
    struct Is_Signed<T, true, true>: Bool_Constant<(T(-1) < T(0))> {};

    template <typename T>
    struct Is_Signed<T, true, false>: True_Type {}; // floating point

    template <typename T>
    inline constexpr bool is_signed = Is_Signed<T>::value;

    // Is_Unsigned
    template <typename T, bool arithmetic = is_arithmetic<T>, bool integral = is_integral<T>>
    struct Is_Unsigned: False_Type {};

    template <typename T>
    struct Is_Unsigned<T, true, true>: Bool_Constant<(T(-1) > T(0))> {};

    template <typename T>
    struct Is_Unsigned<T, true, false>: False_Type {}; // floating point

    template <typename T>
    inline constexpr bool is_unsigned = Is_Unsigned<T>::value;

} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_DETAIL_TRAITS_PROPERTIES_HPP_INCLUDE
