#include "shaderfile.hpp"

#include "containers/vector.hpp"
#include "debug_macros.hpp"
#include "shader_exceptions.hpp"
#include <iostream>

static uint32_t create_shader(Shader_Type type) {
    uint32_t shader = 0;
    if (type == Shader_Type::vertex) {
        shader = glCreateShader(GL_VERTEX_SHADER);
    } else if (type == Shader_Type::fragment) {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    } else if (type == Shader_Type::geometry) {
        shader = glCreateShader(GL_GEOMETRY_SHADER);
    } else if (type == Shader_Type::compute) {
        shader = glCreateShader(GL_COMPUTE_SHADER);
    } else if (type == Shader_Type::tessellation_control) {
        shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    } else {
        shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    }

    CHECK_GL_ERRORS();
    if (shader == 0) {
        throw Shader_Not_Created("");
    }

    return shader;
}

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

Shader_File::Shader_File(std::string n, Shader_Type type, std::string const& source): name(n), type(type), shader(0) {
    shader = create_shader(type);
    set_source(source);
    compile_shader(shader, name);
}

Shader_File::Shader_File(Shader_File&& shader) noexcept: shader(shader.shader), type(shader.type) {
    shader.shader = 0;
}

Shader_File& Shader_File::operator=(Shader_File&& shader) noexcept {
    this->shader = shader.shader;
    shader.shader = 0;
    return *this;
}

Shader_File::~Shader_File() {
    if (shader != 0) {
        glDeleteShader(shader);
    }
    CHECK_GL_ERRORS();
}

void Shader_File::set_source(std::string const& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    CHECK_GL_ERRORS();
}