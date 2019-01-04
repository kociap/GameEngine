#include "shader.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <cassert>

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

static void read_file(std::filesystem::path const& filename, std::string& out) {
    std::ifstream file(filename);
    if (file) {
        file.seekg(0, std::ios::end);
        out.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&out[0], out.size());
        file.close();
    } else {
        throw std::invalid_argument("Could not open file " + filename.string());
    }
}

static Shader_type shader_type_from_filename(std::filesystem::path const& filename) {
    std::string extension(filename.extension().string());
    if (extension == ".vert") {
        return Shader_type::vertex;
    } else if (extension == ".frag") {
        return Shader_type::fragment;
    } else if (extension == ".geom") {
        return Shader_type::geometry;
    } else if (extension == ".comp") {
        return Shader_type::compute;
    } else if (extension == ".tese") {
        return Shader_type::tessellation_evaluation;
    } else if (extension == ".tesc") {
        return Shader_type::tessellation_control;
    } else {
        throw std::invalid_argument("\"" + extension + "\" is not a known shader file extension");
    }
}

void Shader::load_shader_file(std::filesystem::path path) {
    std::string shader_source;
    read_file(path, shader_source);
    Shader_file s(shader_type_from_filename(path), shader_source);
    attach(s);
};

GLint Shader::get_uniform(std::string const& name) {
    GLint location = glGetUniformLocation(program, name.c_str());
	// -1 may also mean a uniform removed by optimizations
	// Exception to catch typos
    if (location == -1) {
        throw std::runtime_error("Uniform location is -1");
	}
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