#ifndef CORE_ANTON_STL_FUNCTION_TRAITS_MACROS_HPP_INCLUDE
#define CORE_ANTON_STL_FUNCTION_TRAITS_MACROS_HPP_INCLUDE

// TODO: calling conventions support
#define ANTON_DEFINE_CALL_QUALIFIER(CLASS, CONST_OPT, REF_OPT, NOEXCEPT_OPT) CLASS(/* CALL */, CONST_OPT, REF_OPT, NOEXCEPT_OPT)

#define ANTON_DEFINE_CALL_CONST_REF_QUALIFIERS(CLASS, NOEXCEPT_OPT) \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, , , NOEXCEPT_OPT)                      \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, , &, NOEXCEPT_OPT)                     \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, , &&, NOEXCEPT_OPT)                    \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, const, , NOEXCEPT_OPT)                 \
    ANTON_DEFINE_CALL_QUALIFIER(CLASS, const, &, NOEXCEPT_OPT)                \
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

#endif // !CORE_ANTON_STL_FUNCTION_TRAITS_MACROS_HPP_INCLUDE
