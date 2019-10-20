#include <builtin_shaders.hpp>

#include <anton_stl/vector.hpp>
#include <assets.hpp>
#include <build_config.hpp>
#include <shader_file.hpp>
#include <utils/enum.hpp>

#if ANTON_WITH_EDITOR
#    include <builtin_editor_shaders.hpp>
#endif // ANTON_WITH_EDITOR

namespace anton_engine {
    static anton_stl::Vector<Shader> builtin_shaders = anton_stl::Vector<Shader>(anton_stl::reserve, 5);
#if ANTON_WITH_EDITOR
    static anton_stl::Vector<Shader> builtin_editor_shaders = anton_stl::Vector<Shader>(anton_stl::reserve, 1);
#endif // ANTON_WITH_EDITOR

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

        auto gamma_correction = assets::load_shader_file("postprocessing/gamma_correction.frag");
        builtin_shaders.push_back(create_shader(postprocess_vert, gamma_correction));

        auto quad_vert = assets::load_shader_file("quad.vert");
        auto quad_frag = assets::load_shader_file("quad.frag");
        builtin_shaders.push_back(create_shader(quad_vert, quad_frag));

#if ANTON_WITH_EDITOR
        Shader_File outline_mix_file = assets::load_shader_file("editor/outline_mix.frag");
        builtin_editor_shaders.push_back(create_shader(outline_mix_file, postprocess_vert));
#endif // ANTON_WITH_EDITOR
    }

    void unload_builtin_shaders() {
        builtin_shaders.clear();
#if ANTON_WITH_EDITOR
        builtin_editor_shaders.clear();
#endif // ANTON_WITH_EDITOR
    }

    Shader& get_builtin_shader(Builtin_Shader const shader) {
        return builtin_shaders[utils::enum_to_value(shader)];
    }

#if ANTON_WITH_EDITOR
    Shader& get_builtin_shader(Builtin_Editor_Shader const shader) {
        return builtin_editor_shaders[utils::enum_to_value(shader)];
    }
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine
