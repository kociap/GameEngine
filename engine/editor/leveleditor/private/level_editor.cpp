#include "level_editor.hpp"

#include "engine.hpp"
#include "math/vector2.hpp"
#include "renderer.hpp"
#include <cstdint>

#include "imgui.h"

void Level_Editor::prepare_editor_ui() {
    namespace imgui = ImGui;

    static Vector2 previous_window_size = Vector2(0.0f, 0.0f);

    Renderer& renderer = Engine::get_renderer();
    imgui::Begin("Level Editor");
    ImVec2 window_size = imgui::GetWindowSize();
    if (window_size.x != previous_window_size.x || window_size.y != previous_window_size.y) {
        previous_window_size.x = window_size.x;
        previous_window_size.y = window_size.y;
        renderer.resize(window_size.x, window_size.y);
    }
    uint32_t texture = renderer.render_frame_as_texture(window_size.x, window_size.y);
    imgui::Image(reinterpret_cast<ImTextureID>(texture), ImVec2(window_size.x, window_size.y), ImVec2(0, 1), ImVec2(1, 0));
    imgui::End();
}