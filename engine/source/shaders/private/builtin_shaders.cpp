#include <builtin_shaders.hpp>

#include <anton_stl/vector.hpp>
#include <assets.hpp>
#include <shader_file.hpp>
#include <utils/enum.hpp>

namespace anton_engine {
    static anton_stl::Vector<Shader> builtin_shaders = anton_stl::Vector<Shader>(anton_stl::reserve, 3);

    // TODO: Should be private (most likely)
    void load_builtin_shaders() {
        Shader_File uniform_color_vert = assets::load_shader_file("uniform_color.vert");
        Shader_File uniform_color_frag = assets::load_shader_file("uniform_color.frag");
        builtin_shaders.push_back(create_shader(uniform_color_vert, uniform_color_frag));

        Shader_File uniform_color_line_vert = assets::load_shader_file("uniform_color_line.vert");
        builtin_shaders.push_back(create_shader(uniform_color_line_vert, uniform_color_frag));

        auto postprocess_vert = assets::load_shader_file("postprocessing/postprocess_vertex.vert");
        auto deferred_frag = assets::load_shader_file("deferred_shading.frag");
        builtin_shaders.push_back(create_shader(deferred_frag, postprocess_vert));
    }

    void unload_builtin_shaders() {
        builtin_shaders.clear();
    }

    Shader& get_builtin_shader(Builtin_Shader shader) {
        return builtin_shaders[utils::enum_to_value(shader)];
    }
} // namespace anton_engine
