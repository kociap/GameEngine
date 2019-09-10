#include <viewport.hpp>

#include <anton_stl/vector.hpp>
#include <assets.hpp>
#include <components/camera.hpp>
#include <components/static_mesh_component.hpp>
#include <components/transform.hpp>
#include <debug_macros.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>
#include <framebuffer.hpp>
#include <gizmo.hpp>
#include <gizmo_internal.hpp>
#include <glad.hpp>
#include <input/input.hpp>
#include <input/input_core.hpp>
#include <intersection_tests.hpp>
#include <line.hpp>
#include <math/vector2.hpp>
#include <mesh/mesh.hpp>
#include <obb.hpp>
#include <qt_key.hpp>
#include <renderer.hpp>
#include <renderer_internal.hpp>
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
        GE_log("Resize event");
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
            GE_log("Non-cursor-locked mouse event");
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
                                Q_EMIT entity_deselected(selected_entity);
                                break;
                            }
                        }
                    }

                    if (already_selected) {
                        Q_EMIT entity_deselected(selected_entity);
                    } else {
                        Q_EMIT entity_selected(selected_entity, false);
                    }
                } else {
                    Q_EMIT entity_selected(selected_entity, true);
                }
            }
        }

        static int gizmo_transform_space = 0;
        /*if (imgui::BeginCombo("Space", space_prev_val)) {
            if (imgui::Selectable("World")) {
                gizmo_transform_space = 0;
                space_prev_val = "World";
            }
            if (imgui::Selectable("Local")) {
                gizmo_transform_space = 1;
                space_prev_val = "Local";
            }
            imgui::EndCombo();
        }*/

        //static int gizmo_type = 0;
        /*if (imgui::BeginCombo("Type", type_prev_val)) {
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
        }*/

        Color axis_blue = {15.f / 255.f, 77.f / 255.f, 186.f / 255.f};
        Color axis_green = {0, 220.0f / 255.0f, 0};
        Color axis_red = {179.f / 255.f, 20.f / 255.f, 5.f / 255.f};
        // Main world axes
        gizmo::draw_line({-10.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}, axis_red);
        gizmo::draw_line({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 10.0f}, axis_blue);

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
                        Vector3 delta_position = math::dot(intersection - gizmo_mouse_grab_point, gizmo_grabbed_axis) * gizmo_grabbed_axis;
                        //for (Entity const entity : selected_entities) {
                        //    ecs.get_component<Transform>(entity);
                        //}
                        transform_ref.local_position = cached_gizmo_transform.local_position + delta_position;
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
    }

    // Leaves the rendered scene with outlines in renderer's front postprocess framebuffer's 1st color attachment
    static void draw_outlines(rendering::Renderer* renderer, Framebuffer* framebuffer, uint32_t const scene_texture,
                              anton_stl::Vector<Entity> const& selected_entities, Matrix4 const view, Matrix4 const projection, Color const outline_color) {
        // copy depth buffer
        Framebuffer::bind(*renderer->framebuffer, Framebuffer::Bind_Mode::read);
        Framebuffer::bind(*framebuffer, Framebuffer::Bind_Mode::draw);
        Framebuffer::blit(*renderer->framebuffer, *framebuffer, opengl::Buffer_Mask::depth_buffer_bit);

        renderer->single_color_shader.use();
        renderer->single_color_shader.set_vec4("color", outline_color);
        renderer->single_color_shader.set_matrix4("view", view);
        renderer->single_color_shader.set_matrix4("projection", projection);

        ECS& ecs = Editor::get_ecs();
        Resource_Manager<Mesh>& mesh_manager = Editor::get_mesh_manager();
        Framebuffer::bind(*framebuffer);
        opengl::clear_color(0.0f, 0.0f, 0.0f, 0.0f);
        opengl::clear(opengl::Buffer_Mask::color_buffer_bit);
        for (Entity const entity: selected_entities) {
            // TODO: Not all selected entities have mesh component.
            // Should we enforce some sort of mesh component in editor mode?
            auto const [transform, static_mesh] = ecs.try_get_component<Transform, Static_Mesh_Component>(entity);
            if (transform && static_mesh) {
                Mesh const& mesh = mesh_manager.get(static_mesh->mesh_handle);
                renderer->single_color_shader.set_matrix4("model", transform->to_matrix());
                rendering::render_mesh(mesh);
            }
        }

        opengl::active_texture(0);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, scene_texture);
        opengl::active_texture(1);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, framebuffer->get_color_texture(0));
        renderer->outline_mix_shader.use();
        Framebuffer::bind(*renderer->postprocess_back_buffer);
        rendering::render_texture_quad();
        renderer->swap_postprocess_buffers();
    }

    static uint32_t draw_gizmo(rendering::Renderer* renderer, Framebuffer* framebuffer, Vector3 const camera_pos, Matrix4 const view,
                               Matrix4 const projection) {
        glEnable(GL_DEPTH_TEST);
        Framebuffer::bind(*renderer->postprocess_front_buffer, Framebuffer::Bind_Mode::read);
        Framebuffer::bind(*framebuffer, Framebuffer::Bind_Mode::draw);
        Framebuffer::blit(*renderer->postprocess_front_buffer, *framebuffer, opengl::Buffer_Mask::color_buffer_bit);
        Framebuffer::bind(*framebuffer);
        gizmo::draw(camera_pos, view, projection);
        glDisable(GL_DEPTH_TEST);
        return framebuffer->get_color_texture(0);
    }

    void Viewport::render(Matrix4 const view_mat, Matrix4 const proj_mat, Transform const camera_transform,
                          anton_stl::Vector<Entity> const& selected_entities) {
        if (!context->makeCurrent(windowHandle())) {
            GE_log("Could not make context current. Skipping rendering.");
            return;
        }

        // TODO fix this shitcode
        uint32_t texture = renderer->render_frame_as_texture(view_mat, proj_mat, camera_transform, width(), height());
        draw_outlines(renderer, framebuffer, texture, selected_entities, view_mat, proj_mat, {241.0f / 255.0f, 88.0f / 255.0f, 0.0f});
        // TODO change camera position from local to global
        texture = draw_gizmo(renderer, framebuffer, camera_transform.local_position, view_mat, proj_mat);
        opengl::bind_framebuffer(GL_FRAMEBUFFER, context->defaultFramebufferObject());
        opengl::active_texture(0);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, texture);
        renderer->gamma_correction_shader.use();
        // TODO vao swap
        opengl::bind_vertex_array(renderer->mesh_vao);
        rendering::render_texture_quad();
        context->swapBuffers(windowHandle());
    }
} // namespace anton_engine
