#include <shaders/shader.hpp>

#include <rendering/glad.hpp>

#include <core/atl/utility.hpp>
#include <core/atl/vector.hpp>
#include <core/color.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/vector3.hpp>
#include <rendering/opengl.hpp>
#include <shaders/shader_exceptions.hpp>
#include <shaders/shader_file.hpp>

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

    Shader::Shader(Shader&& other) noexcept: uniform_cache(atl::move(other.uniform_cache)) {
        atl::swap(other.program, program);
    }

    Shader& Shader::operator=(Shader&& shader) noexcept {
        atl::swap(shader.program, program);
        uniform_cache = atl::move(shader.uniform_cache);
        return *this;
    }

    Shader::~Shader() {
        if (program != 0) {
            glDeleteProgram(program);
        }
    }

    void Shader::attach(Shader_File const& shader) {
        glAttachShader(program, shader.shader);
    }

    static void build_shader_uniform_cache(uint32_t program, std::unordered_map<std::string, int32_t>& uniform_cache) {
        int32_t active_uniforms;
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniforms);
        int32_t uniform_max_name_length;
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_MAX_NAME_LENGTH, &uniform_max_name_length);
        atl::Vector<char> name(uniform_max_name_length);
        for (int32_t uniform_index = 0; uniform_index < active_uniforms; ++uniform_index) {
            int32_t name_length;
            glGetActiveUniformName(program, static_cast<uint32_t>(uniform_index), uniform_max_name_length, &name_length, &name[0]);
            int32_t location = glGetUniformLocation(program, &name[0]);
            atl::String_View name_str = atl::String_View(&name[0], name_length);
            uniform_cache.emplace(std::string(name_str.data()), location);
        }
    }

    void Shader::link() {
        glLinkProgram(program);
        GLint link_status;
        glGetProgramiv(program, GL_LINK_STATUS, &link_status);
        if (link_status == GL_FALSE) {
            GLint log_length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
            atl::Vector<GLchar> log{log_length};
            glGetProgramInfoLog(program, log_length, &log_length, &log[0]);
            throw Program_Linking_Failed(atl::String_View{log.data(), log.size()});
        }

        build_shader_uniform_cache(program, uniform_cache);
    }

    void Shader::use() {
        glUseProgram(program);
    }

    void Shader::detach(Shader_File const& shader) {
        glDetachShader(program, shader.shader);
    }

    void Shader::set_int(atl::String_View const name, i32 const a) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform1i(iter->second, a);
        }
    }

    void Shader::set_uint(atl::String_View const name, u32 const a) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform1ui(iter->second, a);
        }
    }

    void Shader::set_float(atl::String_View const name, float const a) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform1f(iter->second, a);
        }
    }

    void Shader::set_vec2(atl::String_View const name, Vector2 const vec) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform2fv(iter->second, 1, &vec.x);
        }
    }

    void Shader::set_vec3(atl::String_View const name, Vector3 const vec) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform3fv(iter->second, 1, &vec.x);
        }
    }

    void Shader::set_vec3(atl::String_View const name, Color const c) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform3fv(iter->second, 1, &c.r);
        }
    }

    void Shader::set_vec4(atl::String_View const name, Color const c) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniform4fv(iter->second, 1, &c.r);
        }
    }

    void Shader::set_matrix4(atl::String_View const name, Matrix4 const& mat) {
        std::string n(name.data());
        auto iter = uniform_cache.find(n);
        if (iter != uniform_cache.end()) {
            glUniformMatrix4fv(iter->second, 1, GL_FALSE, mat.get_raw());
        }
    }

    void swap(Shader& s1, Shader& s2) {
        atl::swap(s1.program, s2.program);
        atl::swap(s1.uniform_cache, s2.uniform_cache);
    }

    void delete_shader(Shader& shader) {
        if (shader.program != 0) {
            glDeleteProgram(shader.program);
            shader.program = 0;
        }
    }
} // namespace anton_engine
