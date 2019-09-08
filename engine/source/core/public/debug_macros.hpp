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

#    include <stdexcept>

void _GE_log(std::string);
void _GE_conditional_log(bool condition, std::string);
void _GE_check_gl_errors();

#    define GE_log(msg) _GE_log((msg))

// If condition is false, it will log the message
#    define GE_conditional_log(cond, msg) _GE_conditional_log((cond), msg);

#    define CHECK_GL_ERRORS() _GE_check_gl_errors()

#else
#    define GE_assert(condition, msg) ((void)0)
#    define CHECK_GL_ERRORS() ((void)0)
#    define GE_log ((void)0)
#    define GE_conditional_log ((void)0)
#endif // !NDEBUG

#endif // !CORE_DEBUG_MACROS_HPP_INCLUDE
