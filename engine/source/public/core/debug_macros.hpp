#ifndef CORE_DEBUG_MACROS_HPP_INCLUDE
#define CORE_DEBUG_MACROS_HPP_INCLUDE

#ifndef NDEBUG
namespace anton_engine {
    void _GE_check_gl_errors();
} // namespace anton_engine

#    define CHECK_GL_ERRORS() _GE_check_gl_errors()
#else
#    define CHECK_GL_ERRORS() ((void)0)
#endif // !NDEBUG

#endif // !CORE_DEBUG_MACROS_HPP_INCLUDE
