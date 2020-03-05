#include <level_editor/viewport_camera.hpp>

#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>
#include <engine/input.hpp>
#include <core/math/math.hpp>
#include <core/math/transform.hpp>
#include <core/math/vector4.hpp>
#include <engine/time.hpp>

namespace anton_engine {
    void Viewport_Camera::update(Viewport_Camera& camera, Transform& transform) {
        // Look around
        float horizontal_rotation = input::get_axis("mouse_x");
        float vertical_rotation = input::get_axis("mouse_y");
        transform.rotate(Vector3::up, math::radians(-horizontal_rotation));
        camera.camera_side = Vector3(Vector4(camera.camera_side) * math::transform::rotate_y(math::radians(horizontal_rotation)));
        transform.rotate(camera.camera_side, math::radians(vertical_rotation));

        // Move
        Vector3 camera_front = get_camera_front(transform);
        float camera_speed = 0.15f * 60 * get_delta_time();
        float forward = input::get_axis("move_forward");
        transform.translate(camera_front * camera_speed * forward);
        float sideways = input::get_axis("move_sideways");
        transform.translate(camera.camera_side * camera_speed * sideways);
        float vertical = input::get_axis("move_vertical");
        transform.translate(Vector3::up * camera_speed * vertical);

        float scroll = input::get_axis("scroll");
        transform.translate(camera_front * scroll);
    }
} // namespace anton_engine
