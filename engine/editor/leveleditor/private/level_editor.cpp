#include "level_editor.hpp"

#include "collisions.hpp"
#include "components/static_mesh_component.hpp"
#include "components/transform.hpp"
#include "debug_macros.hpp"
#include "ecs/ecs.hpp"
#include "engine.hpp"
#include "gizmo.hpp"
#include "input/input.hpp"
#include "math/vector2.hpp"
#include "mesh/mesh.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "resource_manager.hpp"
#include "window.hpp"
#include <cstdint>

#include "imgui.h"

constexpr float window_border_width = 1.0f;

static Vector2 to_vec2(ImVec2 vec) {
    return {vec.x, vec.y};
}

static Vector2 get_mouse_position_relative_to_window() {
    Vector2 window_pos = to_vec2(ImGui::GetWindowPos());
    Vector2 mouse_absolute = Engine::get_window().get_cursor_position();
    Vector2 mouse_pos = mouse_absolute - window_pos;
    return mouse_pos;
}

void Level_Editor::prepare_editor_ui() {
    namespace imgui = ImGui;

    Entity camera_entity = null_entity;
    ECS& ecs = Engine::get_ecs();
    auto rendering = ecs.access<Camera, Transform>();
    for (Entity const entity: rendering) {
        auto& [camera, transform] = rendering.get<Camera, Transform>(entity);
        if (camera.active) {
            camera_entity = entity;
            break;
        }
    }

    imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
    ImGui::Begin("Level Editor");
    ImVec2 window_size = imgui::GetWindowSize();
    Vector2 mouse_pos = get_mouse_position_relative_to_window();
    // -1 because window_size is the number of pixels and mouse_pos is position relative to top-left corner
    mouse_pos.y = window_size.y - mouse_pos.y - 1.0f;
    mouse_pos -= window_border_width;
    // border width is multiplied by 1 instead of 2 because who knows why.
    // ImGui's documentation is poor a.k.a. doesn't exist
    ImVec2 window_content_size = {window_size.x - 1.0f * window_border_width, window_size.y - imgui::GetWindowTitleBarHeight() - 1.0f * window_border_width};
    ImGui::End();
    imgui::PopStyleVar(ImGuiStyleVar_WindowPadding);

    Input::Key_State state = Input::get_key_state(Key::right_mouse_button);
    auto& [camera, camera_transform] = rendering.get<Camera, Transform>(camera_entity);
    static Transform selected_obj_transform;
    if (!state.down && state.up_down_transitioned) {
        Matrix4 inv_proj_mat = math::inverse(get_camera_projection_matrix(camera, window_content_size.x, window_content_size.y));
        Matrix4 inv_view_mat = math::inverse(get_camera_view_matrix(camera_transform));
        physics::Ray ray = screen_to_ray(inv_view_mat, inv_proj_mat, window_content_size.x, window_content_size.y, mouse_pos);
        gizmo::draw_line(ray.origin, ray.origin + 20 * ray.direction, Color::green, 20.0f);

        ECS& ecs = Engine::get_ecs();
        Resource_Manager<Mesh>& mesh_manager = Engine::get_mesh_manager();
        Component_Access access = ecs.access<Static_Mesh_Component, Transform>();
        Entity selected = null_entity;
        physics::Raycast_Hit closest_hit;
        closest_hit.distance = math::constants<float>::infinity;
        for (Entity const entity: access) {
            auto& [c, transform] = access.get<Static_Mesh_Component, Transform>(entity);
            Mesh const& mesh = mesh_manager.get(c.mesh_handle);
            physics::Raycast_Hit hit;
            if (physics::intersect_ray_mesh(ray, mesh, transform.to_matrix(), hit)) {
                gizmo::draw_point(hit.hit_point, 0.05, Color::red, 20.0f);
                float distance = math::length(hit.hit_point - ray.origin);
                GE_log("Ray hit at distance " + std::to_string(hit.distance) + "; Real distance " + std::to_string(distance));
                if (hit.distance < closest_hit.distance) {
                    closest_hit = hit;
                    selected = entity;
                }
            }
        }

        if (selected != null_entity) {
            selected_obj_transform = ecs.get_component<Transform>(selected);
        }
    }

    imgui::Begin("Inspector");
    imgui::Text("Location");
    imgui::InputFloat("", &selected_obj_transform.local_position.x);
    imgui::InputFloat("", &selected_obj_transform.local_position.y);
    imgui::InputFloat("", &selected_obj_transform.local_position.z);
    imgui::Text("Window size");
    imgui::InputFloat2("", &window_size.x);
    imgui::Text("Window content size");
    imgui::InputFloat2("", &window_content_size.x);
    imgui::Text("Mouse Position");
    imgui::InputFloat2("", &mouse_pos.x);
    imgui::End();

    static Vector2 previous_window_content_size = Vector2(0.0f, 0.0f);
    Renderer& renderer = Engine::get_renderer();
    imgui::Begin("Level Editor");
    if (window_content_size.x != previous_window_content_size.x || window_content_size.y != previous_window_content_size.y) {
        previous_window_content_size.x = window_content_size.x;
        previous_window_content_size.y = window_content_size.y;
        renderer.resize(window_content_size.x, window_content_size.y);
    }

    // Main world axes
    gizmo::draw_line({-10.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}, {179.0f / 255.0f, 20.0f / 255.0f, 5.0f / 255.0f});
    gizmo::draw_line({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 10.0f}, {15.0f / 255.0f, 77.0f / 255.0f, 186.0f / 255.0f});

    uint32_t texture = renderer.render_frame_as_texture(camera, camera_transform, window_content_size.x, window_content_size.y);
    imgui::Image(reinterpret_cast<ImTextureID>(texture), window_content_size, ImVec2(0, 1), ImVec2(1, 0));
    imgui::End();
}