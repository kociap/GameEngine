#include <debug_hotkeys.hpp>

#include <input/input.hpp>

#include <assets.hpp>
#include <debug_macros.hpp>
#include <editor.hpp>
#include <engine.hpp>
#include <renderer.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <shader_exceptions.hpp>
#include <window.hpp>

template <typename... T>
void reload_shader(Shader& shader_to_reload, T&&... path) {
    try {
        Shader shader = create_shader(assets::load_shader_file(std::forward<T>(path))...);
        swap(shader_to_reload, shader);
    } catch (Program_Linking_Failed const& e) {
        GE_log("Failed to reload shaders due to linking error");
        GE_log(e.what());
    } catch (Shader_Compilation_Failed const& e) {
        GE_log("Failed to reload shaders due to compilation error");
        GE_log(e.what());
    } catch (std::exception const& e) {
        GE_log("Failed to reload shaders due to unknown error");
        GE_log(e.what());
    }
}

static void reload_renderer_shaders(char const*) {
    rendering::Renderer& rend = Engine::get_renderer();
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

void Debug_Hotkeys::update(Debug_Hotkeys&) {
    auto print_press = Input::get_action("print_press");
    if (print_press.pressed) {
        GE_log("Pressed");
    }
    if (print_press.released) {
        GE_log("Released");
    }

    auto reload = Input::get_action("reload_shaders");
    if (reload.released) {
        // TODO reloading shaders
        //Engine::get_shader_manager().reload_shaders();
        reload_renderer_shaders(current_fxaa);
    }

    auto swap_fxaa = Input::get_action("swap_fxaa_shaders");
    if (swap_fxaa.released) {
        swap_fxaa_shader();
    }

    //auto capture_mouse = Input::get_action("capture_mouse");
    //if (capture_mouse.released) {
    //    if (debug_hotkeys.cursor_captured) {
    //        debug_hotkeys.cursor_captured = false;
    //        Engine::get_window().unlock_cursor();
    //    } else {
    //        debug_hotkeys.cursor_captured = true;
    //        Engine::get_window().lock_cursor();
    //    }
    //    Editor::set_mouse_captured(debug_hotkeys.cursor_captured);
    //}
}
