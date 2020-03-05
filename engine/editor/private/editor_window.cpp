#include <editor_window.hpp>

#include <core/stl/string.hpp>
#include <shaders/builtin_shaders.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>
#include <engine/ecs/ecs.hpp>
#include <editor_preferences.hpp>
#include <engine/input.hpp>
#include <core/logging.hpp>
#include <rendering/opengl.hpp>
#include <rendering/renderer.hpp>

#include <editor.hpp>
#include <rendering/imgui_rendering.hpp>

#include <stdexcept>

namespace anton_engine {
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
