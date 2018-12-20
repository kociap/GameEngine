#ifndef SHADERS_SHADER_FILE_HPP_INCLUDE
#define SHADERS_SHADER_FILE_HPP_INCLUDE

#include "exceptions.hpp"
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

class Shader;

class Shader_file {
public:
    Shader_file() = delete;
    Shader_file(Shader_file const&) = delete;
    Shader_file& operator=(Shader_file const&) = delete;

    Shader_file(Shader_type type, std::string const& source);
    Shader_file(Shader_file&& shader) noexcept;
    Shader_file& operator=(Shader_file&& shader) noexcept;
    ~Shader_file();

private:
    friend class Shader;

    Shader_type type;
    GLuint shader;

    void create();
    void set_source(std::string const& source);
    void compile();
};

#endif // !SHADERS_SHADER_FILE_HPP_INCLUDE