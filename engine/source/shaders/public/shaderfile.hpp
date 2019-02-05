#ifndef SHADERS_SHADER_FILE_HPP_INCLUDE
#define SHADERS_SHADER_FILE_HPP_INCLUDE

#include "glad/glad.h"
#include <string>
#include <vector>

enum class Shader_type {
    vertex,
    fragment,
    geometry,
    tessellation_evaluation,
    tessellation_control,
    compute,
};

class Shader_File {
public:
    Shader_File() = delete;
    Shader_File(Shader_File const&) = delete;
    Shader_File& operator=(Shader_File const&) = delete;

    Shader_File(Shader_type type, std::string const& source);
    Shader_File(Shader_File&& shader) noexcept;
    Shader_File& operator=(Shader_File&& shader) noexcept;
    ~Shader_File();

private:
    friend class Shader;

    Shader_type type;
    GLuint shader;

    void create();
    void set_source(std::string const& source);
    void compile();
};

#endif // !SHADERS_SHADER_FILE_HPP_INCLUDE