#include <editor_window.hpp>

#include <anton_stl/string.hpp>
#include <builtin_shaders.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <ecs/ecs.hpp>
#include <editor_preferences.hpp>
#include <input.hpp>
#include <logging.hpp>
#include <opengl.hpp>
#include <renderer.hpp>

#include <editor.hpp>
#include <imgui_rendering.hpp>

#include <stdexcept>

namespace anton_engine {
    constexpr uint32_t max_viewports = 4;

    Editor_Window::Editor_Window() {}

    Editor_Window::~Editor_Window() {}

    void Editor_Window::update() {
        if (auto const state = input::get_key_state(Key::y); state.down && state.up_down_transitioned) {
            reload_builtin_shaders();
        }

        auto const state = input::get_key_state(Key::u);
        if (state.up_down_transitioned && !state.down) {
            // Lock cursor
        }
    }

    void Editor_Window::render() {
        rendering::update_dynamic_lights();
    }
} // namespace anton_engine
