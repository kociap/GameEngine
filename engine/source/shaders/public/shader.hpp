#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

#include "color.hpp"
#include "glad/glad.h"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "shaderfile.hpp"
#include <filesystem>
#include <string>

class Shader {
public:
    Shader(bool create_program = true);
    Shader(Shader&&);
    Shader& operator=(Shader&&);
    ~Shader();

    void load_shader_file(std::filesystem::path);

    void create();
    void attach(Shader_file const& shader);
    void link();
    void use();
    void detach(Shader_file const&);

    GLint get_uniform(std::string const&);
    GLint get_uniform(char const*);

    void set_int(std::string const&, int);
    void set_float(std::string const&, float);
    void set_vec3(std::string const&, Vector3 const&);
    void set_vec3(std::string const&, Color const&);
    void set_matrix4(std::string const&, Matrix4 const&);

private:
    GLuint program = 0;
};

#endif // !SHADERS_SHADER_HPP_INCLUDE