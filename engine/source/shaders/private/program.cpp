#include "program.hpp"

#include <string>
#include <vector>

Program::Program() {
    program = glCreateProgram();
    if (program == 0) {
        throw Program_not_created("");
    }
}

void Program::attach(Shader const& shader) {
    glAttachShader(program, shader.shader);
}

void Program::link() {
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

void Program::use() {
    glUseProgram(program);
}

void Program::detach(Shader const& shader) {
    glDetachShader(program, shader.shader);
}

GLint Program::get_uniform(std::string const& name) {
    return glGetUniformLocation(program, name.c_str());
}

GLint Program::get_uniform(char const* name) {
    return glGetUniformLocation(program, name);
}