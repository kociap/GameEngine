#include "camera_movement.hpp"

#include "components/camera.hpp"
#include "entity.hpp"
#include "input/input.hpp"
#include "math/math.hpp"
#include "math/transform.hpp"

#include "components/component_system.hpp"

void Camera_Movement::update() {
    Entity const& entity = get_entity();
    Camera& camera = get_component<Camera>(entity);
    Transform& transform = get_component<Transform>(entity);

    // Look around
    float horizontal_rotation = Input::get_axis("mouse_x");
    float vertical_rotation = Input::get_axis("mouse_y");
    transform.rotate(Vector3::up, math::radians(-horizontal_rotation));
    camera_side = Vector3(Vector4(camera_side) * transform::rotate_y(math::radians(horizontal_rotation)));
    transform.rotate(camera_side, math::radians(vertical_rotation));

    // Move
    Vector3 camera_front = camera.get_front();
    float camera_speed = 0.15f;
    float forward = Input::get_axis("move_forward");
    transform.translate(camera_front * camera_speed * forward);
    float sideways = Input::get_axis("move_sideways");
    transform.translate(camera_side * camera_speed * sideways);
    float vertical = Input::get_axis("move_vertical");
    transform.translate(Vector3::up * camera_speed * vertical);

    float scroll = Input::get_axis("scroll");
    transform.translate(camera_front * scroll);
}