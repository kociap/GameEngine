#include <shader.hpp>

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS()
#include <glad.hpp>
ANTON_RESTORE_WARNINGS()

#include <anton_stl/vector.hpp>
#include <color.hpp>
#include <debug_macros.hpp> // CHECK_GL_ERRORS
#include <math/matrix4.hpp>
#include <math/vector3.hpp>
#include <opengl.hpp>
#include <shader_exceptions.hpp>
#include <shader_file.hpp>

#include <fstream>
#include <string>

namespace anton_engine {
    Shader::Shader(bool create) {
        if (create) {
            program = glCreateProgram();
            if (program == 0) {
                throw Program_Not_Created("");
            }
        }
    }

    Shader::Shader(Shader&& shader) noexcept {
        std::swap(shader.program, program);
        std::swap(shader.uniform_cache, uniform_cache);
    }

    Shader& Shader::operator=(Shader&& shader) noexcept {
        std::swap(shader.program, program);
        uniform_cache = std::move(shader.uniform_cache);
        return *this;
    }

    Shader::~Shader() {
        opengl::delete_program(program);
    }

    void Shader::attach(Shader_File const& shader) {
        glAttachShader(program, shader.shader);
        CHECK_GL_ERRORS();
    }

    static void build_shader_uniform_cache(uint32_t program, std::unordered_map<std::string, int32_t>& uniform_cache) {
        int32_t active_uniforms;
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniforms);
        int32_t uniform_max_name_length;
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_MAX_NAME_LENGTH, &uniform_max_name_length);
        anton_stl::Vector<char> name(uniform_max_name_length);
        for (int32_t uniform_index = 0; uniform_index < active_uniforms; ++uniform_index) {
            int32_t name_length;
            glGetActiveUniformName(program, static_cast<uint32_t>(uniform_index), uniform_max_name_length, &name_length, &name[0]);
            int32_t location = glGetUniformLocation(program, &name[0]);
            std::string name_str = std::string(&name[0], name_length);
            uniform_cache.emplace(std::move(name_str), location);
        }
    }

    void Shader::link() {
        glLinkProgram(program);
        CHECK_GL_ERRORS();
        GLint link_status;
        glGetProgramiv(program, GL_LINK_STATUS, &link_status);
        if (link_status == GL_FALSE) {
            GLint log_length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
            anton_stl::Vector<GLchar> log(log_length);
            glGetProgramInfoLog(program, log_length, &log_length, &log[0]);
            std::string log_string(log.begin(), log.end());
            throw Program_Linking_Failed(std::move(log_string));
        }
        CHECK_GL_ERRORS();

        build_shader_uniform_cache(program, uniform_cache);
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
        auto iter = uniform_cache.find(name);
        if (iter != uniform_cache.end()) {
            glUniform1i(iter->second, a);
            CHECK_GL_ERRORS();
        }
    }

    void Shader::set_int(char const* name, int a) {
        set_int(std::string(name), a);
    }

    void Shader::set_float(std::string const& name, float a) {
        auto iter = uniform_cache.find(name);
        if (iter != uniform_cache.end()) {
            glUniform1f(iter->second, a);
            CHECK_GL_ERRORS();
        }
    }

    void Shader::set_float(char const* name, float a) {
        set_float(std::string(name), a);
    }

    void Shader::set_vec3(std::string const& name, Vector3 vec) {
        auto iter = uniform_cache.find(name);
        if (iter != uniform_cache.end()) {
            glUniform3fv(iter->second, 1, &vec.x);
            CHECK_GL_ERRORS();
        }
    }

    void Shader::set_vec3(char const* name, Vector3 vec) {
        set_vec3(std::string(name), vec);
    }

    void Shader::set_vec3(std::string const& name, Color c) {
        auto iter = uniform_cache.find(name);
        if (iter != uniform_cache.end()) {
            glUniform3fv(iter->second, 1, &c.r);
            CHECK_GL_ERRORS();
        }
    }

    void Shader::set_vec3(char const* name, Color c) {
        set_vec3(std::string(name), c);
    }

    void Shader::set_vec4(std::string const& name, Color c) {
        auto iter = uniform_cache.find(name);
        if (iter != uniform_cache.end()) {
            glUniform4fv(iter->second, 1, &c.r);
            CHECK_GL_ERRORS();
        }
    }

    void Shader::set_vec4(char const* name, Color c) {
        set_vec4(std::string(name), c);
    }

    void Shader::set_matrix4(std::string const& name, Matrix4 const& mat) {
        auto iter = uniform_cache.find(name);
        if (iter != uniform_cache.end()) {
            glUniformMatrix4fv(iter->second, 1, GL_FALSE, mat.get_raw());
            CHECK_GL_ERRORS();
        }
    }

    void Shader::set_matrix4(char const* name, Matrix4 const& mat) {
        set_matrix4(std::string(name), mat);
    }

    void swap(Shader& s1, Shader& s2) {
        std::swap(s1.program, s2.program);
    }

    void delete_shader(Shader& shader) {
        opengl::delete_program(shader.program);
        shader.program = 0;
    }
} // namespace anton_engine
