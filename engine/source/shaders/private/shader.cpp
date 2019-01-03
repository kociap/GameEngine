#include "shader.hpp"

#include <string>
#include <vector>

Shader::Shader() {
    program = glCreateProgram();
    if (program == 0) {
        throw Program_not_created("");
    }
}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::attach(Shader_file const& shader) {
    glAttachShader(program, shader.shader);
}

void Shader::link() {
    glLinkProgram(program);
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> log(log_length);
        glGetProgramInfoLog(program, log_length, &log_length, &log[0]);
        std::string log_string(log.begin(), log.end());
        throw Program_linking_failed(std::move(log_string));
    }
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::detach(Shader_file const& shader) {
    glDetachShader(program, shader.shader);
}

GLint Shader::get_uniform(std::string const& name) {
    return glGetUniformLocation(program, name.c_str());
}

GLint Shader::get_uniform(char const* name) {
    return glGetUniformLocation(program, name);
}

void Shader::set_int(std::string const& name, int a) {
    glUniform1i(get_uniform(name), a);
}

void Shader::set_float(std::string const& name, float a) {
    glUniform1f(get_uniform(name), a);
}

void Shader::set_vec3(std::string const& name, Vector3 const& vec) {
    glUniform3fv(get_uniform(name), 1, &vec.x);
}

void Shader::set_vec3(std::string const& name, Color const& c) {
    glUniform3fv(get_uniform(name), 1,&c.r);
}

void Shader::set_matrix4(std::string const& name, Matrix4 const& mat) {
    glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, mat.get_raw());
}