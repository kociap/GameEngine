#include <scripts/camera_movement.hpp>

#include <build_config.hpp>
#include <core/math/math.hpp>
#include <core/math/transform.hpp>
#include <engine/ecs/entity.hpp>
#include <engine/input.hpp>
#include <engine/time.hpp>

namespace anton_engine {
    void Camera_Movement::update(Camera_Movement& camera_mov, Camera&, Transform& transform) {
        // Look around
        float horizontal_rotation = input::get_axis("mouse_x");
        float vertical_rotation = input::get_axis("mouse_y");
        transform.rotate(Vector3{0.0f, 1.0f, 0.0f}, math::radians(-horizontal_rotation));
        camera_mov.camera_side = Vector3(Vector4(camera_mov.camera_side) * math::rotate_y(math::radians(horizontal_rotation)));
        transform.rotate(camera_mov.camera_side, math::radians(vertical_rotation));

        // Move
        Vector3 camera_front = get_camera_front(transform);
        float camera_speed = 0.15f * 60.0f * get_delta_time();
        float forward = input::get_axis("move_forward");
        transform.translate(camera_front * camera_speed * forward);
        float sideways = input::get_axis("move_sideways");
        transform.translate(camera_mov.camera_side * camera_speed * sideways);
        float vertical = input::get_axis("move_vertical");
        transform.translate(Vector3{0.0f, 1.0f, 0.0f} * camera_speed * vertical);

        float scroll = input::get_axis("scroll");
        transform.translate(camera_front * scroll);
    }
} // namespace anton_engine
