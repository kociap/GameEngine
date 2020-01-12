#include <debug_hotkeys.hpp>

#include <input.hpp>

#include <anton_stl/string.hpp>
#include <assets.hpp>
#include <engine.hpp>
#include <logging.hpp>
#include <renderer.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <shader_exceptions.hpp>
#include <window.hpp>

namespace anton_engine {
    template <typename... T>
    void reload_shader(Shader& shader_to_reload, T&&... path) {
        try {
            Shader shader = create_shader(assets::load_shader_file(std::forward<T>(path))...);
            swap(shader_to_reload, shader);
        } catch (Program_Linking_Failed const& e) {
            ANTON_LOG_ERROR(anton_stl::String(u8"Failed to reload shaders due to linking error: ") + e.what());
        } catch (Shader_Compilation_Failed const& e) {
            ANTON_LOG_ERROR(anton_stl::String(u8"Failed to reload shaders due to compilation error: ") + e.what());
        } catch (std::exception const& e) {
            ANTON_LOG_ERROR(anton_stl::String(u8"Failed to reload shaders due to unknown error: ") + e.what()); //
        }
    }

    static char const* current_fxaa = "postprocessing/fxaa.frag";
    static char const* next_fxaa = "postprocessing/nv_fxaa_port.frag";

    static void swap_fxaa_shader() {
        std::swap(current_fxaa, next_fxaa);
    }

    void Debug_Hotkeys::update(Debug_Hotkeys& debug_hotkeys) {
        auto reload = input::get_action("reload_shaders");
        if (reload.released) {
            // TODO reloading shaders
            //Engine::get_shader_manager().reload_shaders();
        }

        auto swap_fxaa = input::get_action("swap_fxaa_shaders");
        if (swap_fxaa.released) {
            swap_fxaa_shader();
        }

#if !ANTON_WITH_EDITOR
        auto capture_mouse = input::get_action("capture_mouse");
        if (capture_mouse.released) {
            // if (debug_hotkeys.cursor_captured) {
            //     debug_hotkeys.cursor_captured = false;
            //     Engine::get_window().unlock_cursor();
            // } else {
            //     debug_hotkeys.cursor_captured = true;
            //     Engine::get_window().lock_cursor();
            // }
            // Editor::set_mouse_captured(debug_hotkeys.cursor_captured);
        }
#endif // !ANTON_WITH_EDITOR
    }
} // namespace anton_engine
