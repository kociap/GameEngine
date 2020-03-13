#include <shaders/builtin_shaders.hpp>

#include <core/atl/string.hpp>
#include <core/atl/vector.hpp>
#include <engine/assets.hpp>
#include <build_config.hpp>
#include <shaders/shader_file.hpp>
#include <core/utils/enum.hpp>

#include <core/logging.hpp>

#if ANTON_WITH_EDITOR
#    include <rendering/builtin_editor_shaders.hpp>
#endif // ANTON_WITH_EDITOR

namespace anton_engine {
    static atl::Vector<Shader> builtin_shaders = atl::Vector<Shader>(atl::reserve, 5);
#if ANTON_WITH_EDITOR
    static atl::Vector<Shader> builtin_editor_shaders = atl::Vector<Shader>(atl::reserve, 3);
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

        auto skybox_vert = assets::load_shader_file("skybox.vert");
        auto skybox_frag = assets::load_shader_file("skybox.frag");
        builtin_shaders.push_back(create_shader(skybox_vert, skybox_frag));

        auto gamma_correction = assets::load_shader_file("postprocessing/gamma_correction.frag");
        builtin_shaders.push_back(create_shader(postprocess_vert, gamma_correction));

        auto quad_vert = assets::load_shader_file("quad.vert");
        auto quad_frag = assets::load_shader_file("quad.frag");
        builtin_shaders.push_back(create_shader(quad_vert, quad_frag));

#if ANTON_WITH_EDITOR
        Shader_File outline_mix_file = assets::load_shader_file("editor/outline_mix.frag");
        builtin_editor_shaders.push_back(create_shader(outline_mix_file, postprocess_vert));

        Shader_File grid_vert = assets::load_shader_file("editor/grid.vert");
        Shader_File grid_frag = assets::load_shader_file("editor/grid.frag");
        builtin_editor_shaders.push_back(create_shader(grid_vert, grid_frag));

        Shader_File imgui_vert = assets::load_shader_file("editor/imgui.vert");
        Shader_File imgui_frag = assets::load_shader_file("editor/imgui.frag");
        builtin_editor_shaders.push_back(create_shader(imgui_vert, imgui_frag));
#endif // ANTON_WITH_EDITOR
    }

    void unload_builtin_shaders() {
        builtin_shaders.clear();
#if ANTON_WITH_EDITOR
        builtin_editor_shaders.clear();
#endif // ANTON_WITH_EDITOR
    }

    void reload_builtin_shaders() {
        try {
            Shader_File uniform_color_vert = assets::load_shader_file("uniform_color.vert");
            Shader_File uniform_color_frag = assets::load_shader_file("uniform_color.frag");
            builtin_shaders[0] = create_shader(uniform_color_vert, uniform_color_frag);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading uniform_color shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            Shader_File uniform_color_line_vert = assets::load_shader_file("uniform_color_line.vert");
            Shader_File uniform_color_frag = assets::load_shader_file("uniform_color.frag");
            builtin_shaders[1] = create_shader(uniform_color_line_vert, uniform_color_frag);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading uniform_color_line shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            auto postprocess_vert = assets::load_shader_file("postprocessing/postprocess_vertex.vert");
            auto deferred_frag = assets::load_shader_file("deferred_shading.frag");
            builtin_shaders[2] = create_shader(deferred_frag, postprocess_vert);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading deferred_shading shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            auto skybox_vert = assets::load_shader_file("skybox.vert");
            auto skybox_frag = assets::load_shader_file("skybox.frag");
            builtin_shaders[3] = create_shader(skybox_vert, skybox_frag);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading skybox shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            auto postprocess_vert = assets::load_shader_file("postprocessing/postprocess_vertex.vert");
            auto gamma_correction = assets::load_shader_file("postprocessing/gamma_correction.frag");
            builtin_shaders[4] = create_shader(postprocess_vert, gamma_correction);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading postprocessing/gamma_correction shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            auto quad_vert = assets::load_shader_file("quad.vert");
            auto quad_frag = assets::load_shader_file("quad.frag");
            builtin_shaders[5] = create_shader(quad_vert, quad_frag);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading quad shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

#if ANTON_WITH_EDITOR
        try {
            auto postprocess_vert = assets::load_shader_file("postprocessing/postprocess_vertex.vert");
            Shader_File outline_mix_file = assets::load_shader_file("editor/outline_mix.frag");
            builtin_editor_shaders[0] = create_shader(outline_mix_file, postprocess_vert);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading editor/outline_mix shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            Shader_File grid_vert = assets::load_shader_file("editor/grid.vert");
            Shader_File grid_frag = assets::load_shader_file("editor/grid.frag");
            builtin_editor_shaders[1] = create_shader(grid_vert, grid_frag);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading editor/grid shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        try {
            Shader_File imgui_vert = assets::load_shader_file("editor/imgui.vert");
            Shader_File imgui_frag = assets::load_shader_file("editor/imgui.frag");
            builtin_editor_shaders[2] = create_shader(imgui_vert, imgui_frag);
        } catch (std::runtime_error const& error) {
            atl::String error_message = atl::String(error.what()) + u8"\nContinuing without reloading editor/imgui shader\n-";
            ANTON_LOG_ERROR(error_message);
        }

        ANTON_LOG_INFO(u8"Builtin shaders reloaded\n-");
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
