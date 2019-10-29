#include <viewport.hpp>

#include <anton_stl/vector.hpp>
#include <assets.hpp>
#include <builtin_editor_shaders.hpp>
#include <builtin_shaders.hpp>
#include <components/camera.hpp>
#include <components/static_mesh_component.hpp>
#include <components/transform.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>
#include <editor_events.hpp>
#include <editor_preferences.hpp>
#include <framebuffer.hpp>
#include <gizmo/arrow_3d.hpp>
#include <gizmo/dial_3d.hpp>
#include <glad.hpp>
#include <input/input.hpp>
#include <input/input_core.hpp>
#include <intersection_tests.hpp>
#include <line.hpp>
#include <logging.hpp>
#include <math/transform.hpp>
#include <math/vector2.hpp>
#include <mesh.hpp>
#include <obb.hpp>
#include <qt_key.hpp>
#include <renderer.hpp>
#include <resource_manager.hpp>
#include <shader_file.hpp>
#include <viewport_camera.hpp>

ANTON_DISABLE_WARNINGS();
#include <QCloseEvent>
#include <QEvent>
#include <QMouseEvent>

#include <QCursor>
#include <QObject>
#include <QOpenGLContext>
#include <QWindow>
ANTON_RESTORE_WARNINGS();

#include <cstdint>
#include <tuple>

namespace anton_engine {
    Viewport::Viewport(int32_t vindex, QOpenGLContext* ctx): Viewport(vindex, nullptr, ctx) {}
    Viewport::Viewport(int32_t vindex, QWidget* parent, QOpenGLContext* ctx): QWidget(parent), context(ctx), index(vindex) {
        // Force native window and direct drawing, so that we have a surface to render onto
        setAttribute(Qt::WA_NativeWindow, true);
        setAttribute(Qt::WA_PaintOnScreen, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        setMinimumWidth(700);
        setMinimumHeight(700);

        setWindowTitle("Viewport");

        int32_t const w = width();
        int32_t const h = height();
        context->makeCurrent(windowHandle());
        Framebuffer::Construct_Info construct_info;
        construct_info.color_buffers.resize(1);
        construct_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgba8;
        construct_info.depth_buffer.enabled = true;
        construct_info.width = w;
        construct_info.height = h;
        framebuffer = new Framebuffer(construct_info);
        renderer = new rendering::Renderer(w, h);

        ECS& ecs = Editor::get_ecs();
        auto [entity, viewport_camera, camera, transform] = ecs.create<Viewport_Camera, Camera, Transform>();
        viewport_entity = entity;
        viewport_camera.viewport_index = index;
    }

    Viewport::~Viewport() {
        ECS& ecs = Editor::get_ecs();
        ecs.destroy(viewport_entity);
        delete framebuffer;
        delete renderer;
    }

    void Viewport::resize(int32_t const w, int32_t const h) {
        renderer->resize(w, h);
        framebuffer->resize(w, h);
        QWidget::resize(w, h);
    }

    QPaintEngine* Viewport::paintEngine() const {
        return nullptr;
    }

    bool Viewport::is_cursor_locked() const {
        return cursor_locked;
    }

    void Viewport::lock_cursor_at(int32_t x, int32_t y) {
        cursor_locked = true;
        lock_pos_x = x;
        lock_pos_y = y;
        QCursor::setPos(x, y);
        setCursor(Qt::BlankCursor);
        setMouseTracking(true);
    }

    void Viewport::unlock_cursor() {
        setMouseTracking(false);
        setCursor(Qt::ArrowCursor);
        cursor_locked = false;
    }

    void Viewport::closeEvent(QCloseEvent* e) {
        QWidget::closeEvent(e);
        Q_EMIT window_closed(index);
    }

    void Viewport::resizeEvent(QResizeEvent*) {
        ANTON_LOG_INFO("Resize event");
        int32_t w = width();
        int32_t h = height();
        resize(w, h);
    }

    void Viewport::mouseMoveEvent(QMouseEvent* mouse_event) {
        if (cursor_locked) {
            Input::Manager& input_manager = Editor::get_input_manager();
            int32_t delta_x = mouse_event->globalX() - lock_pos_x;
            int32_t delta_y = lock_pos_y - mouse_event->globalY();
            input_manager.add_event(Input::Mouse_Event(delta_x, delta_y, 0.0f));
            QCursor::setPos(lock_pos_x, lock_pos_y);
        } else {
            // ANTON_LOG_INFO("Non-cursor-locked mouse event");
        }
    }

    void Viewport::mousePressEvent(QMouseEvent* mouse_event) {
        Q_EMIT made_active(index);
        Input::Manager& input_manager = Editor::get_input_manager();
        Key key = key_from_qt_mouse_button(mouse_event->button());
        input_manager.add_event(Input::Event(key, mouse_event->type() == QEvent::MouseButtonPress));
    }

    static Entity pick_object(Ray const ray) {
        // gizmo::draw_line(ray.origin, ray.origin + 20 * ray.direction, Color::green, 200.0f);

        ECS& ecs = Editor::get_ecs();
        Resource_Manager<Mesh>& mesh_manager = Editor::get_mesh_manager();
        Component_View access = ecs.view<Static_Mesh_Component, Transform>();
        Entity selected = null_entity;
        Raycast_Hit closest_hit;
        closest_hit.distance = math::constants::infinity;
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

    // static void draw_wireframe_cuboid(Vector3 pos, Vector3 x, Vector3 y, Vector3 z) {
    //     gizmo::draw_line(pos - x + y - z, pos - x - y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    //     gizmo::draw_line(pos + x + y - z, pos + x - y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    //     gizmo::draw_line(pos + x + y - z, pos - x + y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    //     gizmo::draw_line(pos + x - y - z, pos - x - y - z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    //     gizmo::draw_line(pos - x + y + z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    //     gizmo::draw_line(pos + x + y + z, pos + x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    //     gizmo::draw_line(pos + x + y + z, pos - x + y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    //     gizmo::draw_line(pos + x - y + z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    //     gizmo::draw_line(pos - x + y - z, pos - x + y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    //     gizmo::draw_line(pos + x + y - z, pos + x + y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);

    //     gizmo::draw_line(pos - x - y - z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    //     gizmo::draw_line(pos + x - y - z, pos - x - y + z, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f}, 1.0f, 0.0f, false);
    // }

    void Viewport::process_actions(Matrix4 const view_mat, Matrix4 const projection_mat, Matrix4 const inv_view_mat, Matrix4 const inv_projection_mat,
                                   Transform const camera_transform, anton_stl::Vector<Entity>& selected_entities) {
        int32_t const window_content_size_x = width();
        int32_t const window_content_size_y = height();
        QPoint qcursor_pos = mapFromGlobal(QCursor::pos()); // TODO choose screen
        // Transform from top-left to bottom-left
        Vector2 mouse_pos(qcursor_pos.x(), height() - qcursor_pos.y());

        auto const state = Input::get_key_state(Key::right_mouse_button);
        auto const shift_state = Input::get_key_state(Key::left_shift);
        Ray const ray = screen_to_ray(inv_view_mat, inv_projection_mat, window_content_size_x, window_content_size_y, mouse_pos);
        if (!state.down && state.up_down_transitioned) {
            Entity selected_entity = pick_object(ray);
            if (selected_entity != null_entity) {
                if (shift_state.down) {
                    bool already_selected = false;
                    if (selected_entities.size() != 0 && selected_entity == selected_entities[0]) {
                        already_selected = true;
                    } else {
                        for (anton_stl::Vector<Entity>::size_type i = 1; i < selected_entities.size(); ++i) {
                            if (selected_entities[i] == selected_entity) {
                                editor_events::entity_deselected(selected_entity);
                                break;
                            }
                        }
                    }

                    if (already_selected) {
                        editor_events::entity_deselected(selected_entity);
                    } else {
                        editor_events::entity_selected(selected_entity, false);
                    }
                } else {
                    editor_events::entity_selected(selected_entity, true);
                }
            }
        }

        int gizmo_transform_space = 0;

        Color axis_blue = {15.f / 255.f, 77.f / 255.f, 186.f / 255.f};
        Color axis_green = {0, 220.0f / 255.0f, 0};
        Color axis_red = {179.f / 255.f, 20.f / 255.f, 5.f / 255.f};

        if (selected_entities.size() > 0) {
            ECS& ecs = Editor::get_ecs();
            Transform& transform_ref = ecs.get_component<Transform>(selected_entities.front());
            Transform transform = transform_ref;

            // TODO Have to somehow make those truly fixed size (window_size independent)
            float w_comp = (Vector4(0, 0, 0, 1) * transform.to_matrix() * view_mat * projection_mat).w;
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
                        float distance = math::constants::infinity;
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
                        Vector3 delta_position = math::dot(intersection - gizmo_mouse_grab_point, gizmo_grabbed_axis) * gizmo_grabbed_axis;
                        //for (Entity const entity : selected_entities) {
                        //    ecs.get_component<Transform>(entity);
                        //}
                        transform_ref.local_position = cached_gizmo_transform.local_position + delta_position;
                    }
                }

                // draw_wireframe_cuboid(obb[0].center - transform.local_position + transform_ref.local_position, obb[0].local_x * obb[0].halfwidths.x,
                //                       obb[0].local_y * obb[0].halfwidths.y, obb[0].local_z * obb[0].halfwidths.z);
                // draw_wireframe_cuboid(obb[1].center - transform.local_position + transform_ref.local_position, obb[1].local_x * obb[1].halfwidths.x,
                //                       obb[1].local_y * obb[1].halfwidths.y, obb[1].local_z * obb[1].halfwidths.z);
                // draw_wireframe_cuboid(obb[2].center - transform.local_position + transform_ref.local_position, obb[2].local_x * obb[2].halfwidths.x,
                //                       obb[2].local_y * obb[2].halfwidths.y, obb[2].local_z * obb[2].halfwidths.z);
            } else if (gizmo_transform_space == 1) {
                // rotation
            } else {
                // scale
                // TODO duplicated code
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
                        float distance = math::constants::infinity;
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

                // draw_wireframe_cuboid(obb[0].center - transform.local_position + transform_ref.local_position, obb[0].local_x * obb[0].halfwidths.x,
                //                       obb[0].local_y * obb[0].halfwidths.y, obb[0].local_z * obb[0].halfwidths.z);
                // draw_wireframe_cuboid(obb[1].center - transform.local_position + transform_ref.local_position, obb[1].local_x * obb[1].halfwidths.x,
                //                       obb[1].local_y * obb[1].halfwidths.y, obb[1].local_z * obb[1].halfwidths.z);
                // draw_wireframe_cuboid(obb[2].center - transform.local_position + transform_ref.local_position, obb[2].local_x * obb[2].halfwidths.x,
                //                       obb[2].local_y * obb[2].halfwidths.y, obb[2].local_z * obb[2].halfwidths.z);
            }
        }
    }

    // Renders outline data to framebuffer
    // Leaves the rendered scene with outlines in renderer's front postprocess framebuffer's 1st color attachment
    static void draw_outlines(rendering::Renderer& renderer, Framebuffer& framebuffer, anton_stl::Vector<Entity> const& selected_entities, Matrix4 const view,
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

    static void draw_translate_handle(Color const color, Matrix4 const world_transform, Matrix4 const view, Matrix4 const projection, Vector3 const camera_pos,
                                      Vector2 const viewport_size) {
        // TODO: Hardcoded size of handles
        uint32_t const target_handle_size = 125;
        // gizmo::Arrow_3D arrow = {gizmo::Arrow_3D_Style::cone, color, target_handle_size};
        // gizmo::draw_arrow_3d(arrow, world_transform, vp_mat, viewport_size);
        gizmo::Dial_3D dial = {color, target_handle_size};
        gizmo::draw_dial_3d(dial, world_transform, view, projection, camera_pos, viewport_size);

        // float const handle_scale =
        //     compute_handle_scale(math::transform::translate(object_position), target_handle_size, view_projection_mat, 1 / viewport_size.y);
        // float const cone_height = handle_scale * 0.23f;
        // float const cone_radius = handle_scale * 0.07f;
        // Vector3 const forward_end = object_position + handle_scale * axis;
        // gizmo::draw_line(object_position, forward_end, color, 1.0f, 0.0f, false);
        // gizmo::draw_cone(forward_end, axis, cone_radius, 8, cone_height, color, 0.0f, false);
    }

    static void draw_translate_handles(Matrix4 const offset, Matrix4 const view, Matrix4 const projection, Vector3 const camera_pos,
                                       Vector2 const viewport_size) {
        // TODO: Add a way to customize the colors via settings?
        Color const axis_blue = {15.f / 255.f, 77.f / 255.f, 186.f / 255.f, 0.85f};
        Color const axis_green = {0, 220.0f / 255.0f, 0, 0.85f};
        Color const axis_red = {179.f / 255.f, 20.f / 255.f, 5.f / 255.f, 0.85f};
        // TODO: Renders only global space handles
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        draw_translate_handle(axis_blue, Matrix4({1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, -1, 0}, {0, 0, 0, 1}) * offset, view, projection, camera_pos,
                              viewport_size);
        draw_translate_handle(axis_green, Matrix4({1, 0, 0, 0}, {0, 0, 1, 0}, {0, -1, 0, 0}, {0, 0, 0, 1}) * offset, view, projection, camera_pos,
                              viewport_size);
        draw_translate_handle(axis_red, Matrix4({0, 0, 1, 0}, {0, 1, 0, 0}, {-1, 0, 0, 0}, {0, 0, 0, 1}) * offset, view, projection, camera_pos, viewport_size);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    static uint32_t draw_gizmo(rendering::Renderer* renderer, Framebuffer* framebuffer, Vector3 const camera_pos, Matrix4 const view, Matrix4 const projection,
                               anton_stl::Vector<Entity> const& selected_entities) {
        glEnable(GL_DEPTH_TEST);
        bind_framebuffer(renderer->postprocess_front_buffer, Framebuffer::read);
        bind_framebuffer(framebuffer, Framebuffer::draw);
        blit_framebuffer(renderer->postprocess_front_buffer, framebuffer, opengl::color_buffer_bit);
        bind_framebuffer(framebuffer);
        // TODO: That's terrible
        if (selected_entities.size() > 0) {
            ECS& ecs = Editor::get_ecs();
            Transform& transform_ref = ecs.get_component<Transform>(selected_entities.front());
            draw_translate_handles(math::transform::translate(transform_ref.local_position), view, projection, camera_pos, framebuffer->size());
        }
        return framebuffer->get_color_texture(0);
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
        Matrix4 const model_mat = math::transform::rotate_x(math::constants::half_pi) * math::transform::scale(camera.far_plane) *
                                  math::transform::translate({camera_pos.x, 0.0f, camera_pos.z});
        grid_shader.set_matrix4("model_mat", model_mat);
        grid_shader.set_matrix4("vp_mat", view_proj_mat);
        grid_shader.set_vec3("camera_pos", camera_pos);
        grid_shader.set_vec2("rcp_res", {1.0f / viewport_size.x, 1.0f / viewport_size.y});
        Grid_Settings grid = get_editor_preferences().grid_settings;
        grid_shader.set_uint("grid_flags", grid.grid_flags);
        grid_shader.set_vec4("axis_x_color", grid.axis_x_color);
        grid_shader.set_vec4("axis_z_color", grid.axis_z_color);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        // TODO: That's probably the worst thing I've ever written.
        //       Find a way to render adhoc geometry.
        rendering::bind_mesh_vao();
        rendering::render_texture_quad();
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

    void Viewport::render(Matrix4 const view_mat, Matrix4 const proj_mat, Camera const camera, Transform const camera_transform,
                          anton_stl::Vector<Entity> const& selected_entities) {
        // TODO: Repeatedly calling makeCurrent kills performance!!
        // if (!context->makeCurrent(windowHandle())) {
        //     ANTON_LOG_WARNING("Could not make context current. Skipping rendering.");
        //     return;
        // }

        // TODO: Mist instead of sudden clip

        // TODO fix this shitcode
        renderer->render_frame(view_mat, proj_mat, camera_transform, Vector2(width(), height()));

        // Copy depth and color attachment buffers
        bind_framebuffer(renderer->framebuffer, Framebuffer::read);
        bind_framebuffer(framebuffer, Framebuffer::draw);
        blit_framebuffer(framebuffer, renderer->framebuffer, opengl::depth_buffer_bit);
        bind_framebuffer(renderer->postprocess_front_buffer, Framebuffer::read);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        blit_framebuffer(framebuffer, renderer->postprocess_front_buffer, opengl::color_buffer_bit);
        bind_framebuffer(framebuffer);
        // TODO change camera position from local to global
        rendering::bind_vertex_buffers();
        draw_skybox(view_mat, proj_mat);
        draw_grid(view_mat * proj_mat, camera_transform.local_position, camera, framebuffer->size());
        bind_framebuffer(renderer->postprocess_front_buffer, Framebuffer::draw);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        blit_framebuffer(renderer->postprocess_front_buffer, framebuffer, opengl::color_buffer_bit);

        u32 texture = draw_gizmo(renderer, framebuffer, camera_transform.local_position, view_mat, proj_mat, selected_entities);
        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, context->defaultFramebufferObject());
        glDisable(GL_FRAMEBUFFER_SRGB);
        glBindTextureUnit(0, texture);
        Shader& gamma_correction_shader = get_builtin_shader(Builtin_Shader::gamma_correction);
        gamma_correction_shader.use();
        gamma_correction_shader.set_float("gamma", 1 / 2.2f);
        rendering::bind_mesh_vao();
        rendering::render_texture_quad();
        context->swapBuffers(windowHandle());
    }
} // namespace anton_engine
