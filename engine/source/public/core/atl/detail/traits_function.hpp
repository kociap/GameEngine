#ifndef CORE_ATL_DETAIL_TRAITS_FUNCTION_HPP_INCLUDE
#define CORE_ATL_DETAIL_TRAITS_FUNCTION_HPP_INCLUDE

#include <core/atl/detail/traits_base.hpp>
#include <core/atl/detail/traits_common.hpp>

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

namespace anton_engine::atl {
    // Remove_Function_Qualifiers
    // Removes all function qualifiers (const, &, && and noexcept) from a function type.
    // Does not support the volatile qualifier as volatile qualified member functions have been deprecated in C++20.
    //
    template<typename T>
    struct Remove_Function_Qualifiers {
        using type = T;
    };

#define ANTON_DEFINE_TRAIT_REMOVE_FUNCTION_QUALIFIERS(CALL_QUALIFIER, CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    template<typename R, typename... Params>                                                                              \
    struct Remove_Function_Qualifiers<R CALL_QUALIFIER(Params...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER> {     \
        using type = R(Params...);                                                                                        \
    };
    ANTON_DEFINE_CALL_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_REMOVE_FUNCTION_QUALIFIERS)
#undef ANTON_DEFINE_TRAIT_REMOVE_FUNCTION_QUALIFIERS

#define ANTON_DEFINE_TRAIT_REMOVE_ELLIPSIS_FUNCTION_QUALIFIERS(CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    /* No calling convention for ellipsis */                                                                       \
    template<typename R, typename... Params>                                                                       \
    struct Remove_Function_Qualifiers<R(Params..., ...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER> {        \
        using type = R(Params..., ...);                                                                            \
    };
    ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_REMOVE_ELLIPSIS_FUNCTION_QUALIFIERS)
#undef ANTON_DEFINE_TRAIT_REMOVE_ELLIPSIS_FUNCTION_QUALIFIERS

    // Is_Function
    // Does not support the volatile qualifier as volatile qualified member functions have been deprecated in C++20
    //
    template<typename T>
    struct Is_Function: atl::False_Type {};

#define ANTON_DEFINE_TRAIT_IS_FUNCTION(CALL_QUALIFIER, CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    template<typename R, typename... Params>                                                               \
    struct Is_Function<R CALL_QUALIFIER(Params...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER>: atl::True_Type {};
    ANTON_DEFINE_CALL_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_IS_FUNCTION)
#undef ANTON_DEFINE_TRAIT_IS_FUNCTION

#define ANTON_DEFINE_TRAIT_IS_FUNCTION_ELLIPSIS(CONST_QUALIFIER, REF_QUALIFIER, NOEXCEPT_QUALIFIER) \
    template<typename R, typename... Params>                                                        \
    struct Is_Function<R(Params..., ...) CONST_QUALIFIER REF_QUALIFIER NOEXCEPT_QUALIFIER>: atl::True_Type {};
    ANTON_DEFINE_CONST_REF_NOEXCEPT_QUALIFIERS(ANTON_DEFINE_TRAIT_IS_FUNCTION_ELLIPSIS)
#undef ANTON_DEFINE_TRAIT_IS_FUNCTION_ELLIPSIS

    template<typename T>
    constexpr bool is_function = Is_Function<T>::value;

    namespace detail {
        template<typename Fn, typename... Args>
        struct Is_Invocable {
        private:
            template<typename XFn, typename... XArgs>
            static atl::type_sink<atl::True_Type, decltype(atl::declval<XFn>()(atl::declval<XArgs>()...))> test(int);

            template<typename...>
            static atl::False_Type test(...);

        public:
            static constexpr bool value = decltype(test<Fn, Args...>(0))::value;
        };
    } // namespace detail

    template<typename Fn, typename... Args>
    struct Invoke_Result: enable_if<detail::Is_Invocable<Fn, Args...>::value, Identity<decltype(declval<Fn>()(declval<Args>()...))>> {};

    template<typename Fn, typename... Args>
    using invoke_result = typename Invoke_Result<Fn, Args...>::type;

    // Is_Invocable
    //
    template<typename Fn, typename... Args>
    struct Is_Invocable: Bool_Constant<detail::Is_Invocable<Fn, Args...>::value> {};

    template<typename Fn, typename... Args>
    constexpr bool is_invocable = Is_Invocable<Fn, Args...>::value;

    // Is_Invocable_R
    //
    template<typename Return, typename Fn, typename... Args>
    struct Is_Invocable_R: Bool_Constant<conjunction<Is_Invocable<Fn, Args...>, detail::Is_Same_Type<Identity<Return>, Invoke_Result<Fn, Args...>>>> {};

    template<typename Return, typename Fn, typename... Args>
    constexpr bool is_invocable_r = Is_Invocable_R<Return, Fn, Args...>::value;
} // namespace anton_engine::atl

#endif // !CORE_ATL_DETAIL_TRAITS_FUNCTION_HPP_INCLUDE
