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
        throw Program_linking_failed(std::string(log.begin(), log.end()));
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

void Shader::setMatrix4(std::string const& name, Matrix4 const& mat) {
    glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, mat.get_raw());
}