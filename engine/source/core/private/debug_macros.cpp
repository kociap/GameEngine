#include <debug_macros.hpp>

#include <glad.hpp>
#include <intrinsics.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

namespace anton_engine {
    void _GE_check_gl_errors() {
        GLenum error = glGetError();
        switch (error) {
            case GL_INVALID_ENUM:
                throw std::runtime_error("GL_INVALID_ENUM");
            case GL_INVALID_VALUE:
                throw std::runtime_error("GL_INVALID_VALUE");
            case GL_INVALID_OPERATION:
                throw std::runtime_error("GL_INVALID_OPERATION");
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                throw std::runtime_error("GL_INVALID_FRAMEBUFFER_OPERATION");
            case GL_OUT_OF_MEMORY:
                throw std::runtime_error("GL_OUT_OF_MEMORY");
            case GL_STACK_UNDERFLOW:
                throw std::runtime_error("GL_STACK_UNDERFLOW");
            case GL_STACK_OVERFLOW:
                throw std::runtime_error("GL_STACK_OVERFLOW");
        }
    }
} // namespace anton_engine
