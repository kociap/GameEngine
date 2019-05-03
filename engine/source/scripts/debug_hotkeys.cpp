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
    auto print_press = Input::get_action("print_press");
    if (print_press.pressed) {
        GE_log("Pressed");
    }
    if (print_press.released) {
        GE_log("Released");
    }

    auto reload = Input::get_action("reload_shaders");
    if (reload.released) {
        Engine::get_shader_manager().reload_shaders();
        reload_renderer_shaders(current_fxaa);
    }

    auto swap_fxaa = Input::get_action("swap_fxaa_shaders");
    if (swap_fxaa.released) {
        swap_fxaa_shader();
    }

    auto show_shadow_map = Input::get_action("show_shadow_map");
    if (show_shadow_map.released) {
        renderer::Renderer& renderer = Engine::get_renderer();
        renderer.output_shadow_map = !renderer.output_shadow_map;
    }
}