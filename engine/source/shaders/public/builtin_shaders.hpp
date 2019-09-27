#ifndef SHADERS_BUILTIN_SHADERS_HPP_INCLUDE
#define SHADERS_BUILTIN_SHADERS_HPP_INCLUDE

#include <shader.hpp>

namespace anton_engine {
    enum class Builtin_Shader {
        uniform_color_3d = 0,
        uniform_color_line_3d = 1,
        deferred_shading = 2,
    };

    // TODO: Should be private (most likely)
    void load_builtin_shaders();
    void unload_builtin_shaders();

    Shader& get_builtin_shader(Builtin_Shader);
} // namespace anton_engine

#endif // !SHADERS_BUILTIN_SHADERS_HPP_INCLUDE
