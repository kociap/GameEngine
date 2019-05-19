#include "shader_file.hpp"

#include "containers/vector.hpp"
#include "debug_macros.hpp"
#include "opengl.hpp"
#include "shader_exceptions.hpp"
#include <iostream>

static void compile_shader(uint32_t shader, std::string const& name = "Unnamed Shader") {
    glCompileShader(shader);
    CHECK_GL_ERRORS();
    GLint compilation_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);
    if (compilation_status == GL_FALSE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        containers::Vector<GLchar> log(log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, &log[0]);
        std::string log_string(log.begin(), log.end());
        log_string = "Shader compilation failed (" + name + ")\n" + log_string;
        throw Shader_Compilation_Failed(std::move(log_string));
    }
    CHECK_GL_ERRORS();
}

static void set_shader_source(uint32_t shader, std::string const& source) {
    char const* src = source.c_str();
    opengl::shader_source(shader, 1, &src, nullptr);
}

static void set_shader_source(uint32_t shader, char const* source) {
    opengl::shader_source(shader, 1, &source, nullptr);
}

Shader_File::Shader_File(std::string n, opengl::Shader_Type type, std::string const& source): type(type), shader(0) {
    shader = opengl::create_shader(type);
    if (shader == 0) {
        throw Shader_Not_Created("");
    }
    set_shader_source(shader, source);
    compile_shader(shader, n);
}

Shader_File::Shader_File(Shader_File&& shader) noexcept: shader(shader.shader), type(shader.type) {
    shader.shader = 0;
}

Shader_File& Shader_File::operator=(Shader_File&& s) noexcept {
    std::swap(shader, s.shader);
    return *this;
}

Shader_File::~Shader_File() {
    opengl::delete_shader(shader);
}