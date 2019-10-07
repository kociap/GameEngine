#include <shader_file.hpp>

#include <anton_stl/vector.hpp>
#include <debug_macros.hpp> // CHECK_GL_ERRORS
#include <glad.hpp>
#include <iostream>
#include <opengl.hpp>
#include <shader_exceptions.hpp>
#include <string_view>

namespace anton_engine {
    static void compile_shader(uint32_t shader, anton_stl::String_View name = "Unnamed Shader") {
        glCompileShader(shader);
        CHECK_GL_ERRORS();
        GLint compilation_status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);
        if (compilation_status == GL_FALSE) {
            GLint log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
            anton_stl::Vector<GLchar> log(log_length);
            glGetShaderInfoLog(shader, log_length, &log_length, &log[0]);
            std::string log_string(log.begin(), log.end());
            log_string = std::string("Shader compilation failed (") + name.data() + ")\n" + log_string;
            throw Shader_Compilation_Failed(std::move(log_string));
        }
        CHECK_GL_ERRORS();
    }

    static void set_shader_source(uint32_t shader, anton_stl::String_View source) {
        char const* src = source.data();
        opengl::shader_source(shader, 1, &src, nullptr);
    }

    Shader_File::Shader_File(anton_stl::String_View n, opengl::Shader_Type type, anton_stl::String_View source): type(type), shader(0) {
        shader = opengl::create_shader(type);
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
            CHECK_GL_ERRORS();
		}
    }
} // namespace anton_engine
