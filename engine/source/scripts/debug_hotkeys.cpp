#include "debug_hotkeys.hpp"

#include "input/input.hpp"

#include "assets.hpp"
#include "debug_macros.hpp"
#include "engine.hpp"
#include "renderer.hpp"
#include "shader_exceptions.hpp"
#include "shader_manager.hpp"

static void reload_shader(Shader& shader_to_reload, std::filesystem::path const& vertex, std::filesystem::path const& fragment) {
    try {
        Shader shader;
        Assets::load_shader_file_and_attach(shader, vertex);
        Assets::load_shader_file_and_attach(shader, fragment);
        shader.link();

        Shader::swap_programs(shader_to_reload, shader);
    } catch (Program_Linking_Failed& e) {
        GE_log("Failed to reload shaders due to linking error");
        GE_log(e.what());
    } catch (Shader_Compilation_Failed& e) {
        GE_log("Failed to reload shaders due to compilation error");
        GE_log(e.what());
    } catch (std::runtime_error& e) {
        GE_log("Failed to reload shaders due to unknown error");
        GE_log(e.what());
    }
}

static void reload_shader(Shader& shader_to_reload, std::filesystem::path const& vertex, std::filesystem::path const& geom,
                          std::filesystem::path const& fragment) {
    try {
        Shader shader;
        Assets::load_shader_file_and_attach(shader, vertex);
        Assets::load_shader_file_and_attach(shader, geom);
        Assets::load_shader_file_and_attach(shader, fragment);
        shader.link();

        Shader::swap_programs(shader_to_reload, shader);
    } catch (Program_Linking_Failed& e) {
        GE_log("Failed to reload shaders due to linking error");
        GE_log(e.what());
    } catch (Shader_Compilation_Failed& e) {
        GE_log("Failed to reload shaders due to compilation error");
        GE_log(e.what());
    } catch (std::runtime_error& e) {
        GE_log("Failed to reload shaders due to unknown error");
        GE_log(e.what());
    }
}

static void reload_renderer_shaders(char const* fxaa) {
    renderer::Renderer& rend = Engine::get_renderer();
    //reload_shader(rend.fxaa_shader, "postprocessing/postprocess_vertex.vert", fxaa);
    reload_shader(rend.tangents, "tangents.vert", "tangents.geom", "tangents.frag");
    reload_shader(rend.deferred_shading_shader, "quad.vert", "deferred_shading.frag");
    rend.deferred_shading_shader.use();
    rend.deferred_shading_shader.set_int("gbuffer_position", 0);
    rend.deferred_shading_shader.set_int("gbuffer_normal", 1);
    rend.deferred_shading_shader.set_int("gbuffer_albedo_spec", 2);
}

static char const* current_fxaa = "postprocessing/fxaa.frag";
static char const* next_fxaa = "postprocessing/nv_fxaa_port.frag";

static void swap_fxaa_shader() {
    std::swap(current_fxaa, next_fxaa);
    reload_renderer_shaders(current_fxaa);
}

void Debug_Hotkeys::update(Debug_Hotkeys& debug_hotkeys) {
    float reload = Input::get_axis_raw("reload_shaders");
    if (debug_hotkeys.reload_old == 0.0f && reload == 1.0f) {
        Engine::get_shader_manager().reload_shaders();
        reload_renderer_shaders(current_fxaa);
    }
    debug_hotkeys.reload_old = reload;

    float swap_fxaa = Input::get_axis_raw("swap_fxaa_shaders");
    if (debug_hotkeys.swap_fxaa_old == 0.0f && swap_fxaa == 1.0f) {
        swap_fxaa_shader();
    }
    debug_hotkeys.swap_fxaa_old = swap_fxaa;

    float show_shadow_map = Input::get_axis_raw("show_shadow_map");
    if (show_shadow_map == 1.0f && debug_hotkeys.show_shadow_map_old == 0.0f) {
        renderer::Renderer& renderer = Engine::get_renderer();
        renderer.output_shadow_map = !renderer.output_shadow_map;
    }
    debug_hotkeys.show_shadow_map_old = show_shadow_map;
}