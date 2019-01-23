#include "components/camera.hpp"
#include "math/transform.hpp"
#include "components/transform.hpp"

extern uint32_t window_width;
extern uint32_t window_height;

Camera* Camera::main = nullptr;

Camera::Camera(Game_Object& go): Base_Component(go) {
    if (!main) {
        set_as_active_camera();
	}
}

Camera::~Camera() {
    if (main == this) {
        main = nullptr;
	}
}

void Camera::set_as_active_camera() {
    main = this;
}

void Camera::set_projection(Projection proj) {
    projection = proj;
}

Vector3 Camera::get_front() {
    Vector4 front = Vector4(0, 0, -1, 0) * transform::rotate(transform.rotation);
    return {front.x, front.y, front.z};
}

Matrix4 Camera::get_view_transform() {
    return transform::look_at(transform.position, transform.position + get_front(), Vector3::up);
}

Matrix4 Camera::get_projection_transform() {
    float aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
    if (projection == Projection::perspective) {
        return transform::perspective(fov, aspect_ratio, near_plane, far_plane);
    } else {
        return transform::orthographic(-size * aspect_ratio, size * aspect_ratio, -size, size, near_plane, far_plane);
    }
}