#ifndef CORE_DEBUG_MACROS_HPP_INCLUDE
#define CORE_DEBUG_MACROS_HPP_INCLUDE

#define GE_UNUSED(x) ((void)(x))

#if defined(__clang__) || defined(__GNUC__)
#    define ANTON_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#    define ANTON_UNREACHABLE() __assume(0)
#else
#    define ANTON_UNREACHABLE()
#endif

#ifndef NDEBUG

namespace anton_engine {
    void _GE_check_gl_errors();
} // namespace anton_engine

#    define CHECK_GL_ERRORS() _GE_check_gl_errors()
#else
#    define CHECK_GL_ERRORS() ((void)0)
#endif // !NDEBUG

#endif // !CORE_DEBUG_MACROS_HPP_INCLUDE
