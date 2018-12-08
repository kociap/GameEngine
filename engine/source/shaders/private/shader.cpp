#include "shader.hpp"
#include <iostream>

Shader::Shader(Shader_type type, std::string const& source) : type(type), shader(0) {
    create();
    set_source(source);
    compile();
}

Shader::Shader(Shader&& shader) noexcept : shader(shader.shader), type(shader.type) {
    shader.shader = 0;
}

Shader& Shader::operator=(Shader&& shader) noexcept {
    this->shader = shader.shader;
    shader.shader = 0;
    return *this;
}

Shader::~Shader() {
    if (shader != 0) {
        glDeleteShader(shader);
    }
}

void Shader::create() {
    if (type == Shader_type::vertex) {
        shader = glCreateShader(GL_VERTEX_SHADER);
    } else if (type == Shader_type::fragment) {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    } else if (type == Shader_type::geometry) {
        shader = glCreateShader(GL_GEOMETRY_SHADER);
    } else if (type == Shader_type::compute) {
        shader = glCreateShader(GL_COMPUTE_SHADER);
    } else if (type == Shader_type::tessellation_control) {
        shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    } else {
        shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    }

    if (shader == 0) {
        throw Shader_not_created("");
    }
}

void Shader::set_source(std::string const& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
}

void Shader::compile() {
    glCompileShader(shader);

    GLint compilation_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);
    if (compilation_status == GL_FALSE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> log(log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, &log[0]);
        throw Shader_compilation_failed(std::string(log.begin(), log.end()));
    }
}