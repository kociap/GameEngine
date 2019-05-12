#include "shader.hpp"

#include "color.hpp"
#include "debug_macros.hpp"
#include "glad/glad.h"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "opengl.hpp"
#include "shader_exceptions.hpp"
#include "shader_file.hpp"

#include <cassert>
#include <fstream>
#include <string>
#include <vector>

Shader::Shader() {
    program = glCreateProgram();
    if (program == 0) {
        throw Program_Not_Created("");
    }
}

Shader::Shader(Shader&& shader) noexcept: Object(std::move(shader)) {
    std::swap(shader.program, program);
}

Shader& Shader::operator=(Shader&& shader) noexcept {
    Object::operator=(std::move(shader));
    std::swap(shader.program, program);
    return *this;
}

Shader::~Shader() {
    opengl::delete_program(program);
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

void Shader::set_int(std::string const& name, int a) {
    uint32_t location = opengl::get_uniform_location(program, name.c_str());
    glUniform1i(location, a);
    CHECK_GL_ERRORS();
}

void Shader::set_int(char const* name, int a) {
    uint32_t location = opengl::get_uniform_location(program, name);
    glUniform1i(location, a);
    CHECK_GL_ERRORS();
}

void Shader::set_float(std::string const& name, float a) {
    uint32_t location = opengl::get_uniform_location(program, name.c_str());
    glUniform1f(location, a);
    CHECK_GL_ERRORS();
}

void Shader::set_float(char const* name, float a) {
    uint32_t location = opengl::get_uniform_location(program, name);
    glUniform1f(location, a);
    CHECK_GL_ERRORS();
}

void Shader::set_vec3(std::string const& name, Vector3 const& vec) {
    uint32_t location = opengl::get_uniform_location(program, name.c_str());
    glUniform3fv(location, 1, &vec.x);
    CHECK_GL_ERRORS();
}

void Shader::set_vec3(char const* name, Vector3 const& vec) {
    uint32_t location = opengl::get_uniform_location(program, name);
    glUniform3fv(location, 1, &vec.x);
    CHECK_GL_ERRORS();
}

void Shader::set_vec3(std::string const& name, Color const& c) {
    uint32_t location = opengl::get_uniform_location(program, name.c_str());
    glUniform3fv(location, 1, &c.r);
    CHECK_GL_ERRORS();
}

void Shader::set_vec3(char const* name, Color const& c) {
    uint32_t location = opengl::get_uniform_location(program, name);
    glUniform3fv(location, 1, &c.r);
    CHECK_GL_ERRORS();
}

void Shader::set_matrix4(std::string const& name, Matrix4 const& mat) {
    uint32_t location = opengl::get_uniform_location(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, mat.get_raw());
    CHECK_GL_ERRORS();
}

void Shader::set_matrix4(char const* name, Matrix4 const& mat) {
    uint32_t location = opengl::get_uniform_location(program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, mat.get_raw());
    CHECK_GL_ERRORS();
}

void swap(Shader& s1, Shader& s2) {
    std::swap(s1.program, s2.program);
}