#include <debug_macros.hpp>

#include <glad.hpp>
#include <iostream>
#include <string>

void _GE_log(std::string str) {
    std::cout << str << "\n";
}

void _GE_conditional_log(bool cond, std::string msg) {
    if (!cond) {
        std::cout << msg << "\n";
        std::cout << msg << "\n";
    }
}

void _GE_check_gl_errors() {
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
