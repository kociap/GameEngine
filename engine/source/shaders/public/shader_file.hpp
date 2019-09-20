#ifndef SHADERS_SHADER_FILE_HPP_INCLUDE
#define SHADERS_SHADER_FILE_HPP_INCLUDE

#include <opengl.hpp>

#include <anton_stl/string_view.hpp>
#include <cstdint>

namespace anton_engine {
    class Shader;

    class Shader_File {
    public:
        Shader_File() = delete;
        Shader_File(Shader_File const&) = delete;
        Shader_File& operator=(Shader_File const&) = delete;

        Shader_File(anton_stl::String_View name, opengl::Shader_Type type, anton_stl::String_View source);
        Shader_File(Shader_File&& shader) noexcept;
        Shader_File& operator=(Shader_File&& shader) noexcept;
        ~Shader_File();

    private:
        friend class Shader;

        opengl::Shader_Type type;
        uint32_t shader;
    };
} // namespace anton_engine

#endif // !SHADERS_SHADER_FILE_HPP_INCLUDE
