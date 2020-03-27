#include <shaders/shader_file.hpp>

#include <core/atl/vector.hpp>
#include <rendering/glad.hpp>
#include <iostream>
#include <rendering/opengl.hpp>
#include <shaders/shader_exceptions.hpp>
#include <string_view>

namespace anton_engine {
    static void compile_shader(uint32_t shader, atl::String_View name = "Unnamed Shader") {
        glCompileShader(shader);
        GLint compilation_status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);
        if (compilation_status == GL_FALSE) {
            GLint log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
            atl::String log{atl::reserve, log_length + name.size_bytes() + 29};
            log.append(u8"Shader compilation failed (");
            log.append(name);
            log.append(")\n");
            glGetShaderInfoLog(shader, log_length, &log_length, log.data() + log.size_bytes());
            throw Shader_Compilation_Failed(log);
        }
    }

    static void set_shader_source(uint32_t shader, atl::String_View source) {
        char const* src = source.data();
        glShaderSource(shader, 1, &src, nullptr);
    }

    Shader_File::Shader_File(atl::String_View n, opengl::Shader_Type type, atl::String_View source): type(type), shader(0) {
        shader = glCreateShader(utils::enum_to_value(type));
        if (shader == 0) {
            throw Shader_Not_Created("");
        }
        set_shader_source(shader, source);
        compile_shader(shader, n);
    }

    Shader_File::Shader_File(Shader_File&& shader) noexcept: type(shader.type), shader(shader.shader) {
        shader.shader = 0;
    }

    Shader_File& Shader_File::operator=(Shader_File&& s) noexcept {
        std::swap(shader, s.shader);
        return *this;
    }

    Shader_File::~Shader_File() {
        if (shader != 0) {
            glDeleteShader(shader);
        }
    }
} // namespace anton_engine
