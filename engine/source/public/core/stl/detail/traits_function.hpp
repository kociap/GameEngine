#ifndef CORE_ANTON_STL_DETAIL_TRAITS_FUNCTION_HPP_INCLUDE
#define CORE_ANTON_STL_DETAIL_TRAITS_FUNCTION_HPP_INCLUDE

#include <core/stl/detail/traits_base.hpp>
#include <core/stl/detail/traits_common.hpp>

// TODO: calling conventions support
#define ANTON_DEFINE_CALL_QUALIFIER(CLASS, CONST_OPT, REF_OPT, NOEXCEPT_OPT) CLASS(/* CALL */, CONST_OPT, REF_OPT, NOEXCEPT_OPT)

#define ANTON_DEFINE_CALL_CONST_REF_QUALIFIERS(CLASS, NOEXCEPT_OPT) \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, , , NOEXCEPT_OPT)            \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, , &, NOEXCEPT_OPT)           \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, , &&, NOEXCEPT_OPT)          \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, const, , NOEXCEPT_OPT)       \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, const, &, NOEXCEPT_OPT)      \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, const, &&, NOEXCEPT_OPT)

#define ANTON_DEFINE_CALL_CONST_REF_NOEXCEPT_QUALIFIERS(CLASS) \
    ANTON_DEFINE_CALL_CONST_REF_QUALIFIERS(CLASS, )            \
    ANTON_DEFINE_CALL_CONST_REF_QUALIFIERS(CLASS, noexcept)

#define ANTON_DEFINE_CONST_REF_QUALIFIERS(CLASS, NOEXCEPT_OPT) \
    CLASS(, , NOEXCEPT_OPT)                                    \
    CLASS(, &, NOEXCEPT_OPT)                                   \
    CLASS(, &&, NOEXCEPT_OPT)                                  \
    CLASS(const, , NOEXCEPT_OPT)                               \
    CLASS(const, &, NOEXCEPT_OPT)                              \
    CLASS(const, &&, NOEXCEPT_OPT)

#define ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(CLASS) \
    ANTON_DEFINE_CONST_REF_QUALIFIERS(CLASS, )            \
    ANTON_DEFINE_CONST_REF_QUALIFIERS(CLASS, noexcept)

// #define ANTON_DEFINE_CALL_CONST_REF_NOEXCEPT_QUALIFIERS(CLASS)
// ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(CLASS, __stdcall)
// ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(CLASS, __cdecl)

namespace anton_engine::anton_stl {
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
    constexpr bool is_function = Is_Function<T>::value;

    namespace detail {
        template <typename Fn, typename... Args>
        struct Is_Invocable {
        private:
            template <typename XFn, typename... XArgs>
            static anton_stl::type_sink<anton_stl::True_Type, decltype(anton_stl::declval<XFn>(anton_stl::declval<XArgs>()...))> test(int);

            template <typename...>
            static anton_stl::False_Type test(...);

        public:
            using type = decltype(test<Fn, Args...>(0));
        };

        template <typename Return, typename Fn, typename... Args>
        struct Is_Invocable_R {
        private:
            template <typename XReturn, typename XFn, typename... XArgs>
            static anton_stl::enable_if<anton_stl::detail::is_same<XReturn, decltype(anton_stl::declval<XFn>(anton_stl::declval<XArgs>()...))>,
                                        anton_stl::True_Type>
            test(int);

            template <typename...>
            static anton_stl::False_Type test(...);

        public:
            using type = decltype(test<Return, Fn, Args...>(0));
        };
    } // namespace detail

    // Is_Invocable
    //
    template <typename Fn, typename... Args>
    struct Is_Invocable: detail::Is_Invocable<Fn, Args...>::type {};

    template <typename Fn, typename... Args>
    constexpr bool is_invocable = Is_Invocable<Fn, Args...>::value;

    // Is_Invocable_R
    //
    template <typename Return, typename Fn, typename... Args>
    struct Is_Invocable_R: detail::Is_Invocable_R<Return, Fn, Args...>::type {};

    template <typename Return, typename Fn, typename... Args>
    constexpr bool is_invocable_r = Is_Invocable_R<Return, Fn, Args...>::value;
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_DETAIL_TRAITS_FUNCTION_HPP_INCLUDE
