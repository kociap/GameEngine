#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

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

class Program;

class Shader {
public:
    Shader() = delete;
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;

    Shader(Shader_type type, std::string const& source);
    Shader(Shader&& shader) noexcept;
    Shader& operator=(Shader&& shader) noexcept;
    ~Shader();

private:
    friend class Program;

    Shader_type type;
    GLuint shader;

    void create();
    void set_source(std::string const& source);
    void compile();
};

//using Vertex_shader = Shader<Shader_type::vertex>;
//using Fragment_shader = Shader<Shader_type::fragment>;
//using Geometry_shader = Shader<Shader_type::geometry>;
//using Compute_shader = Shader<Shader_type::compute>;
//using Tessellation_control_shader = Shader<Shader_type::tessellation_control>;
//using Tessellation_eveluation_shader = Shader<Shader_type::tessellation_evaluation>;

#endif // !SHADERS_SHADER_HPP_INCLUDE