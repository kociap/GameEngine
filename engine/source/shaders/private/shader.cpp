#include "shader.hpp"

#include "color.hpp"
#include "debug_macros.hpp"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "shader_exceptions.hpp"
#include "shaderfile.hpp"

#include <cassert>
#include <fstream>
#include <string>
#include <vector>

void Shader::swap_programs(Shader& a, Shader& b) {
    std::swap(a.program, b.program);
}

Shader::Shader(bool create_program /* = true */) {
    if (create_program) {
        create();
    }
}

Shader::Shader(Shader&& shader) noexcept : Object(std::move(shader)) {
    std::swap(shader.program, program);
}

Shader& Shader::operator=(Shader&& shader) noexcept {
    Object::operator=(std::move(shader));
    std::swap(shader.program, program);
    return *this;
}

Shader::~Shader() {
    delete_shader();
}

void Shader::create() {
    program = glCreateProgram();
    if (program == 0) {
        throw Program_Not_Created("");
    }
}

void Shader::attach(Shader_File const& shader) {
    glAttachShader(program, shader.shader);
    CHECK_GL_ERRORS();
}

void Shader::link() {
    glLinkProgram(program);
    CHECK_GL_ERRORS();
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> log(log_length);
        glGetProgramInfoLog(program, log_length, &log_length, &log[0]);
        std::string log_string(log.begin(), log.end());
        throw Program_Linking_Failed(std::move(log_string));
    }
    CHECK_GL_ERRORS();
}

void Shader::use() {
    glUseProgram(program);
    CHECK_GL_ERRORS();
}

void Shader::detach(Shader_File const& shader) {
    glDetachShader(program, shader.shader);
    CHECK_GL_ERRORS();
}

void Shader::delete_shader() {
    GE_conditional_log(program != 0, "Attempting to delete not exising program");

    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
    CHECK_GL_ERRORS();
}

GLint Shader::get_uniform(std::string const& name) {
    GLint location = glGetUniformLocation(program, name.c_str());
    // -1 may also mean a uniform removed by optimizations
    // Exception to catch typos
    /*if (location == -1) {
        throw std::runtime_error("Uniform location is -1");
    }*/
    return location;
}

GLint Shader::get_uniform(char const* name) {
    GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        throw std::runtime_error("Uniform location is -1");
    }
    return location;
}

void Shader::set_int(std::string const& name, int a) {
    glUniform1i(get_uniform(name), a);
    CHECK_GL_ERRORS();
}

void Shader::set_float(std::string const& name, float a) {
    glUniform1f(get_uniform(name), a);
    CHECK_GL_ERRORS();
}

void Shader::set_vec3(std::string const& name, Vector3 const& vec) {
    glUniform3fv(get_uniform(name), 1, &vec.x);
    CHECK_GL_ERRORS();
}

void Shader::set_vec3(std::string const& name, Color const& c) {
    glUniform3fv(get_uniform(name), 1, &c.r);
    CHECK_GL_ERRORS();
}

void Shader::set_matrix4(std::string const& name, Matrix4 const& mat) {
    glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, mat.get_raw());
    CHECK_GL_ERRORS();
}