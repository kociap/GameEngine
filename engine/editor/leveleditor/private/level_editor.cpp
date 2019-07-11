#include "level_editor.hpp"

#include "assets.hpp"
#include "components/camera.hpp"
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
#include "intersection_tests.hpp"
#include "line.hpp"
#include "math/vector2.hpp"
#include "mesh/mesh.hpp"
#include "obb.hpp"
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

static Entity pick_object(Ray ray, uint32_t const viewport_w, uint32_t const viewport_h, Vector2 const mouse_pos) {
    // gizmo::draw_line(ray.origin, ray.origin + 20 * ray.direction, Color::green, 200.0f);

    ECS& ecs = Engine::get_ecs();
    Resource_Manager<Mesh>& mesh_manager = Engine::get_mesh_manager();
    Component_Access access = ecs.access<Static_Mesh_Component, Transform>();
    Entity selected = null_entity;
    Raycast_Hit closest_hit;
    closest_hit.distance = math::constants<float>::infinity;
    for (Entity const entity: access) {
        auto const& [c, transform] = access.get<Static_Mesh_Component, Transform>(entity);
        Mesh const& mesh = mesh_manager.get(c.mesh_handle);
        if (auto hit = intersect_ray_mesh(ray, mesh, transform.to_matrix()); hit && hit->distance < closest_hit.distance) {
            // gizmo::draw_point(hit.hit_point, 0.05, Color::red, 200.0f);
            closest_hit = hit.value();
            selected = entity;
        }
    }

    return selected;
}

static void draw_wireframe_cuboid(Vector3 pos, Vector3 x, Vector3 y, Vector3 z) {
    gizmo::draw_line(pos - x + y - z, pos - x - y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    gizmo::draw_line(pos + x + y - z, pos + x - y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    gizmo::draw_line(pos + x + y - z, pos - x + y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    gizmo::draw_line(pos + x - y - z, pos - x - y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    gizmo::draw_line(pos - x + y + z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    gizmo::draw_line(pos + x + y + z, pos + x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    gizmo::draw_line(pos + x + y + z, pos - x + y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    gizmo::draw_line(pos + x - y + z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    gizmo::draw_line(pos - x + y - z, pos - x + y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    gizmo::draw_line(pos + x + y - z, pos + x + y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    gizmo::draw_line(pos - x - y - z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    gizmo::draw_line(pos + x - y - z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
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
        auto [camera, transform] = rendering.get<Camera, Transform>(entity);
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

    auto const state = Input::get_key_state(Key::right_mouse_button);
    auto const shift_state = Input::get_key_state(Key::left_shift);
    auto [camera, camera_transform] = rendering.get<Camera, Transform>(camera_entity);
    static containers::Vector<Entity> selected_entities;
    Matrix4 const camera_view_mat = get_camera_view_matrix(camera_transform);
    Matrix4 const camera_projection_mat = get_camera_projection_matrix(camera, window_content_size.x, window_content_size.y);
    Matrix4 const inv_proj_mat = math::inverse(camera_projection_mat);
    Matrix4 const inv_view_mat = math::inverse(camera_view_mat);
    Ray const ray = screen_to_ray(inv_view_mat, inv_proj_mat, window_content_size.x, window_content_size.y, mouse_pos);
    if (!state.down && state.up_down_transitioned) {
        if (shift_state.down) {
            Entity selected_entity = pick_object(ray, window_content_size.x, window_content_size.y, mouse_pos);
            if (selected_entity != null_entity) {
                bool already_selected = false;
                if (!selected_entities.empty() && selected_entities.front() == selected_entity) {
                    selected_entities.erase_unsorted_unchecked(0);
                    already_selected = true;
                } else {
                    for (containers::Vector<Entity>::size_type i = 1; i < selected_entities.size(); ++i) {
                        if (selected_entities[i] == selected_entity) {
                            swap(selected_entities[0], selected_entities[i]);
                            already_selected = true;
                            break;
                        }
                    }
                }

                if (!already_selected) {
                    selected_entities.insert_unsorted(selected_entities.cbegin(), selected_entity);
                }
            }
        } else {
            Entity selected_entity = pick_object(ray, window_content_size.x, window_content_size.y, mouse_pos);
            if (selected_entity != null_entity) {
                selected_entities.clear();
                selected_entities.push_back(selected_entity);
            }
        }
    }

    Transform active_entity_transform;
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

    static int gizmo_transform_space = 0;
    static char const* space_prev_val = "World";
    if (imgui::BeginCombo("Space", space_prev_val)) {
        if (imgui::Selectable("World")) {
            gizmo_transform_space = 0;
            space_prev_val = "World";
        }
        if (imgui::Selectable("Local")) {
            gizmo_transform_space = 1;
            space_prev_val = "Local";
        }
        imgui::EndCombo();
    }

    static int gizmo_type = 0;
    static char const* type_prev_val = "Translate";
    if (imgui::BeginCombo("Type", type_prev_val)) {
        if (imgui::Selectable("Translate")) {
            gizmo_transform_space = 0;
            type_prev_val = "Translate";
        }
        if (imgui::Selectable("Rotate")) {
            gizmo_transform_space = 1;
            type_prev_val = "Rotate";
        }
        if (imgui::Selectable("Scale")) {
            gizmo_transform_space = 2;
            type_prev_val = "Scale";
        }
        imgui::EndCombo();
    }
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

    Color axis_blue = {15.f / 255.f, 77.f / 255.f, 186.f / 255.f};
    Color axis_green = {0, 220.0f / 255.0f, 0};
    Color axis_red = {179.f / 255.f, 20.f / 255.f, 5.f / 255.f};
    // Main world axes
    gizmo::draw_line({-10.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}, axis_red);
    gizmo::draw_line({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 10.0f}, axis_blue);

    if (selected_entities.size() > 0) {
        Transform& transform_ref = ecs.get_component<Transform>(selected_entities.front());
        Transform transform = transform_ref;
        // Have to somehow make those truly fixed size (window_size independent)
        float w_comp = (Vector4(0, 0, 0, 1) * transform.to_matrix() * camera_view_mat * camera_projection_mat).w;
        float handle_scale = w_comp * 0.1f;
        if (gizmo_transform_space == 0) { // translation
            float handle_line_length = handle_scale * 1.0f;
            float cone_height = handle_scale * 0.23f;
            float cone_radius = handle_scale * 0.07f;
            float constant_offset = handle_scale * 0.1f; // Makes obb bigger so that clicking is doesn't require insane precision
            Vector3 obb_dimensions = {cone_radius + constant_offset, cone_radius + constant_offset, handle_line_length + cone_height + constant_offset};
            OBB obb[3];
            obb[0].local_x = Vector3::right;
            obb[0].local_y = Vector3::up;
            obb[0].local_z = Vector3::forward;
            obb[1].local_x = -Vector3::forward;
            obb[1].local_y = Vector3::up;
            obb[1].local_z = Vector3::right;
            obb[2].local_x = Vector3::right;
            obb[2].local_y = -Vector3::forward;
            obb[2].local_z = Vector3::up;
            obb[0].center = {transform.local_position.x, transform.local_position.y, transform.local_position.z - obb_dimensions.z * 0.5f};
            obb[1].center = {transform.local_position.x + obb_dimensions.z * 0.5f, transform.local_position.y, transform.local_position.z};
            obb[2].center = {transform.local_position.x, transform.local_position.y + obb_dimensions.z * 0.5f, transform.local_position.z};
            obb[0].halfwidths = obb[1].halfwidths = obb[2].halfwidths = obb_dimensions * 0.5f;

            Input::Key_State const lmb_state = Input::get_key_state(Key::left_mouse_button);
            if (lmb_state.up_down_transitioned) {
                if (lmb_state.down) {
                    float distance = math::constants<float>::infinity;
                    if (auto hit = intersect_ray_obb(ray, obb[0]); hit && hit->distance < distance) {
                        distance = hit->distance;
                        gizmo_grabbed = true;
                        cached_gizmo_transform = transform;
                        gizmo_grabbed_axis = Vector3::forward;
                    }
                    if (auto hit = intersect_ray_obb(ray, obb[1]); hit && hit->distance < distance) {
                        distance = hit->distance;
                        gizmo_grabbed = true;
                        cached_gizmo_transform = transform;
                        gizmo_grabbed_axis = Vector3::right;
                    }
                    if (auto hit = intersect_ray_obb(ray, obb[2]); hit && hit->distance < distance) {
                        gizmo_grabbed = true;
                        cached_gizmo_transform = transform;
                        gizmo_grabbed_axis = Vector3::up;
                    }
                    Vector3 camera_pos_projected_on_translation_axis = math::dot(gizmo_grabbed_axis, camera_transform.local_position) * gizmo_grabbed_axis;
                    gizmo_plane_normal = math::normalize(camera_transform.local_position - camera_pos_projected_on_translation_axis);
                    gizmo_plane_distance = math::dot(transform.local_position, gizmo_plane_normal);
                    gizmo_mouse_grab_point = intersect_line_plane({ray.origin, ray.direction}, gizmo_plane_normal, gizmo_plane_distance)->hit_point;
                } else {
                    gizmo_grabbed = false;
                }
            } else {
                if (lmb_state.down && gizmo_grabbed) {
                    Vector3 intersection = intersect_line_plane({ray.origin, ray.direction}, gizmo_plane_normal, gizmo_plane_distance)->hit_point;
                    transform_ref.local_position =
                        cached_gizmo_transform.local_position + math::dot(intersection - gizmo_mouse_grab_point, gizmo_grabbed_axis) * gizmo_grabbed_axis;
                }
            }

            // Global space handles
            // blue handle
            Vector3 line_end = transform_ref.local_position + handle_scale * Vector3::forward;
            gizmo::draw_line(transform_ref.local_position, line_end, axis_blue, 1.0f, 0.0f, false);
            gizmo::draw_cone(line_end, Vector3::forward, cone_radius, 8, cone_height, axis_blue, 0.0f, false);
            // red handle
            line_end = transform_ref.local_position + handle_scale * Vector3::right;
            gizmo::draw_line(transform_ref.local_position, line_end, axis_red, 1.0f, 0.0f, false);
            gizmo::draw_cone(line_end, Vector3::right, cone_radius, 8, cone_height, axis_red, 0.0f, false);
            // green handle
            line_end = transform_ref.local_position + handle_scale * Vector3::up;
            gizmo::draw_line(transform_ref.local_position, line_end, axis_green, 1.0f, 0.0f, false);
            gizmo::draw_cone(line_end, Vector3::up, cone_radius, 8, cone_height, axis_green, 0.0f, false);

            draw_wireframe_cuboid(obb[0].center - transform.local_position + transform_ref.local_position, obb[0].local_x * obb[0].halfwidths.x,
                                  obb[0].local_y * obb[0].halfwidths.y, obb[0].local_z * obb[0].halfwidths.z);
            draw_wireframe_cuboid(obb[1].center - transform.local_position + transform_ref.local_position, obb[1].local_x * obb[1].halfwidths.x,
                                  obb[1].local_y * obb[1].halfwidths.y, obb[1].local_z * obb[1].halfwidths.z);
            draw_wireframe_cuboid(obb[2].center - transform.local_position + transform_ref.local_position, obb[2].local_x * obb[2].halfwidths.x,
                                  obb[2].local_y * obb[2].halfwidths.y, obb[2].local_z * obb[2].halfwidths.z);
        } else if (gizmo_transform_space == 1) { // rotation
        } else {                                 // scale
            float handle_line_length = handle_scale * 1.0f;
            float cube_size = handle_scale * 0.14f;
            float constant_offset = handle_scale * 0.1f; // Makes obb bigger so that clicking is doesn't require insane precision
            Vector3 obb_dimensions = {cube_size + constant_offset, cube_size + constant_offset, handle_line_length + cube_size + constant_offset};
            OBB obb[3];
            obb[0].local_x = Vector3::right;
            obb[0].local_y = Vector3::up;
            obb[0].local_z = Vector3::forward;
            obb[1].local_x = -Vector3::forward;
            obb[1].local_y = Vector3::up;
            obb[1].local_z = Vector3::right;
            obb[2].local_x = Vector3::right;
            obb[2].local_y = -Vector3::forward;
            obb[2].local_z = Vector3::up;
            obb[0].center = {transform.local_position.x, transform.local_position.y, transform.local_position.z - obb_dimensions.z * 0.5f};
            obb[1].center = {transform.local_position.x + obb_dimensions.z * 0.5f, transform.local_position.y, transform.local_position.z};
            obb[2].center = {transform.local_position.x, transform.local_position.y + obb_dimensions.z * 0.5f, transform.local_position.z};
            obb[0].halfwidths = obb[1].halfwidths = obb[2].halfwidths = obb_dimensions * 0.5f;

            Input::Key_State const lmb_state = Input::get_key_state(Key::left_mouse_button);
            if (lmb_state.up_down_transitioned) {
                if (lmb_state.down) {
                    float distance = math::constants<float>::infinity;
                    if (auto hit = intersect_ray_obb(ray, obb[0]); hit && hit->distance < distance) {
                        distance = hit->distance;
                        gizmo_grabbed = true;
                        cached_gizmo_transform = transform;
                        gizmo_grabbed_axis = Vector3::forward;
                    }
                    if (auto hit = intersect_ray_obb(ray, obb[1]); hit && hit->distance < distance) {
                        distance = hit->distance;
                        gizmo_grabbed = true;
                        cached_gizmo_transform = transform;
                        gizmo_grabbed_axis = Vector3::right;
                    }
                    if (auto hit = intersect_ray_obb(ray, obb[2]); hit && hit->distance < distance) {
                        gizmo_grabbed = true;
                        cached_gizmo_transform = transform;
                        gizmo_grabbed_axis = Vector3::up;
                    }
                    Vector3 camera_pos_projected_on_translation_axis = math::dot(gizmo_grabbed_axis, camera_transform.local_position) * gizmo_grabbed_axis;
                    gizmo_plane_normal = math::normalize(camera_transform.local_position - camera_pos_projected_on_translation_axis);
                    gizmo_plane_distance = math::dot(transform.local_position, gizmo_plane_normal);
                    gizmo_mouse_grab_point = intersect_line_plane({ray.origin, ray.direction}, gizmo_plane_normal, gizmo_plane_distance)->hit_point;
                } else {
                    gizmo_grabbed = false;
                }
            } else {
                if (lmb_state.down && gizmo_grabbed) {
                    Vector3 intersection = intersect_line_plane({ray.origin, ray.direction}, gizmo_plane_normal, gizmo_plane_distance)->hit_point;
                    transform_ref.local_scale =
                        cached_gizmo_transform.local_scale + math::dot(intersection - gizmo_mouse_grab_point, gizmo_grabbed_axis) * gizmo_grabbed_axis;
                }
            }

            // Global space handles
            // blue handle
            Vector3 line_end = transform_ref.local_position + handle_scale * Vector3::forward;
            gizmo::draw_line(transform_ref.local_position, line_end, axis_blue, 1.0f, 0.0f, false);
            gizmo::draw_cube(line_end, Vector3::forward, Vector3::right, Vector3::up, cube_size, axis_blue, 0.0f, false);
            // red handle
            line_end = transform_ref.local_position + handle_scale * Vector3::right;
            gizmo::draw_line(transform_ref.local_position, line_end, axis_red, 1.0f, 0.0f, false);
            gizmo::draw_cube(line_end, Vector3::forward, Vector3::right, Vector3::up, cube_size, axis_red, 0.0f, false);
            // green handle
            line_end = transform_ref.local_position + handle_scale * Vector3::up;
            gizmo::draw_line(transform_ref.local_position, line_end, axis_green, 1.0f, 0.0f, false);
            gizmo::draw_cube(line_end, Vector3::forward, Vector3::right, Vector3::up, cube_size, axis_green, 0.0f, false);

            draw_wireframe_cuboid(obb[0].center - transform.local_position + transform_ref.local_position, obb[0].local_x * obb[0].halfwidths.x,
                                  obb[0].local_y * obb[0].halfwidths.y, obb[0].local_z * obb[0].halfwidths.z);
            draw_wireframe_cuboid(obb[1].center - transform.local_position + transform_ref.local_position, obb[1].local_x * obb[1].halfwidths.x,
                                  obb[1].local_y * obb[1].halfwidths.y, obb[1].local_z * obb[1].halfwidths.z);
            draw_wireframe_cuboid(obb[2].center - transform.local_position + transform_ref.local_position, obb[2].local_x * obb[2].halfwidths.x,
                                  obb[2].local_y * obb[2].halfwidths.y, obb[2].local_z * obb[2].halfwidths.z);
        }
    }

    uint32_t texture = renderer.render_frame_as_texture(camera, camera_transform, window_content_size.x, window_content_size.y);
    texture = draw_outlines(framebuffer, texture, selected_entities, camera_view_mat, camera_projection_mat, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f});
    texture = draw_gizmo(framebuffer, camera_transform.local_position, camera_view_mat, camera_projection_mat); // TODO change from local to world
    texture = renderer.apply_gamma_correction(texture);
    imgui::Image(reinterpret_cast<ImTextureID>(texture), window_content_size, ImVec2(0, 1), ImVec2(1, 0));
    imgui::End();
}