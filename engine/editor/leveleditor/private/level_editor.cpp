#include "level_editor.hpp"

#include "assets.hpp"
#include "collisions.hpp"
#include "components/static_mesh_component.hpp"
#include "components/transform.hpp"
#include "containers/vector.hpp"
#include "debug_macros.hpp"
#include "ecs/ecs.hpp"
#include "engine.hpp"
#include "framebuffer.hpp"
#include "gizmo.hpp"
#include "gizmo_internal.hpp"
#include "glad/glad.h"
#include "input/input.hpp"
#include "math/vector2.hpp"
#include "mesh/mesh.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "renderer_internal.hpp"
#include "resource_manager.hpp"
#include "shader_file.hpp"
#include "window.hpp"
#include <cstdint>
#include <tuple>

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

static uint32_t draw_outlines(Framebuffer* framebuffer, uint32_t const scene_texture, containers::Vector<Entity> const& selected_entities, Matrix4 const view,
                              Matrix4 const projection, Color const outline_color) {
    Renderer& renderer = Engine::get_renderer();
    // copy depth buffer
    Framebuffer::bind(*renderer.framebuffer, Framebuffer::Bind_Mode::read);
    Framebuffer::bind(*framebuffer, Framebuffer::Bind_Mode::draw);
    Framebuffer::blit(*renderer.framebuffer, *framebuffer, opengl::Buffer_Mask::depth_buffer_bit);

    renderer.single_color_shader.use();
    renderer.single_color_shader.set_vec4("color", outline_color);
    renderer.single_color_shader.set_matrix4("view", view);
    renderer.single_color_shader.set_matrix4("projection", projection);

    ECS& ecs = Engine::get_ecs();
    Resource_Manager<Mesh>& mesh_manager = Engine::get_mesh_manager();
    Framebuffer::bind(*framebuffer);
    opengl::clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    opengl::clear(opengl::Buffer_Mask::color_buffer_bit);
    for (Entity entity: selected_entities) {
        auto const [transform, static_mesh] = ecs.get_component<Transform, Static_Mesh_Component>(entity);
        Mesh const& mesh = mesh_manager.get(static_mesh.mesh_handle);
        renderer.single_color_shader.set_matrix4("model", transform.to_matrix());
        render_mesh(mesh);
    }

    opengl::active_texture(0);
    opengl::bind_texture(opengl::Texture_Type::texture_2D, scene_texture);
    opengl::active_texture(1);
    opengl::bind_texture(opengl::Texture_Type::texture_2D, framebuffer->get_color_texture(0));
    renderer.outline_mix_shader.use();
    Framebuffer::bind(*renderer.postprocess_back_buffer);
    renderer.render_postprocess();
    renderer.swap_postprocess_buffers();
    return renderer.postprocess_front_buffer->get_color_texture(0);
}

static uint32_t draw_gizmo(Framebuffer* framebuffer, Vector3 const camera_pos, Matrix4 const view, Matrix4 const projection) {
    Renderer& renderer = Engine::get_renderer();
    glEnable(GL_DEPTH_TEST);
    Framebuffer::bind(*renderer.postprocess_front_buffer, Framebuffer::Bind_Mode::read);
    Framebuffer::bind(*framebuffer, Framebuffer::Bind_Mode::draw);
    Framebuffer::blit(*renderer.postprocess_front_buffer, *framebuffer, opengl::Buffer_Mask::color_buffer_bit);
    Framebuffer::bind(*framebuffer);
    gizmo::draw(camera_pos, view, projection);
    glDisable(GL_DEPTH_TEST);
    return framebuffer->get_color_texture(0);
}

static Entity pick_object(Matrix4 const view, Matrix4 const projection, uint32_t const viewport_w, uint32_t const viewport_h, Vector2 const mouse_pos) {
    Matrix4 const inv_proj_mat = math::inverse(projection);
    Matrix4 const inv_view_mat = math::inverse(view);
    physics::Ray const ray = screen_to_ray(inv_view_mat, inv_proj_mat, viewport_w, viewport_h, mouse_pos);
    // gizmo::draw_line(ray.origin, ray.origin + 20 * ray.direction, Color::green, 200.0f);

    ECS& ecs = Engine::get_ecs();
    Resource_Manager<Mesh>& mesh_manager = Engine::get_mesh_manager();
    Component_Access access = ecs.access<Static_Mesh_Component, Transform>();
    Entity selected = null_entity;
    physics::Raycast_Hit closest_hit;
    closest_hit.distance = math::constants<float>::infinity;
    for (Entity const entity: access) {
        auto const& [c, transform] = access.get<Static_Mesh_Component, Transform>(entity);
        Mesh const& mesh = mesh_manager.get(c.mesh_handle);
        physics::Raycast_Hit hit;
        if (physics::intersect_ray_mesh(ray, mesh, transform.to_matrix(), hit)) {
            // gizmo::draw_point(hit.hit_point, 0.05, Color::red, 200.0f);
            if (hit.distance < closest_hit.distance) {
                closest_hit = hit;
                selected = entity;
            }
        }
    }

    return selected;
}

static void draw_translate_handle(Vector3 position, Vector3 direction, Color color, float distance_from_camera) {
    distance_from_camera /= 13.0f;
    gizmo::draw_line(position, position + distance_from_camera * direction, color, 0.0f, false);
    gizmo::draw_cone(position + distance_from_camera * direction, direction, distance_from_camera * 0.07f, 16, distance_from_camera * 0.23f, color, 0.0f,
                     false);
}

Level_Editor::Level_Editor() {
    Framebuffer::Construct_Info construct_info;
    construct_info.color_buffers.resize(1);
    construct_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgba8;
    construct_info.depth_buffer.enabled = true;
    construct_info.width = 1;
    construct_info.height = 1;
    framebuffer = new Framebuffer(construct_info);
}

Level_Editor::~Level_Editor() {
    delete framebuffer;
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

    Input::Key_State const state = Input::get_key_state(Key::right_mouse_button);
    auto& [camera, camera_transform] = rendering.get<Camera, Transform>(camera_entity);
    static containers::Vector<Entity> selected_entities;
    static Entity active_entity = null_entity;
    static Transform active_entity_transform;
    Matrix4 const camera_view_mat = get_camera_view_matrix(camera_transform);
    Matrix4 const camera_projection_mat = get_camera_projection_matrix(camera, window_content_size.x, window_content_size.y);
    if (!state.down && state.up_down_transitioned) {
        Entity selected_entity = pick_object(camera_view_mat, camera_projection_mat, window_content_size.x, window_content_size.y, mouse_pos);
        if (selected_entity != null_entity) {
            bool deselected = false;
            for (containers::Vector<Entity>::size_type i = 0; i < selected_entities.size(); ++i) {
                if (selected_entities[i] == selected_entity) {
                    selected_entities.erase_unsorted_unchecked(i);
                    deselected = true;
                    break;
                }
            }

            if (!deselected) {
                selected_entities.insert_unsorted(selected_entities.cbegin(), selected_entity);
            }
        }
    }

    imgui::Begin("Inspector");
    imgui::Text("Location");
    imgui::InputFloat("", &active_entity_transform.local_position.x);
    imgui::InputFloat("", &active_entity_transform.local_position.y);
    imgui::InputFloat("", &active_entity_transform.local_position.z);
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
        framebuffer->resize(window_content_size.x, window_content_size.y);
    }

    // Main world axes
    gizmo::draw_line({-10.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}, {179.0f / 255.0f, 20.0f / 255.0f, 5.0f / 255.0f});
    gizmo::draw_line({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 10.0f}, {15.0f / 255.0f, 77.0f / 255.0f, 186.0f / 255.0f});

    if (selected_entities.size() > 0) {
        Transform transform = ecs.get_component<Transform>(selected_entities.front());
        float distance_from_camera = (transform.local_position - camera_transform.local_position).length();
        draw_translate_handle(transform.local_position, Vector3::forward, {15.f / 255.f, 77.f / 255.f, 186.f / 255.f}, distance_from_camera);
        draw_translate_handle(transform.local_position, Vector3::right, {179.f / 255.f, 20.f / 255.f, 5.f / 255.f}, distance_from_camera);
        draw_translate_handle(transform.local_position, Vector3::up, {0, 220.0f / 255.0f, 0}, distance_from_camera);
    }

    uint32_t texture = renderer.render_frame_as_texture(camera, camera_transform, window_content_size.x, window_content_size.y);
    texture = draw_outlines(framebuffer, texture, selected_entities, camera_view_mat, camera_projection_mat, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f});
    texture = draw_gizmo(framebuffer, camera_transform.local_position, camera_view_mat, camera_projection_mat); // TODO change from local to world
    texture = renderer.apply_gamma_correction(texture);
    imgui::Image(reinterpret_cast<ImTextureID>(texture), window_content_size, ImVec2(0, 1), ImVec2(1, 0));
    imgui::End();
}