#ifndef SHADERS_SHADER_FILE_HPP_INCLUDE
#define SHADERS_SHADER_FILE_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/string_view.hpp>
#include <rendering/opengl.hpp>

namespace anton_engine {
    class Shader;

    class Shader_File {
    public:
        Shader_File() = delete;
        Shader_File(Shader_File const&) = delete;
        Shader_File& operator=(Shader_File const&) = delete;

        Shader_File(atl::String_View name, opengl::Shader_Type type, atl::String_View source);
        Shader_File(Shader_File&& shader) noexcept;
        Shader_File& operator=(Shader_File&& shader) noexcept;
        ~Shader_File();

    private:
        friend class Shader;

        opengl::Shader_Type type;
        u32 shader;
    };
} // namespace anton_engine

#endif // !SHADERS_SHADER_FILE_HPP_INCLUDE
