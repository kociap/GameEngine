#include <level_editor/viewport.hpp>

#include <core/atl/array.hpp>
#include <core/atl/utility.hpp>
#include <core/logging.hpp>
#include <core/math/transform.hpp>
#include <core/math/vector2.hpp>
#include <editor.hpp>
#include <editor_preferences.hpp>
#include <engine/assets.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/static_mesh_component.hpp>
#include <engine/components/transform.hpp>
#include <engine/ecs/ecs.hpp>
#include <engine/input.hpp>
#include <engine/input/input_internal.hpp>
#include <engine/mesh.hpp>
#include <engine/resource_manager.hpp>
#include <engine/time.hpp>
#include <level_editor/gizmo/arrow_3d.hpp>
#include <level_editor/gizmo/dial_3d.hpp>
#include <level_editor/gizmo/gizmo.hpp>
#include <level_editor/viewport_camera.hpp>
#include <physics/intersection_tests.hpp>
#include <physics/line.hpp>
#include <physics/obb.hpp>
#include <rendering/framebuffer.hpp>
#include <rendering/glad.hpp>
#include <rendering/renderer.hpp>
#include <shaders/builtin_shaders.hpp>
#include <shaders/shader_stage.hpp>

#include <imgui/imgui.hpp>
#include <rendering/builtin_editor_shaders.hpp>

namespace anton_engine {
    Viewport::Viewport(i32 vindex, i32 const width, i32 const height, imgui::Context* imgui_context): index(vindex), ctx(imgui_context) {
        Framebuffer::Construct_Info construct_info;
        construct_info.color_buffers.resize(1);
        construct_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgba8;
        construct_info.depth_buffer.enabled = true;
        construct_info.width = width;
        construct_info.height = height;
        framebuffer = new Framebuffer(construct_info);

        Framebuffer::Construct_Info multisample_info;
        multisample_info.multisampled = true;
        // TODO: Move to preferences and somehow make modifiable at runtime
        multisample_info.samples = 8;
        multisample_info.color_buffers.resize(1);
        multisample_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgba8;
        multisample_info.depth_buffer.enabled = true;
        multisample_info.width = width;
        multisample_info.height = height;
        multisampled_framebuffer = new Framebuffer(multisample_info);

        Framebuffer::Construct_Info deferred_framebuffer_info;
        deferred_framebuffer_info.width = width;
        deferred_framebuffer_info.height = height;
        deferred_framebuffer_info.depth_buffer.enabled = true;
        deferred_framebuffer_info.depth_buffer.buffer_type = Framebuffer::Buffer_Type::texture;
        deferred_framebuffer_info.color_buffers.resize(2);
        // Normal
        deferred_framebuffer_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgb32f;
        // albedo-specular
        deferred_framebuffer_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgba8;
        deferred_framebuffer = new Framebuffer(deferred_framebuffer_info);

        Framebuffer::Construct_Info front_back_buffer_info;
        front_back_buffer_info.width = width;
        front_back_buffer_info.height = height;
        front_back_buffer_info.color_buffers.resize(1);
        front_back_buffer_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgba8;
        front_framebuffer = new Framebuffer(front_back_buffer_info);
        back_framebuffer = new Framebuffer(front_back_buffer_info);

        ECS& ecs = Editor::get_ecs();
        auto [entity, viewport_camera, camera, transform] = ecs.create<Viewport_Camera, Camera, Transform>();
        transform.local_position = Vector3{0.0f, 0.0f, 2.0f};
        viewport_entity = entity;
        viewport_camera.viewport_index = index;

        atl::String window_id{u8"viewport_window"};
        window_id.append(atl::to_string(index));
        imgui::begin_window(*ctx, window_id);
        imgui::set_window_size(*ctx, {(f32)width, (f32)height});
        imgui::end_window(*ctx);
    }

    Viewport::~Viewport() {
        ECS& ecs = Editor::get_ecs();
        ecs.destroy(viewport_entity);
        delete deferred_framebuffer;
        delete multisampled_framebuffer;
        delete framebuffer;
        delete front_framebuffer;
        delete back_framebuffer;
    }

    void Viewport::resize_framebuffers(i32 const w, i32 const h) {
        framebuffer->resize(w, h);
        multisampled_framebuffer->resize(w, h);
        deferred_framebuffer->resize(w, h);
        front_framebuffer->resize(w, h);
        back_framebuffer->resize(w, h);
    }

    Vector2 Viewport::get_size() const {
        atl::String window_id{u8"viewport_window"};
        window_id.append(atl::to_string(index));
        imgui::begin_window(*ctx, window_id);
        Vector2 const viewport_size = imgui::get_window_dimensions(*ctx);
        imgui::end_window(*ctx);
        return viewport_size;
    }

    bool Viewport::is_active() const {
        atl::String window_id{u8"viewport_window"};
        window_id.append(atl::to_string(index));
        imgui::begin_window(*ctx, window_id);
        bool const is_active = imgui::is_window_active(*ctx);
        imgui::end_window(*ctx);
        return is_active;
    }

    static Entity pick_object(Ray const ray) {
        // gizmo::draw_line(ray.origin, ray.origin + 20 * ray.direction, Color::green, 200.0f);

        ECS& ecs = Editor::get_ecs();
        Resource_Manager<Mesh>& mesh_manager = Editor::get_mesh_manager();
        Component_View access = ecs.view<Static_Mesh_Component, Transform>();
        Entity selected = null_entity;
        Raycast_Hit closest_hit;
        closest_hit.distance = math::infinity;
        for(Entity const entity: access) {
            auto const& [c, transform] = access.get<Static_Mesh_Component, Transform>(entity);
            Mesh const& mesh = mesh_manager.get(c.mesh_handle);
            if(auto hit = intersect_ray_mesh(ray, mesh, transform.to_matrix()); hit && hit->distance < closest_hit.distance) {
                // gizmo::draw_point(hit.hit_point, 0.05, Color::red, 200.0f);
                closest_hit = hit.value();
                selected = entity;
            }
        }

        return selected;
    }

    static Matrix4 compute_rotation(Gizmo_Transform_Space const space, Matrix4 const base_rotation, Matrix4 const object_rotation) {
        switch(space) {
            case Gizmo_Transform_Space::world:
                return base_rotation;
            case Gizmo_Transform_Space::local:
                return base_rotation * object_rotation;
        }
    }

    void Viewport::process_actions(Matrix4 const view_mat, Matrix4 const inv_view_mat, Matrix4 const proj_mat, Matrix4 const inv_proj_mat,
                                   Transform const camera_transform, atl::Array<Entity>& selected_entities) {
        atl::String window_id{u8"viewport_window"};
        window_id.append(atl::to_string(index));
        imgui::begin_window(*ctx, window_id);

        Vector2 const viewport_size = imgui::get_window_dimensions(*ctx);
        Vector2 const _cursor_pos = imgui::get_cursor_position(*ctx);
        // Transform from top-left to bottom-left (screen coordinates).
        Vector2 const cursor_pos = {_cursor_pos.x, viewport_size.y - _cursor_pos.y};
        Ray const cursor_ray = screen_to_ray(inv_view_mat, inv_proj_mat, viewport_size.x, viewport_size.y, cursor_pos);

        imgui::end_window(*ctx);

        auto const state = input::get_key_state(Key::right_mouse_button);
        auto const shift_state = input::get_key_state(Key::left_shift);
        if(!state.down && state.up_down_transitioned) {
            Entity selected_entity = pick_object(cursor_ray);
            if(selected_entity != null_entity) {
                if(shift_state.down) {
                    bool already_selected = false;
                    if(selected_entities.size() != 0 && selected_entity == selected_entities[0]) {
                        already_selected = true;
                    } else {
                        // for (atl::Array<Entity>::size_type i = 1; i < selected_entities.size(); ++i) {
                        //     if (selected_entities[i] == selected_entity) {
                        //         editor_events::entity_deselected(selected_entity);
                        //         break;
                        //     }
                        // }
                    }

                    //     if (already_selected) {
                    //         editor_events::entity_deselected(selected_entity);
                    //     } else {
                    //         editor_events::entity_selected(selected_entity, false);
                    //     }
                    // } else {
                    //     editor_events::entity_selected(selected_entity, true);
                }
            }
        }

        if(selected_entities.size() > 0) {
            ECS& ecs = Editor::get_ecs();
            Transform& transform_ref = ecs.get_component<Transform>(selected_entities[0]);
            Transform const transform = transform_ref;

            input::Key_State const lmb_state = input::get_key_state(Key::left_mouse_button);
            Gizmo_Context& gizmo_ctx = get_gizmo_context();
            if(lmb_state.up_down_transitioned) {
                if(lmb_state.down) {
                    switch(gizmo_ctx.type) {
                        case Gizmo_Transform_Type::translate: {
                            // gizmo::debug_draw_line(cursor_ray.origin, cursor_ray.origin + cursor_ray.direction * 1000.0f, 100.0f);

                            Gizmo_Settings gizmo_settings = get_editor_preferences().gizmo_settings;
                            gizmo::Arrow_3D arrow{gizmo::Arrow_3D_Style::cone, {}, gizmo_settings.size};
                            Matrix4 const vp_mat = view_mat * proj_mat;
                            // TODO: Use global position.
                            Matrix4 const translation = math::translate(transform.local_position);
                            float distance = math::infinity;

                            // TODO: Use global object rotation for local gizmos.
                            Matrix4 const base_rotation_x = math::rotate_y(math::half_pi);
                            Matrix4 const rotation_x = compute_rotation(gizmo_ctx.space, base_rotation_x, math::rotate(transform.local_rotation));
                            if(atl::Optional<float> const arrow_distance =
                                   gizmo::intersect_arrow_3d(cursor_ray, arrow, rotation_x * translation, vp_mat, viewport_size);
                               arrow_distance && *arrow_distance < distance) {
                                distance = *arrow_distance;
                                gizmo_ctx.grab.grabbed = true;
                                gizmo_ctx.grab.cached_transform = transform;
                                gizmo_ctx.grab.grabbed_axis = Vector3(Vector4{0.0f, 0.0f, -1.0f, 0.0f} * rotation_x);
                            }

                            Matrix4 const base_rotation_y = math::rotate_x(-math::half_pi);
                            Matrix4 const rotation_y = compute_rotation(gizmo_ctx.space, base_rotation_y, math::rotate(transform.local_rotation));
                            if(atl::Optional<float> const arrow_distance =
                                   gizmo::intersect_arrow_3d(cursor_ray, arrow, rotation_y * translation, vp_mat, viewport_size);
                               arrow_distance && *arrow_distance < distance) {
                                distance = *arrow_distance;
                                gizmo_ctx.grab.grabbed = true;
                                gizmo_ctx.grab.cached_transform = transform;
                                gizmo_ctx.grab.grabbed_axis = Vector3(Vector4{0.0f, 0.0f, -1.0f, 0} * rotation_y);
                            }

                            Matrix4 const base_rotation_z = math::rotate_y(math::pi);
                            Matrix4 const rotation_z = compute_rotation(gizmo_ctx.space, base_rotation_z, math::rotate(transform.local_rotation));
                            if(atl::Optional<float> const arrow_distance =
                                   gizmo::intersect_arrow_3d(cursor_ray, arrow, rotation_z * translation, vp_mat, viewport_size);
                               arrow_distance && *arrow_distance < distance) {
                                distance = *arrow_distance;
                                gizmo_ctx.grab.grabbed = true;
                                gizmo_ctx.grab.cached_transform = transform;
                                gizmo_ctx.grab.grabbed_axis = Vector3(Vector4{0.0f, 0.0f, -1.0f, 0.0f} * rotation_z);
                            }

                            if(gizmo_ctx.grab.grabbed) {
                                Vector3 camera_pos_projected_on_translation_axis =
                                    math::dot(gizmo_ctx.grab.grabbed_axis, camera_transform.local_position) * gizmo_ctx.grab.grabbed_axis;
                                gizmo_ctx.grab.plane_normal = math::normalize(camera_transform.local_position - camera_pos_projected_on_translation_axis);
                                gizmo_ctx.grab.plane_distance = math::dot(transform.local_position, gizmo_ctx.grab.plane_normal);
                                gizmo_ctx.grab.mouse_grab_point =
                                    intersect_line_plane({cursor_ray.origin, cursor_ray.direction}, gizmo_ctx.grab.plane_normal, gizmo_ctx.grab.plane_distance)
                                        ->hit_point;
                            }
                        } break;
                        case Gizmo_Transform_Type::rotate: {
                        } break;
                        case Gizmo_Transform_Type::scale: {
                            Gizmo_Settings gizmo_settings = get_editor_preferences().gizmo_settings;
                            gizmo::Arrow_3D arrow{gizmo::Arrow_3D_Style::cube, {}, gizmo_settings.size};
                            Matrix4 const vp_mat = view_mat * proj_mat;
                            // TODO: Use global position.
                            Matrix4 const translation = math::translate(transform.local_position);
                            float distance = math::infinity;

                            // TODO: Use global object rotation for local gizmos.
                            Matrix4 const base_rotation_x = math::rotate_y(math::half_pi);
                            Matrix4 const rotation_x = compute_rotation(gizmo_ctx.space, base_rotation_x, math::rotate(transform.local_rotation));
                            if(atl::Optional<float> const arrow_distance =
                                   gizmo::intersect_arrow_3d(cursor_ray, arrow, rotation_x * translation, vp_mat, viewport_size);
                               arrow_distance && *arrow_distance < distance) {
                                distance = *arrow_distance;
                                gizmo_ctx.grab.grabbed = true;
                                gizmo_ctx.grab.cached_transform = transform;
                                gizmo_ctx.grab.grabbed_axis = Vector3(Vector4{0.0f, 0.0f, -1.0f, 0.0f} * rotation_x);
                            }

                            Matrix4 const base_rotation_y = math::rotate_x(-math::half_pi);
                            Matrix4 const rotation_y = compute_rotation(gizmo_ctx.space, base_rotation_y, math::rotate(transform.local_rotation));
                            if(atl::Optional<float> const arrow_distance =
                                   gizmo::intersect_arrow_3d(cursor_ray, arrow, rotation_y * translation, vp_mat, viewport_size);
                               arrow_distance && *arrow_distance < distance) {
                                distance = *arrow_distance;
                                gizmo_ctx.grab.grabbed = true;
                                gizmo_ctx.grab.cached_transform = transform;
                                gizmo_ctx.grab.grabbed_axis = Vector3(Vector4(0, 0, -1, 0) * rotation_y);
                            }

                            Matrix4 const base_rotation_z = math::rotate_y(math::pi);
                            Matrix4 const rotation_z = compute_rotation(gizmo_ctx.space, base_rotation_z, math::rotate(transform.local_rotation));
                            if(atl::Optional<float> const arrow_distance =
                                   gizmo::intersect_arrow_3d(cursor_ray, arrow, rotation_z * translation, vp_mat, viewport_size);
                               arrow_distance && *arrow_distance < distance) {
                                distance = *arrow_distance;
                                gizmo_ctx.grab.grabbed = true;
                                gizmo_ctx.grab.cached_transform = transform;
                                gizmo_ctx.grab.grabbed_axis = Vector3(Vector4(0, 0, -1, 0) * rotation_z);
                            }

                            if(gizmo_ctx.grab.grabbed) {
                                Vector3 camera_pos_projected_on_translation_axis =
                                    math::dot(gizmo_ctx.grab.grabbed_axis, camera_transform.local_position) * gizmo_ctx.grab.grabbed_axis;
                                gizmo_ctx.grab.plane_normal = math::normalize(camera_transform.local_position - camera_pos_projected_on_translation_axis);
                                gizmo_ctx.grab.plane_distance = math::dot(transform.local_position, gizmo_ctx.grab.plane_normal);
                                gizmo_ctx.grab.mouse_grab_point =
                                    intersect_line_plane({cursor_ray.origin, cursor_ray.direction}, gizmo_ctx.grab.plane_normal, gizmo_ctx.grab.plane_distance)
                                        ->hit_point;
                            }
                        } break;
                    }
                } else {
                    gizmo_ctx.grab.grabbed = false;
                }
            } else {
                if(lmb_state.down && gizmo_ctx.grab.grabbed) {
                    // TODO: Choose space in which to modify the transform based on gizmo context.
                    switch(gizmo_ctx.type) {
                        case Gizmo_Transform_Type::translate: {
                            Vector3 intersection =
                                intersect_line_plane({cursor_ray.origin, cursor_ray.direction}, gizmo_ctx.grab.plane_normal, gizmo_ctx.grab.plane_distance)
                                    ->hit_point;
                            Vector3 delta_position =
                                math::dot(intersection - gizmo_ctx.grab.mouse_grab_point, gizmo_ctx.grab.grabbed_axis) * gizmo_ctx.grab.grabbed_axis;
                            //for (Entity const entity : selected_entities) {
                            //    ecs.get_component<Transform>(entity);
                            //}
                            transform_ref.local_position = gizmo_ctx.grab.cached_transform.local_position + delta_position;
                        } break;
                        case Gizmo_Transform_Type::rotate: {
                        } break;
                        case Gizmo_Transform_Type::scale: {
                            Vector3 intersection =
                                intersect_line_plane({cursor_ray.origin, cursor_ray.direction}, gizmo_ctx.grab.plane_normal, gizmo_ctx.grab.plane_distance)
                                    ->hit_point;
                            Vector3 delta_position =
                                math::dot(intersection - gizmo_ctx.grab.mouse_grab_point, gizmo_ctx.grab.grabbed_axis) * gizmo_ctx.grab.grabbed_axis;
                            //for (Entity const entity : selected_entities) {
                            //    ecs.get_component<Transform>(entity);
                            //}
                            transform_ref.local_scale = gizmo_ctx.grab.cached_transform.local_scale + delta_position;
                        } break;
                    }
                }
            }
        }
    }

    // Renders outline data to framebuffer
    // Leaves the rendered scene with outlines in renderer's front postprocess framebuffer's 1st color attachment
    static void draw_outlines(rendering::Renderer& renderer, Framebuffer& framebuffer, atl::Array<Entity> const& selected_entities, Matrix4 const view,
                              Matrix4 const projection, Color const outline_color) {
        Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_3d);
        uniform_color_shader.use();
        uniform_color_shader.set_vec4("color", outline_color);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Resource_Manager<Mesh>& mesh_manager = Editor::get_mesh_manager();
        // ECS& ecs = Editor::get_ecs();
        // for (Entity const entity: selected_entities) {
        //     // TODO: Not all selected entities have mesh component.
        //     // Should we enforce some sort of mesh component in editor mode?
        //     auto const [transform, static_mesh] = ecs.try_get_component<Transform, Static_Mesh_Component>(entity);
        //     if (transform && static_mesh) {
        //         Mesh const& mesh = mesh_manager.get(static_mesh->mesh_handle);
        //         Matrix4 const mvp_mat = transform->to_matrix() * view * projection;
        //         uniform_color_shader.set_matrix4("mvp_mat", mvp_mat);
        //         rendering::render_mesh(mesh);
        //     }
        // }

        glBindTextureUnit(0, renderer.postprocess_front_buffer->get_color_texture(0));
        glBindTextureUnit(1, framebuffer.get_color_texture(0));
        Shader& outline_mix_shader = get_builtin_shader(Builtin_Editor_Shader::outline_mix);
        outline_mix_shader.use();
        bind_framebuffer(renderer.postprocess_back_buffer);
        rendering::render_texture_quad();
        renderer.swap_postprocess_buffers();
    }

    static void draw_gizmo(Ray const cursor_ray, Vector3 const camera_pos, Matrix4 const vp_mat, Vector2 const viewport_size,
                           atl::Slice<Entity const> selected_entities) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // gizmo::debug_commit_draw_lines(vp_mat, camera_pos, timing::get_delta_time());
        // TODO: That's terrible
        if(selected_entities.size() > 0) {
            ECS& ecs = Editor::get_ecs();
            Transform& transform_ref = ecs.get_component<Transform>(selected_entities[0]);
            Matrix4 const offset = math::translate(transform_ref.local_position);
            Matrix4 const object_rotation = math::rotate(transform_ref.local_rotation);
            Gizmo_Settings const gizmo_settings = get_editor_preferences().gizmo_settings;
            Gizmo_Context const gizmo_ctx = get_gizmo_context();

            // TODO: Use global transform instead of local.
            // TODO: Choose order in which to render the handles based on the camera angle, so that alpha blending works correctly.
            switch(gizmo_ctx.type) {
                case Gizmo_Transform_Type::translate: {
                    Color axis_colors[3] = {gizmo_settings.axis_x_color, gizmo_settings.axis_y_color, gizmo_settings.axis_z_color};
                    Matrix4 const rotations[3] = {math::rotate_y(math::half_pi), math::rotate_x(-math::half_pi), math::rotate_y(math::pi)};
                    Matrix4 const transforms[3] = {compute_rotation(gizmo_ctx.space, rotations[0], object_rotation) * offset,
                                                   compute_rotation(gizmo_ctx.space, rotations[1], object_rotation) * offset,
                                                   compute_rotation(gizmo_ctx.space, rotations[2], object_rotation) * offset};

                    if(!gizmo_ctx.grab.grabbed) {
                        gizmo::Arrow_3D const arrow = {gizmo::Arrow_3D_Style::cone, Color::white, gizmo_settings.size};
                        atl::Optional<float> const hits[3] = {gizmo::intersect_arrow_3d(cursor_ray, arrow, transforms[0], vp_mat, viewport_size),
                                                              gizmo::intersect_arrow_3d(cursor_ray, arrow, transforms[1], vp_mat, viewport_size),
                                                              gizmo::intersect_arrow_3d(cursor_ray, arrow, transforms[2], vp_mat, viewport_size)};
                        if(hits[0] || hits[1] || hits[2]) {
                            auto value_or_infty = [](atl::Optional<float> const& hit) { return (hit ? hit.value() : math::infinity); };
                            if(value_or_infty(hits[0]) < value_or_infty(hits[1]) && value_or_infty(hits[0]) < value_or_infty(hits[2])) {
                                axis_colors[0].a = 1.0f;
                            } else if(value_or_infty(hits[1]) < value_or_infty(hits[0]) && value_or_infty(hits[1]) < value_or_infty(hits[2])) {
                                axis_colors[1].a = 1.0f;
                            } else {
                                axis_colors[2].a = 1.0f;
                            }
                        }
                    }

                    for(i32 i = 0; i < 3; ++i) {
                        gizmo::Arrow_3D const arrow = {gizmo::Arrow_3D_Style::cone, axis_colors[i], gizmo_settings.size};
                        gizmo::draw_arrow_3d(arrow, transforms[i], vp_mat, viewport_size, camera_pos);
                    }
                } break;

                case Gizmo_Transform_Type::rotate: {
                    Color axis_colors[3] = {gizmo_settings.axis_x_color, gizmo_settings.axis_y_color, gizmo_settings.axis_z_color};
                    Matrix4 const rotations[3] = {math::rotate_y(math::half_pi), math::rotate_x(-math::half_pi), math::rotate_y(math::pi)};
                    Matrix4 const transforms[3] = {compute_rotation(gizmo_ctx.space, rotations[0], object_rotation) * offset,
                                                   compute_rotation(gizmo_ctx.space, rotations[1], object_rotation) * offset,
                                                   compute_rotation(gizmo_ctx.space, rotations[2], object_rotation) * offset};

                    if(!gizmo_ctx.grab.grabbed) {
                        gizmo::Dial_3D const dial = {Color::white, gizmo_settings.size};
                        atl::Optional<float> const hits[3] = {gizmo::intersect_dial_3d(cursor_ray, dial, transforms[0], vp_mat, viewport_size),
                                                              gizmo::intersect_dial_3d(cursor_ray, dial, transforms[1], vp_mat, viewport_size),
                                                              gizmo::intersect_dial_3d(cursor_ray, dial, transforms[2], vp_mat, viewport_size)};
                        if(hits[0] || hits[1] || hits[2]) {
                            auto value_or_infty = [](atl::Optional<float> const& hit) { return (hit ? hit.value() : math::infinity); };
                            if(value_or_infty(hits[0]) < value_or_infty(hits[1]) && value_or_infty(hits[0]) < value_or_infty(hits[2])) {
                                axis_colors[0].a = 1.0f;
                            } else if(value_or_infty(hits[1]) < value_or_infty(hits[0]) && value_or_infty(hits[1]) < value_or_infty(hits[2])) {
                                axis_colors[1].a = 1.0f;
                            } else {
                                axis_colors[2].a = 1.0f;
                            }
                        }
                    }

                    for(i32 i = 0; i < 3; ++i) {
                        gizmo::Dial_3D const dial = {axis_colors[i], gizmo_settings.size};
                        gizmo::draw_dial_3d(dial, transforms[i], vp_mat, camera_pos, viewport_size);
                    }
                } break;

                case Gizmo_Transform_Type::scale: {
                    Color axis_colors[3] = {gizmo_settings.axis_x_color, gizmo_settings.axis_y_color, gizmo_settings.axis_z_color};
                    Matrix4 const rotations[3] = {math::rotate_y(math::half_pi), math::rotate_x(-math::half_pi), math::rotate_y(math::pi)};
                    Matrix4 const transforms[3] = {compute_rotation(gizmo_ctx.space, rotations[0], object_rotation) * offset,
                                                   compute_rotation(gizmo_ctx.space, rotations[1], object_rotation) * offset,
                                                   compute_rotation(gizmo_ctx.space, rotations[2], object_rotation) * offset};

                    if(!gizmo_ctx.grab.grabbed) {
                        gizmo::Arrow_3D const arrow = {gizmo::Arrow_3D_Style::cube, Color::white, gizmo_settings.size};
                        atl::Optional<float> const hits[3] = {gizmo::intersect_arrow_3d(cursor_ray, arrow, transforms[0], vp_mat, viewport_size),
                                                              gizmo::intersect_arrow_3d(cursor_ray, arrow, transforms[1], vp_mat, viewport_size),
                                                              gizmo::intersect_arrow_3d(cursor_ray, arrow, transforms[2], vp_mat, viewport_size)};
                        if(hits[0] || hits[1] || hits[2]) {
                            auto value_or_infty = [](atl::Optional<float> const& hit) { return (hit ? hit.value() : math::infinity); };
                            if(value_or_infty(hits[0]) < value_or_infty(hits[1]) && value_or_infty(hits[0]) < value_or_infty(hits[2])) {
                                axis_colors[0].a = 1.0f;
                            } else if(value_or_infty(hits[1]) < value_or_infty(hits[0]) && value_or_infty(hits[1]) < value_or_infty(hits[2])) {
                                axis_colors[1].a = 1.0f;
                            } else {
                                axis_colors[2].a = 1.0f;
                            }
                        }
                    }

                    for(i32 i = 0; i < 3; ++i) {
                        gizmo::Arrow_3D const arrow = {gizmo::Arrow_3D_Style::cube, axis_colors[i], gizmo_settings.size};
                        gizmo::draw_arrow_3d(arrow, transforms[i], vp_mat, viewport_size, camera_pos);
                    }
                } break;
            }
        }
        glDisable(GL_BLEND);
    }

    static u64 get_cube_handle() {
        Mesh cube = generate_cube();
        return rendering::write_persistent_geometry(cube.vertices, cube.indices);
    }

    static u32 load_cube_map() {
        u32 texture;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGB8, 1, 1);
        unsigned char const pixel[] = {10, 10, 10};
        glTextureSubImage3D(texture, 0, 0, 0, 0, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTextureSubImage3D(texture, 0, 0, 0, 1, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTextureSubImage3D(texture, 0, 0, 0, 2, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTextureSubImage3D(texture, 0, 0, 0, 3, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTextureSubImage3D(texture, 0, 0, 0, 4, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        glTextureSubImage3D(texture, 0, 0, 0, 5, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        return texture;
    }

    static void draw_skybox(Matrix4 view_mat, Matrix4 const proj_mat) {
        // TODO: Temporary
        static u64 const cube_handle = get_cube_handle();
        static u32 const cube_map_texture = load_cube_map();

        Shader& skybox_shader = get_builtin_shader(Builtin_Shader::skybox);
        skybox_shader.use();
        view_mat[3][0] = 0.0f;
        view_mat[3][1] = 0.0f;
        view_mat[3][2] = 0.0f;
        skybox_shader.set_matrix4("vp_mat", view_mat * proj_mat);
        glBindTextureUnit(0, cube_map_texture);
        rendering::bind_persistent_geometry_buffers();
        rendering::add_draw_command(rendering::Draw_Persistent_Geometry_Command{(u32)cube_handle, 1, 0});
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        rendering::commit_draw();
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
    }

    static void draw_grid(Matrix4 const view_proj_mat, Vector3 const camera_pos, Camera const camera, Vector2 const viewport_size) {
        Shader& grid_shader = get_builtin_shader(Builtin_Editor_Shader::grid);
        grid_shader.use();
        Matrix4 const model_mat = math::rotate_x(math::half_pi) * math::scale(camera.far_plane) * math::translate({camera_pos.x, 0.0f, camera_pos.z});
        grid_shader.set_matrix4("model_mat", model_mat);
        grid_shader.set_matrix4("vp_mat", view_proj_mat);
        grid_shader.set_vec3("camera_pos", camera_pos);
        grid_shader.set_vec2("rcp_res", {1.0f / viewport_size.x, 1.0f / viewport_size.y});
        Grid_Settings const grid = get_editor_preferences().grid_settings;
        grid_shader.set_uint("grid_flags", grid.grid_flags);
        grid_shader.set_vec4("axis_x_color", grid.axis_x_color);
        grid_shader.set_vec4("axis_z_color", grid.axis_z_color);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        // TODO: That's probably the worst thing I've ever written.
        //       Find a way to render adhoc geometry.
        rendering::bind_mesh_vao();
        rendering::render_texture_quad();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

    void Viewport::render(Matrix4 const view_mat, Matrix4 const inv_view_mat, Matrix4 const proj_mat, Matrix4 const inv_proj_mat, Camera const camera,
                          Transform const camera_transform, atl::Slice<Entity const> const selected_entities) {
        // TODO: Mist instead of sudden clip

        atl::String window_id{u8"viewport_window"};
        window_id.append(atl::to_string(index));
        imgui::begin_window(*ctx, window_id);

        Vector2 const viewport_size = imgui::get_window_dimensions(*ctx);
        Vector2 const _cursor_pos = imgui::get_cursor_position(*ctx);
        // Transform from top-left to bottom-left (screen coordinates).
        Vector2 const cursor_pos = {_cursor_pos.x, viewport_size.y - _cursor_pos.y};
        Ray const cursor_ray = screen_to_ray(inv_view_mat, inv_proj_mat, viewport_size.x, viewport_size.y, cursor_pos);

        // TODO fix this shitcode
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, viewport_size.x, viewport_size.y);
        bind_framebuffer(deferred_framebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ECS& ecs = Editor::get_ecs();
        ECS snapshot = ecs.snapshot<Transform, Static_Mesh_Component>();
        rendering::render_scene(snapshot, camera_transform, view_mat, proj_mat);

        bind_framebuffer(multisampled_framebuffer);
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindTextureUnit(0, deferred_framebuffer->get_depth_texture());
        glBindTextureUnit(1, deferred_framebuffer->get_color_texture(0));
        glBindTextureUnit(2, deferred_framebuffer->get_color_texture(1));

        Shader& deferred_shading = get_builtin_shader(Builtin_Shader::deferred_shading);
        deferred_shading.use();
        deferred_shading.set_vec3("camera.position", camera_transform.local_position);
        deferred_shading.set_vec2("viewport_size", viewport_size);
        deferred_shading.set_matrix4("inv_view_mat", inv_view_mat);
        deferred_shading.set_matrix4("inv_proj_mat", inv_proj_mat);
        rendering::render_texture_quad();
        // TODO: Vertex buffers rebind after call to render_texture_quad
        rendering::bind_transient_geometry_buffers();

        // TODO: Change camera position from local to global.
        draw_skybox(view_mat, proj_mat);
        draw_grid(view_mat * proj_mat, camera_transform.local_position, camera, framebuffer->size());
        // TODO: Draw outlines here.
        glDisable(GL_DEPTH_TEST);
        draw_gizmo(cursor_ray, camera_transform.local_position, view_mat * proj_mat, framebuffer->size(), selected_entities);
        bind_framebuffer(framebuffer, Framebuffer::draw);
        blit_framebuffer(framebuffer, multisampled_framebuffer, opengl::color_buffer_bit);

        bind_framebuffer(back_framebuffer);
        glBindTextureUnit(0, framebuffer->get_color_texture(0));
        Shader& gamma_correction_shader = get_builtin_shader(Builtin_Shader::gamma_correction);
        gamma_correction_shader.use();
        gamma_correction_shader.set_float("gamma", 1 / 2.2f);
        rendering::bind_mesh_vao();
        rendering::render_texture_quad();

        atl::swap(front_framebuffer, back_framebuffer);

        imgui::image(*ctx, front_framebuffer->get_color_texture(0), front_framebuffer->size(), Vector2{0.0f, 1.0f}, Vector2{1.0f, 0.0f});
        imgui::end_window(*ctx);
    }
} // namespace anton_engine
