#include "camera_movement.hpp"

#include "ecs/entity.hpp"
#include "input/input.hpp"
#include "math/math.hpp"
#include "math/transform.hpp"
#include "time.hpp"
#include "build_config.hpp"

#if GE_WITH_EDITOR
#    include "editor.hpp"
#endif 

void Camera_Movement::update(Camera_Movement& camera_mov, Camera& camera, Transform& transform) {
#if GE_WITH_EDITOR
	if (Editor::is_mouse_captured()) {
#endif
	// Look around
    float horizontal_rotation = Input::get_axis("mouse_x");
    float vertical_rotation = Input::get_axis("mouse_y");
    transform.rotate(Vector3::up, math::radians(-horizontal_rotation));
    camera_mov.camera_side = Vector3(Vector4(camera_mov.camera_side) * math::transform::rotate_y(math::radians(horizontal_rotation)));
    transform.rotate(camera_mov.camera_side, math::radians(vertical_rotation));

    // Move
    Vector3 camera_front = get_camera_front(transform);
    float camera_speed = 0.15f * 60 * Time::get_delta_time();
    float forward = Input::get_axis("move_forward");
    transform.translate(camera_front * camera_speed * forward);
    float sideways = Input::get_axis("move_sideways");
    transform.translate(camera_mov.camera_side * camera_speed * sideways);
    float vertical = Input::get_axis("move_vertical");
    transform.translate(Vector3::up * camera_speed * vertical);

    float scroll = Input::get_axis("scroll");
    transform.translate(camera_front * scroll);
#if GE_WITH_EDITOR
	}
#endif
}