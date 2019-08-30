#ifndef CORE_ANTON_STL_TYPE_TRAITS_HPP_INCLUDE
#define CORE_ANTON_STL_TYPE_TRAITS_HPP_INCLUDE

// Since volatile has been deprecated in C++20, this implementation provides no traits for removing, adding or identifying volatile.

// anton_stl::is_swappable, anton_stl::is_noexcept_swappable are defined in <anton_stl/utility>
// anton_stl::is_iterator_wrapper is defined in <anton_stl/iterators>

#include <anton_stl/config.hpp>
#include <anton_stl/detail/function_traits_macros.hpp>
#include <type_traits>

namespace anton_stl {
    //
    // Traits Base
    //

    template <typename T, T V>
    struct Integral_Constant {
        using type = Integral_Constant<T, V>;
        using value_type = T;

        constexpr static T value = V;

        constexpr operator value_type() const {
            return V;
        }

        constexpr value_type operator()() const {
            return V;
        }
    };

    template <bool V>
    using Bool_Constant = Integral_Constant<bool, V>;

    using True_Type = Bool_Constant<true>;
    using False_Type = Bool_Constant<false>;

    // Enable_If and Disable_If

    template <bool B, typename = void>
    struct Enable_If {};

    template <typename T>
    struct Enable_If<true, T> {
        using type = T;
    };

    template <bool B, typename T = void>
    using enable_if = typename Enable_If<B, T>::type;

    template <bool B, typename = void>
    struct Disable_If {};

    template <typename T>
    struct Disable_If<false, T> {
        using type = T;
    };

    template <bool B, typename T = void>
    using disable_if = typename Disable_If<B, T>::type;

    template <bool, typename T1, typename T2>
    struct Conditional {
        using type = T2;
    };

    template <typename T1, typename T2>
    struct Conditional<true, T1, T2> {
        using type = T1;
    };

    template <bool B, typename T1, typename T2>
    using conditional = typename Conditional<B, T1, T2>::type;

    // TODO rename
    template <typename...>
    using void_trait = void;

    //
    // Logical operators
    //

    // Negation
    // Unary trait that performs logical NOT on a single type trait.
    //
    // https://en.cppreference.com/w/cpp/types/negation
    //
    template <typename T>
    struct Negation: Bool_Constant<!bool(T::value)> {};

    template <typename T>
    inline constexpr bool negation = Negation<T>::value;

    // Conjunction
    // Trait that performs a short-circuiting logical AND on a sequence of type traits.
    // Inherits from the first trait that whose `value` is false or the last trait otherwise.
    // If the parameter pack is empty, inherits from anton_stl::True_Type.
    // Requires the types to be useable as a base class and expects them to provide `value` member.
    //
    // https://en.cppreference.com/w/cpp/types/conjunction
    //
    template <typename... T>
    struct Conjunction: True_Type {};

    template <typename T, typename... Ts>
    struct Conjunction<T, Ts...>: conditional<!bool(T::value), T, Conjunction<Ts...>> {};

    template <typename T>
    struct Conjunction<T>: T {};

    template <typename... T>
    inline constexpr bool conjunction = Conjunction<T...>::value;

    // Disjunction
    // Trait that performs a short-circuiting logical OR on a sequence of type traits.
    // Inherits from the first trait that whose `value` is true or the last trait otherwise.
    // If the parameter pack is empty, inherits from anton_stl::False_Type.
    // Requires the types to be useable as a base class and expects them to provide `value` member.
    //
    // https://en.cppreference.com/w/cpp/types/disjunction
    //
    template <typename... T>
    struct Disjunction: False_Type {};

    template <typename T, typename... Ts>
    struct Disjunction<T, Ts...>: conditional<bool(T::value), T, Disjunction<Ts...>> {};

    template <typename T>
    struct Disjunction<T>: T {};

    template <typename... T>
    inline constexpr bool disjunction = Disjunction<T...>::value;

    //
    // Type transformations
    //

    namespace detail {
        template <typename T, typename = void>
        struct Add_Reference {
            using lvalue = T;
            using rvalue = T;
        };

        template <typename T>
        struct Add_Reference<T, void_trait<T&>> {
            using lvalue = T&;
            using rvalue = T&&;
        };
    } // namespace detail

    // Add_Lvalue_Reference
    //
    template <typename T>
    struct Add_Lvalue_Reference {
        using type = typename detail::Add_Reference<T>::lvalue;
    };

    template <typename T>
    using add_lvalue_reference = typename Add_Lvalue_Reference<T>::type;

    // Add_Rvalue_Reference
    //
    template <typename T>
    struct Add_Rvalue_Reference {
        using type = typename detail::Add_Reference<T>::rvalue;
    };

    template <typename T>
    using add_rvalue_reference = typename Add_Rvalue_Reference<T>::type;

    // Remove_Extent
    //
    template <typename T>
    struct Remove_Extent {
        using type = T;
    };

    template <typename T>
    struct Remove_Extent<T[]> {
        using type = T;
    };

    template <typename T, anton_stl::size_t N>
    struct Remove_Extent<T[N]> {
        using type = T;
    };

    // Remove_All_Extents
    //
    template <typename T>
    struct Remove_All_Extents {
        using type = T;
    };

    template <typename T>
    struct Remove_All_Extents<T[]> {
        using type = typename Remove_All_Extents<T>::type;
    };

    template <typename T, anton_stl::size_t N>
    struct Remove_All_Extents<T[N]> {
        using type = typename Remove_All_Extents<T>::type;
    };

    // Remove_Function_Qualifiers
    // Removes all function qualifiers (const, &, && and noexcept) from a function type.
    // Does not support the volatile qualifier as volatile qualified member functions have been deprecated in C++20.
    //
    template <typename T>
    struct Remove_Function_Qualifiers {
        using type = T;
    };

#define ANTON_DEFINE_TRAIT_REMOVE_FUNCTION_QUALIFIERS(CALL_QUALIFIER, CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    template <typename R, typename... Params>                                                                             \
    struct Remove_Function_Qualifiers<R CALL_QUALIFIER(Params...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER> {     \
        using type = R(Params...);                                                                                        \
    };
    ANTON_DEFINE_CALL_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_REMOVE_FUNCTION_QUALIFIERS)
#undef ANTON_DEFINE_TRAIT_REMOVE_FUNCTION_QUALIFIERS

#define ANTON_DEFINE_TRAIT_REMOVE_ELLIPSIS_FUNCTION_QUALIFIERS(CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    /* No calling convention for ellipsis */                                                                       \
    template <typename R, typename... Params>                                                                      \
    struct Remove_Function_Qualifiers<R(Params..., ...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER> {        \
        using type = R(Params..., ...);                                                                            \
    };
    ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_REMOVE_ELLIPSIS_FUNCTION_QUALIFIERS)
#undef ANTON_DEFINE_TRAIT_REMOVE_ELLIPSIS_FUNCTION_QUALIFIERS

    template <typename T>
    struct Remove_Reference {
        using type = T;
    };

    template <typename T>
    struct Remove_Reference<T&> {
        using type = T;
    };

    template <typename T>
    struct Remove_Reference<T&&> {
        using type = T;
    };

    template <typename T>
    using remove_reference = typename Remove_Reference<T>::type;

    template <typename T>
    struct Remove_Const {
        using type = T;
    };

    template <typename T>
    struct Remove_Const<T const> {
        using type = T;
    };

    template <typename T>
    using remove_const = typename Remove_Const<T>::type;

    template <typename T>
    struct Remove_Const_Ref {
        using type = remove_const<remove_reference<T>>;
    };

    template <typename T>
    using remove_const_ref = typename Remove_Const_Ref<T>::type;

    //
    // Functions
    //

    // Note: Intentionally left unimplemented because it is intended to be used only in unevaluated context
    template <typename T>
    T&& declval();

    template <typename T>
    inline constexpr T&& forward(remove_reference<T>& v) ANTON_NOEXCEPT {
        return static_cast<T&&>(v);
    }

    template <typename T>
    inline constexpr T&& forward(remove_reference<T>&& v) ANTON_NOEXCEPT {
        // TODO is there any case where this assert would fail?
        // static_assert(!is_lvalue_reference<T>, "Can not forward an rvalue as an lvalue.");
        return static_cast<T&&>(v);
    }

    template <typename T>
    inline constexpr remove_reference<T>&& move(T&& v) ANTON_NOEXCEPT {
        return static_cast<remove_reference<T>&&>(v);
    }

    //
    // Type properties
    //

    // Is_Bounded_Array
    //
    template <typename T>
    struct Is_Bounded_Array: anton_stl::False_Type {};

    template <typename T, anton_stl::size_t N>
    struct Is_Bounded_Array<T[N]>: anton_stl::True_Type {};

    template <typename T>
    inline constexpr bool is_bounded_array = Is_Bounded_Array<T>::value;

    // Is_Unbounded_Array
    template <typename T>
    struct Is_Unbounded_Array: anton_stl::False_Type {};

    template <typename T>
    struct Is_Unbounded_Array<T[]>: anton_stl::True_Type {};

    template <typename T>
    inline constexpr bool is_unbounded_array = Is_Unbounded_Array<T>::value;

    // Is_Array
    //
    template <typename T>
    struct Is_Array: Disjunction<Is_Unbounded_Array<T>, Is_Bounded_Array<T>> {};

    template <typename T>
    inline constexpr bool is_array = Is_Array<T>::value;

    // Rank
    //
    template <typename T>
    struct Rank: anton_stl::Integral_Constant<anton_stl::size_t, 0> {};

    template <typename T>
    struct Rank<T[]>: anton_stl::Integral_Constant<anton_stl::size_t, Rank<T>::value + 1> {};

    template <typename T, anton_stl::size_t N>
    struct Rank<T[N]>: anton_stl::Integral_Constant<anton_stl::size_t, Rank<T>::value + 1> {};

    template <typename T>
    inline constexpr anton_stl::size_t rank = Rank<T>::value;

    // Extent
    //
    template <typename T, anton_stl::size_t N = 0>
    struct Extent: anton_stl::Integral_Constant<anton_stl::size_t, 0> {};

    template <typename T>
    struct Extent<T[], 0>: anton_stl::Integral_Constant<anton_stl::size_t, 0> {};

    template <typename T, anton_stl::size_t N>
    struct Extent<T[], N>: anton_stl::Integral_Constant<anton_stl::size_t, Extent<T, N - 1>::value> {};

    template <typename T, anton_stl::size_t E>
    struct Extent<T[E], 0>: anton_stl::Integral_Constant<anton_stl::size_t, E> {};

    template <typename T, anton_stl::size_t E, anton_stl::size_t N>
    struct Extent<T[E], N>: anton_stl::Integral_Constant<anton_stl::size_t, Extent<T, N - 1>::value> {};

    template <typename T>
    inline constexpr anton_stl::size_t extent = Extent<T>::value;

    // Is_Function
    // Does not support the volatile qualifier as volatile qualified member functions have been deprecated in C++20
    //
    template <typename T>
    struct Is_Function: anton_stl::False_Type {};

#define ANTON_DEFINE_TRAIT_IS_FUNCTION(CALL_QUALIFIER, CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    template <typename R, typename... Params>                                                              \
    struct Is_Function<R CALL_QUALIFIER(Params...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER>: anton_stl::True_Type {};
    ANTON_DEFINE_CALL_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_IS_FUNCTION)
#undef ANTON_DEFINE_TRAIT_IS_FUNCTION

#define ANTON_DEFINE_TRAIT_IS_FUNCTION_ELLIPSIS(CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    template <typename R, typename... Params>                                                       \
    struct Is_Function<R(Params..., ...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER>: anton_stl::True_Type {};
    ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_IS_FUNCTION_ELLIPSIS)
#undef ANTON_DEFINE_TRAIT_IS_FUNCTION_ELLIPSIS

    template <typename T>
    inline constexpr bool is_function = Is_Function<T>::value;

    // Is_Pointer
    //
    template <typename T>
    struct Is_Pointer: False_Type {};

    template <typename T>
    struct Is_Pointer<T*>: True_Type {};

    template <typename T>
    struct Is_Pointer<T* const>: True_Type {};

    template <typename T>
    inline constexpr bool is_pointer = Is_Pointer<T>::value;

    // Is_Lvalue_Reference
    //
    template <typename T>
    struct Is_Lvalue_Reference: False_Type {};

    template <typename T>
    struct Is_Lvalue_Reference<T&>: True_Type {};

    template <typename T>
    inline constexpr bool is_lvalue_reference = Is_Lvalue_Reference<T>::value;

    // Is_Rvalue_Reference
    //
    template <typename T>
    struct Is_Rvalue_Reference: False_Type {};

    template <typename T>
    struct Is_Rvalue_Reference<T&&>: True_Type {};

    template <typename T>
    inline constexpr bool is_rvalue_reference = Is_Rvalue_Reference<T>::value;

    // Is_Reference
    //
    template <typename T>
    struct Is_Reference: anton_stl::False_Type {};

    template <typename T>
    struct Is_Reference<T&>: anton_stl::True_Type {};

    template <typename T>
    struct Is_Reference<T&&>: anton_stl::True_Type {};

    template <typename T>
    inline constexpr bool is_reference = Is_Reference<T>::value;

    // Is_Same
    //
    template <typename T1, typename T2>
    struct Is_Same: False_Type {};

    template <typename T>
    struct Is_Same<T, T>: True_Type {};

    template <typename T1, typename T2>
    inline constexpr bool is_same = Is_Same<T1, T2>::value;

    // Is_Void
    //
    // Note: Does not support volatile qualified void as it has been deprecated in C++20.
    //
    template <typename T>
    struct Is_Void: Bool_Constant<is_same<void, remove_const<T>>> {};

    template <typename T>
    inline constexpr bool is_void = Is_Void<T>::value;

    namespace detail {
        template <typename T>
        struct Is_Complete {
        private:
            template <typename T, anton_stl::size_t = sizeof(T)>
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
    inline constexpr bool is_complete_type = Is_Complete_Type<T>::value;

    // Is_Empty
    //
    template <typename T>
    struct Is_Empty: public Bool_Constant<__is_empty(T)> {};

    template <typename T>
    inline constexpr bool is_empty = Is_Empty<T>::value;

    // Is_Trivial
    //
    template <typename T>
    struct Is_Trivial: public Bool_Constant<__is_trivial(T)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_trivial = Is_Trivial<T>::value;

    // Is_Assignable
    //
    // Note: We assume that all compilers we use support __is_assignable,
    // which apparently is the case for Clang, MSVC and GCC
    //
    template <typename T, typename U>
    struct Is_Assignable: Bool_Constant<__is_assignable(T, U)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T, typename U>
    inline constexpr bool is_assignable = Is_Assignable<T, U>::value;

    // Is_Move_Assignable
    //
    template <typename T>
    struct Is_Move_Assignable: Is_Assignable<add_lvalue_reference<T>, add_rvalue_reference<T>> {};

    template <typename T>
    inline constexpr bool is_move_assignable = Is_Move_Assignable<T>::value;

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
    inline constexpr bool is_constructible = Is_Constructible<T, Args...>::value;

    // Is_Default_Constructible
    template <typename T>
    struct Is_Default_Constructible: Is_Constructible<T> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_default_constructible = Is_Default_Constructible<T>::value;

    // Is_Copy_Constructible
    //
    template <typename T>
    struct Is_Copy_Constructible: Is_Constructible<T, add_lvalue_reference<T const>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_copy_constructible = Is_Copy_Constructible<T>::value;

    // Is_Move_Constructible
    //
    template <typename T>
    struct Is_Move_Constructible: Is_Constructible<T, add_rvalue_reference<T>> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_move_constructible = Is_Move_Constructible<T>::value;

    // Is_Trivially_Constructible
    //
    template <typename T, typename... Args>
    struct Is_Trivially_Constructible: public Bool_Constant<__is_trivially_constructible(T, Args...)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T, typename... Args>
    inline constexpr bool is_trivially_constructible = Is_Trivially_Constructible<T, Args...>::value;

    // Is_Trivially_Copy_Constructible
    //
    template <typename T>
    struct Is_Trivially_Copy_Constructible: public Bool_Constant<__is_trivially_constructible(T, add_lvalue_reference<T const>)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_trivially_copy_constructible = Is_Trivially_Copy_Constructible<T>::value;

    // Is_Trivially_Move_Constructible
    //
    template <typename T>
    struct Is_Trivially_Move_Constructible: public Bool_Constant<__is_trivially_constructible(T, add_rvalue_reference<T>)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_trivially_move_constructible = Is_Trivially_Move_Constructible<T>::value;

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
        inline constexpr bool is_implicitly_default_constructible = Is_Implicitly_Default_Constructible<T>::value;
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
    inline constexpr bool is_convertible = Is_Convertible<From, To>::value;
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
    inline constexpr bool is_convertible = Is_Convertible<From, To>::value;
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
    inline constexpr bool is_destructible = Is_Destructible<T>::value;

    // Is_Trivially_Destructible
    //
#if ANTON_COMPILER_ID == ANTON_COMPILER_CLANG || ANTON_COMPILER_ID == ANTON_COMPILER_MSVC
    template <typename T>
    struct Is_Trivially_Destructible: Bool_Constant<__is_trivially_destructible(T)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_trivially_destructible = Is_Trivially_Destructible<T>::value;
#else
    // GCC provides __has_trivial_destructor instead of __is_trivially_destructible

    template <typename T>
    struct Is_Trivially_Destructible: Bool_Constant<__has_trivial_destructor(T)> {
        static_assert(disjunction<Is_Complete_Type<T>, Is_Void<T>, Is_Unbounded_Array<T>>,
                      "Template argument must be a complete type, void or an unbounded array.");
    };

    template <typename T>
    inline constexpr bool is_trivially_destructible = Is_Trivially_Destructible<T>::value;
#endif // ANTON_COMPILER_ID == ANTON_COMPILER_CLANG || ANTON_COMPILER_ID == ANTON_COMPILER_MSVC
} // namespace anton_stl

#endif // !CORE_ANTON_STL_TYPE_TRAITS_HPP_INCLUDE
