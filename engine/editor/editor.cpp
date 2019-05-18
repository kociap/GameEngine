#include "editor.hpp"

#include "camera_movement.hpp"
#include "components/camera.hpp"
#include "components/transform.hpp"
#include "debug_hotkeys.hpp"
#include "ecs/ecs.hpp"
#include "engine.hpp"
#include "gizmo_internal.hpp"
#include "imgui.h"
#include "imgui_renderer.hpp"
#include "input/input.hpp"
#include "input/input_core.hpp"
#include "level_editor.hpp"
#include "time.hpp"
#include "time/time_core.hpp"
#include "window.hpp"

Level_Editor* Editor::level_editor = nullptr;
Imgui_Renderer* Editor::imgui_renderer = nullptr;
bool Editor::mouse_captured = false;

static void resize_imgui(ImGuiIO& io, uint32_t width, uint32_t height) {
    io.DisplaySize = ImVec2(width, height);
}

static void update_imgui() {
    namespace imgui = ImGui;

    ImGuiIO& io = imgui::GetIO();
    Window& window = Engine::get_window();
    resize_imgui(io, window.width(), window.height());

    io.DeltaTime = Time::get_unscaled_delta_time();

    // TODO update keys/gamepad/whatever

    // Update mouse
    io.MouseDown[0] = Input::get_key_state(Key::left_mouse_button).down;
    io.MouseDown[1] = Input::get_key_state(Key::right_mouse_button).down;
    io.MouseDown[2] = Input::get_key_state(Key::middle_mouse_button).down;

    Vector2 mouse_pos = Engine::get_window().get_cursor_position();
    io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);
}

void Editor::init() {
    gizmo::init();
    imgui_renderer = new Imgui_Renderer();
    level_editor = new Level_Editor();
}

void Editor::terminate() {
    delete level_editor;
    level_editor = nullptr;
    delete imgui_renderer;
    imgui_renderer = nullptr;
    gizmo::terminate();
}

void Editor::loop() {
    Engine::get_window().poll_events();
    Engine::get_time_manager().update_time();
    Engine::get_input_manager().process_events();

    gizmo::update();

    auto camera_mov_view = Engine::get_ecs().access<Camera_Movement, Camera, Transform>();
    for (Entity const entity: camera_mov_view) {
        auto& [camera_mov, camera, transform] = camera_mov_view.get<Camera_Movement, Camera, Transform>(entity);
        Camera_Movement::update(camera_mov, camera, transform);
    }

    auto dbg_hotkeys = Engine::get_ecs().access<Debug_Hotkeys>();
    for (Entity const entity: dbg_hotkeys) {
        Debug_Hotkeys::update(dbg_hotkeys.get(entity));
    }

    update_imgui();
    imgui_renderer->new_frame();
    level_editor->prepare_editor_ui();
    imgui_renderer->render_ui();
    Engine::get_window().swap_buffers();
}

bool Editor::should_close() {
    // TODO Editor should have its own close condition
    return Engine::should_close();
}

void Editor::resize(uint32_t width, uint32_t height) {
    resize_imgui(ImGui::GetIO(), width, height);
}

bool Editor::is_mouse_captured() {
    return mouse_captured;
}

void Editor::set_mouse_captured(bool captured) {
    mouse_captured = captured;
}