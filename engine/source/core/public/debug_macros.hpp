#ifndef CORE_DEBUG_MACROS_HPP_INCLUDE
#define CORE_DEBUG_MACROS_HPP_INCLUDE

#include "glad/glad.h"
#include <cassert>
#include <stdexcept>

#ifndef NDEBUG
#define GE_assert(condition, msg) assert((msg, condition))

void _GE_log(std::string);
void _GE_conditional_log(bool condition, std::string);

#define GE_log(msg) _GE_log((msg))

// If condition is false it will log message
#define GE_conditional_log(cond, msg) _GE_conditional_log((cond), msg);

inline void _GE_check_gl_errors() {
    GLenum error = glGetError();
    if (error == GL_INVALID_ENUM) {
        throw std::runtime_error("GL_INVALID_ENUM");
    } else if (error == GL_INVALID_VALUE) {
        throw std::runtime_error("GL_INVALID_VALUE");
    } else if (error == GL_INVALID_OPERATION) {
        throw std::runtime_error("GL_INVALID_OPERATION");
    } else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
        throw std::runtime_error("GL_INVALID_FRAMEBUFFER_OPERATION");
    } else if (error == GL_OUT_OF_MEMORY) {
        throw std::runtime_error("GL_OUT_OF_MEMORY");
    } else if (error == GL_STACK_UNDERFLOW) {
        throw std::runtime_error("GL_STACK_UNDERFLOW");
    } else if (error == GL_STACK_OVERFLOW) {
        throw std::runtime_error("GL_STACK_OVERFLOW");
    }
}
#define CHECK_GL_ERRORS() _GE_check_gl_errors()
#else
#define GE_assert(condition, msg) ((void)0)
#define CHECK_GL_ERRORS() ((void)0)
#define GE_log ((void)0)
#define GE_conditional_log ((void)0)
#endif // _DEBUG

#endif // !CORE_DEBUG_MACROS_HPP_INCLUDE